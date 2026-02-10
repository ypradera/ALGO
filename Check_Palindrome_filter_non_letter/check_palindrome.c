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
	• Return a boolean value: 1 if true & 0 if false.
Sample Input 0
Z
Sample Output 0
1
Sample Input 1
abc123cba
Sample Output 1
1*/


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
/*
 * Complete the 'isAlphabeticPalindrome' function below.
 *
 * The function is expected to return a BOOLEAN.
 * The function accepts STRING code as parameter.
 */
bool isAlphabeticPalindrome(char* code) {
    //corner case 
    if (!code) return 0;
    
    int len = (int)strlen(code);
    //Need a space to hold for comparison 
    char* tmp = (char*)malloc((len+1)*sizeof(char));
    if (!tmp) return 0; 
    
    int count = 0; 
    for(int i =0; i < len; i++){
        //can use either or to check if ASCII char is a letter 
        if (isalpha((unsigned char)code[i]))
        //if( (code[i] >= 'A' && code[i] <= 'Z') ||
        //   (code[i] >= 'a' && code[i] <= 'z'))
        {
            //if using isalpha() we cast he chat to unsigned char so no need anymore to recast
            tmp[count++] = (char)tolower(code[i]);
            //tmp[count++] = (char)tolower((unsigned char)code[i]); 
            //printf("tmp = %c \n", tmp[count-1]);
        }
    }
    tmp[count] = '\0';
    
    // Analyze if the word is a palindrome
    int idx_left = 0; 
    int idx_right = count-1;
    
    while (idx_left < idx_right){
        if(tmp[idx_left] != tmp[idx_right])
        {
            free(tmp);
            return 0; 
        }
         idx_left++; 
         idx_right--; 
    } 
    

    //After freeing dynamic allocated memory
    //tmp still contains the old address value, but it's now a dangling pointer.
    free(tmp);
    //good habit
    tmp = NULL;
    return 1;
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
int main(void) {
    bool result;

    printf("=== ALPHABETIC PALINDROME CHECKER TESTS ===\n");

    /*
     * TEST 1: EMPTY STRING
     * Input: ""
     * Expected: 1 (true)
     * Purpose: Edge case - empty string is considered palindrome
     */
    result = isAlphabeticPalindrome("");
    printTest("Test 1: Empty string \"\"", result, 1);

    /*
     * TEST 2: SINGLE LETTER
     * Input: "a"
     * Expected: 1 (true)
     * Purpose: Single character is always a palindrome
     */
    result = isAlphabeticPalindrome("a");
    printTest("Test 2: Single letter \"a\"", result, 1);

    /*
     * TEST 3: SINGLE LETTER UPPERCASE
     * Input: "Z"
     * Expected: 1 (true)
     * Purpose: Case-insensitive check for single character
     */
    result = isAlphabeticPalindrome("Z");
    printTest("Test 3: Single letter uppercase \"Z\"", result, 1);

    /*
     * TEST 4: SIMPLE PALINDROME
     * Input: "aba"
     * Expected: 1 (true)
     * Purpose: Basic palindrome with lowercase letters
     */
    result = isAlphabeticPalindrome("aba");
    printTest("Test 4: Simple palindrome \"aba\"", result, 1);

    /*
     * TEST 5: PROBLEM EXAMPLE
     * Input: "A1b2B!a"
     * Expected: 1 (true)
     * Explanation: Extract letters → "AbbA" → lowercase → "abba" (palindrome)
     */
    result = isAlphabeticPalindrome("A1b2B!a");
    printTest("Test 5: Problem example \"A1b2B!a\"", result, 1);

    /*
     * TEST 6: PALINDROME WITH NUMBERS
     * Input: "abc123cba"
     * Expected: 1 (true)
     * Explanation: Extract letters → "abccba" (palindrome)
     */
    result = isAlphabeticPalindrome("abc123cba");
    printTest("Test 6: Palindrome with numbers \"abc123cba\"", result, 1);

    /*
     * TEST 7: NOT A PALINDROME
     * Input: "hello"
     * Expected: 0 (false)
     * Purpose: Simple non-palindrome test
     */
    result = isAlphabeticPalindrome("hello");
    printTest("Test 7: Not a palindrome \"hello\"", result, 0);

    /*
     * TEST 8: NO LETTERS (ONLY NUMBERS)
     * Input: "12345"
     * Expected: 1 (true)
     * Explanation: No letters extracted → empty → considered palindrome
     */
    result = isAlphabeticPalindrome("12345");
    printTest("Test 8: Only numbers \"12345\"", result, 1);

    /*
     * TEST 9: NO LETTERS (ONLY SYMBOLS)
     * Input: "!@#$%"
     * Expected: 1 (true)
     * Explanation: No letters extracted → empty → considered palindrome
     */
    result = isAlphabeticPalindrome("!@#$%");
    printTest("Test 9: Only symbols \"!@#$%\"", result, 1);

    /*
     * TEST 10: CASE INSENSITIVE PALINDROME
     * Input: "RaceCar"
     * Expected: 1 (true)
     * Explanation: "RaceCar" → lowercase → "racecar" (palindrome)
     */
    result = isAlphabeticPalindrome("RaceCar");
    printTest("Test 10: Case insensitive \"RaceCar\"", result, 1);

    /*
     * TEST 11: PALINDROME WITH SPACES AND PUNCTUATION
     * Input: "A man, a plan, a canal: Panama"
     * Expected: 1 (true)
     * Explanation: Extract letters → "AmanaplanacanalPanama" → "amanaplanacanalpanama" (palindrome)
     */
    result = isAlphabeticPalindrome("A man, a plan, a canal: Panama");
    printTest("Test 11: Famous palindrome \"A man, a plan, a canal: Panama\"", result, 1);

    /*
     * TEST 12: NOT PALINDROME WITH SYMBOLS
     * Input: "abc!def"
     * Expected: 0 (false)
     * Explanation: Extract letters → "abcdef" (not a palindrome)
     */
    result = isAlphabeticPalindrome("abc!def");
    printTest("Test 12: Not palindrome with symbols \"abc!def\"", result, 0);

    /*
     * TEST 13: MIXED CASE NON-PALINDROME
     * Input: "Hello123World"
     * Expected: 0 (false)
     * Explanation: Extract letters → "HelloWorld" → "helloworld" (not palindrome)
     */
    result = isAlphabeticPalindrome("Hello123World");
    printTest("Test 13: Mixed case non-palindrome \"Hello123World\"", result, 0);

    /*
     * TEST 14: PALINDROME ALL UPPERCASE
     * Input: "ABBA"
     * Expected: 1 (true)
     * Explanation: "ABBA" → lowercase → "abba" (palindrome)
     */
    result = isAlphabeticPalindrome("ABBA");
    printTest("Test 14: All uppercase palindrome \"ABBA\"", result, 1);

    /*
     * TEST 15: LONG PALINDROME WITH MIXED CONTENT
     * Input: "Was it a car or a cat I saw?"
     * Expected: 1 (true)
     * Explanation: Extract letters → "WasitacaroracatIsaw" → "wasitacaroracatisaw" (palindrome)
     */
    result = isAlphabeticPalindrome("Was it a car or a cat I saw?");
    printTest("Test 15: Long palindrome \"Was it a car or a cat I saw?\"", result, 1);

    printf("\n=== ALL TESTS COMPLETED ===\n");
    printf("\nKEY ALGORITHM POINTS:\n");
    printf("- Filter non-alphabetic characters\n");
    printf("- Case-insensitive comparison (convert to lowercase)\n");
    printf("- Two-pointer technique for palindrome check\n");
    printf("- Time complexity: O(n) for scanning and checking\n");
    printf("- Space complexity: O(n) for filtered string storage\n");

    return 0;
}
