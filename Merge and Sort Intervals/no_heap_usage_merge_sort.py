"""
================================================================================
MERGE INTERVALS - IN-PLACE VERSION (Python)
================================================================================

PROBLEM STATEMENT:
Merge overlapping intervals by modifying the input list in-place.
This version mimics the C implementation that avoids heap allocation.

ALGORITHM APPROACH:
1. Sort intervals in-place by start time
2. Merge overlapping intervals using two-pointer technique
3. Return the modified list with merged intervals at the beginning

KEY FEATURES:
- Modifies input list in-place
- Memory-efficient approach
- Returns slice of original list containing merged intervals

TIME COMPLEXITY: O(n log n) - dominated by sorting
SPACE COMPLEXITY: O(1) - only uses constant extra space (ignoring sort space)

ADVANTAGES:
- Minimal memory allocation
- In-place algorithm
- Efficient for large datasets

LIMITATIONS:
- Modifies original input list
- Less flexible than creating new list

================================================================================
"""


def mergeHighDefinitionIntervals(intervals):
    """
    Merge overlapping intervals in-place using two-pointer technique.

    ALGORITHM:
    1. Sort intervals by start time (and by end time as tie-breaker)
    2. Use write pointer for last merged interval position
    3. Use read pointer to scan through remaining intervals
    4. Merge overlapping intervals by extending write interval
    5. For non-overlapping intervals, advance write and copy

    PARAMETERS:
    intervals - List of [start, end] interval pairs (MODIFIED IN-PLACE)

    RETURN:
    List of merged intervals (slice of input list)
    Returns empty list on invalid input

    EXAMPLES:
    >>> mergeHighDefinitionIntervals([[1,3], [2,6], [8,10], [15,18]])
    [[1, 6], [8, 10], [15, 18]]
    >>> mergeHighDefinitionIntervals([[1,4], [4,5]])
    [[1, 5]]
    """
    # Validate input
    if not intervals or len(intervals) == 0:
        return []

    # PHASE 1: SORT INTERVALS IN-PLACE
    # Sort by start time (primary), then by end time (tie-breaker)
    # Python's sort is stable and sorts tuples/lists element by element
    intervals.sort(key=lambda x: (x[0], x[1]))

    # PHASE 2: MERGE IN-PLACE USING TWO-POINTER TECHNIQUE
    # 'write' - index of last merged interval (slow pointer)
    # 'read'  - index of current interval being processed (fast pointer)
    #
    # Example walkthrough:
    # Input (sorted): [[1,3], [2,6], [8,10], [15,18]]
    #
    # Initial: write=0 (pointing to [1,3])
    #
    # read=1: [2,6]
    #   - 2 <= 3 (overlap) → extend write: [1,6]
    #   - write stays at 0
    #
    # read=2: [8,10]
    #   - 8 > 6 (no overlap) → advance write to 1
    #   - Copy [8,10] to intervals[1]
    #
    # read=3: [15,18]
    #   - 15 > 10 (no overlap) → advance write to 2
    #   - Copy [15,18] to intervals[2]
    #
    # Result: [[1,6], [8,10], [15,18]] (write+1 = 3 intervals)

    write = 0  # Index of last merged interval

    # Process each interval starting from second one
    for read in range(1, len(intervals)):
        cur_start = intervals[read][0]  # Current interval start
        cur_end = intervals[read][1]    # Current interval end

        last_end = intervals[write][1]  # Last merged interval's end

        if cur_start <= last_end:
            # CASE 1: OVERLAP
            # Current interval overlaps with last merged interval
            # Extend the end of last merged interval if needed
            if cur_end > last_end:
                intervals[write][1] = cur_end
            # Note: We don't advance 'write' - keep merging into same interval
        else:
            # CASE 2: NO OVERLAP
            # Current interval doesn't overlap - start a new merged interval
            write += 1  # Move to next slot
            intervals[write] = intervals[read]  # Copy reference to current interval

    # Return slice containing merged intervals
    # Merged intervals are in intervals[0..write]
    return intervals[:write + 1]


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
    print(f"  Result: {result} {status}")
    print(f"  Expected: {expected}")


def main():
    """
    Main function - Comprehensive Test Suite

    Tests the mergeHighDefinitionIntervals algorithm with 15 test cases
    covering edge cases, overlapping scenarios, and boundary conditions

    TEST CATEGORIES:
    1. Edge Cases: Empty, single interval
    2. Overlapping Scenarios: All merge, partial merge
    3. Non-overlapping: Separate intervals
    4. Special Cases: Adjacent, contained, duplicates
    5. Boundary Values: Zero, large values
    """
    print("=== MERGE INTERVALS - IN-PLACE TESTS ===")

    # TEST 1: EMPTY ARRAY
    # Input: []
    # Expected: []
    result = mergeHighDefinitionIntervals([])
    printTest("Test 1: Empty array", result, [])

    # TEST 2: SINGLE INTERVAL
    # Input: [[1,5]]
    # Expected: [[1,5]]
    result = mergeHighDefinitionIntervals([[1, 5]])
    printTest("Test 2: Single interval [[1,5]]", result, [[1, 5]])

    # TEST 3: TWO OVERLAPPING INTERVALS
    # Input: [[1,3], [2,6]]
    # Expected: [[1,6]]
    result = mergeHighDefinitionIntervals([[1, 3], [2, 6]])
    printTest("Test 3: Two overlapping [[1,3], [2,6]]", result, [[1, 6]])

    # TEST 4: TWO NON-OVERLAPPING INTERVALS
    # Input: [[1,2], [5,10]]
    # Expected: [[1,2], [5,10]]
    result = mergeHighDefinitionIntervals([[1, 2], [5, 10]])
    printTest("Test 4: Two non-overlapping [[1,2], [5,10]]", result, [[1, 2], [5, 10]])

    # TEST 5: ALL INTERVALS MERGE INTO ONE
    # Input: [[1,4], [2,5], [3,6], [4,10]]
    # Expected: [[1,10]]
    result = mergeHighDefinitionIntervals([[1, 4], [2, 5], [3, 6], [4, 10]])
    printTest("Test 5: All merge into one [[1,4],[2,5],[3,6],[4,10]]", result, [[1, 10]])

    # TEST 6: NO OVERLAPPING INTERVALS
    # Input: [[1,2], [3,4], [5,6], [7,8]]
    # Expected: [[1,2], [3,4], [5,6], [7,8]]
    result = mergeHighDefinitionIntervals([[1, 2], [3, 4], [5, 6], [7, 8]])
    printTest("Test 6: No overlapping [[1,2],[3,4],[5,6],[7,8]]", result, [[1, 2], [3, 4], [5, 6], [7, 8]])

    # TEST 7: DUPLICATE INTERVALS
    # Input: [[1,5], [1,5], [1,5]]
    # Expected: [[1,5]]
    result = mergeHighDefinitionIntervals([[1, 5], [1, 5], [1, 5]])
    printTest("Test 7: Duplicate intervals [[1,5],[1,5],[1,5]]", result, [[1, 5]])

    # TEST 8: CONTAINED INTERVAL
    # Input: [[1,10], [3,5]]
    # Expected: [[1,10]]
    result = mergeHighDefinitionIntervals([[1, 10], [3, 5]])
    printTest("Test 8: Contained interval [[1,10], [3,5]]", result, [[1, 10]])

    # TEST 9: ADJACENT INTERVALS (TOUCHING)
    # Input: [[1,3], [3,5], [5,10]]
    # Expected: [[1,10]]
    # Explanation: Touching intervals are merged (3 <= 3, 5 <= 5)
    result = mergeHighDefinitionIntervals([[1, 3], [3, 5], [5, 10]])
    printTest("Test 9: Adjacent intervals [[1,3],[3,5],[5,10]]", result, [[1, 10]])

    # TEST 10: REVERSE SORTED INTERVALS
    # Input: [[15,18], [8,10], [2,6], [1,3]]
    # Expected: [[1,6], [8,10], [15,18]]
    result = mergeHighDefinitionIntervals([[15, 18], [8, 10], [2, 6], [1, 3]])
    printTest("Test 10: Reverse sorted [[15,18],[8,10],[2,6],[1,3]]", result, [[1, 6], [8, 10], [15, 18]])

    # TEST 11: SAME START, DIFFERENT ENDS
    # Input: [[1,5], [1,10], [1,3]]
    # Expected: [[1,10]]
    result = mergeHighDefinitionIntervals([[1, 5], [1, 10], [1, 3]])
    printTest("Test 11: Same start, different ends [[1,5],[1,10],[1,3]]", result, [[1, 10]])

    # TEST 12: LARGE VALUES (NEAR 10^9)
    # Input: [[1000000000,1000000001], [999999999,1000000000], [1000000002,1000000005]]
    # Expected: [[999999999,1000000001], [1000000002,1000000005]]
    result = mergeHighDefinitionIntervals([[1000000000, 1000000001], [999999999, 1000000000], [1000000002, 1000000005]])
    printTest("Test 12: Large values near 10^9", result, [[999999999, 1000000001], [1000000002, 1000000005]])

    # TEST 13: ZERO START VALUE
    # Input: [[0,5], [3,10]]
    # Expected: [[0,10]]
    result = mergeHighDefinitionIntervals([[0, 5], [3, 10]])
    printTest("Test 13: Zero start value [[0,5],[3,10]]", result, [[0, 10]])

    # TEST 14: PROBLEM EXAMPLE
    # Input: [[1,3], [2,6], [8,10], [15,18]]
    # Expected: [[1,6], [8,10], [15,18]]
    result = mergeHighDefinitionIntervals([[1, 3], [2, 6], [8, 10], [15, 18]])
    printTest("Test 14: Problem example [[1,3],[2,6],[8,10],[15,18]]", result, [[1, 6], [8, 10], [15, 18]])

    # TEST 15: MULTIPLE CONTAINED INTERVALS
    # Input: [[1,20], [3,5], [7,9], [10,15], [2,4]]
    # Expected: [[1,20]]
    result = mergeHighDefinitionIntervals([[1, 20], [3, 5], [7, 9], [10, 15], [2, 4]])
    printTest("Test 15: Multiple contained [[1,20],[3,5],[7,9],[10,15],[2,4]]", result, [[1, 20]])

    print("\n=== ALL TESTS COMPLETED ===")
    print("\nKEY ADVANTAGES OF THIS VERSION:")
    print("- In-place algorithm (modifies input)")
    print("- Minimal memory allocation")
    print("- O(n log n) time complexity (sorting)")
    print("- O(1) space complexity (ignoring sort)")
    print("- Efficient for large datasets")


if __name__ == "__main__":
    main()
