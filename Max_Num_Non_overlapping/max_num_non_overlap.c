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
 
#include <stdio.h>
#include <stdlib.h>
/*
 * Complete the 'maximizeNonOverlappingMeetings' function below.
 *
 * The function is expected to return an INTEGER.
 * The function accepts 2D_INTEGER_ARRAY meetings as parameter.
 */
static int increase_sort(const void *a, const void *b){

    const int* ra = *(const int* const*)a; 
    const int* rb = *(const int* const*)b;
    
    //Sort End portion first 
    if(ra[1] < rb[1]) return -1; 
    if(ra[1] > rb[1]) return 1; 
    
    //Tie-break: Start portion
    //only happens if two ends are the same  sort the start
    if(ra[0] < rb[0])return -1;  
    if(ra[0] > rb[0]) return 1; 
    

    
    return 0; 
}

int maximizeNonOverlappingMeetings(int meetings_rows, int meetings_columns, int** meetings) {
    
    //edge cases
    if( meetings_rows <= 0 ||meetings_columns <=0 || !meetings) return 0; 
    
    //sort the array in increasing order to access all non-overlapping
    qsort(meetings, (size_t)meetings_rows, (size_t)sizeof(int*),increase_sort );
    
    //start at 1 because after sorting you usually take the first interval
    int count = 1;  
    int lastE = meetings[0][1];
    //Process the end times 
    for(int read = 1; read < meetings_rows; read++){
        int curS = meetings[read][0]; 
        int curE = meetings[read][1];
        //Non-overlapping condition
        if(lastE <= curS){
            count++;
            //This way we can update the variable
            lastE = curE;  
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
void printTest(const char* testName, int result, int expected) {
    printf("\n%s:\n", testName);
    printf("  Result: %d", result);
    if (result == expected) {
        printf(" PASS (Expected: %d)\n", expected);
    } else {
        printf(" FAIL (Expected: %d)\n", expected);
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
int main(void) {
    int result;

    printf("=== MAXIMUM NON-OVERLAPPING INTERVALS TESTS ===\n");

    /*
     * TEST 1: EMPTY ARRAY
     * Input: []
     * Expected: 0
     * Purpose: Edge case - no intervals
     */
    result = maximizeNonOverlappingMeetings(0, 2, NULL);
    printTest("Test 1: Empty array", result, 0);

    /*
     * TEST 2: SINGLE INTERVAL
     * Input: [[1,5]]
     * Expected: 1
     * Purpose: Minimum valid input - one interval always selected
     */
    int row2_0[] = {1, 5};
    int* test2[] = {row2_0};
    result = maximizeNonOverlappingMeetings(1, 2, test2);
    printTest("Test 2: Single interval [[1,5]]", result, 1);

    /*
     * TEST 3: ALL OVERLAPPING
     * Input: [[1,10], [2,5], [3,8], [4,9]]
     * Expected: 1
     * Explanation: All intervals overlap with [1,10], only 1 can be selected
     * After sorting by end: [[2,5], [3,8], [4,9], [1,10]]
     * Select [2,5], all others overlap with it
     */
    int row3_0[] = {1, 10};
    int row3_1[] = {2, 5};
    int row3_2[] = {3, 8};
    int row3_3[] = {4, 9};
    int* test3[] = {row3_0, row3_1, row3_2, row3_3};
    result = maximizeNonOverlappingMeetings(4, 2, test3);
    printTest("Test 3: All overlapping [[1,10],[2,5],[3,8],[4,9]]", result, 1);

    /*
     * TEST 4: NO OVERLAPPING
     * Input: [[1,2], [3,4], [5,6], [7,8]]
     * Expected: 4
     * Explanation: All intervals are separate, all can be selected
     */
    int row4_0[] = {1, 2};
    int row4_1[] = {3, 4};
    int row4_2[] = {5, 6};
    int row4_3[] = {7, 8};
    int* test4[] = {row4_0, row4_1, row4_2, row4_3};
    result = maximizeNonOverlappingMeetings(4, 2, test4);
    printTest("Test 4: No overlapping [[1,2],[3,4],[5,6],[7,8]]", result, 4);

    /*
     * TEST 5: PROBLEM EXAMPLE 1
     * Input: [[1,2], [2,3], [3,4], [1,3]]
     * Expected: 3
     * Explanation: Can select [1,2], [2,3], [3,4]
     */
    int row5_0[] = {1, 2};
    int row5_1[] = {2, 3};
    int row5_2[] = {3, 4};
    int row5_3[] = {1, 3};
    int* test5[] = {row5_0, row5_1, row5_2, row5_3};
    result = maximizeNonOverlappingMeetings(4, 2, test5);
    printTest("Test 5: Problem example [[1,2],[2,3],[3,4],[1,3]]", result, 3);

    /*
     * TEST 6: PROBLEM EXAMPLE 2
     * Input: [[0,5], [0,1], [1,2], [2,3], [3,5], [4,6]]
     * Expected: 4
     * Explanation: Can select [0,1], [1,2], [2,3], [3,5]
     */
    int row6_0[] = {0, 5};
    int row6_1[] = {0, 1};
    int row6_2[] = {1, 2};
    int row6_3[] = {2, 3};
    int row6_4[] = {3, 5};
    int row6_5[] = {4, 6};
    int* test6[] = {row6_0, row6_1, row6_2, row6_3, row6_4, row6_5};
    result = maximizeNonOverlappingMeetings(6, 2, test6);
    printTest("Test 6: Problem example [[0,5],[0,1],[1,2],[2,3],[3,5],[4,6]]", result, 4);

    /*
     * TEST 7: SAME END TIMES
     * Input: [[1,5], [2,5], [3,5], [0,5]]
     * Expected: 1
     * Explanation: All have same end time, only 1 can be selected
     * After sorting: [[0,5], [1,5], [2,5], [3,5]] (tie-break by start)
     * Select [0,5], all others start after lastE=5 is not satisfied
     */
    int row7_0[] = {1, 5};
    int row7_1[] = {2, 5};
    int row7_2[] = {3, 5};
    int row7_3[] = {0, 5};
    int* test7[] = {row7_0, row7_1, row7_2, row7_3};
    result = maximizeNonOverlappingMeetings(4, 2, test7);
    printTest("Test 7: Same end times [[1,5],[2,5],[3,5],[0,5]]", result, 1);

    /*
     * TEST 8: ADJACENT INTERVALS (TOUCHING)
     * Input: [[1,3], [3,5], [5,7], [7,10]]
     * Expected: 4
     * Explanation: End of one = start of next, all can be selected
     * Because condition is lastE <= curS (3 <= 3 is true)
     */
    int row8_0[] = {1, 3};
    int row8_1[] = {3, 5};
    int row8_2[] = {5, 7};
    int row8_3[] = {7, 10};
    int* test8[] = {row8_0, row8_1, row8_2, row8_3};
    result = maximizeNonOverlappingMeetings(4, 2, test8);
    printTest("Test 8: Adjacent intervals [[1,3],[3,5],[5,7],[7,10]]", result, 4);

    /*
     * TEST 9: CONTAINED INTERVALS
     * Input: [[1,20], [2,5], [6,10], [11,15]]
     * Expected: 3
     * Explanation: [2,5], [6,10], [11,15] don't overlap with each other
     * After sorting by end: [[2,5], [6,10], [11,15], [1,20]]
     * Select [2,5] (lastE=5), [6,10] (6>=5, lastE=10), [11,15] (11>=10)
     */
    int row9_0[] = {1, 20};
    int row9_1[] = {2, 5};
    int row9_2[] = {6, 10};
    int row9_3[] = {11, 15};
    int* test9[] = {row9_0, row9_1, row9_2, row9_3};
    result = maximizeNonOverlappingMeetings(4, 2, test9);
    printTest("Test 9: Contained intervals [[1,20],[2,5],[6,10],[11,15]]", result, 3);

    /*
     * TEST 10: REVERSE SORTED BY END
     * Input: [[15,18], [8,10], [5,7], [1,3]]
     * Expected: 4
     * Explanation: After sorting by end: [[1,3], [5,7], [8,10], [15,18]]
     * All are non-overlapping, select all 4
     */
    int row10_0[] = {15, 18};
    int row10_1[] = {8, 10};
    int row10_2[] = {5, 7};
    int row10_3[] = {1, 3};
    int* test10[] = {row10_0, row10_1, row10_2, row10_3};
    result = maximizeNonOverlappingMeetings(4, 2, test10);
    printTest("Test 10: Reverse sorted [[15,18],[8,10],[5,7],[1,3]]", result, 4);

    /*
     * TEST 11: DUPLICATE INTERVALS
     * Input: [[1,5], [1,5], [1,5]]
     * Expected: 1
     * Explanation: All identical, only 1 can be selected
     */
    int row11_0[] = {1, 5};
    int row11_1[] = {1, 5};
    int row11_2[] = {1, 5};
    int* test11[] = {row11_0, row11_1, row11_2};
    result = maximizeNonOverlappingMeetings(3, 2, test11);
    printTest("Test 11: Duplicate intervals [[1,5],[1,5],[1,5]]", result, 1);

    /*
     * TEST 12: LARGE VALUES (NEAR 10^9)
     * Input: [[999999990,999999995], [999999996,1000000000], [1000000001,1000000005]]
     * Expected: 3
     * Explanation: All non-overlapping at boundary values
     */
    int row12_0[] = {999999990, 999999995};
    int row12_1[] = {999999996, 1000000000};
    int row12_2[] = {1000000001, 1000000005};
    int* test12[] = {row12_0, row12_1, row12_2};
    result = maximizeNonOverlappingMeetings(3, 2, test12);
    printTest("Test 12: Large values near 10^9", result, 3);

    /*
     * TEST 13: ZERO START VALUE
     * Input: [[0,2], [1,4], [3,6], [5,8]]
     * Expected: 3
     * Explanation: After sorting by end: [[0,2], [1,4], [3,6], [5,8]]
     * Select [0,2] (lastE=2), [3,6] (3>=2, lastE=6), skip [1,4] and [5,8]
     * Actually after sorting: [[0,2], [1,4], [3,6], [5,8]]
     * Select [0,2], skip [1,4] (1<2), select [3,6] (3>=2), skip [5,8] (5<6)
     */
    int row13_0[] = {0, 2};
    int row13_1[] = {1, 4};
    int row13_2[] = {3, 6};
    int row13_3[] = {5, 8};
    int* test13[] = {row13_0, row13_1, row13_2, row13_3};
    result = maximizeNonOverlappingMeetings(4, 2, test13);
    printTest("Test 13: Zero start value [[0,2],[1,4],[3,6],[5,8]]", result, 2);

    /*
     * TEST 14: SAME START, DIFFERENT ENDS
     * Input: [[1,10], [1,5], [1,3], [1,8]]
     * Expected: 1
     * Explanation: All start at 1, after sorting by end: [[1,3], [1,5], [1,8], [1,10]]
     * Select [1,3], all others start at 1 which is < lastE=3
     */
    int row14_0[] = {1, 10};
    int row14_1[] = {1, 5};
    int row14_2[] = {1, 3};
    int row14_3[] = {1, 8};
    int* test14[] = {row14_0, row14_1, row14_2, row14_3};
    result = maximizeNonOverlappingMeetings(4, 2, test14);
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
    int row15_0[] = {1, 2};
    int row15_1[] = {2, 3};
    int row15_2[] = {1, 3};
    int row15_3[] = {3, 4};
    int row15_4[] = {2, 4};
    int row15_5[] = {4, 5};
    int row15_6[] = {1, 5};
    int* test15[] = {row15_0, row15_1, row15_2, row15_3, row15_4, row15_5, row15_6};
    result = maximizeNonOverlappingMeetings(7, 2, test15);
    printTest("Test 15: Complex mixed [[1,2],[2,3],[1,3],[3,4],[2,4],[4,5],[1,5]]", result, 4);

    printf("\n=== ALL TESTS COMPLETED ===\n");
    printf("\nKEY ALGORITHM POINTS:\n");
    printf("- Greedy approach: Sort by end time\n");
    printf("- Always select interval with earliest end time\n");
    printf("- Skip overlapping intervals\n");
    printf("- Time complexity: O(n log n) for sorting\n");
    printf("- Space complexity: O(1) for in-place algorithm\n");

    return 0;
}