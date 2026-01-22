"""
BACKTRACKING PATTERN
Use: Permutations, combinations, subsets, constraint satisfaction
Time: O(n!) or O(2^n) typical, Space: O(n) recursion
"""

from typing import List


# === SUBSETS ===
def subsets(nums: List[int]) -> List[List[int]]:
    """Generate all subsets"""
    result = []

    def backtrack(start, path):
        result.append(path[:])
        for i in range(start, len(nums)):
            path.append(nums[i])
            backtrack(i + 1, path)
            path.pop()

    backtrack(0, [])
    return result


def subsets_with_duplicates(nums: List[int]) -> List[List[int]]:
    """Generate all unique subsets (input may have duplicates)"""
    nums.sort()
    result = []

    def backtrack(start, path):
        result.append(path[:])
        for i in range(start, len(nums)):
            if i > start and nums[i] == nums[i-1]:
                continue
            path.append(nums[i])
            backtrack(i + 1, path)
            path.pop()

    backtrack(0, [])
    return result


# === PERMUTATIONS ===
def permutations(nums: List[int]) -> List[List[int]]:
    """Generate all permutations"""
    result = []

    def backtrack(path, used):
        if len(path) == len(nums):
            result.append(path[:])
            return

        for i in range(len(nums)):
            if used[i]:
                continue
            used[i] = True
            path.append(nums[i])
            backtrack(path, used)
            path.pop()
            used[i] = False

    backtrack([], [False] * len(nums))
    return result


def permutations_with_duplicates(nums: List[int]) -> List[List[int]]:
    """Generate all unique permutations"""
    nums.sort()
    result = []

    def backtrack(path, used):
        if len(path) == len(nums):
            result.append(path[:])
            return

        for i in range(len(nums)):
            if used[i]:
                continue
            if i > 0 and nums[i] == nums[i-1] and not used[i-1]:
                continue
            used[i] = True
            path.append(nums[i])
            backtrack(path, used)
            path.pop()
            used[i] = False

    backtrack([], [False] * len(nums))
    return result


# === COMBINATIONS ===
def combinations(n: int, k: int) -> List[List[int]]:
    """Generate all k-combinations of 1..n"""
    result = []

    def backtrack(start, path):
        if len(path) == k:
            result.append(path[:])
            return

        # pruning: need k - len(path) more elements
        for i in range(start, n - (k - len(path)) + 2):
            path.append(i)
            backtrack(i + 1, path)
            path.pop()

    backtrack(1, [])
    return result


def combination_sum(candidates: List[int], target: int) -> List[List[int]]:
    """Find combinations that sum to target (can reuse elements)"""
    result = []

    def backtrack(start, path, remaining):
        if remaining == 0:
            result.append(path[:])
            return
        if remaining < 0:
            return

        for i in range(start, len(candidates)):
            path.append(candidates[i])
            backtrack(i, path, remaining - candidates[i])
            path.pop()

    backtrack(0, [], target)
    return result


def combination_sum_no_reuse(candidates: List[int], target: int) -> List[List[int]]:
    """Find combinations that sum to target (each element once)"""
    candidates.sort()
    result = []

    def backtrack(start, path, remaining):
        if remaining == 0:
            result.append(path[:])
            return

        for i in range(start, len(candidates)):
            if candidates[i] > remaining:
                break
            if i > start and candidates[i] == candidates[i-1]:
                continue
            path.append(candidates[i])
            backtrack(i + 1, path, remaining - candidates[i])
            path.pop()

    backtrack(0, [], target)
    return result


# === N-QUEENS ===
def solve_n_queens(n: int) -> List[List[str]]:
    """Find all N-Queens solutions"""
    result = []
    board = [['.' for _ in range(n)] for _ in range(n)]

    cols = set()
    diag1 = set()  # row - col
    diag2 = set()  # row + col

    def backtrack(row):
        if row == n:
            result.append([''.join(r) for r in board])
            return

        for col in range(n):
            if col in cols or (row - col) in diag1 or (row + col) in diag2:
                continue

            cols.add(col)
            diag1.add(row - col)
            diag2.add(row + col)
            board[row][col] = 'Q'

            backtrack(row + 1)

            cols.remove(col)
            diag1.remove(row - col)
            diag2.remove(row + col)
            board[row][col] = '.'

    backtrack(0)
    return result


# === SUDOKU SOLVER ===
def solve_sudoku(board: List[List[str]]) -> bool:
    """Solve sudoku in-place"""
    def is_valid(row, col, num):
        # check row
        if num in board[row]:
            return False
        # check col
        if num in [board[i][col] for i in range(9)]:
            return False
        # check 3x3 box
        box_r, box_c = 3 * (row // 3), 3 * (col // 3)
        for i in range(box_r, box_r + 3):
            for j in range(box_c, box_c + 3):
                if board[i][j] == num:
                    return False
        return True

    def backtrack():
        for i in range(9):
            for j in range(9):
                if board[i][j] == '.':
                    for num in '123456789':
                        if is_valid(i, j, num):
                            board[i][j] = num
                            if backtrack():
                                return True
                            board[i][j] = '.'
                    return False
        return True

    return backtrack()


# === WORD SEARCH ===
def word_search(board: List[List[str]], word: str) -> bool:
    """Check if word exists in grid"""
    m, n = len(board), len(board[0])

    def backtrack(i, j, k):
        if k == len(word):
            return True
        if i < 0 or i >= m or j < 0 or j >= n:
            return False
        if board[i][j] != word[k]:
            return False

        temp = board[i][j]
        board[i][j] = '#'  # mark visited

        found = (backtrack(i+1, j, k+1) or
                 backtrack(i-1, j, k+1) or
                 backtrack(i, j+1, k+1) or
                 backtrack(i, j-1, k+1))

        board[i][j] = temp  # restore
        return found

    for i in range(m):
        for j in range(n):
            if backtrack(i, j, 0):
                return True
    return False


# === PALINDROME PARTITIONING ===
def partition_palindrome(s: str) -> List[List[str]]:
    """Partition string into palindrome substrings"""
    result = []
    n = len(s)

    def is_palindrome(start, end):
        while start < end:
            if s[start] != s[end]:
                return False
            start += 1
            end -= 1
        return True

    def backtrack(start, path):
        if start == n:
            result.append(path[:])
            return

        for end in range(start, n):
            if is_palindrome(start, end):
                path.append(s[start:end+1])
                backtrack(end + 1, path)
                path.pop()

    backtrack(0, [])
    return result


# === GENERATE PARENTHESES ===
def generate_parentheses(n: int) -> List[str]:
    """Generate all valid parentheses combinations"""
    result = []

    def backtrack(path, open_count, close_count):
        if len(path) == 2 * n:
            result.append(''.join(path))
            return

        if open_count < n:
            path.append('(')
            backtrack(path, open_count + 1, close_count)
            path.pop()

        if close_count < open_count:
            path.append(')')
            backtrack(path, open_count, close_count + 1)
            path.pop()

    backtrack([], 0, 0)
    return result


# === LETTER COMBINATIONS (phone) ===
def letter_combinations(digits: str) -> List[str]:
    """Generate letter combinations for phone digits"""
    if not digits:
        return []

    mapping = {
        '2': 'abc', '3': 'def', '4': 'ghi', '5': 'jkl',
        '6': 'mno', '7': 'pqrs', '8': 'tuv', '9': 'wxyz'
    }
    result = []

    def backtrack(idx, path):
        if idx == len(digits):
            result.append(''.join(path))
            return

        for char in mapping[digits[idx]]:
            path.append(char)
            backtrack(idx + 1, path)
            path.pop()

    backtrack(0, [])
    return result


# === RESTORE IP ADDRESSES ===
def restore_ip_addresses(s: str) -> List[str]:
    """Generate all valid IP addresses"""
    result = []

    def backtrack(start, parts):
        if len(parts) == 4:
            if start == len(s):
                result.append('.'.join(parts))
            return

        for length in range(1, 4):
            if start + length > len(s):
                break
            segment = s[start:start + length]
            if (len(segment) > 1 and segment[0] == '0') or int(segment) > 255:
                continue
            backtrack(start + length, parts + [segment])

    backtrack(0, [])
    return result


# === TEMPLATE ===
def backtracking_template(candidates, target):
    """
    Generic backtracking pattern:

    def backtrack(start, path, state):
        if is_solution(path, state):
            result.append(path[:])
            return

        for i in range(start, len(candidates)):
            if not is_valid(candidates[i], state):
                continue

            # make choice
            path.append(candidates[i])
            update_state(state, candidates[i])

            # recurse
            backtrack(i + 1, path, state)  # or i for reuse

            # undo choice
            path.pop()
            restore_state(state, candidates[i])

    result = []
    backtrack(0, [], initial_state)
    return result
    """
    pass
