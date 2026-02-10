/*
 * STATIC LINKED LIST IMPLEMENTATIONS
 * Use: No heap allocation, deterministic memory, embedded systems
 *
 * Features:
 * - Singly linked list (static pool)
 * - Doubly linked list
 * - Circular list
 * - Sorted list
 * - Free list management
 *
 * Time: O(1) insert/remove at known position, O(n) search
 * Space: O(n) fixed at compile time
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* ============================================
 * SINGLY LINKED LIST (Static Pool)
 * ============================================ */

#define SLIST_MAX_NODES     32
#define SLIST_INVALID       (-1)

typedef int16_t slist_idx_t;

typedef struct {
    int32_t data;
    slist_idx_t next;
} SListNode;

typedef struct {
    SListNode nodes[SLIST_MAX_NODES];
    slist_idx_t head;
    slist_idx_t free_list;
    uint16_t count;
} SList;

void slist_init(SList *list) {
    list->head = SLIST_INVALID;
    list->count = 0;

    // Build free list
    for (int i = 0; i < SLIST_MAX_NODES - 1; i++) {
        list->nodes[i].next = i + 1;
    }
    list->nodes[SLIST_MAX_NODES - 1].next = SLIST_INVALID;
    list->free_list = 0;
}

static slist_idx_t slist_alloc_node(SList *list) {
    if (list->free_list == SLIST_INVALID) {
        return SLIST_INVALID;
    }

    slist_idx_t idx = list->free_list;
    list->free_list = list->nodes[idx].next;
    return idx;
}

static void slist_free_node(SList *list, slist_idx_t idx) {
    list->nodes[idx].next = list->free_list;
    list->free_list = idx;
}

bool slist_push_front(SList *list, int32_t data) {
    slist_idx_t idx = slist_alloc_node(list);
    if (idx == SLIST_INVALID) return false;

    list->nodes[idx].data = data;
    list->nodes[idx].next = list->head;
    list->head = idx;
    list->count++;

    return true;
}

bool slist_pop_front(SList *list, int32_t *data) {
    if (list->head == SLIST_INVALID) return false;

    slist_idx_t idx = list->head;
    *data = list->nodes[idx].data;
    list->head = list->nodes[idx].next;
    slist_free_node(list, idx);
    list->count--;

    return true;
}

bool slist_push_back(SList *list, int32_t data) {
    slist_idx_t idx = slist_alloc_node(list);
    if (idx == SLIST_INVALID) return false;

    list->nodes[idx].data = data;
    list->nodes[idx].next = SLIST_INVALID;

    if (list->head == SLIST_INVALID) {
        list->head = idx;
    } else {
        slist_idx_t curr = list->head;
        while (list->nodes[curr].next != SLIST_INVALID) {
            curr = list->nodes[curr].next;
        }
        list->nodes[curr].next = idx;
    }
    list->count++;

    return true;
}

// Insert after given index
bool slist_insert_after(SList *list, slist_idx_t after_idx, int32_t data) {
    if (after_idx == SLIST_INVALID) return slist_push_front(list, data);

    slist_idx_t idx = slist_alloc_node(list);
    if (idx == SLIST_INVALID) return false;

    list->nodes[idx].data = data;
    list->nodes[idx].next = list->nodes[after_idx].next;
    list->nodes[after_idx].next = idx;
    list->count++;

    return true;
}

// Remove node after given index
bool slist_remove_after(SList *list, slist_idx_t after_idx, int32_t *data) {
    if (after_idx == SLIST_INVALID) return slist_pop_front(list, data);

    slist_idx_t to_remove = list->nodes[after_idx].next;
    if (to_remove == SLIST_INVALID) return false;

    *data = list->nodes[to_remove].data;
    list->nodes[after_idx].next = list->nodes[to_remove].next;
    slist_free_node(list, to_remove);
    list->count--;

    return true;
}

// Find node with data, returns index or SLIST_INVALID
slist_idx_t slist_find(SList *list, int32_t data) {
    slist_idx_t curr = list->head;
    while (curr != SLIST_INVALID) {
        if (list->nodes[curr].data == data) {
            return curr;
        }
        curr = list->nodes[curr].next;
    }
    return SLIST_INVALID;
}

// Remove first node with matching data
bool slist_remove(SList *list, int32_t data) {
    if (list->head == SLIST_INVALID) return false;

    // Check head
    if (list->nodes[list->head].data == data) {
        int32_t dummy;
        return slist_pop_front(list, &dummy);
    }

    // Search for node
    slist_idx_t prev = list->head;
    slist_idx_t curr = list->nodes[prev].next;

    while (curr != SLIST_INVALID) {
        if (list->nodes[curr].data == data) {
            list->nodes[prev].next = list->nodes[curr].next;
            slist_free_node(list, curr);
            list->count--;
            return true;
        }
        prev = curr;
        curr = list->nodes[curr].next;
    }

    return false;
}

uint16_t slist_size(SList *list) {
    return list->count;
}

bool slist_is_empty(SList *list) {
    return list->head == SLIST_INVALID;
}


/* ============================================
 * DOUBLY LINKED LIST (Static Pool)
 * ============================================ */

#define DLIST_MAX_NODES     32
#define DLIST_INVALID       (-1)

typedef int16_t dlist_idx_t;

typedef struct {
    int32_t data;
    dlist_idx_t prev;
    dlist_idx_t next;
} DListNode;

typedef struct {
    DListNode nodes[DLIST_MAX_NODES];
    dlist_idx_t head;
    dlist_idx_t tail;
    dlist_idx_t free_list;
    uint16_t count;
} DList;

void dlist_init(DList *list) {
    list->head = DLIST_INVALID;
    list->tail = DLIST_INVALID;
    list->count = 0;

    for (int i = 0; i < DLIST_MAX_NODES - 1; i++) {
        list->nodes[i].next = i + 1;
    }
    list->nodes[DLIST_MAX_NODES - 1].next = DLIST_INVALID;
    list->free_list = 0;
}

static dlist_idx_t dlist_alloc_node(DList *list) {
    if (list->free_list == DLIST_INVALID) {
        return DLIST_INVALID;
    }

    dlist_idx_t idx = list->free_list;
    list->free_list = list->nodes[idx].next;
    return idx;
}

static void dlist_free_node(DList *list, dlist_idx_t idx) {
    list->nodes[idx].next = list->free_list;
    list->free_list = idx;
}

bool dlist_push_front(DList *list, int32_t data) {
    dlist_idx_t idx = dlist_alloc_node(list);
    if (idx == DLIST_INVALID) return false;

    list->nodes[idx].data = data;
    list->nodes[idx].prev = DLIST_INVALID;
    list->nodes[idx].next = list->head;

    if (list->head != DLIST_INVALID) {
        list->nodes[list->head].prev = idx;
    }
    list->head = idx;

    if (list->tail == DLIST_INVALID) {
        list->tail = idx;
    }
    list->count++;

    return true;
}

bool dlist_push_back(DList *list, int32_t data) {
    dlist_idx_t idx = dlist_alloc_node(list);
    if (idx == DLIST_INVALID) return false;

    list->nodes[idx].data = data;
    list->nodes[idx].prev = list->tail;
    list->nodes[idx].next = DLIST_INVALID;

    if (list->tail != DLIST_INVALID) {
        list->nodes[list->tail].next = idx;
    }
    list->tail = idx;

    if (list->head == DLIST_INVALID) {
        list->head = idx;
    }
    list->count++;

    return true;
}

bool dlist_pop_front(DList *list, int32_t *data) {
    if (list->head == DLIST_INVALID) return false;

    dlist_idx_t idx = list->head;
    *data = list->nodes[idx].data;

    list->head = list->nodes[idx].next;
    if (list->head != DLIST_INVALID) {
        list->nodes[list->head].prev = DLIST_INVALID;
    } else {
        list->tail = DLIST_INVALID;
    }

    dlist_free_node(list, idx);
    list->count--;

    return true;
}

bool dlist_pop_back(DList *list, int32_t *data) {
    if (list->tail == DLIST_INVALID) return false;

    dlist_idx_t idx = list->tail;
    *data = list->nodes[idx].data;

    list->tail = list->nodes[idx].prev;
    if (list->tail != DLIST_INVALID) {
        list->nodes[list->tail].next = DLIST_INVALID;
    } else {
        list->head = DLIST_INVALID;
    }

    dlist_free_node(list, idx);
    list->count--;

    return true;
}

// Remove node at specific index
bool dlist_remove_at(DList *list, dlist_idx_t idx) {
    if (idx == DLIST_INVALID) return false;

    DListNode *node = &list->nodes[idx];

    if (node->prev != DLIST_INVALID) {
        list->nodes[node->prev].next = node->next;
    } else {
        list->head = node->next;
    }

    if (node->next != DLIST_INVALID) {
        list->nodes[node->next].prev = node->prev;
    } else {
        list->tail = node->prev;
    }

    dlist_free_node(list, idx);
    list->count--;

    return true;
}

// Insert before given index
bool dlist_insert_before(DList *list, dlist_idx_t before_idx, int32_t data) {
    if (before_idx == DLIST_INVALID || before_idx == list->head) {
        return dlist_push_front(list, data);
    }

    dlist_idx_t idx = dlist_alloc_node(list);
    if (idx == DLIST_INVALID) return false;

    dlist_idx_t prev_idx = list->nodes[before_idx].prev;

    list->nodes[idx].data = data;
    list->nodes[idx].prev = prev_idx;
    list->nodes[idx].next = before_idx;

    list->nodes[prev_idx].next = idx;
    list->nodes[before_idx].prev = idx;
    list->count++;

    return true;
}

// Find node by value
dlist_idx_t dlist_find(DList *list, int32_t data) {
    dlist_idx_t curr = list->head;
    while (curr != DLIST_INVALID) {
        if (list->nodes[curr].data == data) {
            return curr;
        }
        curr = list->nodes[curr].next;
    }
    return DLIST_INVALID;
}

// Iterate forward
typedef void (*DListIterFunc)(int32_t data, void *arg);

void dlist_foreach(DList *list, DListIterFunc func, void *arg) {
    dlist_idx_t curr = list->head;
    while (curr != DLIST_INVALID) {
        func(list->nodes[curr].data, arg);
        curr = list->nodes[curr].next;
    }
}

// Iterate backward
void dlist_foreach_reverse(DList *list, DListIterFunc func, void *arg) {
    dlist_idx_t curr = list->tail;
    while (curr != DLIST_INVALID) {
        func(list->nodes[curr].data, arg);
        curr = list->nodes[curr].prev;
    }
}


/* ============================================
 * SORTED LINKED LIST
 * Maintains ascending order
 * ============================================ */

typedef struct {
    SList base;
} SortedList;

void sorted_list_init(SortedList *list) {
    slist_init(&list->base);
}

bool sorted_list_insert(SortedList *list, int32_t data) {
    slist_idx_t idx = slist_alloc_node(&list->base);
    if (idx == SLIST_INVALID) return false;

    list->base.nodes[idx].data = data;

    // Find insertion point
    if (list->base.head == SLIST_INVALID ||
        list->base.nodes[list->base.head].data >= data) {
        // Insert at head
        list->base.nodes[idx].next = list->base.head;
        list->base.head = idx;
    } else {
        // Find position
        slist_idx_t curr = list->base.head;
        while (list->base.nodes[curr].next != SLIST_INVALID &&
               list->base.nodes[list->base.nodes[curr].next].data < data) {
            curr = list->base.nodes[curr].next;
        }
        list->base.nodes[idx].next = list->base.nodes[curr].next;
        list->base.nodes[curr].next = idx;
    }

    list->base.count++;
    return true;
}

// Remove first occurrence
bool sorted_list_remove(SortedList *list, int32_t data) {
    return slist_remove(&list->base, data);
}

// Binary-style search (not actually binary, but early termination)
bool sorted_list_contains(SortedList *list, int32_t data) {
    slist_idx_t curr = list->base.head;
    while (curr != SLIST_INVALID) {
        if (list->base.nodes[curr].data == data) return true;
        if (list->base.nodes[curr].data > data) return false;  // Early termination
        curr = list->base.nodes[curr].next;
    }
    return false;
}


/* ============================================
 * CIRCULAR LINKED LIST
 * ============================================ */

#define CLIST_MAX_NODES     32

typedef struct {
    int32_t data;
    int16_t next;
} CListNode;

typedef struct {
    CListNode nodes[CLIST_MAX_NODES];
    int16_t tail;           // Points to last node (next is head)
    int16_t free_list;
    uint16_t count;
} CircularList;

void clist_init(CircularList *list) {
    list->tail = -1;
    list->count = 0;

    for (int i = 0; i < CLIST_MAX_NODES - 1; i++) {
        list->nodes[i].next = i + 1;
    }
    list->nodes[CLIST_MAX_NODES - 1].next = -1;
    list->free_list = 0;
}

static int16_t clist_alloc_node(CircularList *list) {
    if (list->free_list == -1) return -1;

    int16_t idx = list->free_list;
    list->free_list = list->nodes[idx].next;
    return idx;
}

static void clist_free_node(CircularList *list, int16_t idx) {
    list->nodes[idx].next = list->free_list;
    list->free_list = idx;
}

// Insert after tail (becomes new tail)
bool clist_push_back(CircularList *list, int32_t data) {
    int16_t idx = clist_alloc_node(list);
    if (idx == -1) return false;

    list->nodes[idx].data = data;

    if (list->tail == -1) {
        // First element - points to itself
        list->nodes[idx].next = idx;
    } else {
        // Insert after tail
        list->nodes[idx].next = list->nodes[list->tail].next;  // New node points to head
        list->nodes[list->tail].next = idx;  // Old tail points to new node
    }
    list->tail = idx;
    list->count++;

    return true;
}

// Insert at head (after tail, before old head)
bool clist_push_front(CircularList *list, int32_t data) {
    int16_t idx = clist_alloc_node(list);
    if (idx == -1) return false;

    list->nodes[idx].data = data;

    if (list->tail == -1) {
        list->nodes[idx].next = idx;
        list->tail = idx;
    } else {
        list->nodes[idx].next = list->nodes[list->tail].next;
        list->nodes[list->tail].next = idx;
        // Don't update tail - new node is at head
    }
    list->count++;

    return true;
}

// Remove head (first after tail)
bool clist_pop_front(CircularList *list, int32_t *data) {
    if (list->tail == -1) return false;

    int16_t head = list->nodes[list->tail].next;
    *data = list->nodes[head].data;

    if (head == list->tail) {
        // Last element
        list->tail = -1;
    } else {
        list->nodes[list->tail].next = list->nodes[head].next;
    }

    clist_free_node(list, head);
    list->count--;

    return true;
}

// Rotate: move head to tail
void clist_rotate(CircularList *list) {
    if (list->tail != -1 && list->count > 1) {
        list->tail = list->nodes[list->tail].next;
    }
}

// Get head without removing
int32_t clist_peek_front(CircularList *list) {
    if (list->tail == -1) return 0;
    return list->nodes[list->nodes[list->tail].next].data;
}

// Get tail without removing
int32_t clist_peek_back(CircularList *list) {
    if (list->tail == -1) return 0;
    return list->nodes[list->tail].data;
}


/* ============================================
 * INTRUSIVE LINKED LIST
 * Node is embedded in data structure
 * ============================================ */

typedef struct IntrusiveNode {
    struct IntrusiveNode *next;
    struct IntrusiveNode *prev;
} IntrusiveNode;

typedef struct {
    IntrusiveNode head;  // Sentinel node
} IntrusiveList;

void intrusive_list_init(IntrusiveList *list) {
    list->head.next = &list->head;
    list->head.prev = &list->head;
}

bool intrusive_list_is_empty(IntrusiveList *list) {
    return list->head.next == &list->head;
}

void intrusive_list_push_front(IntrusiveList *list, IntrusiveNode *node) {
    node->next = list->head.next;
    node->prev = &list->head;
    list->head.next->prev = node;
    list->head.next = node;
}

void intrusive_list_push_back(IntrusiveList *list, IntrusiveNode *node) {
    node->next = &list->head;
    node->prev = list->head.prev;
    list->head.prev->next = node;
    list->head.prev = node;
}

void intrusive_list_remove(IntrusiveNode *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->next = node;
    node->prev = node;
}

IntrusiveNode* intrusive_list_pop_front(IntrusiveList *list) {
    if (intrusive_list_is_empty(list)) return NULL;

    IntrusiveNode *node = list->head.next;
    intrusive_list_remove(node);
    return node;
}

IntrusiveNode* intrusive_list_pop_back(IntrusiveList *list) {
    if (intrusive_list_is_empty(list)) return NULL;

    IntrusiveNode *node = list->head.prev;
    intrusive_list_remove(node);
    return node;
}

// Usage example:
// typedef struct {
//     int data;
//     IntrusiveNode list_node;  // Embed list node
// } MyData;
//
// #define container_of(ptr, type, member) \
//     ((type *)((char *)(ptr) - offsetof(type, member)))
//
// MyData* get_data(IntrusiveNode *node) {
//     return container_of(node, MyData, list_node);
// }


/* ============================================
 * FREE LIST MANAGER
 * For managing a pool of indices
 * ============================================ */

#define FREELIST_MAX    64

typedef struct {
    int16_t next[FREELIST_MAX];
    int16_t free_head;
    uint16_t count;         // Number of free items
} FreeList;

void freelist_init(FreeList *fl) {
    for (int i = 0; i < FREELIST_MAX - 1; i++) {
        fl->next[i] = i + 1;
    }
    fl->next[FREELIST_MAX - 1] = -1;
    fl->free_head = 0;
    fl->count = FREELIST_MAX;
}

int16_t freelist_alloc(FreeList *fl) {
    if (fl->free_head == -1) return -1;

    int16_t idx = fl->free_head;
    fl->free_head = fl->next[idx];
    fl->count--;
    return idx;
}

void freelist_free(FreeList *fl, int16_t idx) {
    if (idx < 0 || idx >= FREELIST_MAX) return;

    fl->next[idx] = fl->free_head;
    fl->free_head = idx;
    fl->count++;
}

uint16_t freelist_available(FreeList *fl) {
    return fl->count;
}

bool freelist_is_empty(FreeList *fl) {
    return fl->free_head == -1;
}


/* ============================================
 * SKIP LIST (Simplified Static Version)
 * For faster search in sorted data
 * ============================================ */

#define SKIPLIST_MAX_LEVEL  4
#define SKIPLIST_MAX_NODES  32

typedef struct {
    int32_t key;
    int32_t value;
    int16_t forward[SKIPLIST_MAX_LEVEL];
    uint8_t level;
} SkipNode;

typedef struct {
    SkipNode nodes[SKIPLIST_MAX_NODES];
    int16_t head;           // Sentinel head node index
    int16_t free_list;
    uint8_t max_level;
    uint16_t count;
    uint32_t rand_state;    // Simple PRNG for level generation
} SkipList;

static uint8_t skiplist_random_level(SkipList *sl) {
    uint8_t level = 1;
    sl->rand_state = sl->rand_state * 1103515245 + 12345;
    while ((sl->rand_state & 0x3) == 0 && level < SKIPLIST_MAX_LEVEL) {
        level++;
        sl->rand_state = sl->rand_state * 1103515245 + 12345;
    }
    return level;
}

void skiplist_init(SkipList *sl) {
    // Initialize free list
    for (int i = 1; i < SKIPLIST_MAX_NODES - 1; i++) {
        sl->nodes[i].forward[0] = i + 1;
    }
    sl->nodes[SKIPLIST_MAX_NODES - 1].forward[0] = -1;
    sl->free_list = 1;  // Node 0 is head

    // Initialize head node
    sl->head = 0;
    for (int i = 0; i < SKIPLIST_MAX_LEVEL; i++) {
        sl->nodes[0].forward[i] = -1;
    }
    sl->nodes[0].level = SKIPLIST_MAX_LEVEL;

    sl->max_level = 1;
    sl->count = 0;
    sl->rand_state = 0x12345678;
}

bool skiplist_insert(SkipList *sl, int32_t key, int32_t value) {
    if (sl->free_list == -1) return false;

    int16_t update[SKIPLIST_MAX_LEVEL];
    int16_t curr = sl->head;

    // Find position
    for (int i = sl->max_level - 1; i >= 0; i--) {
        while (sl->nodes[curr].forward[i] != -1 &&
               sl->nodes[sl->nodes[curr].forward[i]].key < key) {
            curr = sl->nodes[curr].forward[i];
        }
        update[i] = curr;
    }

    // Allocate node
    int16_t idx = sl->free_list;
    sl->free_list = sl->nodes[idx].forward[0];

    uint8_t level = skiplist_random_level(sl);
    if (level > sl->max_level) {
        for (int i = sl->max_level; i < level; i++) {
            update[i] = sl->head;
        }
        sl->max_level = level;
    }

    sl->nodes[idx].key = key;
    sl->nodes[idx].value = value;
    sl->nodes[idx].level = level;

    for (int i = 0; i < level; i++) {
        sl->nodes[idx].forward[i] = sl->nodes[update[i]].forward[i];
        sl->nodes[update[i]].forward[i] = idx;
    }

    sl->count++;
    return true;
}

bool skiplist_find(SkipList *sl, int32_t key, int32_t *value) {
    int16_t curr = sl->head;

    for (int i = sl->max_level - 1; i >= 0; i--) {
        while (sl->nodes[curr].forward[i] != -1 &&
               sl->nodes[sl->nodes[curr].forward[i]].key < key) {
            curr = sl->nodes[curr].forward[i];
        }
    }

    curr = sl->nodes[curr].forward[0];
    if (curr != -1 && sl->nodes[curr].key == key) {
        *value = sl->nodes[curr].value;
        return true;
    }

    return false;
}

