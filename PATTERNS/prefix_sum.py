"""
PREFIX SUM PATTERN
Use: Range sum queries, subarray sums, 2D range queries
Time: O(n) build, O(1) query, Space: O(n)
"""

from typing import List


# === 1D PREFIX SUM ===
def build_prefix_sum(arr: List[int]) -> List[int]:
    """Build prefix sum array. prefix[i] = sum(arr[0:i])"""
    n = len(arr)
    prefix = [0] * (n + 1)

    for i in range(n):
        prefix[i + 1] = prefix[i] + arr[i]

    return prefix


def range_sum(prefix: List[int], left: int, right: int) -> int:
    """Sum of arr[left:right+1] using prefix sum"""
    return prefix[right + 1] - prefix[left]


# === SUBARRAY SUM EQUALS K ===
def count_subarrays_sum_k(arr: List[int], k: int) -> int:
    """Count subarrays with sum exactly k"""
    from collections import defaultdict

    prefix_count = defaultdict(int)
    prefix_count[0] = 1

    count = 0
    prefix_sum = 0

    for num in arr:
        prefix_sum += num
        count += prefix_count[prefix_sum - k]
        prefix_count[prefix_sum] += 1

    return count


# === PRODUCT EXCEPT SELF ===
def product_except_self(arr: List[int]) -> List[int]:
    """Product of all elements except self, without division"""
    n = len(arr)
    result = [1] * n

    # left products
    left_prod = 1
    for i in range(n):
        result[i] = left_prod
        left_prod *= arr[i]

    # right products
    right_prod = 1
    for i in range(n - 1, -1, -1):
        result[i] *= right_prod
        right_prod *= arr[i]

    return result


# === 2D PREFIX SUM ===
class PrefixSum2D:
    """2D prefix sum for rectangle sum queries"""

    def __init__(self, matrix: List[List[int]]):
        if not matrix or not matrix[0]:
            self.prefix = [[]]
            return

        m, n = len(matrix), len(matrix[0])
        self.prefix = [[0] * (n + 1) for _ in range(m + 1)]

        for i in range(m):
            for j in range(n):
                self.prefix[i + 1][j + 1] = (
                    matrix[i][j]
                    + self.prefix[i][j + 1]
                    + self.prefix[i + 1][j]
                    - self.prefix[i][j]
                )

    def query(self, r1: int, c1: int, r2: int, c2: int) -> int:
        """Sum of rectangle from (r1,c1) to (r2,c2) inclusive"""
        return (
            self.prefix[r2 + 1][c2 + 1]
            - self.prefix[r1][c2 + 1]
            - self.prefix[r2 + 1][c1]
            + self.prefix[r1][c1]
        )


# === DIFFERENCE ARRAY ===
def build_difference_array(arr: List[int]) -> List[int]:
    """Build difference array for range updates"""
    n = len(arr)
    diff = [0] * n

    diff[0] = arr[0]
    for i in range(1, n):
        diff[i] = arr[i] - arr[i - 1]

    return diff


def range_add(diff: List[int], left: int, right: int, val: int) -> None:
    """Add val to arr[left:right+1] using difference array"""
    diff[left] += val
    if right + 1 < len(diff):
        diff[right + 1] -= val


def reconstruct_from_diff(diff: List[int]) -> List[int]:
    """Reconstruct original array from difference array"""
    arr = [0] * len(diff)
    arr[0] = diff[0]

    for i in range(1, len(diff)):
        arr[i] = arr[i - 1] + diff[i]

    return arr


# === RANGE UPDATE QUERIES ===
def process_range_updates(n: int, updates: List[List[int]]) -> List[int]:
    """
    Process multiple range updates efficiently.
    updates: [[left, right, val], ...]
    """
    diff = [0] * (n + 1)

    for left, right, val in updates:
        diff[left] += val
        diff[right + 1] -= val

    # prefix sum to get result
    result = [0] * n
    result[0] = diff[0]
    for i in range(1, n):
        result[i] = result[i - 1] + diff[i]

    return result


# === RUNNING SUM ===
def running_sum(arr: List[int]) -> List[int]:
    """Running sum of array"""
    result = []
    total = 0

    for num in arr:
        total += num
        result.append(total)

    return result


# === PIVOT INDEX ===
def pivot_index(arr: List[int]) -> int:
    """Find index where left sum equals right sum"""
    total = sum(arr)
    left_sum = 0

    for i, num in enumerate(arr):
        if left_sum == total - left_sum - num:
            return i
        left_sum += num

    return -1


# === SUBARRAY SUM DIVISIBLE BY K ===
def subarrays_div_by_k(arr: List[int], k: int) -> int:
    """Count subarrays with sum divisible by k"""
    from collections import defaultdict

    remainder_count = defaultdict(int)
    remainder_count[0] = 1

    count = 0
    prefix_sum = 0

    for num in arr:
        prefix_sum += num
        remainder = prefix_sum % k

        # handle negative remainders
        if remainder < 0:
            remainder += k

        count += remainder_count[remainder]
        remainder_count[remainder] += 1

    return count


# === LONGEST SUBARRAY WITH SUM K ===
def longest_subarray_sum_k(arr: List[int], k: int) -> int:
    """Find longest subarray with sum exactly k"""
    prefix_index = {0: -1}
    prefix_sum = 0
    max_len = 0

    for i, num in enumerate(arr):
        prefix_sum += num

        if prefix_sum - k in prefix_index:
            max_len = max(max_len, i - prefix_index[prefix_sum - k])

        # only store first occurrence
        if prefix_sum not in prefix_index:
            prefix_index[prefix_sum] = i

    return max_len


# === CONTIGUOUS ARRAY (equal 0s and 1s) ===
def find_max_equal_01(arr: List[int]) -> int:
    """Find longest subarray with equal 0s and 1s"""
    prefix_index = {0: -1}
    count = 0
    max_len = 0

    for i, num in enumerate(arr):
        count += 1 if num == 1 else -1

        if count in prefix_index:
            max_len = max(max_len, i - prefix_index[count])
        else:
            prefix_index[count] = i

    return max_len
