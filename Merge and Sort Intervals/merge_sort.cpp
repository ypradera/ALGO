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
SPACE COMPLEXITY: O(n) - for storing result

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
- Vector of vectors where each inner vector has [start, end]

CONSTRAINTS:
- 0 <= intervals.length <= 100000
- intervals[i].length == 2
- 0 <= intervals[i][0] < intervals[i][1] <= 10^9

OUTPUT FORMAT:
- Vector of merged intervals sorted by start time
================================================================================
*/

#include <iostream>
#include <vector>
#include <algorithm>

/*
 * Merge Overlapping Intervals - Main Function
 *
 * ALGORITHM OVERVIEW:
 * This function takes a vector of intervals and merges all overlapping ones.
 * It uses a three-phase approach:
 *   Phase 1: Sort intervals by start time
 *   Phase 2: Merge overlapping intervals
 *   Phase 3: Return merged result
 *
 * PARAMETERS:
 * @param intervals - Vector of [start, end] interval pairs
 *
 * RETURN VALUE:
 * - Vector of merged intervals
 * - Returns empty vector for invalid input
 */
std::vector<std::vector<int>> mergeHighDefinitionIntervals(std::vector<std::vector<int>>& intervals) {
    // VALIDATION: Check for invalid input
    if (intervals.empty()) {
        return {};
    }

    // PHASE 1: SORT INTERVALS
    // Sort by start time (primary), then by end time (tie-breaker)
    std::sort(intervals.begin(), intervals.end(),
        [](const std::vector<int>& a, const std::vector<int>& b) {
            if (a[0] != b[0]) {
                return a[0] < b[0];  // Sort by start time
            }
            return a[1] < b[1];  // Tie-break by end time
        });

    // PHASE 2: INITIALIZE OUTPUT WITH FIRST INTERVAL
    std::vector<std::vector<int>> result;
    result.push_back(intervals[0]);

    // PHASE 3: MERGE PASS
    // Process remaining intervals one by one
    for (size_t i = 1; i < intervals.size(); i++) {
        int curS = intervals[i][0];  // Current interval start
        int curE = intervals[i][1];  // Current interval end

        int lastE = result.back()[1];  // Last merged interval's end

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
            if (curE > lastE) {
                result.back()[1] = curE;
            }
            // If curE <= lastE, current interval is fully contained, do nothing
        } else {
            // CASE 2: NO OVERLAP
            // Add current interval as a new separate interval
            result.push_back(intervals[i]);
        }
    }

    return result;
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
 * @param result   - Vector of merged intervals
 * @param testName - Description of the test case
 */
void printResult(const std::vector<std::vector<int>>& result, const std::string& testName) {
    std::cout << "\n" << testName << ":\n";

    // Handle empty result
    if (result.empty()) {
        std::cout << "  []\n";
        return;
    }

    // Print intervals in array format
    std::cout << "  [";
    for (size_t i = 0; i < result.size(); i++) {
        std::cout << "[" << result[i][0] << ", " << result[i][1] << "]";
        if (i < result.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]\n";
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
int main() {
    std::vector<std::vector<int>> result;

    std::cout << "=== MERGE INTERVALS STRESS TESTS ===\n";

    /*
     * TEST 1: EMPTY ARRAY
     * Input: []
     * Expected: []
     * Purpose: Tests handling of empty input
     */
    std::vector<std::vector<int>> test1 = {};
    result = mergeHighDefinitionIntervals(test1);
    printResult(result, "Test 1: Empty array");

    /*
     * TEST 2: SINGLE INTERVAL
     * Input: [[1,5]]
     * Expected: [[1,5]]
     * Purpose: Minimum valid input - nothing to merge
     */
    std::vector<std::vector<int>> test2 = {{1, 5}};
    result = mergeHighDefinitionIntervals(test2);
    printResult(result, "Test 2: Single interval [[1,5]]");

    /*
     * TEST 3: TWO OVERLAPPING INTERVALS
     * Input: [[1,3], [2,6]]
     * Expected: [[1,6]]
     * Explanation:
     *   - [1,3] and [2,6] overlap because 2 <= 3
     *   - Merged interval: [min(1,2), max(3,6)] = [1,6]
     * Purpose: Basic overlap test
     */
    std::vector<std::vector<int>> test3 = {{1, 3}, {2, 6}};
    result = mergeHighDefinitionIntervals(test3);
    printResult(result, "Test 3: Two overlapping [[1,3], [2,6]]");

    /*
     * TEST 4: TWO NON-OVERLAPPING INTERVALS
     * Input: [[1,2], [5,10]]
     * Expected: [[1,2], [5,10]]
     * Explanation:
     *   - [1,2] and [5,10] don't overlap because 5 > 2
     *   - Gap between them: (2, 5)
     * Purpose: Tests that non-overlapping intervals stay separate
     */
    std::vector<std::vector<int>> test4 = {{1, 2}, {5, 10}};
    result = mergeHighDefinitionIntervals(test4);
    printResult(result, "Test 4: Two non-overlapping [[1,2], [5,10]]");

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
    std::vector<std::vector<int>> test5 = {{1, 4}, {2, 5}, {3, 6}, {4, 10}};
    result = mergeHighDefinitionIntervals(test5);
    printResult(result, "Test 5: All merge into one [[1,4],[2,5],[3,6],[4,10]]");

    /*
     * TEST 6: NO OVERLAPPING INTERVALS
     * Input: [[1,2], [3,4], [5,6], [7,8]]
     * Expected: [[1,2], [3,4], [5,6], [7,8]]
     * Purpose: All intervals are separate, all remain in result
     */
    std::vector<std::vector<int>> test6 = {{1, 2}, {3, 4}, {5, 6}, {7, 8}};
    result = mergeHighDefinitionIntervals(test6);
    printResult(result, "Test 6: No overlapping [[1,2],[3,4],[5,6],[7,8]]");

    /*
     * TEST 7: DUPLICATE INTERVALS
     * Input: [[1,5], [1,5], [1,5]]
     * Expected: [[1,5]]
     * Explanation:
     *   - All three intervals are identical
     *   - They all merge into a single [1,5]
     * Purpose: Tests handling of duplicate input
     */
    std::vector<std::vector<int>> test7 = {{1, 5}, {1, 5}, {1, 5}};
    result = mergeHighDefinitionIntervals(test7);
    printResult(result, "Test 7: Duplicate intervals [[1,5],[1,5],[1,5]]");

    /*
     * TEST 8: CONTAINED INTERVAL
     * Input: [[1,10], [3,5]]
     * Expected: [[1,10]]
     * Explanation:
     *   - [3,5] is completely inside [1,10]
     *   - Since 3 <= 10 (overlap) and 5 <= 10 (no extension needed)
     *   - Result remains [1,10]
     * Purpose: Tests that contained intervals don't create new merged intervals
     */
    std::vector<std::vector<int>> test8 = {{1, 10}, {3, 5}};
    result = mergeHighDefinitionIntervals(test8);
    printResult(result, "Test 8: Contained interval [[1,10], [3,5]]");

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
     */
    std::vector<std::vector<int>> test9 = {{1, 3}, {3, 5}, {5, 10}};
    result = mergeHighDefinitionIntervals(test9);
    printResult(result, "Test 9: Adjacent intervals [[1,3],[3,5],[5,10]]");

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
     */
    std::vector<std::vector<int>> test10 = {{15, 18}, {8, 10}, {2, 6}, {1, 3}};
    result = mergeHighDefinitionIntervals(test10);
    printResult(result, "Test 10: Reverse sorted [[15,18],[8,10],[2,6],[1,3]]");

    /*
     * TEST 11: SAME START, DIFFERENT ENDS
     * Input: [[1,5], [1,10], [1,3]]
     * Expected: [[1,10]]
     * Explanation:
     *   - All start at 1
     *   - After sorting by end (tie-breaker): [[1,3], [1,5], [1,10]]
     *   - All overlap, merged to [1,10]
     */
    std::vector<std::vector<int>> test11 = {{1, 5}, {1, 10}, {1, 3}};
    result = mergeHighDefinitionIntervals(test11);
    printResult(result, "Test 11: Same start, different ends [[1,5],[1,10],[1,3]]");

    /*
     * TEST 12: LARGE VALUES (NEAR 10^9)
     * Input: [[1000000000,1000000001], [999999999,1000000000], [1000000002,1000000005]]
     * Expected: [[999999999,1000000001], [1000000002,1000000005]]
     * Explanation:
     *   - After sorting: [[999999999,1000000000], [1000000000,1000000001], [1000000002,1000000005]]
     *   - First two overlap → [999999999,1000000001]
     *   - Third doesn't overlap
     */
    std::vector<std::vector<int>> test12 = {{1000000000, 1000000001}, {999999999, 1000000000}, {1000000002, 1000000005}};
    result = mergeHighDefinitionIntervals(test12);
    printResult(result, "Test 12: Large values near 10^9");

    /*
     * TEST 13: ZERO START VALUE
     * Input: [[0,5], [3,10]]
     * Expected: [[0,10]]
     * Explanation:
     *   - [0,5] and [3,10] overlap (3 <= 5)
     *   - Merged to [0,10]
     * Purpose: Tests minimum boundary value
     */
    std::vector<std::vector<int>> test13 = {{0, 5}, {3, 10}};
    result = mergeHighDefinitionIntervals(test13);
    printResult(result, "Test 13: Zero start value [[0,5],[3,10]]");

    /*
     * TEST 14: PROBLEM EXAMPLE
     * Input: [[1,3], [2,6], [8,10], [15,18]]
     * Expected: [[1,6], [8,10], [15,18]]
     * Purpose: Official problem example
     */
    std::vector<std::vector<int>> test14 = {{1, 3}, {2, 6}, {8, 10}, {15, 18}};
    result = mergeHighDefinitionIntervals(test14);
    printResult(result, "Test 14: Problem example [[1,3],[2,6],[8,10],[15,18]]");

    /*
     * TEST 15: MULTIPLE CONTAINED INTERVALS
     * Input: [[1,20], [3,5], [7,9], [10,15], [2,4]]
     * Expected: [[1,20]]
     * Explanation:
     *   - After sorting: [[1,20], [2,4], [3,5], [7,9], [10,15]]
     *   - All intervals are contained within [1,20]
     *   - Result: [1,20]
     */
    std::vector<std::vector<int>> test15 = {{1, 20}, {3, 5}, {7, 9}, {10, 15}, {2, 4}};
    result = mergeHighDefinitionIntervals(test15);
    printResult(result, "Test 15: Multiple contained [[1,20],[3,5],[7,9],[10,15],[2,4]]");

    std::cout << "\n=== ALL TESTS COMPLETED ===\n";
    return 0;
}
