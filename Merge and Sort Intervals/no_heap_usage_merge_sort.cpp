/*
================================================================================
MERGE INTERVALS - IN-PLACE VERSION (C++)
================================================================================

PROBLEM STATEMENT:
Merge overlapping intervals by modifying the input vector in-place.
This version avoids creating a new vector for the result.

ALGORITHM APPROACH:
1. Sort intervals in-place by start time
2. Merge overlapping intervals using two-pointer technique
3. Return a view (subvector) of the modified input containing merged intervals

KEY FEATURES:
- Modifies input vector in-place
- Memory-efficient approach
- Returns subvector of original vector containing merged intervals

TIME COMPLEXITY: O(n log n) - dominated by sorting
SPACE COMPLEXITY: O(1) - only uses constant extra space (ignoring sort space)

ADVANTAGES:
- Minimal memory allocation
- In-place algorithm
- Efficient for large datasets

LIMITATIONS:
- Modifies original input vector
- Less flexible than creating new vector

================================================================================
*/

#include <iostream>
#include <vector>
#include <algorithm>

/*
 * Merge Overlapping Intervals In-Place
 *
 * ALGORITHM:
 * 1. Sort intervals by start time (and by end time as tie-breaker)
 * 2. Use write pointer for last merged interval position
 * 3. Use read pointer to scan through remaining intervals
 * 4. Merge overlapping intervals by extending write interval
 * 5. For non-overlapping intervals, advance write and copy
 *
 * PARAMETERS:
 * intervals - Vector of [start, end] interval pairs (MODIFIED IN-PLACE)
 *
 * RETURN:
 * Vector of merged intervals (subvector of input)
 * Returns empty vector on invalid input
 *
 * EXAMPLES:
 * mergeHighDefinitionIntervals([[1,3], [2,6], [8,10], [15,18]])
 * → [[1, 6], [8, 10], [15, 18]]
 *
 * mergeHighDefinitionIntervals([[1,4], [4,5]])
 * → [[1, 5]]
 */
std::vector<std::vector<int>> mergeHighDefinitionIntervals(std::vector<std::vector<int>>& intervals) {
    // Validate input
    if (intervals.empty()) {
        return {};
    }

    // PHASE 1: SORT INTERVALS IN-PLACE
    // Sort by start time (primary), then by end time (tie-breaker)
    std::sort(intervals.begin(), intervals.end(),
        [](const std::vector<int>& a, const std::vector<int>& b) {
            if (a[0] != b[0]) {
                return a[0] < b[0];  // Sort by start time
            }
            return a[1] < b[1];  // Tie-break by end time
        });

    // PHASE 2: MERGE IN-PLACE USING TWO-POINTER TECHNIQUE
    // 'write' - index of last merged interval (slow pointer)
    // 'read'  - index of current interval being processed (fast pointer)
    //
    // Example walkthrough:
    // Input (sorted): [[1,3], [2,6], [8,10], [15,18]]
    //
    // Initial: write=0 (pointing to [1,3])
    //
    // read=1: [2,6]
    //   - 2 <= 3 (overlap) → extend write: [1,6]
    //   - write stays at 0
    //
    // read=2: [8,10]
    //   - 8 > 6 (no overlap) → advance write to 1
    //   - Copy [8,10] to intervals[1]
    //
    // read=3: [15,18]
    //   - 15 > 10 (no overlap) → advance write to 2
    //   - Copy [15,18] to intervals[2]
    //
    // Result: [[1,6], [8,10], [15,18]] (write+1 = 3 intervals)

    int write = 0;  // Index of last merged interval

    // Process each interval starting from second one
    for (size_t read = 1; read < intervals.size(); read++) {
        int curStart = intervals[read][0];  // Current interval start
        int curEnd = intervals[read][1];    // Current interval end

        int lastEnd = intervals[write][1];  // Last merged interval's end

        if (curStart <= lastEnd) {
            // CASE 1: OVERLAP
            // Current interval overlaps with last merged interval
            // Extend the end of last merged interval if needed
            if (curEnd > lastEnd) {
                intervals[write][1] = curEnd;
            }
            // Note: We don't advance 'write' - keep merging into same interval
        } else {
            // CASE 2: NO OVERLAP
            // Current interval doesn't overlap - start a new merged interval
            write++;  // Move to next slot
            intervals[write] = intervals[read];  // Copy current interval
        }
    }

    // Return subvector containing merged intervals
    // Merged intervals are in intervals[0..write]
    return std::vector<std::vector<int>>(intervals.begin(), intervals.begin() + write + 1);
}

/*
 * Print Test Result
 *
 * PURPOSE:
 * Displays test case information and result
 *
 * PARAMETERS:
 * @param testName - Description of test case
 * @param result   - Vector of merged intervals
 * @param expected - Expected result
 */
void printTest(const std::string& testName, const std::vector<std::vector<int>>& result,
               const std::vector<std::vector<int>>& expected) {
    std::cout << "\n" << testName << ":\n";
    std::cout << "  Result: [";
    for (size_t i = 0; i < result.size(); i++) {
        std::cout << "[" << result[i][0] << ", " << result[i][1] << "]";
        if (i < result.size() - 1) std::cout << ", ";
    }
    std::cout << "]";

    bool pass = (result == expected);
    std::cout << (pass ? " PASS" : " FAIL") << "\n";
    std::cout << "  Expected: [";
    for (size_t i = 0; i < expected.size(); i++) {
        std::cout << "[" << expected[i][0] << ", " << expected[i][1] << "]";
        if (i < expected.size() - 1) std::cout << ", ";
    }
    std::cout << "]\n";
}

/*
 * Main Function - Comprehensive Test Suite
 *
 * PURPOSE:
 * Tests the mergeHighDefinitionIntervals algorithm with 15 test cases
 * covering edge cases, overlapping scenarios, and boundary conditions
 *
 * TEST CATEGORIES:
 * 1. Edge Cases: Empty, single interval
 * 2. Overlapping Scenarios: All merge, partial merge
 * 3. Non-overlapping: Separate intervals
 * 4. Special Cases: Adjacent, contained, duplicates
 * 5. Boundary Values: Zero, large values
 */
int main() {
    std::vector<std::vector<int>> result;

    std::cout << "=== MERGE INTERVALS - IN-PLACE TESTS ===\n";

    /*
     * TEST 1: EMPTY ARRAY
     * Input: []
     * Expected: []
     */
    std::vector<std::vector<int>> test1 = {};
    result = mergeHighDefinitionIntervals(test1);
    printTest("Test 1: Empty array", result, {});

    /*
     * TEST 2: SINGLE INTERVAL
     * Input: [[1,5]]
     * Expected: [[1,5]]
     */
    std::vector<std::vector<int>> test2 = {{1, 5}};
    result = mergeHighDefinitionIntervals(test2);
    printTest("Test 2: Single interval [[1,5]]", result, {{1, 5}});

    /*
     * TEST 3: TWO OVERLAPPING INTERVALS
     * Input: [[1,3], [2,6]]
     * Expected: [[1,6]]
     */
    std::vector<std::vector<int>> test3 = {{1, 3}, {2, 6}};
    result = mergeHighDefinitionIntervals(test3);
    printTest("Test 3: Two overlapping [[1,3], [2,6]]", result, {{1, 6}});

    /*
     * TEST 4: TWO NON-OVERLAPPING INTERVALS
     * Input: [[1,2], [5,10]]
     * Expected: [[1,2], [5,10]]
     */
    std::vector<std::vector<int>> test4 = {{1, 2}, {5, 10}};
    result = mergeHighDefinitionIntervals(test4);
    printTest("Test 4: Two non-overlapping [[1,2], [5,10]]", result, {{1, 2}, {5, 10}});

    /*
     * TEST 5: ALL INTERVALS MERGE INTO ONE
     * Input: [[1,4], [2,5], [3,6], [4,10]]
     * Expected: [[1,10]]
     */
    std::vector<std::vector<int>> test5 = {{1, 4}, {2, 5}, {3, 6}, {4, 10}};
    result = mergeHighDefinitionIntervals(test5);
    printTest("Test 5: All merge into one [[1,4],[2,5],[3,6],[4,10]]", result, {{1, 10}});

    /*
     * TEST 6: NO OVERLAPPING INTERVALS
     * Input: [[1,2], [3,4], [5,6], [7,8]]
     * Expected: [[1,2], [3,4], [5,6], [7,8]]
     */
    std::vector<std::vector<int>> test6 = {{1, 2}, {3, 4}, {5, 6}, {7, 8}};
    result = mergeHighDefinitionIntervals(test6);
    printTest("Test 6: No overlapping [[1,2],[3,4],[5,6],[7,8]]", result, {{1, 2}, {3, 4}, {5, 6}, {7, 8}});

    /*
     * TEST 7: DUPLICATE INTERVALS
     * Input: [[1,5], [1,5], [1,5]]
     * Expected: [[1,5]]
     */
    std::vector<std::vector<int>> test7 = {{1, 5}, {1, 5}, {1, 5}};
    result = mergeHighDefinitionIntervals(test7);
    printTest("Test 7: Duplicate intervals [[1,5],[1,5],[1,5]]", result, {{1, 5}});

    /*
     * TEST 8: CONTAINED INTERVAL
     * Input: [[1,10], [3,5]]
     * Expected: [[1,10]]
     */
    std::vector<std::vector<int>> test8 = {{1, 10}, {3, 5}};
    result = mergeHighDefinitionIntervals(test8);
    printTest("Test 8: Contained interval [[1,10], [3,5]]", result, {{1, 10}});

    /*
     * TEST 9: ADJACENT INTERVALS (TOUCHING)
     * Input: [[1,3], [3,5], [5,10]]
     * Expected: [[1,10]]
     * Explanation: Touching intervals are merged (3 <= 3, 5 <= 5)
     */
    std::vector<std::vector<int>> test9 = {{1, 3}, {3, 5}, {5, 10}};
    result = mergeHighDefinitionIntervals(test9);
    printTest("Test 9: Adjacent intervals [[1,3],[3,5],[5,10]]", result, {{1, 10}});

    /*
     * TEST 10: REVERSE SORTED INTERVALS
     * Input: [[15,18], [8,10], [2,6], [1,3]]
     * Expected: [[1,6], [8,10], [15,18]]
     */
    std::vector<std::vector<int>> test10 = {{15, 18}, {8, 10}, {2, 6}, {1, 3}};
    result = mergeHighDefinitionIntervals(test10);
    printTest("Test 10: Reverse sorted [[15,18],[8,10],[2,6],[1,3]]", result, {{1, 6}, {8, 10}, {15, 18}});

    /*
     * TEST 11: SAME START, DIFFERENT ENDS
     * Input: [[1,5], [1,10], [1,3]]
     * Expected: [[1,10]]
     */
    std::vector<std::vector<int>> test11 = {{1, 5}, {1, 10}, {1, 3}};
    result = mergeHighDefinitionIntervals(test11);
    printTest("Test 11: Same start, different ends [[1,5],[1,10],[1,3]]", result, {{1, 10}});

    /*
     * TEST 12: LARGE VALUES (NEAR 10^9)
     * Input: [[1000000000,1000000001], [999999999,1000000000], [1000000002,1000000005]]
     * Expected: [[999999999,1000000001], [1000000002,1000000005]]
     */
    std::vector<std::vector<int>> test12 = {{1000000000, 1000000001}, {999999999, 1000000000}, {1000000002, 1000000005}};
    result = mergeHighDefinitionIntervals(test12);
    printTest("Test 12: Large values near 10^9", result, {{999999999, 1000000001}, {1000000002, 1000000005}});

    /*
     * TEST 13: ZERO START VALUE
     * Input: [[0,5], [3,10]]
     * Expected: [[0,10]]
     */
    std::vector<std::vector<int>> test13 = {{0, 5}, {3, 10}};
    result = mergeHighDefinitionIntervals(test13);
    printTest("Test 13: Zero start value [[0,5],[3,10]]", result, {{0, 10}});

    /*
     * TEST 14: PROBLEM EXAMPLE
     * Input: [[1,3], [2,6], [8,10], [15,18]]
     * Expected: [[1,6], [8,10], [15,18]]
     */
    std::vector<std::vector<int>> test14 = {{1, 3}, {2, 6}, {8, 10}, {15, 18}};
    result = mergeHighDefinitionIntervals(test14);
    printTest("Test 14: Problem example [[1,3],[2,6],[8,10],[15,18]]", result, {{1, 6}, {8, 10}, {15, 18}});

    /*
     * TEST 15: MULTIPLE CONTAINED INTERVALS
     * Input: [[1,20], [3,5], [7,9], [10,15], [2,4]]
     * Expected: [[1,20]]
     */
    std::vector<std::vector<int>> test15 = {{1, 20}, {3, 5}, {7, 9}, {10, 15}, {2, 4}};
    result = mergeHighDefinitionIntervals(test15);
    printTest("Test 15: Multiple contained [[1,20],[3,5],[7,9],[10,15],[2,4]]", result, {{1, 20}});

    std::cout << "\n=== ALL TESTS COMPLETED ===\n";
    std::cout << "\nKEY ADVANTAGES OF THIS VERSION:\n";
    std::cout << "- In-place algorithm (modifies input)\n";
    std::cout << "- Minimal memory allocation\n";
    std::cout << "- O(n log n) time complexity (sorting)\n";
    std::cout << "- O(1) space complexity (ignoring sort)\n";
    std::cout << "- Efficient for large datasets\n";

    return 0;
}
