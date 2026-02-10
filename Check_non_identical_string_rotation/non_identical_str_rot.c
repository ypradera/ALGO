/*Check for Non-Identical String Rotation
Given two strings s1 and s2, return 1 if s2 is a rotation of s1 but not identical to s1, otherwise return 0.
Example
Input:
s1 = abcde
s2 = cdeab
Output:
True
Explanation:
- s2 ('cdeab') is a non-trivial rotation of s1 ('abcde'). 
- If you rotate 'abcde' left by 2 positions, you get 'cdeab'. 
- Since s2 is not equal to s1 and is a rotation, the output is true.
Input Format
	• The first line contains the string s1, followed by s2 on the next line.
Constraints
	• 1 <= |s1| <= 1000
	• 1 <= |s2| <= 1000
	• |s1| = |s2|
	• s1 & s2 both consists of lowercase English letters ('a'-'z') only
Output Format
	• The function returns a single BOOLEAN value, 1 for True and 0 for False
Sample Input 0
a
a
Sample Output 0
0
Sample Input 1
a
b
Sample Output 1
0*/

/*
 * Complete the 'isNonTrivialRotation' function below.
 *
 * The function is expected to return a BOOLEAN.
 * The function accepts following parameters:
 *  1. STRING s1
 *  2. STRING s2
 */


 #include <stdio.h>
 #include <stdlib.h>   
 #include <stdbool.h>
 #include <string.h>
bool isNonTrivialRotation(char* s1, char* s2) {
    //corner cases 
    //strcmp returns 0 if equal
    if(!(strcmp(s1, s2))) return false; 
    int len_s1 = strlen(s1);
    int len_s2 = strlen(s2);
    if((len_s1!=len_s2 )||(len_s1 == 0)||(len_s2==0)) return false;
    
    //OPTION 1 
    /*
    int idx_keeper = len_s1 -1; 
    //rotate each char by char to the right  and strcmp to s2 each time 
    for(int i = 0; i < len_s1; i++){
        char tmp = s1[0]; 
        //void *memmove(void *dest, const void *src, size_t n);
        memmove(s1,s1+1, (size_t)len_s1 - 1);  //shift left
        
          //dest: where to copy t0
          //src:  where to copy from
          //n:    how many bytes to copy
        
        s1[len_s1 -1] = tmp; //Add the rotating char to the end
        //strcmp returns 0 when equal 
        if(!(strcmp(s1, s2))) return true;
    }
    
    
    return false; 
    */
    
    //OPTION 2
    //use of C library function strstr()
    // allocate enpugh memory to hold both of the strings and '\0'
    char* dbl_str = malloc(((len_s1*2)+1));
    if (!dbl_str) return false;
    //build the double s1 string 
    //memcpy(void *, const void *, unsigned long)
    memcpy(dbl_str, s1, len_s1);
    memcpy(dbl_str+len_s1, s1, len_s1);
    //add the NULL char at the end '\0'
    dbl_str[2*len_s1] = '\0';
    //strstr(const char *, const char *)
    //strstr return NULL if s2 is not in dbl_str
    bool ret = (strstr(dbl_str, s2) != NULL);
    free(dbl_str);
    dbl_str = NULL;
    return ret;
}

/*
 * Print Test Result
 *
 * PURPOSE:
 * Displays test case information and result
 *
 * PARAMETERS:
 * @param testName - Description of test case
 * @param result   - Actual result from function
 * @param expected - Expected result
 */
void printTest(const char* testName, bool result, bool expected) {
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
 * Tests the isNonTrivialRotation algorithm with 15 test cases
 * covering edge cases, various rotation scenarios, and boundary conditions
 *
 * TEST CATEGORIES:
 * 1. Edge Cases: Empty, single character, identical strings
 * 2. Valid Rotations: Various rotation positions
 * 3. Invalid Cases: Different lengths, non-rotations
 * 4. Special Cases: Repeated patterns, full rotations
 * 5. Boundary Values: Long strings, all same character
 */
int main(void) {
    bool result;

    printf("=== NON-IDENTICAL STRING ROTATION TESTS ===\n");

    /*
     * TEST 1: IDENTICAL STRINGS
     * Input: s1="abcde", s2="abcde"
     * Expected: 0 (false)
     * Purpose: Strings are identical, not a non-trivial rotation
     */
    result = isNonTrivialRotation("abcde", "abcde");
    printTest("Test 1: Identical strings \"abcde\", \"abcde\"", result, 0);

    /*
     * TEST 2: PROBLEM EXAMPLE
     * Input: s1="abcde", s2="cdeab"
     * Expected: 1 (true)
     * Explanation: "cdeab" is "abcde" rotated left by 2 positions
     */
    result = isNonTrivialRotation("abcde", "cdeab");
    printTest("Test 2: Problem example \"abcde\", \"cdeab\"", result, 1);

    /*
     * TEST 3: SINGLE CHARACTER IDENTICAL
     * Input: s1="a", s2="a"
     * Expected: 0 (false)
     * Purpose: Single character, identical strings
     */
    result = isNonTrivialRotation("a", "a");
    printTest("Test 3: Single character identical \"a\", \"a\"", result, 0);

    /*
     * TEST 4: SINGLE CHARACTER DIFFERENT
     * Input: s1="a", s2="b"
     * Expected: 0 (false)
     * Purpose: Single characters that are different (not a rotation)
     */
    result = isNonTrivialRotation("a", "b");
    printTest("Test 4: Single character different \"a\", \"b\"", result, 0);

    /*
     * TEST 5: ROTATION BY ONE POSITION
     * Input: s1="abc", s2="bca"
     * Expected: 1 (true)
     * Explanation: "bca" is "abc" rotated left by 1 position
     */
    result = isNonTrivialRotation("abc", "bca");
    printTest("Test 5: Rotation by one \"abc\", \"bca\"", result, 1);

    /*
     * TEST 6: ROTATION BY TWO POSITIONS
     * Input: s1="abc", s2="cab"
     * Expected: 1 (true)
     * Explanation: "cab" is "abc" rotated left by 2 positions
     */
    result = isNonTrivialRotation("abc", "cab");
    printTest("Test 6: Rotation by two \"abc\", \"cab\"", result, 1);

    /*
     * TEST 7: NOT A ROTATION
     * Input: s1="abc", s2="acb"
     * Expected: 0 (false)
     * Purpose: Same characters but different order (not a rotation)
     */
    result = isNonTrivialRotation("abc", "acb");
    printTest("Test 7: Not a rotation \"abc\", \"acb\"", result, 0);

    /*
     * TEST 8: DIFFERENT LENGTHS
     * Input: s1="abc", s2="abcd"
     * Expected: 0 (false)
     * Purpose: Different lengths cannot be rotations
     */
    result = isNonTrivialRotation("abc", "abcd");
    printTest("Test 8: Different lengths \"abc\", \"abcd\"", result, 0);

    /*
     * TEST 9: REPEATED PATTERN ROTATION
     * Input: s1="abab", s2="baba"
     * Expected: 1 (true)
     * Explanation: "baba" is "abab" rotated left by 2 positions
     */
    result = isNonTrivialRotation("abab", "baba");
    printTest("Test 9: Repeated pattern \"abab\", \"baba\"", result, 1);

    /*
     * TEST 10: ALL SAME CHARACTER
     * Input: s1="aaaa", s2="aaaa"
     * Expected: 0 (false)
     * Purpose: All same character, strings are identical
     */
    result = isNonTrivialRotation("aaaa", "aaaa");
    printTest("Test 10: All same character \"aaaa\", \"aaaa\"", result, 0);

    /*
     * TEST 11: TWO CHARACTER ROTATION
     * Input: s1="ab", s2="ba"
     * Expected: 1 (true)
     * Explanation: "ba" is "ab" rotated left by 1 position
     */
    result = isNonTrivialRotation("ab", "ba");
    printTest("Test 11: Two character rotation \"ab\", \"ba\"", result, 1);

    /*
     * TEST 12: LONG STRING ROTATION
     * Input: s1="abcdefghij", s2="fghijabcde"
     * Expected: 1 (true)
     * Explanation: "fghijabcde" is "abcdefghij" rotated left by 5 positions
     */
    result = isNonTrivialRotation("abcdefghij", "fghijabcde");
    printTest("Test 12: Long string rotation \"abcdefghij\", \"fghijabcde\"", result, 1);

    /*
     * TEST 13: LONG STRING NOT ROTATION
     * Input: s1="abcdefghij", s2="abcdefghji"
     * Expected: 0 (false)
     * Purpose: Similar but last two characters swapped (not a rotation)
     */
    result = isNonTrivialRotation("abcdefghij", "abcdefghji");
    printTest("Test 13: Long string not rotation \"abcdefghij\", \"abcdefghji\"", result, 0);

    /*
     * TEST 14: ROTATION BY LAST POSITION
     * Input: s1="hello", s2="ohell"
     * Expected: 1 (true)
     * Explanation: "ohell" is "hello" rotated right by 1 position
     */
    result = isNonTrivialRotation("hello", "ohell");
    printTest("Test 14: Rotation by last position \"hello\", \"ohell\"", result, 1);

    /*
     * TEST 15: PALINDROME ROTATION
     * Input: s1="racecar", s2="carrace"
     * Expected: 1 (true)
     * Explanation: "carrace" is "racecar" rotated left by 4 positions
     */
    result = isNonTrivialRotation("racecar", "carrace");
    printTest("Test 15: Palindrome rotation \"racecar\", \"carrace\"", result, 1);

    printf("\n=== ALL TESTS COMPLETED ===\n");
    printf("\nKEY ALGORITHM POINTS:\n");
    printf("- Check strings are not identical\n");
    printf("- Check strings have same length\n");
    printf("- Concatenate s1 with itself: s1+s1\n");
    printf("- Check if s2 is substring of (s1+s1)\n");
    printf("- Time complexity: O(n) using strstr()\n");
    printf("- Space complexity: O(n) for concatenated string\n");

    return 0;
}
