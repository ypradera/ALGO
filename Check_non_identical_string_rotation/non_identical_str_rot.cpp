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

#include <iostream>
#include <string>

/*
 * Complete the 'isNonTrivialRotation' function below.
 *
 * The function is expected to return a BOOLEAN.
 * The function accepts following parameters:
 *  1. STRING s1
 *  2. STRING s2
 */
bool isNonTrivialRotation(const std::string& s1, const std::string& s2) {
    // Check if strings are identical
    if (s1 == s2) {
        return false;
    }

    // Check if strings have same length and are not empty
    if (s1.length() != s2.length() || s1.empty()) {
        return false;
    }

    // Create double string and check if s2 is a substring
    // If s2 is a rotation of s1, it will be in s1+s1
    std::string double_str = s1 + s1;
    return double_str.find(s2) != std::string::npos;
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
void printTest(const std::string& testName, bool result, bool expected) {
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
int main() {
    bool result;

    std::cout << "=== NON-IDENTICAL STRING ROTATION TESTS ===\n";

    /*
     * TEST 1: IDENTICAL STRINGS
     * Input: s1="abcde", s2="abcde"
     * Expected: 0 (false)
     * Purpose: Strings are identical, not a non-trivial rotation
     */
    result = isNonTrivialRotation("abcde", "abcde");
    printTest("Test 1: Identical strings \"abcde\", \"abcde\"", result, false);

    /*
     * TEST 2: PROBLEM EXAMPLE
     * Input: s1="abcde", s2="cdeab"
     * Expected: 1 (true)
     * Explanation: "cdeab" is "abcde" rotated left by 2 positions
     */
    result = isNonTrivialRotation("abcde", "cdeab");
    printTest("Test 2: Problem example \"abcde\", \"cdeab\"", result, true);

    /*
     * TEST 3: SINGLE CHARACTER IDENTICAL
     * Input: s1="a", s2="a"
     * Expected: 0 (false)
     * Purpose: Single character, identical strings
     */
    result = isNonTrivialRotation("a", "a");
    printTest("Test 3: Single character identical \"a\", \"a\"", result, false);

    /*
     * TEST 4: SINGLE CHARACTER DIFFERENT
     * Input: s1="a", s2="b"
     * Expected: 0 (false)
     * Purpose: Single characters that are different (not a rotation)
     */
    result = isNonTrivialRotation("a", "b");
    printTest("Test 4: Single character different \"a\", \"b\"", result, false);

    /*
     * TEST 5: ROTATION BY ONE POSITION
     * Input: s1="abc", s2="bca"
     * Expected: 1 (true)
     * Explanation: "bca" is "abc" rotated left by 1 position
     */
    result = isNonTrivialRotation("abc", "bca");
    printTest("Test 5: Rotation by one \"abc\", \"bca\"", result, true);

    /*
     * TEST 6: ROTATION BY TWO POSITIONS
     * Input: s1="abc", s2="cab"
     * Expected: 1 (true)
     * Explanation: "cab" is "abc" rotated left by 2 positions
     */
    result = isNonTrivialRotation("abc", "cab");
    printTest("Test 6: Rotation by two \"abc\", \"cab\"", result, true);

    /*
     * TEST 7: NOT A ROTATION
     * Input: s1="abc", s2="acb"
     * Expected: 0 (false)
     * Purpose: Same characters but different order (not a rotation)
     */
    result = isNonTrivialRotation("abc", "acb");
    printTest("Test 7: Not a rotation \"abc\", \"acb\"", result, false);

    /*
     * TEST 8: DIFFERENT LENGTHS
     * Input: s1="abc", s2="abcd"
     * Expected: 0 (false)
     * Purpose: Different lengths cannot be rotations
     */
    result = isNonTrivialRotation("abc", "abcd");
    printTest("Test 8: Different lengths \"abc\", \"abcd\"", result, false);

    /*
     * TEST 9: REPEATED PATTERN ROTATION
     * Input: s1="abab", s2="baba"
     * Expected: 1 (true)
     * Explanation: "baba" is "abab" rotated left by 2 positions
     */
    result = isNonTrivialRotation("abab", "baba");
    printTest("Test 9: Repeated pattern \"abab\", \"baba\"", result, true);

    /*
     * TEST 10: ALL SAME CHARACTER
     * Input: s1="aaaa", s2="aaaa"
     * Expected: 0 (false)
     * Purpose: All same character, strings are identical
     */
    result = isNonTrivialRotation("aaaa", "aaaa");
    printTest("Test 10: All same character \"aaaa\", \"aaaa\"", result, false);

    /*
     * TEST 11: TWO CHARACTER ROTATION
     * Input: s1="ab", s2="ba"
     * Expected: 1 (true)
     * Explanation: "ba" is "ab" rotated left by 1 position
     */
    result = isNonTrivialRotation("ab", "ba");
    printTest("Test 11: Two character rotation \"ab\", \"ba\"", result, true);

    /*
     * TEST 12: LONG STRING ROTATION
     * Input: s1="abcdefghij", s2="fghijabcde"
     * Expected: 1 (true)
     * Explanation: "fghijabcde" is "abcdefghij" rotated left by 5 positions
     */
    result = isNonTrivialRotation("abcdefghij", "fghijabcde");
    printTest("Test 12: Long string rotation \"abcdefghij\", \"fghijabcde\"", result, true);

    /*
     * TEST 13: LONG STRING NOT ROTATION
     * Input: s1="abcdefghij", s2="abcdefghji"
     * Expected: 0 (false)
     * Purpose: Similar but last two characters swapped (not a rotation)
     */
    result = isNonTrivialRotation("abcdefghij", "abcdefghji");
    printTest("Test 13: Long string not rotation \"abcdefghij\", \"abcdefghji\"", result, false);

    /*
     * TEST 14: ROTATION BY LAST POSITION
     * Input: s1="hello", s2="ohell"
     * Expected: 1 (true)
     * Explanation: "ohell" is "hello" rotated right by 1 position
     */
    result = isNonTrivialRotation("hello", "ohell");
    printTest("Test 14: Rotation by last position \"hello\", \"ohell\"", result, true);

    /*
     * TEST 15: PALINDROME ROTATION
     * Input: s1="racecar", s2="carrace"
     * Expected: 1 (true)
     * Explanation: "carrace" is "racecar" rotated left by 4 positions
     */
    result = isNonTrivialRotation("racecar", "carrace");
    printTest("Test 15: Palindrome rotation \"racecar\", \"carrace\"", result, true);

    std::cout << "\n=== ALL TESTS COMPLETED ===\n";
    std::cout << "\nKEY ALGORITHM POINTS:\n";
    std::cout << "- Check strings are not identical\n";
    std::cout << "- Check strings have same length\n";
    std::cout << "- Concatenate s1 with itself: s1+s1\n";
    std::cout << "- Check if s2 is substring of (s1+s1)\n";
    std::cout << "- Time complexity: O(n) using substring search\n";
    std::cout << "- Space complexity: O(n) for concatenated string\n";

    return 0;
}
