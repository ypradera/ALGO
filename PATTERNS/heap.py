"""
HEAP / PRIORITY QUEUE PATTERNS
Use: Top-K, median, scheduling, merge K sorted
Time: O(log n) insert/delete, O(1) peek
"""

from typing import List, Optional
import heapq


# === BASIC HEAP OPERATIONS ===
def heapify_demo():
    """Python heapq is min-heap by default"""
    arr = [3, 1, 4, 1, 5, 9, 2, 6]

    # convert list to min-heap in O(n)
    heapq.heapify(arr)

    # push O(log n)
    heapq.heappush(arr, 0)

    # pop minimum O(log n)
    minimum = heapq.heappop(arr)

    # push and pop in one operation
    result = heapq.heappushpop(arr, 7)

    # pop and push in one operation
    result = heapq.heapreplace(arr, 8)

    return arr


# === MAX HEAP (negate values) ===
class MaxHeap:
    """Max heap using negation"""

    def __init__(self):
        self.heap = []

    def push(self, val: int) -> None:
        heapq.heappush(self.heap, -val)

    def pop(self) -> int:
        return -heapq.heappop(self.heap)

    def peek(self) -> int:
        return -self.heap[0]

    def __len__(self) -> int:
        return len(self.heap)


# === KTH LARGEST ELEMENT ===
def find_kth_largest(nums: List[int], k: int) -> int:
    """Find kth largest using min-heap of size k"""
    heap = []

    for num in nums:
        heapq.heappush(heap, num)
        if len(heap) > k:
            heapq.heappop(heap)

    return heap[0]


def find_kth_largest_quickselect(nums: List[int], k: int) -> int:
    """Find kth largest using quickselect O(n) average"""
    import random

    def partition(left, right, pivot_idx):
        pivot = nums[pivot_idx]
        nums[pivot_idx], nums[right] = nums[right], nums[pivot_idx]
        store_idx = left

        for i in range(left, right):
            if nums[i] < pivot:
                nums[i], nums[store_idx] = nums[store_idx], nums[i]
                store_idx += 1

        nums[store_idx], nums[right] = nums[right], nums[store_idx]
        return store_idx

    def select(left, right, k_smallest):
        if left == right:
            return nums[left]

        pivot_idx = random.randint(left, right)
        pivot_idx = partition(left, right, pivot_idx)

        if k_smallest == pivot_idx:
            return nums[k_smallest]
        elif k_smallest < pivot_idx:
            return select(left, pivot_idx - 1, k_smallest)
        else:
            return select(pivot_idx + 1, right, k_smallest)

    return select(0, len(nums) - 1, len(nums) - k)


# === TOP K FREQUENT ELEMENTS ===
def top_k_frequent(nums: List[int], k: int) -> List[int]:
    """Find k most frequent elements"""
    from collections import Counter

    count = Counter(nums)
    return heapq.nlargest(k, count.keys(), key=count.get)


def top_k_frequent_bucket(nums: List[int], k: int) -> List[int]:
    """Top k frequent using bucket sort O(n)"""
    from collections import Counter

    count = Counter(nums)
    buckets = [[] for _ in range(len(nums) + 1)]

    for num, freq in count.items():
        buckets[freq].append(num)

    result = []
    for i in range(len(buckets) - 1, -1, -1):
        for num in buckets[i]:
            result.append(num)
            if len(result) == k:
                return result

    return result


# === MERGE K SORTED LISTS ===
class ListNode:
    def __init__(self, val=0, next=None):
        self.val = val
        self.next = next

    def __lt__(self, other):
        return self.val < other.val


def merge_k_lists(lists: List[Optional[ListNode]]) -> Optional[ListNode]:
    """Merge k sorted linked lists"""
    heap = []

    for i, lst in enumerate(lists):
        if lst:
            heapq.heappush(heap, (lst.val, i, lst))

    dummy = ListNode()
    curr = dummy

    while heap:
        val, i, node = heapq.heappop(heap)
        curr.next = node
        curr = curr.next

        if node.next:
            heapq.heappush(heap, (node.next.val, i, node.next))

    return dummy.next


# === MERGE K SORTED ARRAYS ===
def merge_k_arrays(arrays: List[List[int]]) -> List[int]:
    """Merge k sorted arrays"""
    heap = []
    result = []

    for i, arr in enumerate(arrays):
        if arr:
            heapq.heappush(heap, (arr[0], i, 0))

    while heap:
        val, arr_idx, elem_idx = heapq.heappop(heap)
        result.append(val)

        if elem_idx + 1 < len(arrays[arr_idx]):
            next_val = arrays[arr_idx][elem_idx + 1]
            heapq.heappush(heap, (next_val, arr_idx, elem_idx + 1))

    return result


# === FIND MEDIAN FROM DATA STREAM ===
class MedianFinder:
    """Find median using two heaps"""

    def __init__(self):
        self.small = []  # max heap (negate)
        self.large = []  # min heap

    def add_num(self, num: int) -> None:
        # add to max heap
        heapq.heappush(self.small, -num)

        # balance: ensure small's max <= large's min
        if self.large and -self.small[0] > self.large[0]:
            heapq.heappush(self.large, -heapq.heappop(self.small))

        # balance sizes: small can have at most 1 more
        if len(self.small) > len(self.large) + 1:
            heapq.heappush(self.large, -heapq.heappop(self.small))
        elif len(self.large) > len(self.small):
            heapq.heappush(self.small, -heapq.heappop(self.large))

    def find_median(self) -> float:
        if len(self.small) > len(self.large):
            return -self.small[0]
        return (-self.small[0] + self.large[0]) / 2


# === SLIDING WINDOW MEDIAN ===
def sliding_window_median(nums: List[int], k: int) -> List[float]:
    """Median of each sliding window of size k"""
    from sortedcontainers import SortedList

    window = SortedList()
    result = []

    for i, num in enumerate(nums):
        window.add(num)

        if len(window) > k:
            window.remove(nums[i - k])

        if len(window) == k:
            if k % 2:
                result.append(float(window[k // 2]))
            else:
                result.append((window[k // 2 - 1] + window[k // 2]) / 2)

    return result


# === TASK SCHEDULER ===
def least_interval(tasks: List[str], n: int) -> int:
    """Minimum intervals to complete tasks with cooldown n"""
    from collections import Counter

    count = Counter(tasks)
    max_heap = [-c for c in count.values()]
    heapq.heapify(max_heap)

    time = 0
    queue = []  # (count, available_time)

    while max_heap or queue:
        time += 1

        if max_heap:
            cnt = heapq.heappop(max_heap) + 1  # decrement (remember negated)
            if cnt:
                queue.append((cnt, time + n))

        if queue and queue[0][1] == time:
            heapq.heappush(max_heap, queue.pop(0)[0])

    return time


# === K CLOSEST POINTS ===
def k_closest(points: List[List[int]], k: int) -> List[List[int]]:
    """Find k closest points to origin"""
    heap = []

    for x, y in points:
        dist = x*x + y*y
        heapq.heappush(heap, (-dist, x, y))
        if len(heap) > k:
            heapq.heappop(heap)

    return [[x, y] for _, x, y in heap]


# === REORGANIZE STRING ===
def reorganize_string(s: str) -> str:
    """Reorganize string so no adjacent chars are same"""
    from collections import Counter

    count = Counter(s)
    max_heap = [(-c, char) for char, c in count.items()]
    heapq.heapify(max_heap)

    result = []
    prev = (0, '')

    while max_heap or prev[0] < 0:
        if not max_heap:
            return ""  # impossible

        cnt, char = heapq.heappop(max_heap)
        result.append(char)

        if prev[0] < 0:
            heapq.heappush(max_heap, prev)

        prev = (cnt + 1, char)

    return ''.join(result)


# === IPO (maximize capital) ===
def find_maximized_capital(k: int, w: int, profits: List[int], capital: List[int]) -> int:
    """Maximize capital after k projects"""
    n = len(profits)
    projects = sorted(zip(capital, profits))

    max_heap = []
    i = 0

    for _ in range(k):
        while i < n and projects[i][0] <= w:
            heapq.heappush(max_heap, -projects[i][1])
            i += 1

        if not max_heap:
            break

        w -= heapq.heappop(max_heap)

    return w


# === SMALLEST RANGE COVERING K LISTS ===
def smallest_range(nums: List[List[int]]) -> List[int]:
    """Smallest range that includes at least one from each list"""
    heap = []
    max_val = float('-inf')

    for i, arr in enumerate(nums):
        heapq.heappush(heap, (arr[0], i, 0))
        max_val = max(max_val, arr[0])

    result = [float('-inf'), float('inf')]

    while heap:
        min_val, list_idx, elem_idx = heapq.heappop(heap)

        if max_val - min_val < result[1] - result[0]:
            result = [min_val, max_val]

        if elem_idx + 1 == len(nums[list_idx]):
            break

        next_val = nums[list_idx][elem_idx + 1]
        max_val = max(max_val, next_val)
        heapq.heappush(heap, (next_val, list_idx, elem_idx + 1))

    return result
