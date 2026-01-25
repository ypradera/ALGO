/*
================================================================================
MERGE AND SORT INTERVALS - COMPREHENSIVE EXPLANATION
================================================================================

PROBLEM STATEMENT:
Given an array of intervals [startTime, endTime], merge all overlapping
intervals and return a sorted array of non-overlapping intervals.

ALGORITHM APPROACH:
1. Sort all intervals by start time (and by end time if start times are equal)
2. Iterate through sorted intervals and merge overlapping ones
3. Two intervals [a,b] and [c,d] overlap if: c <= b (second starts before first ends)

TIME COMPLEXITY: O(n log n) - dominated by sorting
SPACE COMPLEXITY: O(n) - for storing result and temporary sorting array

EXAMPLE WALKTHROUGH:
Input:  [[1,3], [2,6], [8,10], [15,18]]

Step 1 - Sort by start time:
   Already sorted: [[1,3], [2,6], [8,10], [15,18]]

Step 2 - Initialize with first interval:
   Result: [[1,3]]

Step 3 - Process [2,6]:
   Compare: Does 2 <= 3? YES → They overlap!
   Merge: [1, max(3,6)] = [1,6]
   Result: [[1,6]]

Step 4 - Process [8,10]:
   Compare: Does 8 <= 6? NO → No overlap
   Append: [8,10]
   Result: [[1,6], [8,10]]

Step 5 - Process [15,18]:
   Compare: Does 15 <= 10? NO → No overlap
   Append: [15,18]
   Result: [[1,6], [8,10], [15,18]]

EDGE CASES HANDLED:
- Empty input
- Single interval
- All intervals merge into one
- No overlapping intervals
- Duplicate intervals
- Adjacent/touching intervals (treated as overlapping)
- Intervals with same start but different ends
- Very large values (up to 10^9)

INPUT FORMAT:
- First line: number of intervals
- Second line: length of individual interval array (always 2)
- Next N lines: two integers representing [start, end]

CONSTRAINTS:
- 0 <= intervals.length <= 100000
- intervals[i].length == 2
- 0 <= intervals[i][0] < intervals[i][1] <= 10^9

OUTPUT FORMAT:
- 2D array of merged intervals sorted by start time
================================================================================
*/

#include <stdio.h>
#include <stdlib.h>

/*
 * Interval Structure
 * Represents a single time interval with start and end points
 */
typedef struct {
    int start;  // Start time of the interval
    int end;    // End time of the interval
} Interval;

/*
 * Comparison Function for qsort()
 *
 * PURPOSE:
 * Defines the sorting order for intervals. Used by qsort() to arrange
 * intervals in ascending order.
 *
 * SORTING RULES:
 * 1. Primary: Sort by start time (ascending)
 * 2. Tie-breaker: If start times are equal, sort by end time (ascending)
 *
 * PARAMETERS:
 * @param a - Pointer to first interval
 * @param b - Pointer to second interval
 *
 * RETURN VALUES:
 * -1 : interval 'a' should come before 'b'
 *  0 : intervals are equal (same start and end)
 *  1 : interval 'b' should come before 'a'
 *
 * EXAMPLES:
 * [1,5] vs [2,6] → -1 (1 < 2, so [1,5] comes first)
 * [1,5] vs [1,3] → 1  (same start, but 5 > 3, so [1,3] comes first)
 * [1,5] vs [1,5] → 0  (identical intervals)
 */
static int cmpInterval(const void *a, const void *b) {
    const Interval *ia = (const Interval *)a;
    const Interval *ib = (const Interval *)b;

    // Compare start times first
    if (ia->start < ib->start) return -1;
    if (ia->start > ib->start) return  1;

    // If start times are equal, compare end times (tie-breaker)
    if (ia->end < ib->end) return -1;
    if (ia->end > ib->end) return  1;

    // Intervals are identical
    return 0;
}

/*
 * Merge Overlapping Intervals - Main Function
 *
 * ALGORITHM OVERVIEW:
 * This function takes a 2D array of intervals and merges all overlapping ones.
 * It uses a three-phase approach:
 *   Phase 1: Sort intervals by start time
 *   Phase 2: Merge overlapping intervals
 *   Phase 3: Return merged result
 *
 * PARAMETERS:
 * @param intervals_rows    - Number of intervals in input
 * @param intervals_columns - Should always be 2 (start, end)
 * @param intervals         - 2D array of intervals [[start, end], ...]
 * @param result_rows       - Output: number of merged intervals
 * @param result_columns    - Output: always 2 (start, end)
 *
 * RETURN VALUE:
 * - Dynamically allocated 2D array of merged intervals
 * - Caller is responsible for freeing memory
 * - Returns NULL on error or invalid input
 *
 * MEMORY MANAGEMENT:
 * - Uses malloc() for dynamic allocation
 * - Caller must call freeResult() to avoid memory leaks
 * - Properly cleans up on allocation failures
 */
int** mergeHighDefinitionIntervals(int intervals_rows,
                                  int intervals_columns,
                                  int** intervals,
                                  int* result_rows,
                                  int* result_columns) {
    // Initialize output parameters to safe defaults
    *result_rows = 0;
    *result_columns = 0;

    // VALIDATION: Check for invalid input
    // - Empty input (rows <= 0)
    // - Invalid format (columns != 2)
    // - NULL pointer
    if (intervals_rows <= 0 || intervals_columns != 2 || intervals == NULL) {
        return NULL;
    }

    // PHASE 1: CONVERT TO SORTABLE FORMAT
    // Copy 2D array into struct array for easier manipulation
    // This allows us to use qsort() with a custom comparator
    Interval *arr = (Interval*)malloc((size_t)intervals_rows * sizeof(Interval));
    if (!arr) return NULL;  // Allocation failed

    // Transfer data from input 2D array to struct array
    for (int i = 0; i < intervals_rows; i++) {
        arr[i].start = intervals[i][0];
        arr[i].end   = intervals[i][1];
    }

    // PHASE 2: SORT INTERVALS
    // Sort by start time (and end time as tie-breaker)
    // This makes merging straightforward - we only need to look at adjacent intervals
    qsort(arr, (size_t)intervals_rows, sizeof(Interval), cmpInterval);

    // PHASE 3: ALLOCATE OUTPUT ARRAY
    // Worst case: no intervals overlap, so we need space for all N intervals
    int **out = (int**)malloc((size_t)intervals_rows * sizeof(int*));
    if (!out) {
        free(arr);  // Clean up before returning
        return NULL;
    }

    // PHASE 4: INITIALIZE WITH FIRST INTERVAL
    // The first interval (after sorting) always goes into the result
    int outCount = 0;
    out[outCount] = (int*)malloc(2 * sizeof(int));
    if (!out[outCount]) {
        free(out);
        free(arr);
        return NULL;
    }
    out[outCount][0] = arr[0].start;
    out[outCount][1] = arr[0].end;
    outCount++;

    // PHASE 5: MERGE PASS
    // Process remaining intervals one by one
    // Since intervals are sorted, we only need to check if current overlaps with last merged
    for (int i = 1; i < intervals_rows; i++) {
        int curS = arr[i].start;  // Current interval start
        int curE = arr[i].end;    // Current interval end

        int lastE = out[outCount - 1][1];  // Last merged interval's end

        /*
         * OVERLAP CHECK: curS <= lastE
         *
         * Two intervals overlap if:
         *   [lastS, lastE] and [curS, curE]
         *   where curS <= lastE
         *
         * Examples:
         *   [1,5] and [3,7]  → 3 <= 5 → OVERLAP → merge to [1,7]
         *   [1,5] and [5,8]  → 5 <= 5 → OVERLAP → merge to [1,8] (touching is overlap)
         *   [1,5] and [6,9]  → 6 <= 5 → NO OVERLAP → keep both
         */
        if (curS <= lastE) {
            // CASE 1: INTERVALS OVERLAP
            // Extend the end of last interval if current extends further
            // We don't need to update start because intervals are sorted

            /*
             * Why check if curE > lastE?
             * Consider: [1,10] and [3,5]
             *   - They overlap (3 <= 10)
             *   - But [3,5] is fully contained in [1,10]
             *   - No need to update end (5 < 10)
             * Compare: [1,5] and [3,10]
             *   - They overlap (3 <= 5)
             *   - [3,10] extends beyond [1,5]
             *   - Update end to 10
             */
            if (curE > lastE) {
                out[outCount - 1][1] = curE;
            }
            // If curE <= lastE, current interval is fully contained, do nothing
        } else {
            // CASE 2: NO OVERLAP
            // Add current interval as a new separate interval
            out[outCount] = (int*)malloc(2 * sizeof(int));
            if (!out[outCount]) {
                // Memory allocation failed - cleanup all previous allocations
                for (int k = 0; k < outCount; k++) {
                    free(out[k]);
                }
                free(out);
                free(arr);
                return NULL;
            }
            out[outCount][0] = curS;
            out[outCount][1] = curE;
            outCount++;
        }
    }

    // PHASE 6: CLEANUP AND RETURN
    free(arr);  // No longer need the temporary sorted array

    // Set output parameters
    *result_rows = outCount;
    *result_columns = 2;

    return out;  // Caller must free this memory
}

/*
 * Print Result - Display Merged Intervals
 *
 * PURPOSE:
 * Pretty-prints the merged intervals in a readable format
 *
 * OUTPUT FORMAT:
 * Test Name:
 *   [[start1, end1], [start2, end2], ...]
 *
 * PARAMETERS:
 * @param result   - 2D array of merged intervals
 * @param rows     - Number of intervals
 * @param cols     - Number of columns (always 2, but kept for compatibility)
 * @param testName - Description of the test case
 */
void printResult(int** result, int rows, int cols, const char* testName) {
    printf("\n%s:\n", testName);

    // Handle empty result
    if (!result || rows == 0) {
        printf("  []\n");
        return;
    }

    // Print intervals in array format
    printf("  [");
    for (int i = 0; i < rows; i++) {
        printf("[%d, %d]", result[i][0], result[i][1]);
        if (i < rows - 1) {
            printf(", ");  // Comma separator between intervals
        }
    }
    printf("]\n");
}

/*
 * Free Result - Memory Cleanup
 *
 * PURPOSE:
 * Properly deallocates all memory used by the result array
 *
 * IMPORTANT:
 * - Must be called to prevent memory leaks
 * - Frees both the individual interval arrays and the outer array
 *
 * PARAMETERS:
 * @param result - 2D array returned by mergeHighDefinitionIntervals()
 * @param rows   - Number of intervals in the result
 *
 * MEMORY STRUCTURE:
 * result -> [ptr1, ptr2, ..., ptrN]  (outer array)
 *            |     |           |
 *            v     v           v
 *           [s,e] [s,e]      [s,e]    (inner arrays)
 *
 * We must free:
 * 1. Each inner array (interval)
 * 2. The outer array (pointer array)
 */
void freeResult(int** result, int rows) {
    if (!result) return;  // Nothing to free

    // Free each individual interval
    for (int i = 0; i < rows; i++) {
        free(result[i]);
    }

    // Free the outer array
    free(result);
}

/*
 * Main Function - Comprehensive Test Suite
 *
 * PURPOSE:
 * Demonstrates the merge intervals algorithm with 15 carefully chosen test cases
 * that cover all edge cases and common scenarios
 *
 * TEST CATEGORIES:
 * 1. Edge Cases: Empty, single interval, duplicates
 * 2. Merging Scenarios: All merge, none merge, partial merge
 * 3. Sorting Tests: Reverse sorted, same start times
 * 4. Boundary Values: Zero, large values (10^9)
 * 5. Special Cases: Adjacent, contained, overlapping
 */
int main(){
    int** result;          // Stores merged intervals
    int result_rows;       // Number of merged intervals
    int result_columns;    // Always 2 (start, end)

    printf("=== MERGE INTERVALS STRESS TESTS ===\n");

    /*
     * TEST 1: EMPTY ARRAY
     * Input: []
     * Expected: []
     * Purpose: Tests handling of empty input
     */
    result = mergeHighDefinitionIntervals(0, 2, NULL, &result_rows, &result_columns);
    printResult(result, result_rows, result_columns, "Test 1: Empty array");
    freeResult(result, result_rows);

    /*
     * TEST 2: SINGLE INTERVAL
     * Input: [[1,5]]
     * Expected: [[1,5]]
     * Purpose: Minimum valid input - nothing to merge
     */
    int** test2 = (int**)malloc(1 * sizeof(int*));
    test2[0] = (int*)malloc(2 * sizeof(int));
    test2[0][0] = 1; test2[0][1] = 5;
    result = mergeHighDefinitionIntervals(1, 2, test2, &result_rows, &result_columns);
    printResult(result, result_rows, result_columns, "Test 2: Single interval [[1,5]]");
    freeResult(result, result_rows);
    free(test2[0]);
    free(test2);

    /*
     * TEST 3: TWO OVERLAPPING INTERVALS
     * Input: [[1,3], [2,6]]
     * Expected: [[1,6]]
     * Explanation:
     *   - [1,3] and [2,6] overlap because 2 <= 3
     *   - Merged interval: [min(1,2), max(3,6)] = [1,6]
     * Purpose: Basic overlap test
     */
    int** test3 = (int**)malloc(2 * sizeof(int*));
    test3[0] = (int*)malloc(2 * sizeof(int));
    test3[1] = (int*)malloc(2 * sizeof(int));
    test3[0][0] = 1; test3[0][1] = 3;
    test3[1][0] = 2; test3[1][1] = 6;
    result = mergeHighDefinitionIntervals(2, 2, test3, &result_rows, &result_columns);
    printResult(result, result_rows, result_columns, "Test 3: Two overlapping [[1,3], [2,6]]");
    freeResult(result, result_rows);
    free(test3[0]);
    free(test3[1]);
    free(test3);

    /*
     * TEST 4: TWO NON-OVERLAPPING INTERVALS
     * Input: [[1,2], [5,10]]
     * Expected: [[1,2], [5,10]]
     * Explanation:
     *   - [1,2] and [5,10] don't overlap because 5 > 2
     *   - Gap between them: (2, 5)
     * Purpose: Tests that non-overlapping intervals stay separate
     */
    int** test4 = (int**)malloc(2 * sizeof(int*));
    test4[0] = (int*)malloc(2 * sizeof(int));
    test4[1] = (int*)malloc(2 * sizeof(int));
    test4[0][0] = 1; test4[0][1] = 2;
    test4[1][0] = 5; test4[1][1] = 10;
    result = mergeHighDefinitionIntervals(2, 2, test4, &result_rows, &result_columns);
    printResult(result, result_rows, result_columns, "Test 4: Two non-overlapping [[1,2], [5,10]]");
    freeResult(result, result_rows);
    free(test4[0]);
    free(test4[1]);
    free(test4);

    /*
     * TEST 5: ALL INTERVALS MERGE INTO ONE
     * Input: [[1,4], [2,5], [3,6], [4,10]]
     * Expected: [[1,10]]
     * Explanation:
     *   Step 1: [1,4] (initial)
     *   Step 2: [2,5] overlaps with [1,4] (2<=4) → [1,5]
     *   Step 3: [3,6] overlaps with [1,5] (3<=5) → [1,6]
     *   Step 4: [4,10] overlaps with [1,6] (4<=6) → [1,10]
     * Purpose: Tests cascading merges - multiple intervals combining into one
     */
    int** test5 = (int**)malloc(4 * sizeof(int*));
    for (int i = 0; i < 4; i++) {
        test5[i] = (int*)malloc(2 * sizeof(int));
    }
    test5[0][0] = 1; test5[0][1] = 4;
    test5[1][0] = 2; test5[1][1] = 5;
    test5[2][0] = 3; test5[2][1] = 6;
    test5[3][0] = 4; test5[3][1] = 10;
    result = mergeHighDefinitionIntervals(4, 2, test5, &result_rows, &result_columns);
    printResult(result, result_rows, result_columns, "Test 5: All merge into one [[1,4],[2,5],[3,6],[4,10]]");
    freeResult(result, result_rows);
    for (int i = 0; i < 4; i++) {
        free(test5[i]);
    }
    free(test5);

    // Test 6: No overlapping intervals
    int** test6 = (int**)malloc(4 * sizeof(int*));
    for (int i = 0; i < 4; i++) {
        test6[i] = (int*)malloc(2 * sizeof(int));
    }
    test6[0][0] = 1; test6[0][1] = 2;
    test6[1][0] = 3; test6[1][1] = 4;
    test6[2][0] = 5; test6[2][1] = 6;
    test6[3][0] = 7; test6[3][1] = 8;
    result = mergeHighDefinitionIntervals(4, 2, test6, &result_rows, &result_columns);
    printResult(result, result_rows, result_columns, "Test 6: No overlapping [[1,2],[3,4],[5,6],[7,8]]");
    freeResult(result, result_rows);
    for (int i = 0; i < 4; i++) {
        free(test6[i]);
    }
    free(test6);

    /*
     * TEST 7: DUPLICATE INTERVALS
     * Input: [[1,5], [1,5], [1,5]]
     * Expected: [[1,5]]
     * Explanation:
     *   - All three intervals are identical
     *   - They all merge into a single [1,5]
     * Purpose: Tests handling of duplicate input
     */
    int** test7 = (int**)malloc(3 * sizeof(int*));
    for (int i = 0; i < 3; i++) {
        test7[i] = (int*)malloc(2 * sizeof(int));
    }
    test7[0][0] = 1; test7[0][1] = 5;
    test7[1][0] = 1; test7[1][1] = 5;
    test7[2][0] = 1; test7[2][1] = 5;
    result = mergeHighDefinitionIntervals(3, 2, test7, &result_rows, &result_columns);
    printResult(result, result_rows, result_columns, "Test 7: Duplicate intervals [[1,5],[1,5],[1,5]]");
    freeResult(result, result_rows);
    for (int i = 0; i < 3; i++) {
        free(test7[i]);
    }
    free(test7);

    /*
     * TEST 8: CONTAINED INTERVAL
     * Input: [[1,10], [3,5]]
     * Expected: [[1,10]]
     * Explanation:
     *   - [3,5] is completely inside [1,10]
     *   - Since 3 <= 10 (overlap) and 5 <= 10 (no extension needed)
     *   - Result remains [1,10]
     * Purpose: Tests that contained intervals don't create new merged intervals
     * Visual: [----[1,10]----]
     *              [3,5]
     */
    int** test8 = (int**)malloc(2 * sizeof(int*));
    test8[0] = (int*)malloc(2 * sizeof(int));
    test8[1] = (int*)malloc(2 * sizeof(int));
    test8[0][0] = 1; test8[0][1] = 10;
    test8[1][0] = 3; test8[1][1] = 5;
    result = mergeHighDefinitionIntervals(2, 2, test8, &result_rows, &result_columns);
    printResult(result, result_rows, result_columns, "Test 8: Contained interval [[1,10], [3,5]]");
    freeResult(result, result_rows);
    free(test8[0]);
    free(test8[1]);
    free(test8);

    /*
     * TEST 9: ADJACENT INTERVALS (TOUCHING)
     * Input: [[1,3], [3,5], [5,10]]
     * Expected: [[1,10]]
     * Explanation:
     *   - [1,3] and [3,5]: 3 <= 3 → OVERLAP (touching counts as overlap)
     *   - Merged: [1,5]
     *   - [1,5] and [5,10]: 5 <= 5 → OVERLAP
     *   - Final: [1,10]
     * Purpose: Tests that touching intervals (end of one = start of next) are merged
     * Note: This implementation treats touching as overlapping (curS <= lastE)
     */
    int** test9 = (int**)malloc(3 * sizeof(int*));
    for (int i = 0; i < 3; i++) {
        test9[i] = (int*)malloc(2 * sizeof(int));
    }
    test9[0][0] = 1; test9[0][1] = 3;
    test9[1][0] = 3; test9[1][1] = 5;
    test9[2][0] = 5; test9[2][1] = 10;
    result = mergeHighDefinitionIntervals(3, 2, test9, &result_rows, &result_columns);
    printResult(result, result_rows, result_columns, "Test 9: Adjacent intervals [[1,3],[3,5],[5,10]]");
    freeResult(result, result_rows);
    for (int i = 0; i < 3; i++) {
        free(test9[i]);
    }
    free(test9);

    /*
     * TEST 10: REVERSE SORTED INTERVALS
     * Input: [[15,18], [8,10], [2,6], [1,3]]
     * Expected: [[1,6], [8,10], [15,18]]
     * Explanation:
     *   - Input is in REVERSE order (15→8→2→1)
     *   - After sorting: [[1,3], [2,6], [8,10], [15,18]]
     *   - [1,3] and [2,6] overlap → [1,6]
     *   - [8,10] doesn't overlap with [1,6]
     *   - [15,18] doesn't overlap with [8,10]
     * Purpose: Demonstrates why sorting is CRITICAL to the algorithm
     *          Without sorting, we'd miss the [1,3] and [2,6] overlap!
     */
    int** test10 = (int**)malloc(4 * sizeof(int*));
    for (int i = 0; i < 4; i++) {
        test10[i] = (int*)malloc(2 * sizeof(int));
    }
    test10[0][0] = 15; test10[0][1] = 18;
    test10[1][0] = 8; test10[1][1] = 10;
    test10[2][0] = 2; test10[2][1] = 6;
    test10[3][0] = 1; test10[3][1] = 3;
    result = mergeHighDefinitionIntervals(4, 2, test10, &result_rows, &result_columns);
    printResult(result, result_rows, result_columns, "Test 10: Reverse sorted [[15,18],[8,10],[2,6],[1,3]]");
    freeResult(result, result_rows);
    for (int i = 0; i < 4; i++) {
        free(test10[i]);
    }
    free(test10);

    /*
     * TESTS 11-15: Additional Edge Cases
     * - Test 11: Same start time, different end times (tests tie-breaking)
     * - Test 12: Large values near 10^9 (tests boundary conditions)
     * - Test 13: Zero as start value (tests minimum boundary)
     * - Test 14: Problem example from description
     * - Test 15: Multiple contained intervals in one large interval
     */

    // Test 11: Same start, different ends
    int** test11 = (int**)malloc(3 * sizeof(int*));
    for (int i = 0; i < 3; i++) {
        test11[i] = (int*)malloc(2 * sizeof(int));
    }
    test11[0][0] = 1; test11[0][1] = 5;
    test11[1][0] = 1; test11[1][1] = 10;
    test11[2][0] = 1; test11[2][1] = 3;
    result = mergeHighDefinitionIntervals(3, 2, test11, &result_rows, &result_columns);
    printResult(result, result_rows, result_columns, "Test 11: Same start, different ends [[1,5],[1,10],[1,3]]");
    freeResult(result, result_rows);
    for (int i = 0; i < 3; i++) {
        free(test11[i]);
    }
    free(test11);

    // Test 12: Large values (near 10^9)
    int** test12 = (int**)malloc(3 * sizeof(int*));
    for (int i = 0; i < 3; i++) {
        test12[i] = (int*)malloc(2 * sizeof(int));
    }
    test12[0][0] = 1000000000; test12[0][1] = 1000000001;
    test12[1][0] = 999999999; test12[1][1] = 1000000000;
    test12[2][0] = 1000000002; test12[2][1] = 1000000005;
    result = mergeHighDefinitionIntervals(3, 2, test12, &result_rows, &result_columns);
    printResult(result, result_rows, result_columns, "Test 12: Large values near 10^9");
    freeResult(result, result_rows);
    for (int i = 0; i < 3; i++) {
        free(test12[i]);
    }
    free(test12);

    // Test 13: Zero values
    int** test13 = (int**)malloc(2 * sizeof(int*));
    test13[0] = (int*)malloc(2 * sizeof(int));
    test13[1] = (int*)malloc(2 * sizeof(int));
    test13[0][0] = 0; test13[0][1] = 5;
    test13[1][0] = 3; test13[1][1] = 10;
    result = mergeHighDefinitionIntervals(2, 2, test13, &result_rows, &result_columns);
    printResult(result, result_rows, result_columns, "Test 13: Zero start value [[0,5],[3,10]]");
    freeResult(result, result_rows);
    free(test13[0]);
    free(test13[1]);
    free(test13);

    // Test 14: Complex case from problem description
    int** test14 = (int**)malloc(4 * sizeof(int*));
    for (int i = 0; i < 4; i++) {
        test14[i] = (int*)malloc(2 * sizeof(int));
    }
    test14[0][0] = 1; test14[0][1] = 3;
    test14[1][0] = 2; test14[1][1] = 6;
    test14[2][0] = 8; test14[2][1] = 10;
    test14[3][0] = 15; test14[3][1] = 18;
    result = mergeHighDefinitionIntervals(4, 2, test14, &result_rows, &result_columns);
    printResult(result, result_rows, result_columns, "Test 14: Problem example [[1,3],[2,6],[8,10],[15,18]]");
    freeResult(result, result_rows);
    for (int i = 0; i < 4; i++) {
        free(test14[i]);
    }
    free(test14);

    // Test 15: Multiple contained intervals
    int** test15 = (int**)malloc(5 * sizeof(int*));
    for (int i = 0; i < 5; i++) {
        test15[i] = (int*)malloc(2 * sizeof(int));
    }
    test15[0][0] = 1; test15[0][1] = 20;
    test15[1][0] = 3; test15[1][1] = 5;
    test15[2][0] = 7; test15[2][1] = 9;
    test15[3][0] = 10; test15[3][1] = 15;
    test15[4][0] = 2; test15[4][1] = 4;
    result = mergeHighDefinitionIntervals(5, 2, test15, &result_rows, &result_columns);
    printResult(result, result_rows, result_columns, "Test 15: Multiple contained [[1,20],[3,5],[7,9],[10,15],[2,4]]");
    freeResult(result, result_rows);
    for (int i = 0; i < 5; i++) {
        free(test15[i]);
    }
    free(test15);

    printf("\n=== ALL TESTS COMPLETED ===\n");
    return 0;
}
