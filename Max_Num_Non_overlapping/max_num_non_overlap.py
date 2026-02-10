"""
Maximum Number of Non-Overlapping Intervals

Given an array of intervals where each interval has a start and end time,
return the maximum number of non-overlapping intervals.

ALGORITHM APPROACH:
1. Sort intervals by end time (greedy approach)
2. Select interval with earliest end time
3. Skip overlapping intervals
4. Count total selected intervals

TIME COMPLEXITY: O(n log n) - dominated by sorting
SPACE COMPLEXITY: O(1) - in-place algorithm (ignoring sort space)

Examples:

Example 1:
Input: meetings = [[1, 2], [2, 3], [3, 4], [1, 3]]
Output: 3
Explanation: Select [1,2], [2,3], [3,4]

Example 2:
Input: meetings = [[0, 5], [0, 1], [1, 2], [2, 3], [3, 5], [4, 6]]
Output: 4
Explanation: Select [0,1], [1,2], [2,3], [3,5]

CONSTRAINTS:
- 0 <= meetings.length <= 1000
- meetings[i].length == 2 for all 0 <= i < meetings.length
- 0 <= meetings[i][0] < meetings[i][1] <= 10^9
"""


def maximizeNonOverlappingMeetings(meetings):
    """
    Find maximum number of non-overlapping intervals using greedy approach.

    ALGORITHM:
    1. Sort by end time (earliest end first)
    2. Greedily select intervals with earliest end times
    3. Skip intervals that overlap with last selected

    PARAMETERS:
    meetings - List of [start, end] intervals

    RETURN:
    Integer representing maximum number of non-overlapping intervals

    EXAMPLES:
    >>> maximizeNonOverlappingMeetings([[1,2], [2,3], [3,4]])
    3
    >>> maximizeNonOverlappingMeetings([[1,5], [2,3], [4,6]])
    2
    """
    # Edge case: empty or invalid input
    if not meetings or len(meetings) == 0:
        return 0

    # Sort by end time (primary), then by start time (tie-breaker)
    # This greedy approach ensures we always pick intervals that end earliest
    meetings.sort(key=lambda x: (x[1], x[0]))

    # Start with count=1 because first interval is always selected
    count = 1
    last_end = meetings[0][1]

    # Process remaining intervals
    for i in range(1, len(meetings)):
        curr_start = meetings[i][0]
        curr_end = meetings[i][1]

        # Non-overlapping condition: current start >= last end
        # If intervals touch (curr_start == last_end), they don't overlap
        if last_end <= curr_start:
            count += 1
            last_end = curr_end  # Update last_end to current interval

    return count


def printTest(test_name, result, expected):
    """
    Print test result in a formatted way.

    PARAMETERS:
    test_name - Description of test case
    result    - Actual result from function
    expected  - Expected result
    """
    print(f"\n{test_name}:")
    status = "PASS" if result == expected else "FAIL"
    print(f"  Result: {result} {status} (Expected: {expected})")


def main():
    """
    Main function - Comprehensive Test Suite

    Tests the maximizeNonOverlappingMeetings algorithm with 15 test cases
    covering edge cases, overlapping scenarios, and boundary conditions

    TEST CATEGORIES:
    1. Edge Cases: Empty, single interval
    2. Overlap Scenarios: All overlap, none overlap, partial overlap
    3. Special Cases: Same end times, same start times, adjacent, contained
    4. Boundary Values: Zero, large values
    """
    print("=== MAXIMUM NON-OVERLAPPING INTERVALS TESTS ===")

    # TEST 1: EMPTY ARRAY
    # Input: []
    # Expected: 0
    # Purpose: Edge case - no intervals
    result = maximizeNonOverlappingMeetings([])
    printTest("Test 1: Empty array", result, 0)

    # TEST 2: SINGLE INTERVAL
    # Input: [[1,5]]
    # Expected: 1
    # Purpose: Minimum valid input - one interval always selected
    result = maximizeNonOverlappingMeetings([[1, 5]])
    printTest("Test 2: Single interval [[1,5]]", result, 1)

    # TEST 3: ALL OVERLAPPING
    # Input: [[1,10], [2,5], [3,8], [4,9]]
    # Expected: 1
    # Explanation: All intervals overlap, only 1 can be selected
    # After sorting by end: [[2,5], [3,8], [4,9], [1,10]]
    # Select [2,5], all others overlap with it
    result = maximizeNonOverlappingMeetings([[1, 10], [2, 5], [3, 8], [4, 9]])
    printTest("Test 3: All overlapping [[1,10],[2,5],[3,8],[4,9]]", result, 1)

    # TEST 4: NO OVERLAPPING
    # Input: [[1,2], [3,4], [5,6], [7,8]]
    # Expected: 4
    # Explanation: All intervals are separate, all can be selected
    result = maximizeNonOverlappingMeetings([[1, 2], [3, 4], [5, 6], [7, 8]])
    printTest("Test 4: No overlapping [[1,2],[3,4],[5,6],[7,8]]", result, 4)

    # TEST 5: PROBLEM EXAMPLE 1
    # Input: [[1,2], [2,3], [3,4], [1,3]]
    # Expected: 3
    # Explanation: Can select [1,2], [2,3], [3,4]
    result = maximizeNonOverlappingMeetings([[1, 2], [2, 3], [3, 4], [1, 3]])
    printTest("Test 5: Problem example [[1,2],[2,3],[3,4],[1,3]]", result, 3)

    # TEST 6: PROBLEM EXAMPLE 2
    # Input: [[0,5], [0,1], [1,2], [2,3], [3,5], [4,6]]
    # Expected: 4
    # Explanation: Can select [0,1], [1,2], [2,3], [3,5]
    result = maximizeNonOverlappingMeetings([[0, 5], [0, 1], [1, 2], [2, 3], [3, 5], [4, 6]])
    printTest("Test 6: Problem example [[0,5],[0,1],[1,2],[2,3],[3,5],[4,6]]", result, 4)

    # TEST 7: SAME END TIMES
    # Input: [[1,5], [2,5], [3,5], [0,5]]
    # Expected: 1
    # Explanation: All have same end time, only 1 can be selected
    # After sorting: [[0,5], [1,5], [2,5], [3,5]] (tie-break by start)
    result = maximizeNonOverlappingMeetings([[1, 5], [2, 5], [3, 5], [0, 5]])
    printTest("Test 7: Same end times [[1,5],[2,5],[3,5],[0,5]]", result, 1)

    # TEST 8: ADJACENT INTERVALS (TOUCHING)
    # Input: [[1,3], [3,5], [5,7], [7,10]]
    # Expected: 4
    # Explanation: End of one = start of next, all can be selected
    # Because condition is last_end <= curr_start (3 <= 3 is true)
    result = maximizeNonOverlappingMeetings([[1, 3], [3, 5], [5, 7], [7, 10]])
    printTest("Test 8: Adjacent intervals [[1,3],[3,5],[5,7],[7,10]]", result, 4)

    # TEST 9: CONTAINED INTERVALS
    # Input: [[1,20], [2,5], [6,10], [11,15]]
    # Expected: 3
    # Explanation: [2,5], [6,10], [11,15] don't overlap with each other
    # After sorting by end: [[2,5], [6,10], [11,15], [1,20]]
    # Select [2,5] (last_end=5), [6,10] (6>=5, last_end=10), [11,15] (11>=10)
    result = maximizeNonOverlappingMeetings([[1, 20], [2, 5], [6, 10], [11, 15]])
    printTest("Test 9: Contained intervals [[1,20],[2,5],[6,10],[11,15]]", result, 3)

    # TEST 10: REVERSE SORTED BY END
    # Input: [[15,18], [8,10], [5,7], [1,3]]
    # Expected: 4
    # Explanation: After sorting by end: [[1,3], [5,7], [8,10], [15,18]]
    # All are non-overlapping, select all 4
    result = maximizeNonOverlappingMeetings([[15, 18], [8, 10], [5, 7], [1, 3]])
    printTest("Test 10: Reverse sorted [[15,18],[8,10],[5,7],[1,3]]", result, 4)

    # TEST 11: DUPLICATE INTERVALS
    # Input: [[1,5], [1,5], [1,5]]
    # Expected: 1
    # Explanation: All identical, only 1 can be selected
    result = maximizeNonOverlappingMeetings([[1, 5], [1, 5], [1, 5]])
    printTest("Test 11: Duplicate intervals [[1,5],[1,5],[1,5]]", result, 1)

    # TEST 12: LARGE VALUES (NEAR 10^9)
    # Input: [[999999990,999999995], [999999996,1000000000], [1000000001,1000000005]]
    # Expected: 3
    # Explanation: All non-overlapping at boundary values
    result = maximizeNonOverlappingMeetings([[999999990, 999999995], [999999996, 1000000000], [1000000001, 1000000005]])
    printTest("Test 12: Large values near 10^9", result, 3)

    # TEST 13: ZERO START VALUE
    # Input: [[0,2], [1,4], [3,6], [5,8]]
    # Expected: 2
    # Explanation: After sorting by end: [[0,2], [1,4], [3,6], [5,8]]
    # Select [0,2], skip [1,4] (1<2), select [3,6] (3>=2), skip [5,8] (5<6)
    result = maximizeNonOverlappingMeetings([[0, 2], [1, 4], [3, 6], [5, 8]])
    printTest("Test 13: Zero start value [[0,2],[1,4],[3,6],[5,8]]", result, 2)

    # TEST 14: SAME START, DIFFERENT ENDS
    # Input: [[1,10], [1,5], [1,3], [1,8]]
    # Expected: 1
    # Explanation: All start at 1, after sorting by end: [[1,3], [1,5], [1,8], [1,10]]
    # Select [1,3], all others start at 1 which is < last_end=3
    result = maximizeNonOverlappingMeetings([[1, 10], [1, 5], [1, 3], [1, 8]])
    printTest("Test 14: Same start, different ends [[1,10],[1,5],[1,3],[1,8]]", result, 1)

    # TEST 15: COMPLEX MIXED SCENARIO
    # Input: [[1,2], [2,3], [1,3], [3,4], [2,4], [4,5], [1,5]]
    # Expected: 4
    # Explanation: After sorting by end: [[1,2], [2,3], [1,3], [3,4], [2,4], [4,5], [1,5]]
    # Select [1,2] (last_end=2)
    # Select [2,3] (2>=2, last_end=3)
    # Skip [1,3] (1<3)
    # Select [3,4] (3>=3, last_end=4)
    # Skip [2,4] (2<4)
    # Select [4,5] (4>=4, last_end=5)
    # Skip [1,5] (1<5)
    # Total: 4
    result = maximizeNonOverlappingMeetings([[1, 2], [2, 3], [1, 3], [3, 4], [2, 4], [4, 5], [1, 5]])
    printTest("Test 15: Complex mixed [[1,2],[2,3],[1,3],[3,4],[2,4],[4,5],[1,5]]", result, 4)

    print("\n=== ALL TESTS COMPLETED ===")
    print("\nKEY ALGORITHM POINTS:")
    print("- Greedy approach: Sort by end time")
    print("- Always select interval with earliest end time")
    print("- Skip overlapping intervals")
    print("- Time complexity: O(n log n) for sorting")
    print("- Space complexity: O(1) for in-place algorithm")


if __name__ == "__main__":
    main()
