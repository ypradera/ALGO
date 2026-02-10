/*
 * Arrays - Interview Problems for Embedded Systems
 *
 * ============================================================================
 *                    BEGINNER'S GUIDE: ARRAYS
 * ============================================================================
 *
 * WHAT IS AN ARRAY?
 * -----------------
 * An array is a collection of items stored at CONTIGUOUS memory locations.
 * All elements are the same type and can be accessed by INDEX.
 *
 * VISUAL REPRESENTATION:
 *
 *   Index:    [0]    [1]    [2]    [3]    [4]
 *            +------+------+------+------+------+
 *   Data:    |  10  |  20  |  30  |  40  |  50  |
 *            +------+------+------+------+------+
 *   Address: 0x100  0x104  0x108  0x10C  0x110
 *            (each int is 4 bytes apart)
 *
 * KEY PROPERTY:
 *   Address of arr[i] = Base_Address + (i * sizeof(element))
 *   This is why access is O(1) - simple arithmetic!
 *
 * ARRAYS VS LINKED LISTS:
 * -----------------------
 *   Feature          | Array          | Linked List
 *   -----------------|----------------|---------------
 *   Access by index  | O(1)           | O(n)
 *   Insert at end    | O(1) amortized | O(n) or O(1)*
 *   Insert at middle | O(n)           | O(1)**
 *   Delete at middle | O(n)           | O(1)**
 *   Memory           | Contiguous     | Scattered
 *   Cache friendly   | Yes            | No
 *   Size             | Fixed/Dynamic  | Dynamic
 *
 *   * O(1) if tail pointer maintained
 *   ** O(1) if you have pointer to node
 *
 * WHY ARRAYS ARE PREFERRED IN EMBEDDED:
 *   1. No dynamic allocation needed (can be static)
 *   2. Cache-friendly (contiguous memory)
 *   3. Predictable memory layout
 *   4. No pointer overhead
 *   5. Easier to analyze for worst-case timing
 *
 * ============================================================================
 * BIG-O COMPLEXITY FOR ARRAY OPERATIONS
 * ============================================================================
 *
 * Operation              | Time     | Notes
 * -----------------------|----------|----------------------------------------
 * Access by index        | O(1)     | Direct calculation
 * Search (unsorted)      | O(n)     | Linear search
 * Search (sorted)        | O(log n) | Binary search
 * Insert at end          | O(1)     | If space available
 * Insert at middle       | O(n)     | Must shift elements
 * Delete at end          | O(1)     | Just decrease size
 * Delete at middle       | O(n)     | Must shift elements
 * Find min/max           | O(n)     | Must check all
 * Find min/max (sorted)  | O(1)     | First/last element
 *
 * ============================================================================
 * COMMON INTERVIEW PROBLEMS
 * ============================================================================
 *
 * #  | Problem                          | Time      | Space  | Technique
 * ---|----------------------------------|-----------|--------|------------------
 * 1  | Two Sum                          | O(n)      | O(n)   | Hash map
 * 2  | Maximum Subarray (Kadane's)      | O(n)      | O(1)   | Dynamic programming
 * 3  | Merge Sorted Arrays              | O(n+m)    | O(1)*  | Two pointers
 * 4  | Rotate Array                     | O(n)      | O(1)   | Reverse trick
 * 5  | Find Duplicate                   | O(n)      | O(1)   | Floyd's cycle
 * 6  | Missing Number                   | O(n)      | O(1)   | XOR or math
 * 7  | Move Zeroes                      | O(n)      | O(1)   | Two pointers
 * 8  | Product Except Self              | O(n)      | O(1)   | Prefix/suffix
 * 9  | Container With Most Water        | O(n)      | O(1)   | Two pointers
 * 10 | Sliding Window Maximum           | O(n)      | O(k)   | Deque
 *
 * ============================================================================
 * EMBEDDED SYSTEMS SPECIFIC TOPICS
 * ============================================================================
 *
 * 1. CIRCULAR BUFFERS (Ring Buffers)
 *    - Fixed size array that wraps around
 *    - Used for: UART, DMA, sensor data, audio
 *    - O(1) read and write
 *
 * 2. STATIC ALLOCATION
 *    - No malloc/free in safety-critical code
 *    - Arrays defined at compile time
 *
 * 3. MEMORY-MAPPED I/O
 *    - Hardware registers accessed as arrays
 *    - volatile keyword essential
 *
 * 4. LOOKUP TABLES
 *    - Pre-computed values for fast access
 *    - Sine tables, CRC tables, etc.
 *
 * 5. DMA BUFFERS
 *    - Must be aligned, often ping-pong buffers
 *    - Double/triple buffering patterns
 *
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

/* ============================================================================
 * UTILITY FUNCTIONS
 * ============================================================================ */

void printArray(int arr[], int size) {
    printf("[");
    for (int i = 0; i < size; i++) {
        printf("%d", arr[i]);
        if (i < size - 1) printf(", ");
    }
    printf("]\n");
}

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/* ============================================================================
 * 1. TWO SUM
 * Time: O(n²) naive, O(n) with hash map
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * Find two numbers in array that add up to target.
 * Return their indices.
 *
 * Example: arr = [2, 7, 11, 15], target = 9
 * Answer: [0, 1] because arr[0] + arr[1] = 2 + 7 = 9
 *
 * NAIVE APPROACH (shown here): Try all pairs - O(n²)
 * OPTIMAL APPROACH: Use hash map to check if complement exists - O(n)
 *
 * EMBEDDED NOTE:
 * Hash maps require dynamic memory. In embedded, sometimes the O(n²)
 * solution with no extra memory is preferred if n is small.
 * ============================================================================ */

bool twoSum(int arr[], int size, int target, int* idx1, int* idx2) {
    // Naive O(n²) approach - no extra memory needed
    for (int i = 0; i < size; i++) {
        for (int j = i + 1; j < size; j++) {
            if (arr[i] + arr[j] == target) {
                *idx1 = i;
                *idx2 = j;
                return true;
            }
        }
    }
    return false;
}

void demo_twoSum(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|                    1. TWO SUM                                      |\n");
    printf("+====================================================================+\n");

    int arr[] = {2, 7, 11, 15};
    int size = 4;
    int target = 9;

    printf("\nArray: ");
    printArray(arr, size);
    printf("Target sum: %d\n", target);

    int idx1, idx2;
    if (twoSum(arr, size, target, &idx1, &idx2)) {
        printf("Found: arr[%d] + arr[%d] = %d + %d = %d\n",
               idx1, idx2, arr[idx1], arr[idx2], target);
    } else {
        printf("No pair found\n");
    }

    printf("\n[INTERVIEW TIP]\n");
    printf("  Mention O(n) solution with hash map\n");
    printf("  But explain trade-off: extra O(n) space\n");
    printf("  In embedded, O(n²) might be acceptable for small n\n");
}

/* ============================================================================
 * 2. MAXIMUM SUBARRAY (Kadane's Algorithm)
 * Time: O(n), Space: O(1)
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * Find contiguous subarray with largest sum.
 *
 * Example: arr = [-2, 1, -3, 4, -1, 2, 1, -5, 4]
 * Answer: 6 (subarray [4, -1, 2, 1])
 *
 * KADANE'S ALGORITHM:
 * At each position, decide:
 *   - Start new subarray here? (just current element)
 *   - Or extend previous subarray? (current + previous sum)
 * Pick whichever is larger!
 *
 * VISUAL:
 *   arr:         [-2,  1, -3,  4, -1,  2,  1, -5,  4]
 *   current_sum: [-2,  1, -2,  4,  3,  5,  6,  1,  5]
 *   max_sum:     [-2,  1,  1,  4,  4,  5,  6,  6,  6]
 *                                          ^
 *                                       answer!
 *
 * EMBEDDED USE CASE:
 * Signal processing - find peak signal region in noisy data
 * ============================================================================ */

int maxSubarray(int arr[], int size) {
    if (size == 0) return 0;

    int maxSum = arr[0];      // Best sum found so far
    int currentSum = arr[0];  // Sum of current subarray

    for (int i = 1; i < size; i++) {
        // Either start fresh at arr[i], or extend previous subarray
        currentSum = (arr[i] > currentSum + arr[i]) ? arr[i] : currentSum + arr[i];

        // Update maximum if current is better
        if (currentSum > maxSum) {
            maxSum = currentSum;
        }
    }

    return maxSum;
}

void demo_maxSubarray(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|            2. MAXIMUM SUBARRAY (Kadane's Algorithm)                |\n");
    printf("+====================================================================+\n");

    int arr[] = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
    int size = 9;

    printf("\nArray: ");
    printArray(arr, size);

    int maxSum = maxSubarray(arr, size);
    printf("Maximum subarray sum: %d\n", maxSum);
    printf("(Subarray is [4, -1, 2, 1])\n");

    printf("\n[WHY KADANE'S ALGORITHM IS ELEGANT]\n");
    printf("  - Single pass through array: O(n)\n");
    printf("  - Only 2 variables needed: O(1) space\n");
    printf("  - Simple decision at each step\n");
}

/* ============================================================================
 * 3. MERGE TWO SORTED ARRAYS
 * Time: O(n+m), Space: O(1) if merging in-place
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * Given two sorted arrays, merge them into one sorted array.
 *
 * VISUAL:
 *   arr1: [1, 3, 5, _, _, _]  (has extra space)
 *   arr2: [2, 4, 6]
 *
 *   Result: [1, 2, 3, 4, 5, 6]
 *
 * TRICK FOR IN-PLACE:
 * Start from the END! Fill from largest to smallest.
 * This way we don't overwrite elements we still need.
 *
 * EMBEDDED USE CASE:
 * Merging sorted sensor readings from multiple sources
 * ============================================================================ */

void mergeSortedArrays(int arr1[], int m, int arr2[], int n) {
    // arr1 has enough space to hold both arrays
    // m = number of elements in arr1
    // n = number of elements in arr2

    int i = m - 1;      // Last element in arr1
    int j = n - 1;      // Last element in arr2
    int k = m + n - 1;  // Last position in merged array

    // Start from end, fill with largest elements first
    while (i >= 0 && j >= 0) {
        if (arr1[i] > arr2[j]) {
            arr1[k] = arr1[i];
            i--;
        } else {
            arr1[k] = arr2[j];
            j--;
        }
        k--;
    }

    // Copy remaining elements from arr2 (if any)
    while (j >= 0) {
        arr1[k] = arr2[j];
        j--;
        k--;
    }
    // Note: remaining arr1 elements are already in place
}

void demo_mergeSortedArrays(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|            3. MERGE TWO SORTED ARRAYS                              |\n");
    printf("+====================================================================+\n");

    int arr1[6] = {1, 3, 5};  // Extra space for merged result
    int arr2[] = {2, 4, 6};

    printf("\nBefore merge:\n");
    printf("arr1: [1, 3, 5, _, _, _]\n");
    printf("arr2: [2, 4, 6]\n");

    mergeSortedArrays(arr1, 3, arr2, 3);

    printf("\nAfter merge:\n");
    printf("arr1: ");
    printArray(arr1, 6);

    printf("\n[KEY INSIGHT]\n");
    printf("  Merge from END to avoid overwriting!\n");
    printf("  This is the trick for O(1) space.\n");
}

/* ============================================================================
 * 4. ROTATE ARRAY
 * Time: O(n), Space: O(1)
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * Rotate array by k positions.
 *
 * Example: arr = [1,2,3,4,5,6,7], k = 3
 * Result:  [5,6,7,1,2,3,4]
 *
 * THE REVERSAL TRICK:
 * Instead of moving elements one by one (O(n*k)):
 * 1. Reverse entire array:     [7,6,5,4,3,2,1]
 * 2. Reverse first k elements: [5,6,7,4,3,2,1]
 * 3. Reverse remaining:        [5,6,7,1,2,3,4]
 *
 * WHY IT WORKS:
 * Think of it like splitting a necklace and reattaching.
 *
 * EMBEDDED USE CASE:
 * Circular buffer manipulation, signal phase shifting
 * ============================================================================ */

void reverse(int arr[], int start, int end) {
    while (start < end) {
        swap(&arr[start], &arr[end]);
        start++;
        end--;
    }
}

void rotateArray(int arr[], int size, int k) {
    k = k % size;  // Handle k > size
    if (k == 0) return;

    reverse(arr, 0, size - 1);   // Reverse entire array
    reverse(arr, 0, k - 1);       // Reverse first k elements
    reverse(arr, k, size - 1);   // Reverse remaining elements
}

void demo_rotateArray(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|            4. ROTATE ARRAY                                         |\n");
    printf("+====================================================================+\n");

    int arr[] = {1, 2, 3, 4, 5, 6, 7};
    int size = 7;
    int k = 3;

    printf("\nOriginal: ");
    printArray(arr, size);
    printf("Rotate by k = %d\n", k);

    rotateArray(arr, size, k);

    printf("Result:   ");
    printArray(arr, size);

    printf("\n[THE REVERSAL TRICK]\n");
    printf("  1. Reverse all:    [7,6,5,4,3,2,1]\n");
    printf("  2. Reverse 0..k-1: [5,6,7,4,3,2,1]\n");
    printf("  3. Reverse k..n-1: [5,6,7,1,2,3,4]\n");
}

/* ============================================================================
 * 5. FIND DUPLICATE (Floyd's Cycle Detection)
 * Time: O(n), Space: O(1)
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * Array of n+1 integers where each integer is between 1 and n.
 * Exactly one number is repeated. Find it WITHOUT modifying array.
 *
 * THE TRICK:
 * Treat array values as "next pointers" like a linked list!
 * If arr[i] = j, then there's an arrow from index i to index j.
 * A duplicate means two indices point to same location = CYCLE!
 *
 * Use Floyd's tortoise and hare to find the cycle!
 *
 * VISUAL for arr = [1, 3, 4, 2, 2]:
 *   Index: 0  1  2  3  4
 *   Value: 1  3  4  2  2
 *
 *   0 -> 1 -> 3 -> 2 -> 4
 *             ^         |
 *             +---------+
 *   (Index 3 and 4 both point to index 2 = duplicate is 2)
 * ============================================================================ */

int findDuplicate(int arr[], int size) {
    // Phase 1: Find meeting point (like cycle detection)
    int slow = arr[0];
    int fast = arr[0];

    do {
        slow = arr[slow];           // Move 1 step
        fast = arr[arr[fast]];      // Move 2 steps
    } while (slow != fast);

    // Phase 2: Find cycle start (the duplicate)
    slow = arr[0];
    while (slow != fast) {
        slow = arr[slow];
        fast = arr[fast];
    }

    return slow;
}

void demo_findDuplicate(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|            5. FIND DUPLICATE (Floyd's Cycle)                       |\n");
    printf("+====================================================================+\n");

    int arr[] = {1, 3, 4, 2, 2};
    int size = 5;

    printf("\nArray: ");
    printArray(arr, size);

    int dup = findDuplicate(arr, size);
    printf("Duplicate number: %d\n", dup);

    printf("\n[THE INSIGHT]\n");
    printf("  Treat values as 'pointers' to next index.\n");
    printf("  Duplicate = two values pointing to same index = CYCLE!\n");
    printf("  Use Floyd's algorithm from linked list!\n");
}

/* ============================================================================
 * 6. MOVE ZEROES
 * Time: O(n), Space: O(1)
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * Move all zeros to the end while maintaining order of non-zero elements.
 *
 * Example: [0, 1, 0, 3, 12] -> [1, 3, 12, 0, 0]
 *
 * TWO POINTER TECHNIQUE:
 *   - 'write' pointer: where to place next non-zero
 *   - 'read' pointer: scanning through array
 *
 * VISUAL:
 *   [0, 1, 0, 3, 12]
 *    w  r              read=0 (zero, skip)
 *    w     r           read=1 (non-zero, write at w, w++)
 *   [1, 1, 0, 3, 12]
 *       w     r        read=0 (zero, skip)
 *       w        r     read=3 (non-zero, write at w, w++)
 *   [1, 3, 0, 3, 12]
 *          w        r  read=12 (non-zero, write at w, w++)
 *   [1, 3, 12, 3, 12]
 *              w       Fill remaining with zeros
 *   [1, 3, 12, 0, 0]
 * ============================================================================ */

void moveZeroes(int arr[], int size) {
    int writeIdx = 0;

    // Move all non-zero elements to front
    for (int readIdx = 0; readIdx < size; readIdx++) {
        if (arr[readIdx] != 0) {
            arr[writeIdx] = arr[readIdx];
            writeIdx++;
        }
    }

    // Fill remaining positions with zeros
    while (writeIdx < size) {
        arr[writeIdx] = 0;
        writeIdx++;
    }
}

void demo_moveZeroes(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|            6. MOVE ZEROES                                          |\n");
    printf("+====================================================================+\n");

    int arr[] = {0, 1, 0, 3, 12};
    int size = 5;

    printf("\nOriginal: ");
    printArray(arr, size);

    moveZeroes(arr, size);

    printf("After:    ");
    printArray(arr, size);

    printf("\n[TWO POINTER PATTERN]\n");
    printf("  'write' tracks where to put next non-zero\n");
    printf("  'read' scans through array\n");
    printf("  Very common pattern in array problems!\n");
}

/* ============================================================================
 * 7. CIRCULAR BUFFER (Ring Buffer)
 * CRITICAL FOR EMBEDDED SYSTEMS!
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * A fixed-size buffer that "wraps around" when it reaches the end.
 * Perfect for streaming data like UART, ADC samples, etc.
 *
 * VISUAL:
 *   Logical view:          Physical view (array):
 *       +---+                  [0][1][2][3][4][5][6][7]
 *      /     \                     H        T
 *     |       |                 H = head (read from here)
 *     |       |                 T = tail (write here)
 *      \     /
 *       +---+
 *
 * WHY CIRCULAR BUFFER IN EMBEDDED:
 *   1. Fixed memory - no malloc
 *   2. O(1) read and write
 *   3. Perfect for producer-consumer (ISR writes, main reads)
 *   4. No data loss if buffer sized correctly
 *
 * THE MODULO TRICK:
 *   next_index = (current_index + 1) % buffer_size
 *   This makes the index "wrap around" to 0 after reaching max.
 * ============================================================================ */

#define RING_BUFFER_SIZE 8

typedef struct {
    int buffer[RING_BUFFER_SIZE];
    int head;           // Read from here
    int tail;           // Write here
    int count;          // Number of elements
    bool overflow;      // Flag if data was lost
} RingBuffer;

void ring_init(RingBuffer* rb) {
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
    rb->overflow = false;
    memset(rb->buffer, 0, sizeof(rb->buffer));
}

bool ring_isEmpty(RingBuffer* rb) {
    return rb->count == 0;
}

bool ring_isFull(RingBuffer* rb) {
    return rb->count == RING_BUFFER_SIZE;
}

/*
 * ring_write - Add data to buffer
 *
 * In embedded, this is often called from ISR
 * Must be fast and non-blocking!
 */
bool ring_write(RingBuffer* rb, int data) {
    if (ring_isFull(rb)) {
        rb->overflow = true;
        return false;  // Buffer full, data lost!
    }

    rb->buffer[rb->tail] = data;
    rb->tail = (rb->tail + 1) % RING_BUFFER_SIZE;  // Wrap around!
    rb->count++;
    return true;
}

/*
 * ring_read - Remove and return data from buffer
 *
 * In embedded, this is called from main loop
 */
bool ring_read(RingBuffer* rb, int* data) {
    if (ring_isEmpty(rb)) {
        return false;  // Nothing to read
    }

    *data = rb->buffer[rb->head];
    rb->head = (rb->head + 1) % RING_BUFFER_SIZE;  // Wrap around!
    rb->count--;
    return true;
}

void ring_print(RingBuffer* rb) {
    printf("Ring Buffer (H=%d, T=%d, count=%d): [", rb->head, rb->tail, rb->count);
    for (int i = 0; i < RING_BUFFER_SIZE; i++) {
        if (i == rb->head && i == rb->tail && rb->count > 0) {
            printf("*%d*", rb->buffer[i]);  // Both head and tail
        } else if (i == rb->head && rb->count > 0) {
            printf("H%d", rb->buffer[i]);
        } else if (i == rb->tail) {
            printf("T%d", rb->buffer[i]);
        } else {
            printf("%d", rb->buffer[i]);
        }
        if (i < RING_BUFFER_SIZE - 1) printf(", ");
    }
    printf("]\n");
}

void demo_circularBuffer(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|            7. CIRCULAR BUFFER (Ring Buffer)                        |\n");
    printf("+====================================================================+\n");

    printf("\n[WHY CIRCULAR BUFFERS IN EMBEDDED?]\n");
    printf("  - Fixed memory size (no malloc)\n");
    printf("  - O(1) read and write operations\n");
    printf("  - Perfect for ISR -> main loop communication\n");
    printf("  - UART, ADC, DMA commonly use ring buffers\n");

    RingBuffer rb;
    ring_init(&rb);

    printf("\n[DEMO] Writing values 1-5\n");
    for (int i = 1; i <= 5; i++) {
        ring_write(&rb, i * 10);
        printf("  Write %d: ", i * 10);
        ring_print(&rb);
    }

    printf("\n[DEMO] Reading 3 values\n");
    for (int i = 0; i < 3; i++) {
        int data;
        if (ring_read(&rb, &data)) {
            printf("  Read %d: ", data);
            ring_print(&rb);
        }
    }

    printf("\n[DEMO] Writing more (demonstrating wrap-around)\n");
    for (int i = 6; i <= 10; i++) {
        bool ok = ring_write(&rb, i * 10);
        printf("  Write %d %s: ", i * 10, ok ? "OK" : "FULL");
        ring_print(&rb);
    }

    if (rb.overflow) {
        printf("\n  WARNING: Buffer overflow occurred! Data was lost.\n");
    }
}

/* ============================================================================
 * 8. LOOKUP TABLE
 * Common embedded optimization technique
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * Pre-compute values and store in array for O(1) access.
 * Trade memory for speed.
 *
 * EXAMPLES IN EMBEDDED:
 *   - Sine/cosine lookup tables (avoid expensive floating point)
 *   - CRC tables
 *   - LED brightness curves (gamma correction)
 *   - Sensor calibration tables
 *
 * TRADE-OFF:
 *   - More memory (table storage)
 *   - Faster execution (just array access)
 *   - Less accuracy (quantization) unless interpolated
 * ============================================================================ */

// Example: 8-bit sine lookup table (0-255 output for 0-360 degrees)
// Stored in ROM/Flash to save RAM
static const unsigned char SIN_TABLE[256] = {
    128, 131, 134, 137, 140, 143, 146, 149, 152, 155, 158, 161, 164, 167, 170, 173,
    176, 178, 181, 184, 187, 189, 192, 195, 197, 200, 202, 205, 207, 210, 212, 214,
    217, 219, 221, 223, 225, 227, 229, 231, 233, 235, 237, 238, 240, 241, 243, 244,
    246, 247, 248, 249, 250, 251, 252, 253, 254, 254, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 254, 254, 253, 252, 251, 250, 249, 248, 247, 246,
    244, 243, 241, 240, 238, 237, 235, 233, 231, 229, 227, 225, 223, 221, 219, 217,
    214, 212, 210, 207, 205, 202, 200, 197, 195, 192, 189, 187, 184, 181, 178, 176,
    173, 170, 167, 164, 161, 158, 155, 152, 149, 146, 143, 140, 137, 134, 131, 128,
    125, 122, 119, 116, 113, 110, 107, 104, 101,  98,  95,  92,  89,  86,  83,  80,
     78,  75,  72,  69,  67,  64,  61,  59,  56,  54,  51,  49,  46,  44,  42,  39,
     37,  35,  33,  31,  29,  27,  25,  23,  21,  19,  18,  16,  15,  13,  12,  10,
      9,   8,   7,   6,   5,   4,   3,   2,   2,   1,   1,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   1,   1,   2,   2,   3,   4,   5,   6,   7,   8,
      9,  10,  12,  13,  15,  16,  18,  19,  21,  23,  25,  27,  29,  31,  33,  35,
     37,  39,  42,  44,  46,  49,  51,  54,  56,  59,  61,  64,  67,  69,  72,  75,
     78,  80,  83,  86,  89,  92,  95,  98, 101, 104, 107, 110, 113, 116, 119, 122
};

// Get sine value (0-255) for angle (0-255 maps to 0-360 degrees)
unsigned char fastSin(unsigned char angle) {
    return SIN_TABLE[angle];  // O(1)!
}

// Get cosine value (sine shifted by 64 = 90 degrees)
unsigned char fastCos(unsigned char angle) {
    return SIN_TABLE[(unsigned char)(angle + 64)];  // Wrap-around automatic with unsigned char
}

void demo_lookupTable(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|            8. LOOKUP TABLE                                         |\n");
    printf("+====================================================================+\n");

    printf("\n[WHY LOOKUP TABLES IN EMBEDDED?]\n");
    printf("  - Avoid expensive math (sin, cos, sqrt)\n");
    printf("  - Deterministic timing (O(1))\n");
    printf("  - Common: CRC, sin/cos, gamma correction\n");

    printf("\n[DEMO] Sine lookup table\n");
    printf("  Angle 0   (0°):   sin = %d\n", fastSin(0));
    printf("  Angle 64  (90°):  sin = %d\n", fastSin(64));
    printf("  Angle 128 (180°): sin = %d\n", fastSin(128));
    printf("  Angle 192 (270°): sin = %d\n", fastSin(192));

    printf("\n[MEMORY VS SPEED TRADE-OFF]\n");
    printf("  Table size: %zu bytes\n", sizeof(SIN_TABLE));
    printf("  Access time: O(1) - just array lookup\n");
    printf("  vs computing sin(): O(many) - Taylor series\n");
}

/* ============================================================================
 * 9. BINARY SEARCH
 * Time: O(log n) for sorted arrays
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * Find element in SORTED array by repeatedly halving search space.
 *
 * VISUAL for finding 7 in [1, 3, 5, 7, 9, 11, 13]:
 *
 *   Step 1: mid = 7, target = 7, FOUND!
 *
 * For finding 3:
 *   Step 1: [1, 3, 5, 7, 9, 11, 13], mid = 7, 3 < 7, search left half
 *   Step 2: [1, 3, 5], mid = 3, FOUND!
 *
 * KEY INSIGHT:
 *   Each comparison eliminates HALF the remaining elements!
 *   For 1 million elements: only ~20 comparisons needed.
 *
 * COMMON BUG:
 *   mid = (low + high) / 2  // Can overflow!
 *   mid = low + (high - low) / 2  // Safe
 * ============================================================================ */

int binarySearch(int arr[], int size, int target) {
    int low = 0;
    int high = size - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;  // Avoid overflow!

        if (arr[mid] == target) {
            return mid;  // Found
        } else if (arr[mid] < target) {
            low = mid + 1;   // Search right half
        } else {
            high = mid - 1;  // Search left half
        }
    }

    return -1;  // Not found
}

void demo_binarySearch(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|            9. BINARY SEARCH                                        |\n");
    printf("+====================================================================+\n");

    int arr[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19};
    int size = 10;

    printf("\nSorted array: ");
    printArray(arr, size);

    int target = 7;
    int idx = binarySearch(arr, size, target);
    printf("Search for %d: found at index %d\n", target, idx);

    target = 8;
    idx = binarySearch(arr, size, target);
    printf("Search for %d: %s\n", target, idx == -1 ? "not found" : "found");

    printf("\n[COMPLEXITY]\n");
    printf("  Linear search: O(n) - check every element\n");
    printf("  Binary search: O(log n) - halve each time\n");
    printf("  For n=1,000,000: 1M checks vs ~20 checks!\n");

    printf("\n[COMMON BUG]\n");
    printf("  mid = (low + high) / 2  // OVERFLOW for large arrays!\n");
    printf("  mid = low + (high - low) / 2  // SAFE\n");
}

/* ============================================================================
 * MAIN
 * ============================================================================ */

int main(void) {
    printf("+====================================================================+\n");
    printf("|         ARRAYS - EMBEDDED SYSTEMS INTERVIEW GUIDE                  |\n");
    printf("+====================================================================+\n");

    demo_twoSum();
    demo_maxSubarray();
    demo_mergeSortedArrays();
    demo_rotateArray();
    demo_findDuplicate();
    demo_moveZeroes();
    demo_circularBuffer();
    demo_lookupTable();
    demo_binarySearch();

    printf("\n");
    printf("+====================================================================+\n");
    printf("|                    SUMMARY                                         |\n");
    printf("+====================================================================+\n");
    printf("| Key techniques:                                                    |\n");
    printf("|   - Two pointers (move zeroes, merge, etc.)                        |\n");
    printf("|   - Binary search (O(log n) for sorted)                           |\n");
    printf("|   - Kadane's algorithm (max subarray)                              |\n");
    printf("|   - Reversal trick (rotate array)                                  |\n");
    printf("|                                                                    |\n");
    printf("| Embedded-specific:                                                 |\n");
    printf("|   - Circular buffers (UART, ADC, DMA)                              |\n");
    printf("|   - Lookup tables (avoid expensive math)                           |\n");
    printf("|   - Static allocation (no malloc)                                  |\n");
    printf("+====================================================================+\n");

    return 0;
}
