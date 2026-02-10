/*
 * BACKTRACKING PATTERN
 * Use: Permutations, combinations, subsets, constraint satisfaction
 * Time: O(n!) or O(2^n) typical, Space: O(n) recursion
 */

#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// === SUBSETS ===
void subsetsHelper(vector<int>& nums, int start, vector<int>& path, vector<vector<int>>& result) {
    result.push_back(path);

    for (int i = start; i < nums.size(); i++) {
        path.push_back(nums[i]);
        subsetsHelper(nums, i + 1, path, result);
        path.pop_back();
    }
}

vector<vector<int>> subsets(vector<int>& nums) {
    vector<vector<int>> result;
    vector<int> path;
    subsetsHelper(nums, 0, path, result);
    return result;
}

void subsetsWithDupHelper(vector<int>& nums, int start, vector<int>& path, vector<vector<int>>& result) {
    result.push_back(path);

    for (int i = start; i < nums.size(); i++) {
        if (i > start && nums[i] == nums[i-1]) continue;
        path.push_back(nums[i]);
        subsetsWithDupHelper(nums, i + 1, path, result);
        path.pop_back();
    }
}

vector<vector<int>> subsetsWithDuplicates(vector<int>& nums) {
    sort(nums.begin(), nums.end());
    vector<vector<int>> result;
    vector<int> path;
    subsetsWithDupHelper(nums, 0, path, result);
    return result;
}

// === PERMUTATIONS ===
void permuteHelper(vector<int>& nums, vector<int>& path, vector<bool>& used, vector<vector<int>>& result) {
    if (path.size() == nums.size()) {
        result.push_back(path);
        return;
    }

    for (int i = 0; i < nums.size(); i++) {
        if (used[i]) continue;
        used[i] = true;
        path.push_back(nums[i]);
        permuteHelper(nums, path, used, result);
        path.pop_back();
        used[i] = false;
    }
}

vector<vector<int>> permutations(vector<int>& nums) {
    vector<vector<int>> result;
    vector<int> path;
    vector<bool> used(nums.size(), false);
    permuteHelper(nums, path, used, result);
    return result;
}

void permuteUniqHelper(vector<int>& nums, vector<int>& path, vector<bool>& used, vector<vector<int>>& result) {
    if (path.size() == nums.size()) {
        result.push_back(path);
        return;
    }

    for (int i = 0; i < nums.size(); i++) {
        if (used[i]) continue;
        if (i > 0 && nums[i] == nums[i-1] && !used[i-1]) continue;
        used[i] = true;
        path.push_back(nums[i]);
        permuteUniqHelper(nums, path, used, result);
        path.pop_back();
        used[i] = false;
    }
}

vector<vector<int>> permutationsUnique(vector<int>& nums) {
    sort(nums.begin(), nums.end());
    vector<vector<int>> result;
    vector<int> path;
    vector<bool> used(nums.size(), false);
    permuteUniqHelper(nums, path, used, result);
    return result;
}

// === COMBINATIONS ===
void combineHelper(int n, int k, int start, vector<int>& path, vector<vector<int>>& result) {
    if (path.size() == k) {
        result.push_back(path);
        return;
    }

    for (int i = start; i <= n - (k - path.size()) + 1; i++) {
        path.push_back(i);
        combineHelper(n, k, i + 1, path, result);
        path.pop_back();
    }
}

vector<vector<int>> combinations(int n, int k) {
    vector<vector<int>> result;
    vector<int> path;
    combineHelper(n, k, 1, path, result);
    return result;
}

// === COMBINATION SUM ===
void combSumHelper(vector<int>& candidates, int target, int start, vector<int>& path, vector<vector<int>>& result) {
    if (target == 0) {
        result.push_back(path);
        return;
    }
    if (target < 0) return;

    for (int i = start; i < candidates.size(); i++) {
        path.push_back(candidates[i]);
        combSumHelper(candidates, target - candidates[i], i, path, result);
        path.pop_back();
    }
}

vector<vector<int>> combinationSum(vector<int>& candidates, int target) {
    vector<vector<int>> result;
    vector<int> path;
    combSumHelper(candidates, target, 0, path, result);
    return result;
}

// === N-QUEENS ===
void solveNQueensHelper(int n, int row, vector<int>& cols, vector<int>& diag1, vector<int>& diag2,
                        vector<string>& board, vector<vector<string>>& result) {
    if (row == n) {
        result.push_back(board);
        return;
    }

    for (int col = 0; col < n; col++) {
        if (cols[col] || diag1[row - col + n - 1] || diag2[row + col]) continue;

        cols[col] = diag1[row - col + n - 1] = diag2[row + col] = 1;
        board[row][col] = 'Q';

        solveNQueensHelper(n, row + 1, cols, diag1, diag2, board, result);

        cols[col] = diag1[row - col + n - 1] = diag2[row + col] = 0;
        board[row][col] = '.';
    }
}

vector<vector<string>> solveNQueens(int n) {
    vector<vector<string>> result;
    vector<string> board(n, string(n, '.'));
    vector<int> cols(n, 0), diag1(2 * n - 1, 0), diag2(2 * n - 1, 0);
    solveNQueensHelper(n, 0, cols, diag1, diag2, board, result);
    return result;
}

// === WORD SEARCH ===
bool wordSearchHelper(vector<vector<char>>& board, string& word, int i, int j, int k) {
    if (k == word.size()) return true;
    if (i < 0 || i >= board.size() || j < 0 || j >= board[0].size()) return false;
    if (board[i][j] != word[k]) return false;

    char temp = board[i][j];
    board[i][j] = '#';

    bool found = wordSearchHelper(board, word, i + 1, j, k + 1) ||
                 wordSearchHelper(board, word, i - 1, j, k + 1) ||
                 wordSearchHelper(board, word, i, j + 1, k + 1) ||
                 wordSearchHelper(board, word, i, j - 1, k + 1);

    board[i][j] = temp;
    return found;
}

bool wordSearch(vector<vector<char>>& board, string& word) {
    for (int i = 0; i < board.size(); i++) {
        for (int j = 0; j < board[0].size(); j++) {
            if (wordSearchHelper(board, word, i, j, 0)) {
                return true;
            }
        }
    }
    return false;
}

// === PALINDROME PARTITIONING ===
bool isPalindrome(string& s, int start, int end) {
    while (start < end) {
        if (s[start++] != s[end--]) return false;
    }
    return true;
}

void partitionHelper(string& s, int start, vector<string>& path, vector<vector<string>>& result) {
    if (start == s.size()) {
        result.push_back(path);
        return;
    }

    for (int end = start; end < s.size(); end++) {
        if (isPalindrome(s, start, end)) {
            path.push_back(s.substr(start, end - start + 1));
            partitionHelper(s, end + 1, path, result);
            path.pop_back();
        }
    }
}

vector<vector<string>> partitionPalindrome(string& s) {
    vector<vector<string>> result;
    vector<string> path;
    partitionHelper(s, 0, path, result);
    return result;
}

// === GENERATE PARENTHESES ===
void generateHelper(int n, int open, int close, string& path, vector<string>& result) {
    if (path.size() == 2 * n) {
        result.push_back(path);
        return;
    }

    if (open < n) {
        path += '(';
        generateHelper(n, open + 1, close, path, result);
        path.pop_back();
    }

    if (close < open) {
        path += ')';
        generateHelper(n, open, close + 1, path, result);
        path.pop_back();
    }
}

vector<string> generateParentheses(int n) {
    vector<string> result;
    string path;
    generateHelper(n, 0, 0, path, result);
    return result;
}

// === LETTER COMBINATIONS (phone) ===
void letterCombHelper(string& digits, int idx, string& path, vector<string>& result,
                      vector<string>& mapping) {
    if (idx == digits.size()) {
        result.push_back(path);
        return;
    }

    for (char c : mapping[digits[idx] - '0']) {
        path += c;
        letterCombHelper(digits, idx + 1, path, result, mapping);
        path.pop_back();
    }
}

vector<string> letterCombinations(string& digits) {
    if (digits.empty()) return {};

    vector<string> mapping = {"", "", "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz"};
    vector<string> result;
    string path;
    letterCombHelper(digits, 0, path, result, mapping);
    return result;
}

// === RESTORE IP ADDRESSES ===
void restoreHelper(string& s, int start, vector<string>& parts, vector<string>& result) {
    if (parts.size() == 4) {
        if (start == s.size()) {
            result.push_back(parts[0] + "." + parts[1] + "." + parts[2] + "." + parts[3]);
        }
        return;
    }

    for (int len = 1; len <= 3 && start + len <= s.size(); len++) {
        string segment = s.substr(start, len);
        if ((len > 1 && segment[0] == '0') || stoi(segment) > 255) continue;
        parts.push_back(segment);
        restoreHelper(s, start + len, parts, result);
        parts.pop_back();
    }
}

vector<string> restoreIpAddresses(string& s) {
    vector<string> result;
    vector<string> parts;
    restoreHelper(s, 0, parts, result);
    return result;
}
