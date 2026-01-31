/*
================================================================================
MERGE INTERVALS - NO HEAP USAGE VERSION
================================================================================

PROBLEM STATEMENT:
Merge overlapping intervals without using dynamic memory allocation (malloc/free).
This version modifies the input array IN-PLACE.

ALGORITHM APPROACH:
1. Sort intervals in-place by start time
2. Merge overlapping intervals in-place using two-pointer technique
3. Return the input array with merged intervals at the beginning

KEY DIFFERENCE FROM HEAP VERSION:
- NO malloc() or free() calls
- Modifies input array directly (in-place algorithm)
- More memory-efficient for embedded systems or constrained environments
- MISRA C friendly (no dynamic allocation)

TIME COMPLEXITY: O(n log n) - dominated by sorting
SPACE COMPLEXITY: O(1) - only uses constant extra space (no heap allocation)

ADVANTAGES:
- Zero heap allocation
- Deterministic memory usage
- Suitable for embedded systems
- MISRA C compliant
- No risk of memory leaks

LIMITATIONS:
- Modifies original input array
- Caller cannot free individual intervals
- Less flexible than heap-based version

================================================================================
*/

#include <stdio.h>
#include <stdlib.h>

/*
 * Comparison Function for qsort() - Pointer-to-Pointer Version
 *
 * PURPOSE:
 * Compares two interval pointers for sorting the intervals array.
 * Since intervals is int**, each element is a pointer to int[2].
 *
 * SORTING RULES:
 * 1. Primary: Sort by start time (ascending)
 * 2. Tie-breaker: If start times equal, sort by end time (ascending)
 *
 * PARAMETERS:
 * @param a - Pointer to an element in intervals array (type: void*)
 * @param b - Pointer to another element in intervals array (type: void*)
 *
 * RETURN VALUES:
 * -1 : interval 'a' should come before 'b'
 *  0 : intervals are equal
 *  1 : interval 'b' should come before 'a'
 *
 * ============================================================================
 * DETAILED EXPLANATION OF POINTER DEREFERENCING AND qsort()
 * ============================================================================
 *
 * HOW qsort() CALLS THIS FUNCTION:
 *
 * When we call: qsort(intervals, size, sizeof(int*), cmpIntPtrRows)
 *
 * qsort() passes POINTERS to the elements being compared.
 * Since each element in 'intervals' is already a pointer (int*),
 * we receive POINTERS TO POINTERS (int**).
 *
 * MEMORY LAYOUT:
 *
 * intervals array:    [ptr0] [ptr1] [ptr2] [ptr3]   <-- Array of int* pointers
 *                       |      |      |      |
 *                       v      v      v      v
 * Actual intervals:   [1,3] [2,6] [8,10] [15,18]   <-- Arrays of 2 ints
 *
 * When qsort compares intervals[0] and intervals[1]:
 * - It passes &intervals[0] and &intervals[1] to our function
 * - These are addresses of the pointers (type: int**)
 * - But qsort declares them as (void*) for genericity
 *
 * STEP-BY-STEP DEREFERENCING:
 *
 * Step 1: qsort calls cmpIntPtrRows(&intervals[0], &intervals[1])
 *         - a receives address of intervals[0]  (type: void*, really int**)
 *         - b receives address of intervals[1]  (type: void*, really int**)
 *
 * Step 2: Cast void* to int**
 *         (const int * const *)a
 *         This tells compiler: "a points to a const pointer to const int"
 *
 * Step 3: Dereference to get int*
 *         *(const int * const *)a
 *         This gives us intervals[0], which points to [1,3]
 *
 * VISUAL BREAKDOWN:
 *
 * void* a               -->  Cast to (int**)  -->  Dereference to get int*
 * points to             -->  interprets as    -->  accesses the value
 * &intervals[0]         -->  pointer to int*  -->  intervals[0] = ptr to [1,3]
 *
 * TYPE TRANSFORMATION:
 *
 * Original:  void* a
 *            ↓
 * Cast:      (const int * const *)a     // "pointer to (const pointer to const int)"
 *            ↓
 * Deref:     *(const int * const *)a    // "const pointer to const int" = int*
 *            ↓
 * Result:    const int *ra               // Points to first element of interval
 *
 * WHY const int * const *?
 *
 * - Outer const: We won't modify what ra points to (the interval values)
 * - Inner const: We won't modify the pointer itself
 * - This matches the type of intervals[i] which is a pointer to constant data
 *
 * EXAMPLE WITH ACTUAL VALUES:
 *
 * If intervals[0] points to array [1, 3] at address 0x1000:
 *
 * &intervals[0] = 0x5000  (address where intervals[0] is stored)
 * intervals[0]  = 0x1000  (value stored at intervals[0], points to [1,3])
 *
 * In comparison function:
 * a              = 0x5000  (passed by qsort)
 * (int**)a       = 0x5000  (cast to correct type)
 * *(int**)a      = 0x1000  (dereference: now we have intervals[0])
 * ra             = 0x1000  (points to [1,3])
 * ra[0]          = 1       (first element of interval)
 * ra[1]          = 3       (second element of interval)
 */
static int cmpIntPtrRows(const void *a, const void *b) {
    /*
     * COMPLEX POINTER DEREFERENCING EXPLAINED:
     *
     * The line below performs TWO operations:
     * 1. Cast: (const int * const *)a  - Reinterpret void* as int**
     * 2. Dereference: *(...) - Get the int* value stored at that address
     *
     * Why this complexity?
     * - qsort works with void* for genericity (works with any type)
     * - Our array is int** (array of pointers)
     * - qsort passes &intervals[i] (address of a pointer)
     * - We need to dereference to get intervals[i] (the pointer itself)
     */

    // Step 1: Cast void* to int**, then dereference to get int* (pointer to interval)
    const int *ra = *(const int * const *)a;  // row a: dereference to get int*
    const int *rb = *(const int * const *)b;  // row b: dereference to get int*

    /*
     * Now ra and rb point to the actual interval arrays:
     * ra[0] = start time of interval a
     * ra[1] = end time of interval a
     * rb[0] = start time of interval b
     * rb[1] = end time of interval b
     */

    // Compare start times first
    if (ra[0] < rb[0]) return -1;
    if (ra[0] > rb[0]) return  1;

    // Tie-break by end times
    if (ra[1] < rb[1]) return -1;
    if (ra[1] > rb[1]) return  1;

    return 0;  // Intervals are identical
}

/*
 * Merge Overlapping Intervals - IN-PLACE (No Heap Allocation)
 *
 * ALGORITHM OVERVIEW:
 * This function merges overlapping intervals WITHOUT using malloc/free.
 * It modifies the input array in-place using a two-pointer technique.
 *
 * PHASE 1: Sort intervals by start time (in-place)
 * PHASE 2: Merge using write/read pointers
 *   - 'write' points to last merged interval
 *   - 'read' scans through remaining intervals
 *   - Overlapping intervals extend the 'write' interval
 *   - Non-overlapping intervals advance 'write' and copy
 *
 * PARAMETERS:
 * @param intervals_rows    - Number of intervals in input
 * @param intervals_columns - Should always be 2 (start, end)
 * @param intervals         - 2D array of intervals (MODIFIED IN-PLACE)
 * @param result_rows       - Output: number of merged intervals
 * @param result_columns    - Output: always 2 (start, end)
 *
 * RETURN VALUE:
 * - Returns the same pointer as 'intervals' (not a new allocation)
 * - Merged intervals are stored in intervals[0..result_rows-1]
 * - Remaining intervals (intervals[result_rows..intervals_rows-1]) are undefined
 * - Returns NULL on error
 *
 * IMPORTANT NOTES:
 * - INPUT ARRAY IS MODIFIED - original data is lost
 * - No malloc() or free() calls
 * - Merged result occupies beginning of input array
 * - Caller should NOT free individual intervals after calling this
 */
int** mergeHighDefinitionIntervals(int intervals_rows,
                                  int intervals_columns,
                                  int** intervals,
                                  int* result_rows,
                                  int* result_columns) {
    // Initialize output parameters
    *result_rows = 0;
    *result_columns = 0;

    // Validate input
    if (intervals_rows <= 0 || intervals_columns != 2 || intervals == NULL) {
        return NULL;
    }

    // PHASE 1: SORT INTERVALS IN-PLACE
    // Sort the array of pointers by comparing the intervals they point to
    /*
     * qsort() CALL EXPLANATION:
     *
     * qsort(intervals, intervals_rows, sizeof(int*), cmpIntPtrRows)
     *
     * Parameter 1: intervals
     *   - Base address of array to sort
     *   - Type: int** (array of pointers to int[2])
     *
     * Parameter 2: intervals_rows
     *   - Number of elements in the array
     *   - Each element is one interval (pointer to int[2])
     *
     * Parameter 3: sizeof(int*)
     *   - Size of EACH ELEMENT in the array
     *   - Since each element is a pointer (int*), we use sizeof(int*)
     *   - NOT sizeof(int[2]) because we're sorting the POINTERS, not the arrays
     *
     * Parameter 4: cmpIntPtrRows
     *   - Comparison function
     *   - qsort will call this with &intervals[i] and &intervals[j]
     *   - These are addresses of pointers (type: int**)
     *   - Our function casts them and dereferences to get the interval data
     *
     * HOW qsort WORKS:
     *
     * 1. qsort looks at intervals as a byte array
     * 2. It divides it into chunks of size sizeof(int*) = 8 bytes (on 64-bit)
     * 3. To compare chunk i and chunk j:
     *    - It calls: cmpIntPtrRows(&intervals[i], &intervals[j])
     *    - Passes addresses of the pointers being compared
     * 4. Based on return value (-1, 0, 1), it reorders the pointers
     * 5. This reordering happens IN-PLACE (no heap allocation)
     *
     * MEMORY BEFORE SORT:
     * intervals: [ptr_to_[15,18]] [ptr_to_[8,10]] [ptr_to_[2,6]] [ptr_to_[1,3]]
     *
     * MEMORY AFTER SORT (pointers reordered):
     * intervals: [ptr_to_[1,3]] [ptr_to_[2,6]] [ptr_to_[8,10]] [ptr_to_[15,18]]
     *
     * The actual interval arrays [1,3], [2,6], etc. STAY IN PLACE.
     * Only the POINTERS to them get reordered.
     */
    qsort(intervals, (size_t)intervals_rows, sizeof(int*), cmpIntPtrRows);

    // PHASE 2: MERGE IN-PLACE USING TWO-POINTER TECHNIQUE
    /*
     * TWO-POINTER APPROACH:
     * 'write' - index of last merged interval (slow pointer)
     * 'read'  - index of current interval being processed (fast pointer)
     *
     * Example:
     * Input (sorted): [[1,3], [2,6], [8,10], [15,18]]
     *
     * Initial: write=0 (pointing to [1,3])
     *
     * read=1: [2,6]
     *   - 2 <= 3 (overlap) → extend write: [1,6]
     *   - write stays at 0
     *
     * read=2: [8,10]
     *   - 8 > 6 (no overlap) → advance write to 1
     *   - Copy [8,10] to intervals[1]
     *
     * read=3: [15,18]
     *   - 15 > 10 (no overlap) → advance write to 2
     *   - Copy [15,18] to intervals[2]
     *
     * Result: [[1,6], [8,10], [15,18]] (write+1 = 3 intervals)
     */
    int write = 0; // Index of last merged interval

    // Process each interval starting from second one
    for (int read = 1; read < intervals_rows; read++) {
        int curS = intervals[read][0];  // Current interval start
        int curE = intervals[read][1];  // Current interval end

        int lastE = intervals[write][1];  // Last merged interval's end

        if (curS <= lastE) {
            // CASE 1: OVERLAP
            // Current interval overlaps with last merged interval
            // Extend the end of last merged interval if needed
            if (curE > lastE) {
                intervals[write][1] = curE;
            }
            // Note: We don't advance 'write' - keep merging into same interval
        } else {
            // CASE 2: NO OVERLAP
            // Current interval doesn't overlap - start a new merged interval
            ++write;  // Move to next slot
            intervals[write] = intervals[read];  // Copy pointer to current interval
        }
    }

    // Set output parameters
    // Number of merged intervals = write index + 1
    *result_rows = write + 1;
    *result_columns = 2;

    // Return the same input array (now with merged intervals at the beginning)
    return intervals;
}

/*
 * Print Result - Display Merged Intervals
 *
 * PURPOSE:
 * Pretty-prints the merged intervals
 *
 * PARAMETERS:
 * @param result   - 2D array of merged intervals
 * @param rows     - Number of intervals
 * @param testName - Description of test case
 */
void printResult(int** result, int rows, const char* testName) {
    printf("\n%s:\n", testName);

    if (!result || rows == 0) {
        printf("  []\n");
        return;
    }

    printf("  [");
    for (int i = 0; i < rows; i++) {
        printf("[%d, %d]", result[i][0], result[i][1]);
        if (i < rows - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}

/*
 * Main Function - Comprehensive Test Suite
 *
 * PURPOSE:
 * Demonstrates the heap-free merge intervals algorithm with test cases
 *
 * NOTE:
 * Since this version modifies input in-place, each test uses static arrays.
 * After merging, the original input data is lost.
 */
int main(void) {
    int** result;
    int result_rows, result_columns;

    printf("=== MERGE INTERVALS - NO HEAP USAGE TESTS ===\n");

    /*
     * TEST 1: EMPTY ARRAY
     * Input: []
     * Expected: []
     */
    result = mergeHighDefinitionIntervals(0, 2, NULL, &result_rows, &result_columns);
    printResult(result, result_rows, "Test 1: Empty array");

    /*
     * TEST 2: SINGLE INTERVAL
     * Input: [[1,5]]
     * Expected: [[1,5]]
     */
    int row2_0[] = {1, 5};
    int* test2[] = {row2_0};
    result = mergeHighDefinitionIntervals(1, 2, test2, &result_rows, &result_columns);
    printResult(result, result_rows, "Test 2: Single interval [[1,5]]");

    /*
     * TEST 3: TWO OVERLAPPING INTERVALS
     * Input: [[1,3], [2,6]]
     * Expected: [[1,6]]
     */
    int row3_0[] = {1, 3};
    int row3_1[] = {2, 6};
    int* test3[] = {row3_0, row3_1};
    result = mergeHighDefinitionIntervals(2, 2, test3, &result_rows, &result_columns);
    printResult(result, result_rows, "Test 3: Two overlapping [[1,3], [2,6]]");

    /*
     * TEST 4: TWO NON-OVERLAPPING INTERVALS
     * Input: [[1,2], [5,10]]
     * Expected: [[1,2], [5,10]]
     */
    int row4_0[] = {1, 2};
    int row4_1[] = {5, 10};
    int* test4[] = {row4_0, row4_1};
    result = mergeHighDefinitionIntervals(2, 2, test4, &result_rows, &result_columns);
    printResult(result, result_rows, "Test 4: Two non-overlapping [[1,2], [5,10]]");

    /*
     * TEST 5: ALL INTERVALS MERGE INTO ONE
     * Input: [[1,4], [2,5], [3,6], [4,10]]
     * Expected: [[1,10]]
     */
    int row5_0[] = {1, 4};
    int row5_1[] = {2, 5};
    int row5_2[] = {3, 6};
    int row5_3[] = {4, 10};
    int* test5[] = {row5_0, row5_1, row5_2, row5_3};
    result = mergeHighDefinitionIntervals(4, 2, test5, &result_rows, &result_columns);
    printResult(result, result_rows, "Test 5: All merge into one [[1,4],[2,5],[3,6],[4,10]]");

    /*
     * TEST 6: NO OVERLAPPING INTERVALS
     * Input: [[1,2], [3,4], [5,6], [7,8]]
     * Expected: [[1,2], [3,4], [5,6], [7,8]]
     */
    int row6_0[] = {1, 2};
    int row6_1[] = {3, 4};
    int row6_2[] = {5, 6};
    int row6_3[] = {7, 8};
    int* test6[] = {row6_0, row6_1, row6_2, row6_3};
    result = mergeHighDefinitionIntervals(4, 2, test6, &result_rows, &result_columns);
    printResult(result, result_rows, "Test 6: No overlapping [[1,2],[3,4],[5,6],[7,8]]");

    /*
     * TEST 7: DUPLICATE INTERVALS
     * Input: [[1,5], [1,5], [1,5]]
     * Expected: [[1,5]]
     */
    int row7_0[] = {1, 5};
    int row7_1[] = {1, 5};
    int row7_2[] = {1, 5};
    int* test7[] = {row7_0, row7_1, row7_2};
    result = mergeHighDefinitionIntervals(3, 2, test7, &result_rows, &result_columns);
    printResult(result, result_rows, "Test 7: Duplicate intervals [[1,5],[1,5],[1,5]]");

    /*
     * TEST 8: CONTAINED INTERVAL
     * Input: [[1,10], [3,5]]
     * Expected: [[1,10]]
     */
    int row8_0[] = {1, 10};
    int row8_1[] = {3, 5};
    int* test8[] = {row8_0, row8_1};
    result = mergeHighDefinitionIntervals(2, 2, test8, &result_rows, &result_columns);
    printResult(result, result_rows, "Test 8: Contained interval [[1,10], [3,5]]");

    /*
     * TEST 9: ADJACENT INTERVALS (TOUCHING)
     * Input: [[1,3], [3,5], [5,10]]
     * Expected: [[1,10]]
     */
    int row9_0[] = {1, 3};
    int row9_1[] = {3, 5};
    int row9_2[] = {5, 10};
    int* test9[] = {row9_0, row9_1, row9_2};
    result = mergeHighDefinitionIntervals(3, 2, test9, &result_rows, &result_columns);
    printResult(result, result_rows, "Test 9: Adjacent intervals [[1,3],[3,5],[5,10]]");

    /*
     * TEST 10: REVERSE SORTED INTERVALS
     * Input: [[15,18], [8,10], [2,6], [1,3]]
     * Expected: [[1,6], [8,10], [15,18]]
     */
    int row10_0[] = {15, 18};
    int row10_1[] = {8, 10};
    int row10_2[] = {2, 6};
    int row10_3[] = {1, 3};
    int* test10[] = {row10_0, row10_1, row10_2, row10_3};
    result = mergeHighDefinitionIntervals(4, 2, test10, &result_rows, &result_columns);
    printResult(result, result_rows, "Test 10: Reverse sorted [[15,18],[8,10],[2,6],[1,3]]");

    /*
     * TEST 11: SAME START, DIFFERENT ENDS
     * Input: [[1,5], [1,10], [1,3]]
     * Expected: [[1,10]]
     */
    int row11_0[] = {1, 5};
    int row11_1[] = {1, 10};
    int row11_2[] = {1, 3};
    int* test11[] = {row11_0, row11_1, row11_2};
    result = mergeHighDefinitionIntervals(3, 2, test11, &result_rows, &result_columns);
    printResult(result, result_rows, "Test 11: Same start, different ends [[1,5],[1,10],[1,3]]");

    /*
     * TEST 12: LARGE VALUES (NEAR 10^9)
     * Input: [[1000000000,1000000001], [999999999,1000000000], [1000000002,1000000005]]
     * Expected: [[999999999,1000000001], [1000000002,1000000005]]
     */
    int row12_0[] = {1000000000, 1000000001};
    int row12_1[] = {999999999, 1000000000};
    int row12_2[] = {1000000002, 1000000005};
    int* test12[] = {row12_0, row12_1, row12_2};
    result = mergeHighDefinitionIntervals(3, 2, test12, &result_rows, &result_columns);
    printResult(result, result_rows, "Test 12: Large values near 10^9");

    /*
     * TEST 13: ZERO VALUES
     * Input: [[0,5], [3,10]]
     * Expected: [[0,10]]
     */
    int row13_0[] = {0, 5};
    int row13_1[] = {3, 10};
    int* test13[] = {row13_0, row13_1};
    result = mergeHighDefinitionIntervals(2, 2, test13, &result_rows, &result_columns);
    printResult(result, result_rows, "Test 13: Zero start value [[0,5],[3,10]]");

    /*
     * TEST 14: PROBLEM EXAMPLE
     * Input: [[1,3], [2,6], [8,10], [15,18]]
     * Expected: [[1,6], [8,10], [15,18]]
     */
    int row14_0[] = {1, 3};
    int row14_1[] = {2, 6};
    int row14_2[] = {8, 10};
    int row14_3[] = {15, 18};
    int* test14[] = {row14_0, row14_1, row14_2, row14_3};
    result = mergeHighDefinitionIntervals(4, 2, test14, &result_rows, &result_columns);
    printResult(result, result_rows, "Test 14: Problem example [[1,3],[2,6],[8,10],[15,18]]");

    /*
     * TEST 15: MULTIPLE CONTAINED INTERVALS
     * Input: [[1,20], [3,5], [7,9], [10,15], [2,4]]
     * Expected: [[1,20]]
     */
    int row15_0[] = {1, 20};
    int row15_1[] = {3, 5};
    int row15_2[] = {7, 9};
    int row15_3[] = {10, 15};
    int row15_4[] = {2, 4};
    int* test15[] = {row15_0, row15_1, row15_2, row15_3, row15_4};
    result = mergeHighDefinitionIntervals(5, 2, test15, &result_rows, &result_columns);
    printResult(result, result_rows, "Test 15: Multiple contained [[1,20],[3,5],[7,9],[10,15],[2,4]]");

    printf("\n=== ALL TESTS COMPLETED ===\n");
    printf("\nKEY ADVANTAGES OF THIS VERSION:\n");
    printf("- Zero heap allocation (no malloc/free)\n");
    printf("- O(1) space complexity\n");
    printf("- MISRA C compliant\n");
    printf("- Suitable for embedded systems\n");
    printf("- No risk of memory leaks\n");

    return 0;
}