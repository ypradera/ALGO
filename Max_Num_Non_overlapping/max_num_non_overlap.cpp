/*
Maximum Number of Non-Overlapping Intervals
Given an array of intervals where each interval has a start and end time, return the maximum number of non-overlapping intervals.

Examples
Example 1

Input:

meetings = [[1, 2], [2, 3], [3, 4], [1, 3]]
Output:

3
Explanation:

Step 1: Sort meetings by end time → [[1,2],[2,3],[1,3],[3,4]].
Step 2: Select [1,2] (count=1, last_end=2).
Step 3: [2,3] has start 2 ≥ 2, select (count=2, last_end=3).
Step 4: [1,3] start 1 < 3, skip.
Step 5: [3,4] start 3 ≥ 3, select (count=3). Result = 3.
Example 2

Input:

meetings = [[0, 5], [0, 1], [1, 2], [2, 3], [3, 5], [4, 6]]
Output:

4
Explanation:

Step 1: Sort by end time → [[0,1],[1,2],[2,3],[3,5],[0,5],[4,6]].
Step 2: Select [0,1] (count=1, end=1).
Step 3: [1,2] (count=2, end=2).
Step 4: [2,3] (count=3, end=3).
Step 5: [3,5] (count=4, end=5). Skip [0,5] and [4,6] because their start times are before last_end=5. Result = 4.
Input Format

The first line contains a single integer, meetings_rows, representing the total number of meetings.
The second line contains a single integer, meetings_columns, representing the number of properties for each meeting (typically 2 for a start and end time).
The next meetings_rows lines describe the meetings. Each line contains meetings_columns space-separated integers, representing the properties of a single meeting.
6
2
1 2
3 4
0 6
5 7
8 9
5 9
Explanation:

The first line, 6, is the value for meetings_rows.
The second line, 2, is the value for meetings_columns.
The following 6 lines represent the individual meetings, which are parsed into a 2D list: [[1, 2], [3, 4], [0, 6], [5, 7], [8, 9], [5, 9]].
Constraints

0 <= meetings.length <= 1000
meetings[i].length == 2 for all 0 <= i < meetings.length
0 <= meetings[i][0] < meetings[i][1] <= 10^9
All start and end times are integers
Meetings may share start or end times
Output Format

Return a single integer denoting the maximum number of non-overlapping meetings that can be scheduled.*/

#include <iostream>
#include <vector>
#include <algorithm>

/*
 * Complete the 'maximizeNonOverlappingMeetings' function below.
 *
 * The function is expected to return an INTEGER.
 * The function accepts 2D_INTEGER_ARRAY meetings as parameter.
 */
int maximizeNonOverlappingMeetings(std::vector<std::vector<int>>& meetings) {
    // Edge case: empty meetings
    if (meetings.empty()) {
        return 0;
    }

    // Sort meetings by end time (primary), start time (tie-breaker)
    std::sort(meetings.begin(), meetings.end(),
        [](const std::vector<int>& a, const std::vector<int>& b) {
            if (a[1] != b[1]) {
                return a[1] < b[1];  // Sort by end time
            }
            return a[0] < b[0];  // Tie-break by start time
        });

    // Start at 1 because after sorting, we always take the first interval
    int count = 1;
    int lastE = meetings[0][1];

    // Process the remaining intervals
    for (size_t i = 1; i < meetings.size(); i++) {
        int curS = meetings[i][0];
        int curE = meetings[i][1];

        // Non-overlapping condition: current start >= last end
        if (lastE <= curS) {
            count++;
            lastE = curE;  // Update last end time
        }
    }

    return count;
}

/*
 * Print Test Result
 *
 * PURPOSE:
 * Displays test case information and result
 *
 * PARAMETERS:
 * @param testName - Description of test case
 * @param result   - Maximum number of non-overlapping intervals
 * @param expected - Expected result
 */
void printTest(const std::string& testName, int result, int expected) {
    std::cout << "\n" << testName << ":\n";
    std::cout << "  Result: " << result;
    if (result == expected) {
        std::cout << " PASS (Expected: " << expected << ")\n";
    } else {
        std::cout << " FAIL (Expected: " << expected << ")\n";
    }
}

/*
 * Main Function - Comprehensive Test Suite
 *
 * PURPOSE:
 * Tests the maximizeNonOverlappingMeetings algorithm with 15 test cases
 * covering edge cases, overlapping scenarios, and boundary conditions
 *
 * TEST CATEGORIES:
 * 1. Edge Cases: Empty, single interval
 * 2. Overlap Scenarios: All overlap, none overlap, partial overlap
 * 3. Special Cases: Same end times, same start times, adjacent, contained
 * 4. Boundary Values: Zero, large values
 */
int main() {
    int result;

    std::cout << "=== MAXIMUM NON-OVERLAPPING INTERVALS TESTS ===\n";

    /*
     * TEST 1: EMPTY ARRAY
     * Input: []
     * Expected: 0
     * Purpose: Edge case - no intervals
     */
    std::vector<std::vector<int>> test1 = {};
    result = maximizeNonOverlappingMeetings(test1);
    printTest("Test 1: Empty array", result, 0);

    /*
     * TEST 2: SINGLE INTERVAL
     * Input: [[1,5]]
     * Expected: 1
     * Purpose: Minimum valid input - one interval always selected
     */
    std::vector<std::vector<int>> test2 = {{1, 5}};
    result = maximizeNonOverlappingMeetings(test2);
    printTest("Test 2: Single interval [[1,5]]", result, 1);

    /*
     * TEST 3: ALL OVERLAPPING
     * Input: [[1,10], [2,5], [3,8], [4,9]]
     * Expected: 1
     * Explanation: All intervals overlap with [1,10], only 1 can be selected
     * After sorting by end: [[2,5], [3,8], [4,9], [1,10]]
     * Select [2,5], all others overlap with it
     */
    std::vector<std::vector<int>> test3 = {{1, 10}, {2, 5}, {3, 8}, {4, 9}};
    result = maximizeNonOverlappingMeetings(test3);
    printTest("Test 3: All overlapping [[1,10],[2,5],[3,8],[4,9]]", result, 1);

    /*
     * TEST 4: NO OVERLAPPING
     * Input: [[1,2], [3,4], [5,6], [7,8]]
     * Expected: 4
     * Explanation: All intervals are separate, all can be selected
     */
    std::vector<std::vector<int>> test4 = {{1, 2}, {3, 4}, {5, 6}, {7, 8}};
    result = maximizeNonOverlappingMeetings(test4);
    printTest("Test 4: No overlapping [[1,2],[3,4],[5,6],[7,8]]", result, 4);

    /*
     * TEST 5: PROBLEM EXAMPLE 1
     * Input: [[1,2], [2,3], [3,4], [1,3]]
     * Expected: 3
     * Explanation: Can select [1,2], [2,3], [3,4]
     */
    std::vector<std::vector<int>> test5 = {{1, 2}, {2, 3}, {3, 4}, {1, 3}};
    result = maximizeNonOverlappingMeetings(test5);
    printTest("Test 5: Problem example [[1,2],[2,3],[3,4],[1,3]]", result, 3);

    /*
     * TEST 6: PROBLEM EXAMPLE 2
     * Input: [[0,5], [0,1], [1,2], [2,3], [3,5], [4,6]]
     * Expected: 4
     * Explanation: Can select [0,1], [1,2], [2,3], [3,5]
     */
    std::vector<std::vector<int>> test6 = {{0, 5}, {0, 1}, {1, 2}, {2, 3}, {3, 5}, {4, 6}};
    result = maximizeNonOverlappingMeetings(test6);
    printTest("Test 6: Problem example [[0,5],[0,1],[1,2],[2,3],[3,5],[4,6]]", result, 4);

    /*
     * TEST 7: SAME END TIMES
     * Input: [[1,5], [2,5], [3,5], [0,5]]
     * Expected: 1
     * Explanation: All have same end time, only 1 can be selected
     * After sorting: [[0,5], [1,5], [2,5], [3,5]] (tie-break by start)
     * Select [0,5], all others start after lastE=5 is not satisfied
     */
    std::vector<std::vector<int>> test7 = {{1, 5}, {2, 5}, {3, 5}, {0, 5}};
    result = maximizeNonOverlappingMeetings(test7);
    printTest("Test 7: Same end times [[1,5],[2,5],[3,5],[0,5]]", result, 1);

    /*
     * TEST 8: ADJACENT INTERVALS (TOUCHING)
     * Input: [[1,3], [3,5], [5,7], [7,10]]
     * Expected: 4
     * Explanation: End of one = start of next, all can be selected
     * Because condition is lastE <= curS (3 <= 3 is true)
     */
    std::vector<std::vector<int>> test8 = {{1, 3}, {3, 5}, {5, 7}, {7, 10}};
    result = maximizeNonOverlappingMeetings(test8);
    printTest("Test 8: Adjacent intervals [[1,3],[3,5],[5,7],[7,10]]", result, 4);

    /*
     * TEST 9: CONTAINED INTERVALS
     * Input: [[1,20], [2,5], [6,10], [11,15]]
     * Expected: 3
     * Explanation: [2,5], [6,10], [11,15] don't overlap with each other
     * After sorting by end: [[2,5], [6,10], [11,15], [1,20]]
     * Select [2,5] (lastE=5), [6,10] (6>=5, lastE=10), [11,15] (11>=10)
     */
    std::vector<std::vector<int>> test9 = {{1, 20}, {2, 5}, {6, 10}, {11, 15}};
    result = maximizeNonOverlappingMeetings(test9);
    printTest("Test 9: Contained intervals [[1,20],[2,5],[6,10],[11,15]]", result, 3);

    /*
     * TEST 10: REVERSE SORTED BY END
     * Input: [[15,18], [8,10], [5,7], [1,3]]
     * Expected: 4
     * Explanation: After sorting by end: [[1,3], [5,7], [8,10], [15,18]]
     * All are non-overlapping, select all 4
     */
    std::vector<std::vector<int>> test10 = {{15, 18}, {8, 10}, {5, 7}, {1, 3}};
    result = maximizeNonOverlappingMeetings(test10);
    printTest("Test 10: Reverse sorted [[15,18],[8,10],[5,7],[1,3]]", result, 4);

    /*
     * TEST 11: DUPLICATE INTERVALS
     * Input: [[1,5], [1,5], [1,5]]
     * Expected: 1
     * Explanation: All identical, only 1 can be selected
     */
    std::vector<std::vector<int>> test11 = {{1, 5}, {1, 5}, {1, 5}};
    result = maximizeNonOverlappingMeetings(test11);
    printTest("Test 11: Duplicate intervals [[1,5],[1,5],[1,5]]", result, 1);

    /*
     * TEST 12: LARGE VALUES (NEAR 10^9)
     * Input: [[999999990,999999995], [999999996,1000000000], [1000000001,1000000005]]
     * Expected: 3
     * Explanation: All non-overlapping at boundary values
     */
    std::vector<std::vector<int>> test12 = {{999999990, 999999995}, {999999996, 1000000000}, {1000000001, 1000000005}};
    result = maximizeNonOverlappingMeetings(test12);
    printTest("Test 12: Large values near 10^9", result, 3);

    /*
     * TEST 13: ZERO START VALUE
     * Input: [[0,2], [1,4], [3,6], [5,8]]
     * Expected: 2
     * Explanation: After sorting by end: [[0,2], [1,4], [3,6], [5,8]]
     * Select [0,2] (lastE=2), skip [1,4] (1<2), select [3,6] (3>=2, lastE=6), skip [5,8] (5<6)
     */
    std::vector<std::vector<int>> test13 = {{0, 2}, {1, 4}, {3, 6}, {5, 8}};
    result = maximizeNonOverlappingMeetings(test13);
    printTest("Test 13: Zero start value [[0,2],[1,4],[3,6],[5,8]]", result, 2);

    /*
     * TEST 14: SAME START, DIFFERENT ENDS
     * Input: [[1,10], [1,5], [1,3], [1,8]]
     * Expected: 1
     * Explanation: All start at 1, after sorting by end: [[1,3], [1,5], [1,8], [1,10]]
     * Select [1,3], all others start at 1 which is < lastE=3
     */
    std::vector<std::vector<int>> test14 = {{1, 10}, {1, 5}, {1, 3}, {1, 8}};
    result = maximizeNonOverlappingMeetings(test14);
    printTest("Test 14: Same start, different ends [[1,10],[1,5],[1,3],[1,8]]", result, 1);

    /*
     * TEST 15: COMPLEX MIXED SCENARIO
     * Input: [[1,2], [2,3], [1,3], [3,4], [2,4], [4,5], [1,5]]
     * Expected: 4
     * Explanation: After sorting by end: [[1,2], [2,3], [1,3], [3,4], [2,4], [4,5], [1,5]]
     * Select [1,2] (lastE=2)
     * Select [2,3] (2>=2, lastE=3)
     * Skip [1,3] (1<3)
     * Select [3,4] (3>=3, lastE=4)
     * Skip [2,4] (2<4)
     * Select [4,5] (4>=4, lastE=5)
     * Skip [1,5] (1<5)
     * Total: 4
     */
    std::vector<std::vector<int>> test15 = {{1, 2}, {2, 3}, {1, 3}, {3, 4}, {2, 4}, {4, 5}, {1, 5}};
    result = maximizeNonOverlappingMeetings(test15);
    printTest("Test 15: Complex mixed [[1,2],[2,3],[1,3],[3,4],[2,4],[4,5],[1,5]]", result, 4);

    std::cout << "\n=== ALL TESTS COMPLETED ===\n";
    std::cout << "\nKEY ALGORITHM POINTS:\n";
    std::cout << "- Greedy approach: Sort by end time\n";
    std::cout << "- Always select interval with earliest end time\n";
    std::cout << "- Skip overlapping intervals\n";
    std::cout << "- Time complexity: O(n log n) for sorting\n";
    std::cout << "- Space complexity: O(1) for in-place algorithm\n";

    return 0;
}
