/*
 * BIT ARRAY / BITMAP IMPLEMENTATIONS
 * Use: Flags, status bits, resource allocation, bloom filters, memory management
 *
 * Features:
 * - Basic bit array operations
 * - Bitmap allocator
 * - Bloom filter
 * - Bit field manipulation
 *
 * Time: O(1) for single bit ops, O(n/32) for scans
 * Space: O(n/8) bytes for n bits
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* ============================================
 * BASIC BIT ARRAY
 * ============================================ */

#define BITARRAY_SIZE   256     // Number of bits
#define BITARRAY_WORDS  ((BITARRAY_SIZE + 31) / 32)

typedef struct {
    uint32_t bits[BITARRAY_WORDS];
} BitArray;

void bitarray_init(BitArray *ba) {
    memset(ba->bits, 0, sizeof(ba->bits));
}

void bitarray_set_all(BitArray *ba) {
    memset(ba->bits, 0xFF, sizeof(ba->bits));
}

void bitarray_set(BitArray *ba, uint16_t bit) {
    if (bit < BITARRAY_SIZE) {
        ba->bits[bit / 32] |= (1U << (bit % 32));
    }
}

void bitarray_clear(BitArray *ba, uint16_t bit) {
    if (bit < BITARRAY_SIZE) {
        ba->bits[bit / 32] &= ~(1U << (bit % 32));
    }
}

void bitarray_toggle(BitArray *ba, uint16_t bit) {
    if (bit < BITARRAY_SIZE) {
        ba->bits[bit / 32] ^= (1U << (bit % 32));
    }
}

bool bitarray_test(BitArray *ba, uint16_t bit) {
    if (bit >= BITARRAY_SIZE) return false;
    return (ba->bits[bit / 32] & (1U << (bit % 32))) != 0;
}

// Count set bits (popcount)
uint16_t bitarray_count(BitArray *ba) {
    uint16_t count = 0;
    for (int i = 0; i < BITARRAY_WORDS; i++) {
        uint32_t n = ba->bits[i];
        // Brian Kernighan's algorithm
        while (n) {
            n &= (n - 1);
            count++;
        }
    }
    return count;
}

// Find first set bit (-1 if none)
int bitarray_find_first_set(BitArray *ba) {
    for (int i = 0; i < BITARRAY_WORDS; i++) {
        if (ba->bits[i]) {
            // Find position in this word
            uint32_t n = ba->bits[i];
            int pos = 0;
            while ((n & 1) == 0) {
                n >>= 1;
                pos++;
            }
            return i * 32 + pos;
        }
    }
    return -1;
}

// Find first clear bit (-1 if none)
int bitarray_find_first_clear(BitArray *ba) {
    for (int i = 0; i < BITARRAY_WORDS; i++) {
        if (ba->bits[i] != 0xFFFFFFFF) {
            uint32_t n = ~ba->bits[i];
            int pos = 0;
            while ((n & 1) == 0) {
                n >>= 1;
                pos++;
            }
            int bit = i * 32 + pos;
            return (bit < BITARRAY_SIZE) ? bit : -1;
        }
    }
    return -1;
}


/* ============================================
 * BITMAP ALLOCATOR
 * For resource/slot allocation
 * ============================================ */

#define BITMAP_ALLOC_SIZE   128

typedef struct {
    uint32_t bitmap[(BITMAP_ALLOC_SIZE + 31) / 32];
    uint16_t free_count;
} BitmapAllocator;

void bitmap_alloc_init(BitmapAllocator *ba) {
    memset(ba->bitmap, 0, sizeof(ba->bitmap));
    ba->free_count = BITMAP_ALLOC_SIZE;
}

// Allocate single slot, returns -1 if full
int bitmap_alloc_one(BitmapAllocator *ba) {
    if (ba->free_count == 0) return -1;

    for (int i = 0; i < (BITMAP_ALLOC_SIZE + 31) / 32; i++) {
        if (ba->bitmap[i] != 0xFFFFFFFF) {
            uint32_t n = ~ba->bitmap[i];
            int pos = 0;
            while ((n & 1) == 0) {
                n >>= 1;
                pos++;
            }
            int slot = i * 32 + pos;
            if (slot < BITMAP_ALLOC_SIZE) {
                ba->bitmap[i] |= (1U << pos);
                ba->free_count--;
                return slot;
            }
        }
    }
    return -1;
}

// Free single slot
void bitmap_free_one(BitmapAllocator *ba, int slot) {
    if (slot < 0 || slot >= BITMAP_ALLOC_SIZE) return;

    int word = slot / 32;
    int bit = slot % 32;

    if (ba->bitmap[word] & (1U << bit)) {
        ba->bitmap[word] &= ~(1U << bit);
        ba->free_count++;
    }
}

bool bitmap_is_allocated(BitmapAllocator *ba, int slot) {
    if (slot < 0 || slot >= BITMAP_ALLOC_SIZE) return false;
    return (ba->bitmap[slot / 32] & (1U << (slot % 32))) != 0;
}

// Allocate contiguous block, returns start index or -1
int bitmap_alloc_contiguous(BitmapAllocator *ba, uint16_t count) {
    if (count == 0 || count > ba->free_count) return -1;

    int consecutive = 0;
    int start = -1;

    for (int i = 0; i < BITMAP_ALLOC_SIZE; i++) {
        if (!bitmap_is_allocated(ba, i)) {
            if (consecutive == 0) start = i;
            consecutive++;
            if (consecutive == count) {
                // Mark all as allocated
                for (int j = start; j < start + count; j++) {
                    ba->bitmap[j / 32] |= (1U << (j % 32));
                }
                ba->free_count -= count;
                return start;
            }
        } else {
            consecutive = 0;
            start = -1;
        }
    }
    return -1;
}


/* ============================================
 * BIT FIELD MANIPULATION
 * For packed structures, registers
 * ============================================ */

// Extract bits [start, start+width) from value
static inline uint32_t extract_bits(uint32_t value, uint8_t start, uint8_t width) {
    uint32_t mask = (1U << width) - 1;
    return (value >> start) & mask;
}

// Insert bits into value at [start, start+width)
static inline uint32_t insert_bits(uint32_t value, uint32_t bits, uint8_t start, uint8_t width) {
    uint32_t mask = ((1U << width) - 1) << start;
    return (value & ~mask) | ((bits << start) & mask);
}

// Bit field descriptor
typedef struct {
    uint8_t start;      // Start bit position
    uint8_t width;      // Number of bits
    const char *name;   // Field name (optional)
} BitField;

// Get field value from register
uint32_t bitfield_get(uint32_t reg, const BitField *field) {
    return extract_bits(reg, field->start, field->width);
}

// Set field value in register
uint32_t bitfield_set(uint32_t reg, const BitField *field, uint32_t value) {
    return insert_bits(reg, value, field->start, field->width);
}


/* ============================================
 * HARDWARE REGISTER HELPERS
 * ============================================ */

typedef volatile uint32_t reg32_t;

static inline void reg_set_bits(reg32_t *reg, uint32_t mask) {
    *reg |= mask;
}

static inline void reg_clear_bits(reg32_t *reg, uint32_t mask) {
    *reg &= ~mask;
}

static inline void reg_toggle_bits(reg32_t *reg, uint32_t mask) {
    *reg ^= mask;
}

static inline bool reg_test_bits(reg32_t *reg, uint32_t mask) {
    return (*reg & mask) == mask;
}

static inline bool reg_test_any(reg32_t *reg, uint32_t mask) {
    return (*reg & mask) != 0;
}

static inline void reg_modify(reg32_t *reg, uint32_t clear_mask, uint32_t set_mask) {
    *reg = (*reg & ~clear_mask) | set_mask;
}


/* ============================================
 * FLAG REGISTER
 * Named flags for status bits
 * ============================================ */

typedef struct {
    uint32_t flags;
} FlagRegister;

// Common flag bits
#define FLAG_READY      (1U << 0)
#define FLAG_BUSY       (1U << 1)
#define FLAG_ERROR      (1U << 2)
#define FLAG_TIMEOUT    (1U << 3)
#define FLAG_OVERFLOW   (1U << 4)
#define FLAG_UNDERFLOW  (1U << 5)
#define FLAG_COMPLETE   (1U << 6)
#define FLAG_PENDING    (1U << 7)

void flags_init(FlagRegister *fr) {
    fr->flags = 0;
}

void flags_set(FlagRegister *fr, uint32_t mask) {
    fr->flags |= mask;
}

void flags_clear(FlagRegister *fr, uint32_t mask) {
    fr->flags &= ~mask;
}

bool flags_test(FlagRegister *fr, uint32_t mask) {
    return (fr->flags & mask) == mask;
}

bool flags_test_any(FlagRegister *fr, uint32_t mask) {
    return (fr->flags & mask) != 0;
}

// Atomic test and clear (useful for ISR communication)
bool flags_test_and_clear(FlagRegister *fr, uint32_t mask) {
    bool was_set = (fr->flags & mask) != 0;
    fr->flags &= ~mask;
    return was_set;
}


/* ============================================
 * SIMPLE BLOOM FILTER
 * For probabilistic set membership
 * ============================================ */

#define BLOOM_SIZE_BITS     1024
#define BLOOM_SIZE_WORDS    ((BLOOM_SIZE_BITS + 31) / 32)
#define BLOOM_NUM_HASHES    3

typedef struct {
    uint32_t bits[BLOOM_SIZE_WORDS];
} BloomFilter;

void bloom_init(BloomFilter *bf) {
    memset(bf->bits, 0, sizeof(bf->bits));
}

// Simple hash functions
static uint32_t bloom_hash1(uint32_t key) {
    key = ((key >> 16) ^ key) * 0x45d9f3b;
    key = ((key >> 16) ^ key) * 0x45d9f3b;
    key = (key >> 16) ^ key;
    return key % BLOOM_SIZE_BITS;
}

static uint32_t bloom_hash2(uint32_t key) {
    key = (key ^ 61) ^ (key >> 16);
    key = key + (key << 3);
    key = key ^ (key >> 4);
    key = key * 0x27d4eb2d;
    key = key ^ (key >> 15);
    return key % BLOOM_SIZE_BITS;
}

static uint32_t bloom_hash3(uint32_t key) {
    key = ~key + (key << 15);
    key = key ^ (key >> 12);
    key = key + (key << 2);
    key = key ^ (key >> 4);
    key = key * 2057;
    key = key ^ (key >> 16);
    return key % BLOOM_SIZE_BITS;
}

void bloom_add(BloomFilter *bf, uint32_t key) {
    uint32_t h1 = bloom_hash1(key);
    uint32_t h2 = bloom_hash2(key);
    uint32_t h3 = bloom_hash3(key);

    bf->bits[h1 / 32] |= (1U << (h1 % 32));
    bf->bits[h2 / 32] |= (1U << (h2 % 32));
    bf->bits[h3 / 32] |= (1U << (h3 % 32));
}

// Returns true if possibly in set, false if definitely not
bool bloom_maybe_contains(BloomFilter *bf, uint32_t key) {
    uint32_t h1 = bloom_hash1(key);
    uint32_t h2 = bloom_hash2(key);
    uint32_t h3 = bloom_hash3(key);

    return (bf->bits[h1 / 32] & (1U << (h1 % 32))) &&
           (bf->bits[h2 / 32] & (1U << (h2 % 32))) &&
           (bf->bits[h3 / 32] & (1U << (h3 % 32)));
}


/* ============================================
 * GPIO PORT ABSTRACTION
 * Bit-banding style for port operations
 * ============================================ */

typedef struct {
    uint8_t port_state;
    uint8_t port_direction;     // 0 = input, 1 = output
    uint8_t pullup_enable;
    uint8_t interrupt_enable;
} GPIOPort;

void gpio_init(GPIOPort *gpio) {
    gpio->port_state = 0;
    gpio->port_direction = 0;
    gpio->pullup_enable = 0;
    gpio->interrupt_enable = 0;
}

void gpio_set_direction(GPIOPort *gpio, uint8_t pin, bool output) {
    if (pin > 7) return;
    if (output) {
        gpio->port_direction |= (1U << pin);
    } else {
        gpio->port_direction &= ~(1U << pin);
    }
}

void gpio_set_output(GPIOPort *gpio, uint8_t pin, bool high) {
    if (pin > 7) return;
    if (high) {
        gpio->port_state |= (1U << pin);
    } else {
        gpio->port_state &= ~(1U << pin);
    }
}

void gpio_toggle(GPIOPort *gpio, uint8_t pin) {
    if (pin > 7) return;
    gpio->port_state ^= (1U << pin);
}

bool gpio_read(GPIOPort *gpio, uint8_t pin) {
    if (pin > 7) return false;
    return (gpio->port_state & (1U << pin)) != 0;
}

void gpio_write_port(GPIOPort *gpio, uint8_t value) {
    gpio->port_state = value;
}

uint8_t gpio_read_port(GPIOPort *gpio) {
    return gpio->port_state;
}


/* ============================================
 * BIT MANIPULATION UTILITIES
 * ============================================ */

// Count leading zeros
static inline uint8_t clz32(uint32_t x) {
    if (x == 0) return 32;
    uint8_t n = 0;
    if ((x & 0xFFFF0000) == 0) { n += 16; x <<= 16; }
    if ((x & 0xFF000000) == 0) { n += 8; x <<= 8; }
    if ((x & 0xF0000000) == 0) { n += 4; x <<= 4; }
    if ((x & 0xC0000000) == 0) { n += 2; x <<= 2; }
    if ((x & 0x80000000) == 0) { n += 1; }
    return n;
}

// Count trailing zeros
static inline uint8_t ctz32(uint32_t x) {
    if (x == 0) return 32;
    uint8_t n = 0;
    if ((x & 0x0000FFFF) == 0) { n += 16; x >>= 16; }
    if ((x & 0x000000FF) == 0) { n += 8; x >>= 8; }
    if ((x & 0x0000000F) == 0) { n += 4; x >>= 4; }
    if ((x & 0x00000003) == 0) { n += 2; x >>= 2; }
    if ((x & 0x00000001) == 0) { n += 1; }
    return n;
}

// Population count (number of 1 bits)
static inline uint8_t popcount32(uint32_t x) {
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0F0F0F0F;
    return (x * 0x01010101) >> 24;
}

// Is power of 2
static inline bool is_power_of_2(uint32_t x) {
    return x && !(x & (x - 1));
}

// Next power of 2
static inline uint32_t next_power_of_2(uint32_t x) {
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

// Reverse bits
static inline uint32_t reverse_bits32(uint32_t x) {
    x = ((x >> 1) & 0x55555555) | ((x & 0x55555555) << 1);
    x = ((x >> 2) & 0x33333333) | ((x & 0x33333333) << 2);
    x = ((x >> 4) & 0x0F0F0F0F) | ((x & 0x0F0F0F0F) << 4);
    x = ((x >> 8) & 0x00FF00FF) | ((x & 0x00FF00FF) << 8);
    return (x >> 16) | (x << 16);
}

// Swap bytes (endian conversion)
static inline uint32_t swap_bytes32(uint32_t x) {
    return ((x >> 24) & 0xFF) |
           ((x >> 8) & 0xFF00) |
           ((x << 8) & 0xFF0000) |
           ((x << 24) & 0xFF000000);
}

static inline uint16_t swap_bytes16(uint16_t x) {
    return (x >> 8) | (x << 8);
}

