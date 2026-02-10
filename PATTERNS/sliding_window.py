"""
SLIDING WINDOW PATTERN
Use: Contiguous subarray/substring problems
Time: O(n), Space: O(1) or O(k) for frequency maps
"""

from collections import defaultdict, deque
from typing import List, Tuple


# === FIXED-SIZE WINDOW ===
def max_sum_fixed_window(arr: List[int], k: int) -> int:
    """Find max sum of subarray of size k"""
    if len(arr) < k:
        return -1

    window_sum = sum(arr[:k])
    max_sum = window_sum

    for i in range(k, len(arr)):
        window_sum += arr[i] - arr[i - k]
        max_sum = max(max_sum, window_sum)

    return max_sum


# === VARIABLE-SIZE WINDOW (shrinkable) ===
def longest_subarray_sum_at_most(arr: List[int], target: int) -> int:
    """Find longest subarray with sum <= target"""
    left = 0
    curr_sum = 0
    best = 0

    for right in range(len(arr)):
        curr_sum += arr[right]

        while left <= right and curr_sum > target:
            curr_sum -= arr[left]
            left += 1

        best = max(best, right - left + 1)

    return best


# === VARIABLE-SIZE WINDOW (non-shrinkable optimization) ===
def longest_subarray_optimized(arr: List[int], target: int) -> int:
    """Window only grows or slides, never shrinks"""
    left = 0
    curr_sum = 0

    for right in range(len(arr)):
        curr_sum += arr[right]

        if curr_sum > target:
            curr_sum -= arr[left]
            left += 1

    return len(arr) - left


# === SMALLEST WINDOW WITH SUM >= TARGET ===
def min_subarray_sum_at_least(arr: List[int], target: int) -> int:
    """Find minimum length subarray with sum >= target"""
    left = 0
    curr_sum = 0
    min_len = float('inf')

    for right in range(len(arr)):
        curr_sum += arr[right]

        while curr_sum >= target:
            min_len = min(min_len, right - left + 1)
            curr_sum -= arr[left]
            left += 1

    return min_len if min_len != float('inf') else 0


# === LONGEST SUBSTRING WITH K DISTINCT CHARS ===
def longest_k_distinct(s: str, k: int) -> int:
    """Longest substring with at most k distinct characters"""
    freq = defaultdict(int)
    left = 0
    best = 0

    for right, char in enumerate(s):
        freq[char] += 1

        while len(freq) > k:
            freq[s[left]] -= 1
            if freq[s[left]] == 0:
                del freq[s[left]]
            left += 1

        best = max(best, right - left + 1)

    return best


# === LONGEST SUBSTRING WITHOUT REPEATING ===
def longest_no_repeat(s: str) -> int:
    """Longest substring without repeating characters"""
    last_seen = {}
    left = 0
    best = 0

    for right, char in enumerate(s):
        if char in last_seen and last_seen[char] >= left:
            left = last_seen[char] + 1

        last_seen[char] = right
        best = max(best, right - left + 1)

    return best


# === MINIMUM WINDOW SUBSTRING ===
def min_window_substring(s: str, pattern: str) -> str:
    """Find smallest window containing all chars of pattern"""
    if not s or not pattern:
        return ""

    need = defaultdict(int)
    for c in pattern:
        need[c] += 1

    have = defaultdict(int)
    required = len(need)
    formed = 0
    left = 0
    min_len = float('inf')
    result = (0, 0)

    for right, char in enumerate(s):
        have[char] += 1

        if char in need and have[char] == need[char]:
            formed += 1

        while formed == required:
            if right - left + 1 < min_len:
                min_len = right - left + 1
                result = (left, right + 1)

            have[s[left]] -= 1
            if s[left] in need and have[s[left]] < need[s[left]]:
                formed -= 1
            left += 1

    return s[result[0]:result[1]] if min_len != float('inf') else ""


# === SLIDING WINDOW MAXIMUM ===
def sliding_window_max(arr: List[int], k: int) -> List[int]:
    """Find maximum in each window of size k using monotonic deque"""
    if not arr or k == 0:
        return []

    dq = deque()  # stores indices
    result = []

    for i, num in enumerate(arr):
        # remove elements outside window
        while dq and dq[0] <= i - k:
            dq.popleft()

        # remove smaller elements
        while dq and arr[dq[-1]] <= num:
            dq.pop()

        dq.append(i)

        if i >= k - 1:
            result.append(arr[dq[0]])

    return result


# === COUNT SUBARRAYS WITH SUM EQUALS K ===
def count_subarrays_sum_k(arr: List[int], k: int) -> int:
    """Count subarrays with sum exactly k using prefix sum + hashmap"""
    prefix_count = defaultdict(int)
    prefix_count[0] = 1

    count = 0
    prefix_sum = 0

    for num in arr:
        prefix_sum += num
        count += prefix_count[prefix_sum - k]
        prefix_count[prefix_sum] += 1

    return count


# === MAX CONSECUTIVE ONES WITH K FLIPS ===
def max_ones_with_k_flips(arr: List[int], k: int) -> int:
    """Max consecutive 1s if you can flip at most k 0s"""
    left = 0
    zeros = 0

    for right in range(len(arr)):
        if arr[right] == 0:
            zeros += 1

        if zeros > k:
            if arr[left] == 0:
                zeros -= 1
            left += 1

    return len(arr) - left


# === TEMPLATE ===
def sliding_window_template(arr: List[int], condition) -> int:
    """
    Generic sliding window pattern:

    left = 0
    state = initial_state
    best = initial_answer

    for right in range(len(arr)):
        # 1. EXPAND: add arr[right] to window
        update_state(state, arr[right])

        # 2. SHRINK: while window is invalid
        while is_invalid(state):
            remove_from_state(state, arr[left])
            left += 1

        # 3. UPDATE: record answer
        best = update_answer(best, left, right, state)

    return best
    """
    pass
