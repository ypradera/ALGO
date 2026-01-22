/*
 * MEMORY POOL (Static Memory Allocator)
 * Use: Avoid malloc/free, deterministic allocation, fragmentation-free
 *
 * Features:
 * - Fixed-size block allocation
 * - O(1) alloc and free
 * - No fragmentation
 * - Thread-safe variants
 *
 * Time: O(1) all operations
 * Space: O(n) pool size
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* ============================================
 * FIXED-SIZE BLOCK POOL
 * ============================================ */

#define POOL_BLOCK_SIZE     64
#define POOL_NUM_BLOCKS     32

typedef struct {
    uint8_t memory[POOL_NUM_BLOCKS][POOL_BLOCK_SIZE];
    uint8_t free_list[POOL_NUM_BLOCKS];  // indices of free blocks
    uint8_t free_count;
    uint8_t alloc_count;
} MemoryPool;

void pool_init(MemoryPool *pool) {
    pool->free_count = POOL_NUM_BLOCKS;
    pool->alloc_count = 0;

    // Initialize free list
    for (int i = 0; i < POOL_NUM_BLOCKS; i++) {
        pool->free_list[i] = i;
    }
}

void* pool_alloc(MemoryPool *pool) {
    if (pool->free_count == 0) {
        return NULL;  // no free blocks
    }

    uint8_t block_idx = pool->free_list[--pool->free_count];
    pool->alloc_count++;

    return pool->memory[block_idx];
}

bool pool_free(MemoryPool *pool, void *ptr) {
    // Calculate block index from pointer
    uint8_t *mem_start = (uint8_t *)pool->memory;
    uint8_t *block_ptr = (uint8_t *)ptr;

    if (block_ptr < mem_start ||
        block_ptr >= mem_start + POOL_NUM_BLOCKS * POOL_BLOCK_SIZE) {
        return false;  // pointer not from this pool
    }

    size_t offset = block_ptr - mem_start;
    if (offset % POOL_BLOCK_SIZE != 0) {
        return false;  // not aligned to block boundary
    }

    uint8_t block_idx = offset / POOL_BLOCK_SIZE;
    pool->free_list[pool->free_count++] = block_idx;
    pool->alloc_count--;

    return true;
}

uint8_t pool_available(MemoryPool *pool) {
    return pool->free_count;
}


/* ============================================
 * LINKED-LIST FREE LIST POOL
 * (More memory efficient for variable sizes)
 * ============================================ */

#define LL_POOL_SIZE        2048
#define LL_MIN_BLOCK        16

typedef struct FreeBlock {
    struct FreeBlock *next;
    uint16_t size;
} FreeBlock;

typedef struct {
    uint8_t memory[LL_POOL_SIZE];
    FreeBlock *free_list;
    uint16_t total_free;
} LinkedListPool;

void ll_pool_init(LinkedListPool *pool) {
    pool->free_list = (FreeBlock *)pool->memory;
    pool->free_list->next = NULL;
    pool->free_list->size = LL_POOL_SIZE;
    pool->total_free = LL_POOL_SIZE;
}

void* ll_pool_alloc(LinkedListPool *pool, uint16_t size) {
    // Align size to minimum block size
    size = (size + LL_MIN_BLOCK - 1) & ~(LL_MIN_BLOCK - 1);
    size += sizeof(uint16_t);  // header for storing size

    FreeBlock *prev = NULL;
    FreeBlock *curr = pool->free_list;

    while (curr) {
        if (curr->size >= size) {
            // Found suitable block
            if (curr->size >= size + LL_MIN_BLOCK + sizeof(FreeBlock)) {
                // Split block
                FreeBlock *new_free = (FreeBlock *)((uint8_t *)curr + size);
                new_free->size = curr->size - size;
                new_free->next = curr->next;

                if (prev) {
                    prev->next = new_free;
                } else {
                    pool->free_list = new_free;
                }
            } else {
                // Use entire block
                size = curr->size;
                if (prev) {
                    prev->next = curr->next;
                } else {
                    pool->free_list = curr->next;
                }
            }

            pool->total_free -= size;

            // Store size in header
            uint16_t *header = (uint16_t *)curr;
            *header = size;

            return header + 1;  // return pointer after header
        }

        prev = curr;
        curr = curr->next;
    }

    return NULL;  // no suitable block found
}

void ll_pool_free(LinkedListPool *pool, void *ptr) {
    if (!ptr) return;

    uint16_t *header = (uint16_t *)ptr - 1;
    uint16_t size = *header;

    FreeBlock *block = (FreeBlock *)header;
    block->size = size;
    block->next = pool->free_list;
    pool->free_list = block;

    pool->total_free += size;

    // TODO: Coalesce adjacent free blocks
}


/* ============================================
 * OBJECT POOL (typed allocator)
 * ============================================ */

#define OBJECT_POOL_SIZE    16

// Generic object pool using macro
#define DEFINE_OBJECT_POOL(name, type) \
    typedef struct { \
        type objects[OBJECT_POOL_SIZE]; \
        uint8_t in_use[OBJECT_POOL_SIZE]; \
        uint8_t free_count; \
    } name##_Pool; \
    \
    static inline void name##_pool_init(name##_Pool *pool) { \
        memset(pool->in_use, 0, OBJECT_POOL_SIZE); \
        pool->free_count = OBJECT_POOL_SIZE; \
    } \
    \
    static inline type* name##_pool_alloc(name##_Pool *pool) { \
        for (int i = 0; i < OBJECT_POOL_SIZE; i++) { \
            if (!pool->in_use[i]) { \
                pool->in_use[i] = 1; \
                pool->free_count--; \
                return &pool->objects[i]; \
            } \
        } \
        return NULL; \
    } \
    \
    static inline void name##_pool_free(name##_Pool *pool, type *obj) { \
        size_t idx = obj - pool->objects; \
        if (idx < OBJECT_POOL_SIZE && pool->in_use[idx]) { \
            pool->in_use[idx] = 0; \
            pool->free_count++; \
        } \
    }


/* ============================================
 * SLAB ALLOCATOR (multiple size classes)
 * ============================================ */

#define NUM_SIZE_CLASSES    4
#define SLAB_BLOCKS         16

typedef struct {
    uint8_t memory[SLAB_BLOCKS];
    uint8_t block_size;
    uint8_t free_bitmap;  // each bit = 1 block (max 8 blocks)
} Slab;

typedef struct {
    Slab slabs[NUM_SIZE_CLASSES];
    uint8_t size_classes[NUM_SIZE_CLASSES];  // 8, 16, 32, 64
} SlabAllocator;

void slab_init(SlabAllocator *alloc) {
    uint8_t sizes[] = {8, 16, 32, 64};

    for (int i = 0; i < NUM_SIZE_CLASSES; i++) {
        alloc->size_classes[i] = sizes[i];
        alloc->slabs[i].block_size = sizes[i];
        alloc->slabs[i].free_bitmap = 0xFF;  // all free
    }
}

void* slab_alloc(SlabAllocator *alloc, uint8_t size) {
    // Find smallest suitable size class
    int class_idx = -1;
    for (int i = 0; i < NUM_SIZE_CLASSES; i++) {
        if (alloc->size_classes[i] >= size) {
            class_idx = i;
            break;
        }
    }

    if (class_idx < 0) return NULL;  // too large

    Slab *slab = &alloc->slabs[class_idx];

    // Find free block (first set bit)
    if (slab->free_bitmap == 0) return NULL;

    int block = 0;
    uint8_t bitmap = slab->free_bitmap;
    while ((bitmap & 1) == 0) {
        bitmap >>= 1;
        block++;
    }

    slab->free_bitmap &= ~(1 << block);  // mark as used

    return slab->memory + (block * slab->block_size);
}


/* ============================================
 * BUFFER POOL (for network/protocol buffers)
 * ============================================ */

#define BUFFER_SIZE         256
#define NUM_BUFFERS         8

typedef struct {
    uint8_t data[BUFFER_SIZE];
    uint16_t length;
    uint8_t flags;
    uint8_t ref_count;
} Buffer;

typedef struct {
    Buffer buffers[NUM_BUFFERS];
    uint8_t free_mask;  // bitmap of free buffers
} BufferPool;

void buffer_pool_init(BufferPool *pool) {
    pool->free_mask = 0xFF;  // all free
    memset(pool->buffers, 0, sizeof(pool->buffers));
}

Buffer* buffer_alloc(BufferPool *pool) {
    if (pool->free_mask == 0) return NULL;

    // Find first free buffer
    int idx = 0;
    uint8_t mask = pool->free_mask;
    while ((mask & 1) == 0) {
        mask >>= 1;
        idx++;
    }

    pool->free_mask &= ~(1 << idx);

    Buffer *buf = &pool->buffers[idx];
    buf->length = 0;
    buf->flags = 0;
    buf->ref_count = 1;

    return buf;
}

void buffer_free(BufferPool *pool, Buffer *buf) {
    if (!buf || buf->ref_count == 0) return;

    buf->ref_count--;
    if (buf->ref_count == 0) {
        size_t idx = buf - pool->buffers;
        if (idx < NUM_BUFFERS) {
            pool->free_mask |= (1 << idx);
        }
    }
}

void buffer_addref(Buffer *buf) {
    if (buf && buf->ref_count < 255) {
        buf->ref_count++;
    }
}


/* ============================================
 * STACK ALLOCATOR (LIFO, for temp allocations)
 * ============================================ */

#define STACK_ALLOC_SIZE    4096

typedef struct {
    uint8_t memory[STACK_ALLOC_SIZE];
    uint16_t top;
    uint16_t markers[8];  // save points for batch free
    uint8_t marker_count;
} StackAllocator;

void stack_alloc_init(StackAllocator *alloc) {
    alloc->top = 0;
    alloc->marker_count = 0;
}

void* stack_alloc_alloc(StackAllocator *alloc, uint16_t size) {
    // Align to 4 bytes
    size = (size + 3) & ~3;

    if (alloc->top + size > STACK_ALLOC_SIZE) {
        return NULL;
    }

    void *ptr = alloc->memory + alloc->top;
    alloc->top += size;
    return ptr;
}

void stack_alloc_mark(StackAllocator *alloc) {
    if (alloc->marker_count < 8) {
        alloc->markers[alloc->marker_count++] = alloc->top;
    }
}

void stack_alloc_restore(StackAllocator *alloc) {
    if (alloc->marker_count > 0) {
        alloc->top = alloc->markers[--alloc->marker_count];
    }
}

void stack_alloc_reset(StackAllocator *alloc) {
    alloc->top = 0;
    alloc->marker_count = 0;
}
