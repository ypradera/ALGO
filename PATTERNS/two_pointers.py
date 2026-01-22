"""
TWO POINTERS PATTERN
Use: Sorted arrays, finding pairs, partitioning
Time: O(n) typically, Space: O(1)
"""

from typing import List, Tuple, Optional


# === OPPOSITE DIRECTION (sorted array) ===
def two_sum_sorted(arr: List[int], target: int) -> Tuple[int, int]:
    """Find two numbers that sum to target in sorted array"""
    left, right = 0, len(arr) - 1

    while left < right:
        curr_sum = arr[left] + arr[right]

        if curr_sum == target:
            return (left, right)
        elif curr_sum < target:
            left += 1
        else:
            right -= 1

    return (-1, -1)


def three_sum(arr: List[int], target: int = 0) -> List[List[int]]:
    """Find all unique triplets that sum to target"""
    arr.sort()
    result = []
    n = len(arr)

    for i in range(n - 2):
        if i > 0 and arr[i] == arr[i - 1]:
            continue

        left, right = i + 1, n - 1

        while left < right:
            curr_sum = arr[i] + arr[left] + arr[right]

            if curr_sum == target:
                result.append([arr[i], arr[left], arr[right]])
                left += 1
                right -= 1
                while left < right and arr[left] == arr[left - 1]:
                    left += 1
                while left < right and arr[right] == arr[right + 1]:
                    right -= 1
            elif curr_sum < target:
                left += 1
            else:
                right -= 1

    return result


def container_with_most_water(heights: List[int]) -> int:
    """Max area between two lines"""
    left, right = 0, len(heights) - 1
    max_area = 0

    while left < right:
        width = right - left
        height = min(heights[left], heights[right])
        max_area = max(max_area, width * height)

        if heights[left] < heights[right]:
            left += 1
        else:
            right -= 1

    return max_area


# === SAME DIRECTION ===
def remove_duplicates(arr: List[int]) -> int:
    """Remove duplicates in-place from sorted array, return new length"""
    if not arr:
        return 0

    write = 1

    for read in range(1, len(arr)):
        if arr[read] != arr[read - 1]:
            arr[write] = arr[read]
            write += 1

    return write


def move_zeros(arr: List[int]) -> None:
    """Move all zeros to end while maintaining order"""
    write = 0

    for read in range(len(arr)):
        if arr[read] != 0:
            arr[write], arr[read] = arr[read], arr[write]
            write += 1


def remove_element(arr: List[int], val: int) -> int:
    """Remove all occurrences of val in-place"""
    write = 0

    for read in range(len(arr)):
        if arr[read] != val:
            arr[write] = arr[read]
            write += 1

    return write


# === DUTCH NATIONAL FLAG (3-way partition) ===
def sort_colors(arr: List[int]) -> None:
    """Sort array of 0s, 1s, 2s in-place"""
    low, mid, high = 0, 0, len(arr) - 1

    while mid <= high:
        if arr[mid] == 0:
            arr[low], arr[mid] = arr[mid], arr[low]
            low += 1
            mid += 1
        elif arr[mid] == 1:
            mid += 1
        else:  # arr[mid] == 2
            arr[mid], arr[high] = arr[high], arr[mid]
            high -= 1


def partition_around_pivot(arr: List[int], pivot: int) -> None:
    """Partition array: elements < pivot, then >= pivot"""
    write = 0

    for read in range(len(arr)):
        if arr[read] < pivot:
            arr[write], arr[read] = arr[read], arr[write]
            write += 1


# === MERGE TWO SORTED ARRAYS ===
def merge_sorted(arr1: List[int], arr2: List[int]) -> List[int]:
    """Merge two sorted arrays"""
    result = []
    i, j = 0, 0

    while i < len(arr1) and j < len(arr2):
        if arr1[i] <= arr2[j]:
            result.append(arr1[i])
            i += 1
        else:
            result.append(arr2[j])
            j += 1

    result.extend(arr1[i:])
    result.extend(arr2[j:])
    return result


# === PALINDROME CHECK ===
def is_palindrome(s: str) -> bool:
    """Check if string is palindrome (alphanumeric only)"""
    left, right = 0, len(s) - 1

    while left < right:
        while left < right and not s[left].isalnum():
            left += 1
        while left < right and not s[right].isalnum():
            right -= 1

        if s[left].lower() != s[right].lower():
            return False

        left += 1
        right -= 1

    return True


# === TRAPPING RAIN WATER ===
def trap_water(heights: List[int]) -> int:
    """Calculate trapped rainwater"""
    if not heights:
        return 0

    left, right = 0, len(heights) - 1
    left_max, right_max = heights[left], heights[right]
    water = 0

    while left < right:
        if left_max < right_max:
            left += 1
            left_max = max(left_max, heights[left])
            water += left_max - heights[left]
        else:
            right -= 1
            right_max = max(right_max, heights[right])
            water += right_max - heights[right]

    return water


# === SQUARES OF SORTED ARRAY ===
def sorted_squares(arr: List[int]) -> List[int]:
    """Return squares in sorted order from sorted array with negatives"""
    n = len(arr)
    result = [0] * n
    left, right = 0, n - 1
    pos = n - 1

    while left <= right:
        if abs(arr[left]) > abs(arr[right]):
            result[pos] = arr[left] ** 2
            left += 1
        else:
            result[pos] = arr[right] ** 2
            right -= 1
        pos -= 1

    return result
