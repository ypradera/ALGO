/*
 * BIT MANIPULATION PATTERN
 * Use: Flags, masks, power of 2, XOR tricks, embedded registers
 * Time: O(1) or O(log n), Space: O(1)
 */

#include <stdint.h>

// === BASIC OPERATIONS ===
#define SET_BIT(x, pos)      ((x) |  (1U << (pos)))
#define CLEAR_BIT(x, pos)    ((x) & ~(1U << (pos)))
#define TOGGLE_BIT(x, pos)   ((x) ^  (1U << (pos)))
#define CHECK_BIT(x, pos)    (((x) >> (pos)) & 1U)

// === BIT MASKS ===
#define MASK_LOWER_N(n)      ((1U << (n)) - 1)          // n lower bits set
#define MASK_RANGE(hi, lo)   (((1U << ((hi)-(lo)+1)) - 1) << (lo))

// Extract bits [hi:lo] from x
#define EXTRACT_BITS(x, hi, lo)  (((x) >> (lo)) & ((1U << ((hi)-(lo)+1)) - 1))

// === COMMON TRICKS ===
// Check if power of 2
#define IS_POWER_OF_2(x)     ((x) && !((x) & ((x) - 1)))

// Count set bits (Brian Kernighan)
int count_bits(uint32_t n) {
    int count = 0;
    while (n) {
        n &= (n - 1);  // clear lowest set bit
        count++;
    }
    return count;
}

// Get lowest set bit
#define LOWEST_SET_BIT(x)    ((x) & -(x))

// Clear lowest set bit
#define CLEAR_LOWEST_BIT(x)  ((x) & ((x) - 1))

// === XOR TRICKS ===
// Find single number (all others appear twice)
int find_single(int *arr, int n) {
    int result = 0;
    for (int i = 0; i < n; i++) {
        result ^= arr[i];
    }
    return result;
}

// Swap without temp
#define SWAP_XOR(a, b)  do { (a) ^= (b); (b) ^= (a); (a) ^= (b); } while(0)

// === EMBEDDED REGISTER PATTERNS ===
// Read-modify-write for hardware register
// volatile uint32_t *REG = (volatile uint32_t *)0x40000000;
// *REG = (*REG & ~MASK) | (value << shift);

// Set multiple bits atomically (example)
// *REG |= (BIT_A | BIT_B | BIT_C);

// === ITERATE THROUGH SET BITS ===
void iterate_set_bits(uint32_t n) {
    while (n) {
        uint32_t lowest = n & (-n);       // isolate lowest set bit
        int pos = __builtin_ctz(n);       // position (GCC built-in)
        // process bit at position 'pos'
        n &= (n - 1);                     // clear lowest set bit
    }
}

// === SIGN AND ABSOLUTE ===
#define ABS_NO_BRANCH(x)     (((x) ^ ((x) >> 31)) - ((x) >> 31))
#define SIGN(x)              (((x) >> 31) | (-(x) >> 31))  // -1, 0, or 1
