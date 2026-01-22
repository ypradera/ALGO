"""
DYNAMIC PROGRAMMING PATTERNS
Use: Optimization, counting, decision problems
Time: O(n*m) typical, Space: O(n) with optimization
"""

from typing import List
from functools import lru_cache


# === 1D DP: FIBONACCI / CLIMBING STAIRS ===
def climb_stairs(n: int) -> int:
    """Number of ways to climb n stairs (1 or 2 steps)"""
    if n <= 2:
        return n

    prev2, prev1 = 1, 2
    for _ in range(3, n + 1):
        prev2, prev1 = prev1, prev2 + prev1

    return prev1


def fibonacci(n: int) -> int:
    """Nth fibonacci number"""
    if n <= 1:
        return n

    prev2, prev1 = 0, 1
    for _ in range(2, n + 1):
        prev2, prev1 = prev1, prev2 + prev1

    return prev1


# === HOUSE ROBBER ===
def rob(nums: List[int]) -> int:
    """Max money without robbing adjacent houses"""
    if not nums:
        return 0
    if len(nums) == 1:
        return nums[0]

    prev2, prev1 = 0, nums[0]
    for i in range(1, len(nums)):
        prev2, prev1 = prev1, max(prev1, prev2 + nums[i])

    return prev1


def rob_circular(nums: List[int]) -> int:
    """House robber with circular houses"""
    if len(nums) == 1:
        return nums[0]

    def rob_linear(arr):
        prev2, prev1 = 0, 0
        for num in arr:
            prev2, prev1 = prev1, max(prev1, prev2 + num)
        return prev1

    return max(rob_linear(nums[:-1]), rob_linear(nums[1:]))


# === COIN CHANGE ===
def coin_change(coins: List[int], amount: int) -> int:
    """Minimum coins needed for amount"""
    dp = [float('inf')] * (amount + 1)
    dp[0] = 0

    for i in range(1, amount + 1):
        for coin in coins:
            if coin <= i and dp[i - coin] != float('inf'):
                dp[i] = min(dp[i], dp[i - coin] + 1)

    return dp[amount] if dp[amount] != float('inf') else -1


def coin_change_ways(coins: List[int], amount: int) -> int:
    """Number of ways to make amount"""
    dp = [0] * (amount + 1)
    dp[0] = 1

    for coin in coins:
        for i in range(coin, amount + 1):
            dp[i] += dp[i - coin]

    return dp[amount]


# === 0/1 KNAPSACK ===
def knapsack_01(weights: List[int], values: List[int], capacity: int) -> int:
    """Max value with capacity constraint (each item once)"""
    n = len(weights)
    dp = [0] * (capacity + 1)

    for i in range(n):
        for w in range(capacity, weights[i] - 1, -1):
            dp[w] = max(dp[w], dp[w - weights[i]] + values[i])

    return dp[capacity]


# === UNBOUNDED KNAPSACK ===
def knapsack_unbounded(weights: List[int], values: List[int], capacity: int) -> int:
    """Max value with unlimited items"""
    dp = [0] * (capacity + 1)

    for w in range(1, capacity + 1):
        for i in range(len(weights)):
            if weights[i] <= w:
                dp[w] = max(dp[w], dp[w - weights[i]] + values[i])

    return dp[capacity]


# === LONGEST COMMON SUBSEQUENCE ===
def lcs(s1: str, s2: str) -> int:
    """Length of longest common subsequence"""
    m, n = len(s1), len(s2)
    dp = [[0] * (n + 1) for _ in range(2)]

    for i in range(1, m + 1):
        for j in range(1, n + 1):
            if s1[i-1] == s2[j-1]:
                dp[i % 2][j] = dp[(i-1) % 2][j-1] + 1
            else:
                dp[i % 2][j] = max(dp[(i-1) % 2][j], dp[i % 2][j-1])

    return dp[m % 2][n]


def lcs_string(s1: str, s2: str) -> str:
    """Return actual LCS string"""
    m, n = len(s1), len(s2)
    dp = [[0] * (n + 1) for _ in range(m + 1)]

    for i in range(1, m + 1):
        for j in range(1, n + 1):
            if s1[i-1] == s2[j-1]:
                dp[i][j] = dp[i-1][j-1] + 1
            else:
                dp[i][j] = max(dp[i-1][j], dp[i][j-1])

    # backtrack
    result = []
    i, j = m, n
    while i > 0 and j > 0:
        if s1[i-1] == s2[j-1]:
            result.append(s1[i-1])
            i -= 1
            j -= 1
        elif dp[i-1][j] > dp[i][j-1]:
            i -= 1
        else:
            j -= 1

    return ''.join(reversed(result))


# === LONGEST INCREASING SUBSEQUENCE ===
def lis(nums: List[int]) -> int:
    """Length of longest increasing subsequence - O(n log n)"""
    from bisect import bisect_left

    tails = []
    for num in nums:
        idx = bisect_left(tails, num)
        if idx == len(tails):
            tails.append(num)
        else:
            tails[idx] = num

    return len(tails)


def lis_dp(nums: List[int]) -> int:
    """LIS using O(n^2) DP"""
    if not nums:
        return 0

    n = len(nums)
    dp = [1] * n

    for i in range(1, n):
        for j in range(i):
            if nums[j] < nums[i]:
                dp[i] = max(dp[i], dp[j] + 1)

    return max(dp)


# === EDIT DISTANCE ===
def edit_distance(word1: str, word2: str) -> int:
    """Minimum edits to transform word1 to word2"""
    m, n = len(word1), len(word2)
    dp = list(range(n + 1))

    for i in range(1, m + 1):
        prev = dp[0]
        dp[0] = i
        for j in range(1, n + 1):
            temp = dp[j]
            if word1[i-1] == word2[j-1]:
                dp[j] = prev
            else:
                dp[j] = 1 + min(prev, dp[j], dp[j-1])
            prev = temp

    return dp[n]


# === GRID DP: UNIQUE PATHS ===
def unique_paths(m: int, n: int) -> int:
    """Number of paths from top-left to bottom-right"""
    dp = [1] * n

    for _ in range(1, m):
        for j in range(1, n):
            dp[j] += dp[j-1]

    return dp[n-1]


def unique_paths_obstacles(grid: List[List[int]]) -> int:
    """Unique paths with obstacles (1 = obstacle)"""
    m, n = len(grid), len(grid[0])
    dp = [0] * n
    dp[0] = 1 if grid[0][0] == 0 else 0

    for i in range(m):
        for j in range(n):
            if grid[i][j] == 1:
                dp[j] = 0
            elif j > 0:
                dp[j] += dp[j-1]

    return dp[n-1]


# === MINIMUM PATH SUM ===
def min_path_sum(grid: List[List[int]]) -> int:
    """Minimum sum path from top-left to bottom-right"""
    m, n = len(grid), len(grid[0])
    dp = [float('inf')] * n
    dp[0] = 0

    for i in range(m):
        dp[0] += grid[i][0]
        for j in range(1, n):
            dp[j] = min(dp[j], dp[j-1]) + grid[i][j]

    return dp[n-1]


# === LONGEST PALINDROMIC SUBSEQUENCE ===
def longest_palindrome_subseq(s: str) -> int:
    """Length of longest palindromic subsequence"""
    n = len(s)
    dp = [1] * n

    for i in range(n - 2, -1, -1):
        prev = 0
        for j in range(i + 1, n):
            temp = dp[j]
            if s[i] == s[j]:
                dp[j] = prev + 2
            else:
                dp[j] = max(dp[j], dp[j-1])
            prev = temp

    return dp[n-1]


# === PARTITION EQUAL SUBSET SUM ===
def can_partition(nums: List[int]) -> bool:
    """Can partition array into two equal sum subsets"""
    total = sum(nums)
    if total % 2:
        return False

    target = total // 2
    dp = [False] * (target + 1)
    dp[0] = True

    for num in nums:
        for j in range(target, num - 1, -1):
            dp[j] = dp[j] or dp[j - num]

    return dp[target]


# === WORD BREAK ===
def word_break(s: str, word_dict: List[str]) -> bool:
    """Can s be segmented into dictionary words"""
    word_set = set(word_dict)
    n = len(s)
    dp = [False] * (n + 1)
    dp[0] = True

    for i in range(1, n + 1):
        for j in range(i):
            if dp[j] and s[j:i] in word_set:
                dp[i] = True
                break

    return dp[n]


# === MAXIMUM SUBARRAY (Kadane's) ===
def max_subarray(nums: List[int]) -> int:
    """Maximum sum contiguous subarray"""
    max_sum = curr_sum = nums[0]

    for num in nums[1:]:
        curr_sum = max(num, curr_sum + num)
        max_sum = max(max_sum, curr_sum)

    return max_sum


# === DECODE WAYS ===
def num_decodings(s: str) -> int:
    """Number of ways to decode digit string"""
    if not s or s[0] == '0':
        return 0

    n = len(s)
    prev2, prev1 = 1, 1

    for i in range(1, n):
        curr = 0
        if s[i] != '0':
            curr = prev1
        if 10 <= int(s[i-1:i+1]) <= 26:
            curr += prev2
        prev2, prev1 = prev1, curr

    return prev1


# === BUY AND SELL STOCK ===
def max_profit_one_transaction(prices: List[int]) -> int:
    """Max profit with one transaction"""
    min_price = float('inf')
    max_profit = 0

    for price in prices:
        min_price = min(min_price, price)
        max_profit = max(max_profit, price - min_price)

    return max_profit


def max_profit_unlimited(prices: List[int]) -> int:
    """Max profit with unlimited transactions"""
    profit = 0
    for i in range(1, len(prices)):
        if prices[i] > prices[i-1]:
            profit += prices[i] - prices[i-1]
    return profit
