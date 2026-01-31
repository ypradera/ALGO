/*Check Palindrome by Filtering Non-Letters
Given a string containing letters, digits, and symbols, determine if it reads the same forwards and backwards when considering only alphabetic characters (case-insensitive).
Example
Input
code = A1b2B!a
Output
1
Explanation
- Step 1: Extract only letters → ['A','b','B','a']
- Step 2: Convert to lowercase → ['a','b','b','a']
- Step 3: Compare sequence forward and backward: 'abba' == 'abba' → true

Input Format
	• A string code containing letters (A–Z, a–z), digits (0–9), and symbols
Constraints
	• 0 <= code.length <= 1000
	• For all 0 <= i < code.length: 33 <= ASCII(code[i]) <= 126
	• code contains only printable ASCII characters (letters, digits, symbols)
Output Format
	• Return a boolean value: true if palindrome, false otherwise.
Sample Input 0
Z
Sample Output 0
1
Sample Input 1
abc123cba
Sample Output 1
1*/


#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>

/*
 * Complete the 'isAlphabeticPalindrome' function below.
 *
 * The function is expected to return a BOOLEAN.
 * The function accepts STRING code as parameter.
 */
bool isAlphabeticPalindrome(const std::string& code) {
    // Filter and convert to lowercase
    std::string filtered;
    for (char ch : code) {
        if (std::isalpha(static_cast<unsigned char>(ch))) {
            filtered += std::tolower(static_cast<unsigned char>(ch));
        }
    }

    // Two-pointer palindrome check
    int left = 0;
    int right = filtered.length() - 1;

    while (left < right) {
        if (filtered[left] != filtered[right]) {
            return false;
        }
        left++;
        right--;
    }

    return true;
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
 * Tests the isAlphabeticPalindrome algorithm with 15 test cases
 * covering edge cases, various string formats, and boundary conditions
 *
 * TEST CATEGORIES:
 * 1. Edge Cases: Empty, single character, no letters
 * 2. Simple Palindromes: Pure letters, case variations
 * 3. Complex Palindromes: With numbers, symbols, spaces
 * 4. Non-Palindromes: Various failing cases
 * 5. Boundary Values: Long strings, special characters
 */
int main() {
    bool result;

    std::cout << "=== ALPHABETIC PALINDROME CHECKER TESTS ===\n";

    /*
     * TEST 1: EMPTY STRING
     * Input: ""
     * Expected: 1 (true)
     * Purpose: Edge case - empty string is considered palindrome
     */
    result = isAlphabeticPalindrome("");
    printTest("Test 1: Empty string \"\"", result, true);

    /*
     * TEST 2: SINGLE LETTER
     * Input: "a"
     * Expected: 1 (true)
     * Purpose: Single character is always a palindrome
     */
    result = isAlphabeticPalindrome("a");
    printTest("Test 2: Single letter \"a\"", result, true);

    /*
     * TEST 3: SINGLE LETTER UPPERCASE
     * Input: "Z"
     * Expected: 1 (true)
     * Purpose: Case-insensitive check for single character
     */
    result = isAlphabeticPalindrome("Z");
    printTest("Test 3: Single letter uppercase \"Z\"", result, true);

    /*
     * TEST 4: SIMPLE PALINDROME
     * Input: "aba"
     * Expected: 1 (true)
     * Purpose: Basic palindrome with lowercase letters
     */
    result = isAlphabeticPalindrome("aba");
    printTest("Test 4: Simple palindrome \"aba\"", result, true);

    /*
     * TEST 5: PROBLEM EXAMPLE
     * Input: "A1b2B!a"
     * Expected: 1 (true)
     * Explanation: Extract letters → "AbbA" → lowercase → "abba" (palindrome)
     */
    result = isAlphabeticPalindrome("A1b2B!a");
    printTest("Test 5: Problem example \"A1b2B!a\"", result, true);

    /*
     * TEST 6: PALINDROME WITH NUMBERS
     * Input: "abc123cba"
     * Expected: 1 (true)
     * Explanation: Extract letters → "abccba" (palindrome)
     */
    result = isAlphabeticPalindrome("abc123cba");
    printTest("Test 6: Palindrome with numbers \"abc123cba\"", result, true);

    /*
     * TEST 7: NOT A PALINDROME
     * Input: "hello"
     * Expected: 0 (false)
     * Purpose: Simple non-palindrome test
     */
    result = isAlphabeticPalindrome("hello");
    printTest("Test 7: Not a palindrome \"hello\"", result, false);

    /*
     * TEST 8: NO LETTERS (ONLY NUMBERS)
     * Input: "12345"
     * Expected: 1 (true)
     * Explanation: No letters extracted → empty → considered palindrome
     */
    result = isAlphabeticPalindrome("12345");
    printTest("Test 8: Only numbers \"12345\"", result, true);

    /*
     * TEST 9: NO LETTERS (ONLY SYMBOLS)
     * Input: "!@#$%"
     * Expected: 1 (true)
     * Explanation: No letters extracted → empty → considered palindrome
     */
    result = isAlphabeticPalindrome("!@#$%");
    printTest("Test 9: Only symbols \"!@#$%\"", result, true);

    /*
     * TEST 10: CASE INSENSITIVE PALINDROME
     * Input: "RaceCar"
     * Expected: 1 (true)
     * Explanation: "RaceCar" → lowercase → "racecar" (palindrome)
     */
    result = isAlphabeticPalindrome("RaceCar");
    printTest("Test 10: Case insensitive \"RaceCar\"", result, true);

    /*
     * TEST 11: PALINDROME WITH SPACES AND PUNCTUATION
     * Input: "A man, a plan, a canal: Panama"
     * Expected: 1 (true)
     * Explanation: Extract letters → "AmanaplanacanalPanama" → "amanaplanacanalpanama" (palindrome)
     */
    result = isAlphabeticPalindrome("A man, a plan, a canal: Panama");
    printTest("Test 11: Famous palindrome \"A man, a plan, a canal: Panama\"", result, true);

    /*
     * TEST 12: NOT PALINDROME WITH SYMBOLS
     * Input: "abc!def"
     * Expected: 0 (false)
     * Explanation: Extract letters → "abcdef" (not a palindrome)
     */
    result = isAlphabeticPalindrome("abc!def");
    printTest("Test 12: Not palindrome with symbols \"abc!def\"", result, false);

    /*
     * TEST 13: MIXED CASE NON-PALINDROME
     * Input: "Hello123World"
     * Expected: 0 (false)
     * Explanation: Extract letters → "HelloWorld" → "helloworld" (not palindrome)
     */
    result = isAlphabeticPalindrome("Hello123World");
    printTest("Test 13: Mixed case non-palindrome \"Hello123World\"", result, false);

    /*
     * TEST 14: PALINDROME ALL UPPERCASE
     * Input: "ABBA"
     * Expected: 1 (true)
     * Explanation: "ABBA" → lowercase → "abba" (palindrome)
     */
    result = isAlphabeticPalindrome("ABBA");
    printTest("Test 14: All uppercase palindrome \"ABBA\"", result, true);

    /*
     * TEST 15: LONG PALINDROME WITH MIXED CONTENT
     * Input: "Was it a car or a cat I saw?"
     * Expected: 1 (true)
     * Explanation: Extract letters → "WasitacaroracatIsaw" → "wasitacaroracatisaw" (palindrome)
     */
    result = isAlphabeticPalindrome("Was it a car or a cat I saw?");
    printTest("Test 15: Long palindrome \"Was it a car or a cat I saw?\"", result, true);

    std::cout << "\n=== ALL TESTS COMPLETED ===\n";
    std::cout << "\nKEY ALGORITHM POINTS:\n";
    std::cout << "- Filter non-alphabetic characters\n";
    std::cout << "- Case-insensitive comparison (convert to lowercase)\n";
    std::cout << "- Two-pointer technique for palindrome check\n";
    std::cout << "- Time complexity: O(n) for scanning and checking\n";
    std::cout << "- Space complexity: O(n) for filtered string storage\n";

    return 0;
}
