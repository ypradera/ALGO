"""
MERGE INTERVALS PATTERN
Use: Scheduling, range problems, overlap detection
Time: O(n log n) for sorting, Space: O(n)
"""

from typing import List
import heapq


# === MERGE OVERLAPPING INTERVALS ===
def merge_intervals(intervals: List[List[int]]) -> List[List[int]]:
    """Merge all overlapping intervals"""
    if not intervals:
        return []

    intervals.sort(key=lambda x: x[0])
    merged = [intervals[0]]

    for start, end in intervals[1:]:
        if start <= merged[-1][1]:
            merged[-1][1] = max(merged[-1][1], end)
        else:
            merged.append([start, end])

    return merged


# === INSERT INTERVAL ===
def insert_interval(intervals: List[List[int]], new_interval: List[int]) -> List[List[int]]:
    """Insert and merge new interval"""
    result = []
    i = 0
    n = len(intervals)

    # add all intervals before new_interval
    while i < n and intervals[i][1] < new_interval[0]:
        result.append(intervals[i])
        i += 1

    # merge overlapping intervals
    while i < n and intervals[i][0] <= new_interval[1]:
        new_interval[0] = min(new_interval[0], intervals[i][0])
        new_interval[1] = max(new_interval[1], intervals[i][1])
        i += 1
    result.append(new_interval)

    # add remaining intervals
    while i < n:
        result.append(intervals[i])
        i += 1

    return result


# === INTERVAL INTERSECTION ===
def interval_intersection(first: List[List[int]], second: List[List[int]]) -> List[List[int]]:
    """Find intersection of two interval lists"""
    result = []
    i = j = 0

    while i < len(first) and j < len(second):
        start = max(first[i][0], second[j][0])
        end = min(first[i][1], second[j][1])

        if start <= end:
            result.append([start, end])

        # move pointer with smaller end
        if first[i][1] < second[j][1]:
            i += 1
        else:
            j += 1

    return result


# === MEETING ROOMS I (can attend all?) ===
def can_attend_meetings(intervals: List[List[int]]) -> bool:
    """Check if person can attend all meetings (no overlap)"""
    intervals.sort(key=lambda x: x[0])

    for i in range(1, len(intervals)):
        if intervals[i][0] < intervals[i-1][1]:
            return False

    return True


# === MEETING ROOMS II (min rooms needed) ===
def min_meeting_rooms(intervals: List[List[int]]) -> int:
    """Minimum meeting rooms required"""
    if not intervals:
        return 0

    intervals.sort(key=lambda x: x[0])
    heap = []  # end times

    for start, end in intervals:
        if heap and heap[0] <= start:
            heapq.heappop(heap)
        heapq.heappush(heap, end)

    return len(heap)


def min_meeting_rooms_sweep(intervals: List[List[int]]) -> int:
    """Using sweep line technique"""
    events = []
    for start, end in intervals:
        events.append((start, 1))   # meeting starts
        events.append((end, -1))    # meeting ends

    events.sort()

    rooms = max_rooms = 0
    for _, delta in events:
        rooms += delta
        max_rooms = max(max_rooms, rooms)

    return max_rooms


# === NON-OVERLAPPING INTERVALS (min removals) ===
def erase_overlap_intervals(intervals: List[List[int]]) -> int:
    """Minimum intervals to remove for no overlap"""
    if not intervals:
        return 0

    # sort by end time (greedy)
    intervals.sort(key=lambda x: x[1])
    count = 0
    prev_end = float('-inf')

    for start, end in intervals:
        if start >= prev_end:
            prev_end = end
        else:
            count += 1

    return count


# === EMPLOYEE FREE TIME ===
def employee_free_time(schedules: List[List[List[int]]]) -> List[List[int]]:
    """Find common free time for all employees"""
    # flatten and sort all intervals
    all_intervals = []
    for schedule in schedules:
        all_intervals.extend(schedule)

    all_intervals.sort(key=lambda x: x[0])

    result = []
    prev_end = all_intervals[0][1]

    for start, end in all_intervals[1:]:
        if start > prev_end:
            result.append([prev_end, start])
        prev_end = max(prev_end, end)

    return result


# === INTERVAL LIST UNION ===
def interval_union(intervals: List[List[int]]) -> List[List[int]]:
    """Union of intervals (same as merge)"""
    return merge_intervals(intervals)


# === INTERVAL COVERAGE ===
def min_intervals_to_cover(intervals: List[List[int]], target: List[int]) -> int:
    """Minimum intervals needed to cover target range"""
    intervals.sort()
    start, end = target

    count = 0
    i = 0
    n = len(intervals)

    while start < end:
        # find interval that starts <= start with max end
        max_end = start

        while i < n and intervals[i][0] <= start:
            max_end = max(max_end, intervals[i][1])
            i += 1

        if max_end == start:
            return -1  # gap, impossible

        start = max_end
        count += 1

    return count


# === COUNT OVERLAPPING INTERVALS AT POINT ===
def max_overlap(intervals: List[List[int]]) -> int:
    """Maximum number of overlapping intervals at any point"""
    events = []
    for start, end in intervals:
        events.append((start, 1))
        events.append((end, -1))

    events.sort()

    current = max_count = 0
    for _, delta in events:
        current += delta
        max_count = max(max_count, current)

    return max_count


# === REMOVE COVERED INTERVALS ===
def remove_covered_intervals(intervals: List[List[int]]) -> int:
    """Count intervals that are not covered by any other"""
    # sort by start (asc), then by end (desc)
    intervals.sort(key=lambda x: (x[0], -x[1]))

    count = 0
    prev_end = 0

    for _, end in intervals:
        if end > prev_end:
            count += 1
            prev_end = end

    return count


# === PARTITION LABELS ===
def partition_labels(s: str) -> List[int]:
    """Partition string so each letter appears in one part"""
    last = {c: i for i, c in enumerate(s)}
    result = []
    start = end = 0

    for i, c in enumerate(s):
        end = max(end, last[c])
        if i == end:
            result.append(end - start + 1)
            start = i + 1

    return result


# === BURST BALLOONS (interval covering) ===
def find_min_arrow_shots(points: List[List[int]]) -> int:
    """Minimum arrows to burst all balloons"""
    if not points:
        return 0

    points.sort(key=lambda x: x[1])
    arrows = 1
    end = points[0][1]

    for start, curr_end in points[1:]:
        if start > end:
            arrows += 1
            end = curr_end

    return arrows


# === CAR POOLING ===
def car_pooling(trips: List[List[int]], capacity: int) -> bool:
    """Check if car can complete all trips with capacity"""
    events = []

    for passengers, start, end in trips:
        events.append((start, passengers))
        events.append((end, -passengers))

    events.sort()

    current = 0
    for _, delta in events:
        current += delta
        if current > capacity:
            return False

    return True


# === MY CALENDAR (booking) ===
class MyCalendar:
    """Book events without double booking"""

    def __init__(self):
        self.bookings = []

    def book(self, start: int, end: int) -> bool:
        for s, e in self.bookings:
            if start < e and end > s:  # overlap
                return False
        self.bookings.append((start, end))
        return True


# === SUMMARY RANGES ===
def summary_ranges(nums: List[int]) -> List[str]:
    """Group consecutive numbers into ranges"""
    if not nums:
        return []

    result = []
    start = nums[0]

    for i in range(1, len(nums) + 1):
        if i == len(nums) or nums[i] != nums[i-1] + 1:
            if start == nums[i-1]:
                result.append(str(start))
            else:
                result.append(f"{start}->{nums[i-1]}")
            if i < len(nums):
                start = nums[i]

    return result
