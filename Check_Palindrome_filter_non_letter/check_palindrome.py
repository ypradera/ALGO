"""
Check Palindrome by Filtering Non-Letters

Given a string containing letters, digits, and symbols, determine if it reads the
same forwards and backwards when considering only alphabetic characters (case-insensitive).

ALGORITHM APPROACH:
1. Filter out all non-alphabetic characters
2. Convert all letters to lowercase for case-insensitive comparison
3. Use two-pointer technique to check if filtered string is palindrome

TIME COMPLEXITY: O(n) - single pass to filter, single pass to check
SPACE COMPLEXITY: O(n) - for storing filtered characters

Example:
Input: code = "A1b2B!a"
Output: True
Explanation:
- Step 1: Extract only letters → ['A','b','B','a']
- Step 2: Convert to lowercase → ['a','b','b','a']
- Step 3: Compare sequence forward and backward: 'abba' == 'abba' → True

CONSTRAINTS:
- 0 <= code.length <= 1000
- For all 0 <= i < code.length: 33 <= ASCII(code[i]) <= 126
- code contains only printable ASCII characters (letters, digits, symbols)
"""


def isAlphabeticPalindrome(code):
    """
    Check if a string is a palindrome considering only alphabetic characters.

    ALGORITHM:
    1. Extract only alphabetic characters and convert to lowercase
    2. Use two-pointer technique to compare from both ends
    3. Return True if all characters match, False otherwise

    PARAMETERS:
    code - String containing letters, digits, and symbols

    RETURN:
    Boolean - True if alphabetic palindrome, False otherwise

    EXAMPLES:
    >>> isAlphabeticPalindrome("A1b2B!a")
    True
    >>> isAlphabeticPalindrome("hello")
    False
    >>> isAlphabeticPalindrome("")
    True
    """
    # Corner case: None input
    if code is None:
        return False

    # Step 1: Filter and convert to lowercase
    # Extract only alphabetic characters and convert to lowercase
    filtered = [char.lower() for char in code if char.isalpha()]

    # Step 2: Two-pointer palindrome check
    left = 0
    right = len(filtered) - 1

    while left < right:
        if filtered[left] != filtered[right]:
            return False
        left += 1
        right -= 1

    return True


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

    Tests the isAlphabeticPalindrome algorithm with 15 test cases
    covering edge cases, various string formats, and boundary conditions

    TEST CATEGORIES:
    1. Edge Cases: Empty, single character, no letters
    2. Simple Palindromes: Pure letters, case variations
    3. Complex Palindromes: With numbers, symbols, spaces
    4. Non-Palindromes: Various failing cases
    5. Boundary Values: Long strings, special characters
    """
    print("=== ALPHABETIC PALINDROME CHECKER TESTS ===")

    # TEST 1: EMPTY STRING
    # Input: ""
    # Expected: True
    # Purpose: Edge case - empty string is considered palindrome
    result = isAlphabeticPalindrome("")
    printTest("Test 1: Empty string \"\"", result, True)

    # TEST 2: SINGLE LETTER
    # Input: "a"
    # Expected: True
    # Purpose: Single character is always a palindrome
    result = isAlphabeticPalindrome("a")
    printTest("Test 2: Single letter \"a\"", result, True)

    # TEST 3: SINGLE LETTER UPPERCASE
    # Input: "Z"
    # Expected: True
    # Purpose: Case-insensitive check for single character
    result = isAlphabeticPalindrome("Z")
    printTest("Test 3: Single letter uppercase \"Z\"", result, True)

    # TEST 4: SIMPLE PALINDROME
    # Input: "aba"
    # Expected: True
    # Purpose: Basic palindrome with lowercase letters
    result = isAlphabeticPalindrome("aba")
    printTest("Test 4: Simple palindrome \"aba\"", result, True)

    # TEST 5: PROBLEM EXAMPLE
    # Input: "A1b2B!a"
    # Expected: True
    # Explanation: Extract letters → "AbbA" → lowercase → "abba" (palindrome)
    result = isAlphabeticPalindrome("A1b2B!a")
    printTest("Test 5: Problem example \"A1b2B!a\"", result, True)

    # TEST 6: PALINDROME WITH NUMBERS
    # Input: "abc123cba"
    # Expected: True
    # Explanation: Extract letters → "abccba" (palindrome)
    result = isAlphabeticPalindrome("abc123cba")
    printTest("Test 6: Palindrome with numbers \"abc123cba\"", result, True)

    # TEST 7: NOT A PALINDROME
    # Input: "hello"
    # Expected: False
    # Purpose: Simple non-palindrome test
    result = isAlphabeticPalindrome("hello")
    printTest("Test 7: Not a palindrome \"hello\"", result, False)

    # TEST 8: NO LETTERS (ONLY NUMBERS)
    # Input: "12345"
    # Expected: True
    # Explanation: No letters extracted → empty → considered palindrome
    result = isAlphabeticPalindrome("12345")
    printTest("Test 8: Only numbers \"12345\"", result, True)

    # TEST 9: NO LETTERS (ONLY SYMBOLS)
    # Input: "!@#$%"
    # Expected: True
    # Explanation: No letters extracted → empty → considered palindrome
    result = isAlphabeticPalindrome("!@#$%")
    printTest("Test 9: Only symbols \"!@#$%\"", result, True)

    # TEST 10: CASE INSENSITIVE PALINDROME
    # Input: "RaceCar"
    # Expected: True
    # Explanation: "RaceCar" → lowercase → "racecar" (palindrome)
    result = isAlphabeticPalindrome("RaceCar")
    printTest("Test 10: Case insensitive \"RaceCar\"", result, True)

    # TEST 11: PALINDROME WITH SPACES AND PUNCTUATION
    # Input: "A man, a plan, a canal: Panama"
    # Expected: True
    # Explanation: Extract letters → "AmanaplanacanalPanama" → "amanaplanacanalpanama" (palindrome)
    result = isAlphabeticPalindrome("A man, a plan, a canal: Panama")
    printTest("Test 11: Famous palindrome \"A man, a plan, a canal: Panama\"", result, True)

    # TEST 12: NOT PALINDROME WITH SYMBOLS
    # Input: "abc!def"
    # Expected: False
    # Explanation: Extract letters → "abcdef" (not a palindrome)
    result = isAlphabeticPalindrome("abc!def")
    printTest("Test 12: Not palindrome with symbols \"abc!def\"", result, False)

    # TEST 13: MIXED CASE NON-PALINDROME
    # Input: "Hello123World"
    # Expected: False
    # Explanation: Extract letters → "HelloWorld" → "helloworld" (not palindrome)
    result = isAlphabeticPalindrome("Hello123World")
    printTest("Test 13: Mixed case non-palindrome \"Hello123World\"", result, False)

    # TEST 14: PALINDROME ALL UPPERCASE
    # Input: "ABBA"
    # Expected: True
    # Explanation: "ABBA" → lowercase → "abba" (palindrome)
    result = isAlphabeticPalindrome("ABBA")
    printTest("Test 14: All uppercase palindrome \"ABBA\"", result, True)

    # TEST 15: LONG PALINDROME WITH MIXED CONTENT
    # Input: "Was it a car or a cat I saw?"
    # Expected: True
    # Explanation: Extract letters → "WasitacaroracatIsaw" → "wasitacaroracatisaw" (palindrome)
    result = isAlphabeticPalindrome("Was it a car or a cat I saw?")
    printTest("Test 15: Long palindrome \"Was it a car or a cat I saw?\"", result, True)

    print("\n=== ALL TESTS COMPLETED ===")
    print("\nKEY ALGORITHM POINTS:")
    print("- Filter non-alphabetic characters")
    print("- Case-insensitive comparison (convert to lowercase)")
    print("- Two-pointer technique for palindrome check")
    print("- Time complexity: O(n) for scanning and checking")
    print("- Space complexity: O(n) for filtered string storage")


if __name__ == "__main__":
    main()
