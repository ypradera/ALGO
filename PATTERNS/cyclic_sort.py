"""
CYCLIC SORT PATTERN
Use: Arrays with values in range [1, n] or [0, n-1]
Time: O(n), Space: O(1)
"""

from typing import List


# === CYCLIC SORT (values 1 to n) ===
def cyclic_sort(nums: List[int]) -> List[int]:
    """Sort array where values are 1 to n"""
    i = 0
    while i < len(nums):
        correct_idx = nums[i] - 1

        if nums[i] != nums[correct_idx]:
            nums[i], nums[correct_idx] = nums[correct_idx], nums[i]
        else:
            i += 1

    return nums


# === CYCLIC SORT (values 0 to n-1) ===
def cyclic_sort_zero_based(nums: List[int]) -> List[int]:
    """Sort array where values are 0 to n-1"""
    i = 0
    while i < len(nums):
        correct_idx = nums[i]

        if 0 <= correct_idx < len(nums) and nums[i] != nums[correct_idx]:
            nums[i], nums[correct_idx] = nums[correct_idx], nums[i]
        else:
            i += 1

    return nums


# === FIND MISSING NUMBER (0 to n, one missing) ===
def find_missing_number(nums: List[int]) -> int:
    """Find missing number in [0, n]"""
    n = len(nums)
    i = 0

    while i < n:
        if nums[i] < n and nums[i] != nums[nums[i]]:
            nums[nums[i]], nums[i] = nums[i], nums[nums[i]]
        else:
            i += 1

    for i in range(n):
        if nums[i] != i:
            return i

    return n


def find_missing_number_xor(nums: List[int]) -> int:
    """Find missing using XOR (doesn't modify array)"""
    n = len(nums)
    result = n

    for i in range(n):
        result ^= i ^ nums[i]

    return result


# === FIND ALL MISSING NUMBERS ===
def find_all_missing(nums: List[int]) -> List[int]:
    """Find all missing numbers in [1, n]"""
    i = 0
    n = len(nums)

    while i < n:
        correct_idx = nums[i] - 1

        if 0 <= correct_idx < n and nums[i] != nums[correct_idx]:
            nums[i], nums[correct_idx] = nums[correct_idx], nums[i]
        else:
            i += 1

    return [i + 1 for i in range(n) if nums[i] != i + 1]


# === FIND DUPLICATE NUMBER ===
def find_duplicate(nums: List[int]) -> int:
    """Find the duplicate in array of [1, n] with one duplicate"""
    # Floyd's cycle detection (array as linked list)
    slow = fast = nums[0]

    while True:
        slow = nums[slow]
        fast = nums[nums[fast]]
        if slow == fast:
            break

    slow = nums[0]
    while slow != fast:
        slow = nums[slow]
        fast = nums[fast]

    return slow


def find_duplicate_cyclic(nums: List[int]) -> int:
    """Find duplicate using cyclic sort (modifies array)"""
    i = 0
    while i < len(nums):
        if nums[i] != i + 1:
            correct_idx = nums[i] - 1
            if nums[i] != nums[correct_idx]:
                nums[i], nums[correct_idx] = nums[correct_idx], nums[i]
            else:
                return nums[i]
        else:
            i += 1

    return -1


# === FIND ALL DUPLICATES ===
def find_all_duplicates(nums: List[int]) -> List[int]:
    """Find all duplicates in [1, n] where elements appear 1 or 2 times"""
    i = 0
    n = len(nums)

    while i < n:
        correct_idx = nums[i] - 1

        if nums[i] != nums[correct_idx]:
            nums[i], nums[correct_idx] = nums[correct_idx], nums[i]
        else:
            i += 1

    return [nums[i] for i in range(n) if nums[i] != i + 1]


def find_all_duplicates_marking(nums: List[int]) -> List[int]:
    """Find duplicates by marking (doesn't rearrange)"""
    result = []

    for num in nums:
        idx = abs(num) - 1
        if nums[idx] < 0:
            result.append(abs(num))
        else:
            nums[idx] = -nums[idx]

    return result


# === FIND CORRUPT PAIR (one missing, one duplicate) ===
def find_corrupt_pair(nums: List[int]) -> List[int]:
    """Find [duplicate, missing] in [1, n]"""
    i = 0
    n = len(nums)

    while i < n:
        correct_idx = nums[i] - 1

        if nums[i] != nums[correct_idx]:
            nums[i], nums[correct_idx] = nums[correct_idx], nums[i]
        else:
            i += 1

    for i in range(n):
        if nums[i] != i + 1:
            return [nums[i], i + 1]

    return []


# === FIRST MISSING POSITIVE ===
def first_missing_positive(nums: List[int]) -> int:
    """Find smallest missing positive integer"""
    n = len(nums)
    i = 0

    while i < n:
        correct_idx = nums[i] - 1

        if 0 < nums[i] <= n and nums[i] != nums[correct_idx]:
            nums[i], nums[correct_idx] = nums[correct_idx], nums[i]
        else:
            i += 1

    for i in range(n):
        if nums[i] != i + 1:
            return i + 1

    return n + 1


# === KTH MISSING POSITIVE NUMBER ===
def find_kth_positive(arr: List[int], k: int) -> int:
    """Find kth missing positive in sorted array"""
    lo, hi = 0, len(arr)

    while lo < hi:
        mid = (lo + hi) // 2
        # number of missing before arr[mid]
        missing = arr[mid] - (mid + 1)

        if missing < k:
            lo = mid + 1
        else:
            hi = mid

    # at position lo, we've passed arr[lo-1] - lo missing numbers
    # answer = lo + k
    return lo + k


def find_kth_positive_linear(arr: List[int], k: int) -> int:
    """Linear approach for unsorted or simple cases"""
    missing = 0
    current = 1
    i = 0

    while missing < k:
        if i < len(arr) and arr[i] == current:
            i += 1
        else:
            missing += 1
            if missing == k:
                return current
        current += 1

    return current


# === SMALLEST MISSING NON-NEGATIVE ===
def smallest_missing_non_negative(nums: List[int]) -> int:
    """Find smallest missing non-negative integer"""
    n = len(nums)
    i = 0

    while i < n:
        correct_idx = nums[i]

        if 0 <= correct_idx < n and nums[i] != nums[correct_idx]:
            nums[i], nums[correct_idx] = nums[correct_idx], nums[i]
        else:
            i += 1

    for i in range(n):
        if nums[i] != i:
            return i

    return n


# === SET MISMATCH ===
def find_error_nums(nums: List[int]) -> List[int]:
    """Find [duplicate, missing] - same as corrupt pair"""
    return find_corrupt_pair(nums)


# === TEMPLATE ===
def cyclic_sort_template(nums: List[int]) -> None:
    """
    Generic cyclic sort pattern:

    i = 0
    while i < len(nums):
        # calculate correct index for nums[i]
        correct_idx = calculate_correct_position(nums[i])

        # if not in correct position and won't cause infinite loop
        if in_valid_range(nums[i]) and nums[i] != nums[correct_idx]:
            # swap to correct position
            nums[i], nums[correct_idx] = nums[correct_idx], nums[i]
        else:
            i += 1

    # after sorting, check for anomalies
    for i in range(len(nums)):
        if nums[i] != expected_value(i):
            # found missing/duplicate/etc
            pass
    """
    pass
