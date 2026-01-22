"""
BINARY SEARCH PATTERN
Use: Sorted arrays, search space reduction, optimization problems
Time: O(log n), Space: O(1)
"""

from typing import List, Callable


# === STANDARD BINARY SEARCH ===
def binary_search(arr: List[int], target: int) -> int:
    """Find index of target, return -1 if not found"""
    lo, hi = 0, len(arr) - 1

    while lo <= hi:
        mid = lo + (hi - lo) // 2

        if arr[mid] == target:
            return mid
        elif arr[mid] < target:
            lo = mid + 1
        else:
            hi = mid - 1

    return -1


# === LOWER BOUND (first >= target) ===
def lower_bound(arr: List[int], target: int) -> int:
    """Find first index where arr[i] >= target"""
    lo, hi = 0, len(arr)

    while lo < hi:
        mid = lo + (hi - lo) // 2

        if arr[mid] < target:
            lo = mid + 1
        else:
            hi = mid

    return lo


# === UPPER BOUND (first > target) ===
def upper_bound(arr: List[int], target: int) -> int:
    """Find first index where arr[i] > target"""
    lo, hi = 0, len(arr)

    while lo < hi:
        mid = lo + (hi - lo) // 2

        if arr[mid] <= target:
            lo = mid + 1
        else:
            hi = mid

    return lo


# === FIND FIRST AND LAST OCCURRENCE ===
def search_range(arr: List[int], target: int) -> List[int]:
    """Find first and last index of target"""
    first = lower_bound(arr, target)

    if first == len(arr) or arr[first] != target:
        return [-1, -1]

    last = upper_bound(arr, target) - 1
    return [first, last]


# === SEARCH IN ROTATED SORTED ARRAY ===
def search_rotated(arr: List[int], target: int) -> int:
    """Search in rotated sorted array (no duplicates)"""
    lo, hi = 0, len(arr) - 1

    while lo <= hi:
        mid = lo + (hi - lo) // 2

        if arr[mid] == target:
            return mid

        # left half is sorted
        if arr[lo] <= arr[mid]:
            if arr[lo] <= target < arr[mid]:
                hi = mid - 1
            else:
                lo = mid + 1
        # right half is sorted
        else:
            if arr[mid] < target <= arr[hi]:
                lo = mid + 1
            else:
                hi = mid - 1

    return -1


# === FIND MINIMUM IN ROTATED ARRAY ===
def find_min_rotated(arr: List[int]) -> int:
    """Find minimum in rotated sorted array"""
    lo, hi = 0, len(arr) - 1

    while lo < hi:
        mid = lo + (hi - lo) // 2

        if arr[mid] > arr[hi]:
            lo = mid + 1
        else:
            hi = mid

    return arr[lo]


# === FIND PEAK ELEMENT ===
def find_peak(arr: List[int]) -> int:
    """Find index of any peak element"""
    lo, hi = 0, len(arr) - 1

    while lo < hi:
        mid = lo + (hi - lo) // 2

        if arr[mid] < arr[mid + 1]:
            lo = mid + 1
        else:
            hi = mid

    return lo


# === SEARCH ON ANSWER (minimization) ===
def search_on_answer_min(lo: int, hi: int, is_valid: Callable[[int], bool]) -> int:
    """
    Find minimum value where is_valid returns True.
    Assumes: is_valid is monotonic (False...False, True...True)
    """
    while lo < hi:
        mid = lo + (hi - lo) // 2

        if is_valid(mid):
            hi = mid
        else:
            lo = mid + 1

    return lo


# === SEARCH ON ANSWER (maximization) ===
def search_on_answer_max(lo: int, hi: int, is_valid: Callable[[int], bool]) -> int:
    """
    Find maximum value where is_valid returns True.
    Assumes: is_valid is monotonic (True...True, False...False)
    """
    while lo < hi:
        mid = lo + (hi - lo + 1) // 2  # bias right to avoid infinite loop

        if is_valid(mid):
            lo = mid
        else:
            hi = mid - 1

    return lo


# === KOKO EATING BANANAS ===
def min_eating_speed(piles: List[int], h: int) -> int:
    """Minimum eating speed to finish all piles in h hours"""
    def can_finish(speed: int) -> bool:
        hours = sum((pile + speed - 1) // speed for pile in piles)
        return hours <= h

    lo, hi = 1, max(piles)

    while lo < hi:
        mid = lo + (hi - lo) // 2

        if can_finish(mid):
            hi = mid
        else:
            lo = mid + 1

    return lo


# === SPLIT ARRAY LARGEST SUM ===
def split_array(arr: List[int], k: int) -> int:
    """Split array into k subarrays, minimize largest sum"""
    def can_split(max_sum: int) -> bool:
        count = 1
        curr_sum = 0

        for num in arr:
            if curr_sum + num > max_sum:
                count += 1
                curr_sum = num
            else:
                curr_sum += num

        return count <= k

    lo, hi = max(arr), sum(arr)

    while lo < hi:
        mid = lo + (hi - lo) // 2

        if can_split(mid):
            hi = mid
        else:
            lo = mid + 1

    return lo


# === SQRT(X) ===
def sqrt_int(x: int) -> int:
    """Integer square root"""
    if x < 2:
        return x

    lo, hi = 1, x // 2

    while lo <= hi:
        mid = lo + (hi - lo) // 2
        sq = mid * mid

        if sq == x:
            return mid
        elif sq < x:
            lo = mid + 1
        else:
            hi = mid - 1

    return hi


# === BINARY SEARCH ON FLOATS ===
def sqrt_float(x: float, epsilon: float = 1e-9) -> float:
    """Float square root with precision epsilon"""
    lo, hi = 0.0, max(1.0, x)

    while hi - lo > epsilon:
        mid = (lo + hi) / 2

        if mid * mid < x:
            lo = mid
        else:
            hi = mid

    return lo
