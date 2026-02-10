"""
Check for Non-Identical String Rotation

Given two strings s1 and s2, return True if s2 is a rotation of s1 but not
identical to s1, otherwise return False.

ALGORITHM APPROACH:
1. Check if strings are identical (if so, return False)
2. Check if strings have same length (if not, return False)
3. Concatenate s1 with itself: s1 + s1
4. Check if s2 is a substring of the concatenated string

The key insight: If s2 is a rotation of s1, then s2 will always be found
as a substring in s1+s1.

Example: s1 = "abcde", s2 = "cdeab"
- s1 + s1 = "abcdeabcde"
- s2 "cdeab" appears in "abcdeabcde" at position 2
- Therefore, s2 is a rotation of s1

TIME COMPLEXITY: O(n) - string concatenation and substring search
SPACE COMPLEXITY: O(n) - for concatenated string

CONSTRAINTS:
- 1 <= |s1| <= 1000
- 1 <= |s2| <= 1000
- |s1| = |s2|
- s1 & s2 both consist of lowercase English letters ('a'-'z') only
"""


def isNonTrivialRotation(s1, s2):
    """
    Check if s2 is a non-identical rotation of s1.

    ALGORITHM:
    1. Return False if strings are identical
    2. Return False if strings have different lengths or are empty
    3. Create double string: s1 + s1
    4. Return True if s2 is found in double string, False otherwise

    PARAMETERS:
    s1 - First string
    s2 - Second string to check if it's a rotation of s1

    RETURN:
    Boolean - True if s2 is a non-identical rotation of s1, False otherwise

    EXAMPLES:
    >>> isNonTrivialRotation("abcde", "cdeab")
    True
    >>> isNonTrivialRotation("abcde", "abcde")
    False
    >>> isNonTrivialRotation("abc", "bca")
    True
    """
    # Check if strings are identical
    if s1 == s2:
        return False

    # Check if strings have same length and are not empty
    len_s1 = len(s1)
    len_s2 = len(s2)
    if len_s1 != len_s2 or len_s1 == 0:
        return False

    # Create double string and check if s2 is a substring
    # If s2 is a rotation of s1, it will be in s1+s1
    double_str = s1 + s1
    return s2 in double_str


def printTest(testName, result, expected):
    """
    Print test result in a formatted way.

    PARAMETERS:
    testName - Description of test case
    result   - Actual result from function
    expected - Expected result
    """
    print(f"\n{testName}:")
    status = "PASS" if result == expected else "FAIL"
    print(f"  Result: {result} {status} (Expected: {expected})")


def main():
    """
    Main function - Comprehensive Test Suite

    Tests the isNonTrivialRotation algorithm with 15 test cases
    covering edge cases, various rotation scenarios, and boundary conditions

    TEST CATEGORIES:
    1. Edge Cases: Empty, single character, identical strings
    2. Valid Rotations: Various rotation positions
    3. Invalid Cases: Different lengths, non-rotations
    4. Special Cases: Repeated patterns, full rotations
    5. Boundary Values: Long strings, all same character
    """
    print("=== NON-IDENTICAL STRING ROTATION TESTS ===")

    # TEST 1: IDENTICAL STRINGS
    # Input: s1="abcde", s2="abcde"
    # Expected: False
    # Purpose: Strings are identical, not a non-trivial rotation
    result = isNonTrivialRotation("abcde", "abcde")
    printTest("Test 1: Identical strings \"abcde\", \"abcde\"", result, False)

    # TEST 2: PROBLEM EXAMPLE
    # Input: s1="abcde", s2="cdeab"
    # Expected: True
    # Explanation: "cdeab" is "abcde" rotated left by 2 positions
    result = isNonTrivialRotation("abcde", "cdeab")
    printTest("Test 2: Problem example \"abcde\", \"cdeab\"", result, True)

    # TEST 3: SINGLE CHARACTER IDENTICAL
    # Input: s1="a", s2="a"
    # Expected: False
    # Purpose: Single character, identical strings
    result = isNonTrivialRotation("a", "a")
    printTest("Test 3: Single character identical \"a\", \"a\"", result, False)

    # TEST 4: SINGLE CHARACTER DIFFERENT
    # Input: s1="a", s2="b"
    # Expected: False
    # Purpose: Single characters that are different (not a rotation)
    result = isNonTrivialRotation("a", "b")
    printTest("Test 4: Single character different \"a\", \"b\"", result, False)

    # TEST 5: ROTATION BY ONE POSITION
    # Input: s1="abc", s2="bca"
    # Expected: True
    # Explanation: "bca" is "abc" rotated left by 1 position
    result = isNonTrivialRotation("abc", "bca")
    printTest("Test 5: Rotation by one \"abc\", \"bca\"", result, True)

    # TEST 6: ROTATION BY TWO POSITIONS
    # Input: s1="abc", s2="cab"
    # Expected: True
    # Explanation: "cab" is "abc" rotated left by 2 positions
    result = isNonTrivialRotation("abc", "cab")
    printTest("Test 6: Rotation by two \"abc\", \"cab\"", result, True)

    # TEST 7: NOT A ROTATION
    # Input: s1="abc", s2="acb"
    # Expected: False
    # Purpose: Same characters but different order (not a rotation)
    result = isNonTrivialRotation("abc", "acb")
    printTest("Test 7: Not a rotation \"abc\", \"acb\"", result, False)

    # TEST 8: DIFFERENT LENGTHS
    # Input: s1="abc", s2="abcd"
    # Expected: False
    # Purpose: Different lengths cannot be rotations
    result = isNonTrivialRotation("abc", "abcd")
    printTest("Test 8: Different lengths \"abc\", \"abcd\"", result, False)

    # TEST 9: REPEATED PATTERN ROTATION
    # Input: s1="abab", s2="baba"
    # Expected: True
    # Explanation: "baba" is "abab" rotated left by 2 positions
    result = isNonTrivialRotation("abab", "baba")
    printTest("Test 9: Repeated pattern \"abab\", \"baba\"", result, True)

    # TEST 10: ALL SAME CHARACTER
    # Input: s1="aaaa", s2="aaaa"
    # Expected: False
    # Purpose: All same character, strings are identical
    result = isNonTrivialRotation("aaaa", "aaaa")
    printTest("Test 10: All same character \"aaaa\", \"aaaa\"", result, False)

    # TEST 11: TWO CHARACTER ROTATION
    # Input: s1="ab", s2="ba"
    # Expected: True
    # Explanation: "ba" is "ab" rotated left by 1 position
    result = isNonTrivialRotation("ab", "ba")
    printTest("Test 11: Two character rotation \"ab\", \"ba\"", result, True)

    # TEST 12: LONG STRING ROTATION
    # Input: s1="abcdefghij", s2="fghijabcde"
    # Expected: True
    # Explanation: "fghijabcde" is "abcdefghij" rotated left by 5 positions
    result = isNonTrivialRotation("abcdefghij", "fghijabcde")
    printTest("Test 12: Long string rotation \"abcdefghij\", \"fghijabcde\"", result, True)

    # TEST 13: LONG STRING NOT ROTATION
    # Input: s1="abcdefghij", s2="abcdefghji"
    # Expected: False
    # Purpose: Similar but last two characters swapped (not a rotation)
    result = isNonTrivialRotation("abcdefghij", "abcdefghji")
    printTest("Test 13: Long string not rotation \"abcdefghij\", \"abcdefghji\"", result, False)

    # TEST 14: ROTATION BY LAST POSITION
    # Input: s1="hello", s2="ohell"
    # Expected: True
    # Explanation: "ohell" is "hello" rotated right by 1 position
    result = isNonTrivialRotation("hello", "ohell")
    printTest("Test 14: Rotation by last position \"hello\", \"ohell\"", result, True)

    # TEST 15: PALINDROME ROTATION
    # Input: s1="racecar", s2="carrace"
    # Expected: True
    # Explanation: "carrace" is "racecar" rotated left by 4 positions
    result = isNonTrivialRotation("racecar", "carrace")
    printTest("Test 15: Palindrome rotation \"racecar\", \"carrace\"", result, True)

    print("\n=== ALL TESTS COMPLETED ===")
    print("\nKEY ALGORITHM POINTS:")
    print("- Check strings are not identical")
    print("- Check strings have same length")
    print("- Concatenate s1 with itself: s1+s1")
    print("- Check if s2 is substring of (s1+s1)")
    print("- Time complexity: O(n) using substring search")
    print("- Space complexity: O(n) for concatenated string")


if __name__ == "__main__":
    main()
