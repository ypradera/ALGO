"""
HASHMAP / HASHTABLE PATTERNS
Use: Two sum, frequency counting, caching, grouping
Time: O(1) average operations, Space: O(n)
"""

from typing import List, Dict, Optional, Tuple
from collections import defaultdict, Counter


# === TWO SUM ===
def two_sum(nums: List[int], target: int) -> List[int]:
    """Find indices of two numbers that sum to target"""
    seen = {}

    for i, num in enumerate(nums):
        complement = target - num
        if complement in seen:
            return [seen[complement], i]
        seen[num] = i

    return []


def two_sum_all_pairs(nums: List[int], target: int) -> List[List[int]]:
    """Find all unique pairs that sum to target"""
    seen = {}
    result = set()

    for i, num in enumerate(nums):
        complement = target - num
        if complement in seen:
            pair = tuple(sorted([num, complement]))
            result.add(pair)
        seen[num] = i

    return [list(p) for p in result]


# === SUBARRAY SUM EQUALS K ===
def subarray_sum_k(nums: List[int], k: int) -> int:
    """Count subarrays with sum exactly k"""
    prefix_count = defaultdict(int)
    prefix_count[0] = 1

    count = 0
    prefix_sum = 0

    for num in nums:
        prefix_sum += num
        count += prefix_count[prefix_sum - k]
        prefix_count[prefix_sum] += 1

    return count


# === LONGEST SUBARRAY WITH SUM K ===
def longest_subarray_sum_k(nums: List[int], k: int) -> int:
    """Find longest subarray with sum exactly k"""
    prefix_idx = {0: -1}
    prefix_sum = 0
    max_len = 0

    for i, num in enumerate(nums):
        prefix_sum += num

        if prefix_sum - k in prefix_idx:
            max_len = max(max_len, i - prefix_idx[prefix_sum - k])

        if prefix_sum not in prefix_idx:
            prefix_idx[prefix_sum] = i

    return max_len


# === CONTIGUOUS ARRAY (equal 0s and 1s) ===
def find_max_length(nums: List[int]) -> int:
    """Find longest subarray with equal 0s and 1s"""
    prefix_idx = {0: -1}
    count = 0
    max_len = 0

    for i, num in enumerate(nums):
        count += 1 if num == 1 else -1

        if count in prefix_idx:
            max_len = max(max_len, i - prefix_idx[count])
        else:
            prefix_idx[count] = i

    return max_len


# === GROUP ANAGRAMS ===
def group_anagrams(strs: List[str]) -> List[List[str]]:
    """Group strings that are anagrams"""
    groups = defaultdict(list)

    for s in strs:
        key = tuple(sorted(s))
        groups[key].append(s)

    return list(groups.values())


def group_anagrams_count(strs: List[str]) -> List[List[str]]:
    """Group anagrams using character count as key"""
    groups = defaultdict(list)

    for s in strs:
        count = [0] * 26
        for c in s:
            count[ord(c) - ord('a')] += 1
        groups[tuple(count)].append(s)

    return list(groups.values())


# === VALID ANAGRAM ===
def is_anagram(s: str, t: str) -> bool:
    """Check if t is anagram of s"""
    if len(s) != len(t):
        return False
    return Counter(s) == Counter(t)


# === FIRST UNIQUE CHARACTER ===
def first_uniq_char(s: str) -> int:
    """Find first non-repeating character index"""
    count = Counter(s)

    for i, c in enumerate(s):
        if count[c] == 1:
            return i

    return -1


# === LRU CACHE ===
class LRUCache:
    """Least Recently Used Cache"""

    def __init__(self, capacity: int):
        from collections import OrderedDict
        self.cache = OrderedDict()
        self.capacity = capacity

    def get(self, key: int) -> int:
        if key not in self.cache:
            return -1
        self.cache.move_to_end(key)
        return self.cache[key]

    def put(self, key: int, value: int) -> None:
        if key in self.cache:
            self.cache.move_to_end(key)
        self.cache[key] = value
        if len(self.cache) > self.capacity:
            self.cache.popitem(last=False)


# === LFU CACHE ===
class LFUCache:
    """Least Frequently Used Cache"""

    def __init__(self, capacity: int):
        self.capacity = capacity
        self.min_freq = 0
        self.key_to_val = {}
        self.key_to_freq = {}
        self.freq_to_keys = defaultdict(OrderedDict)

    def get(self, key: int) -> int:
        if key not in self.key_to_val:
            return -1
        self._increase_freq(key)
        return self.key_to_val[key]

    def put(self, key: int, value: int) -> None:
        if self.capacity <= 0:
            return

        if key in self.key_to_val:
            self.key_to_val[key] = value
            self._increase_freq(key)
            return

        if len(self.key_to_val) >= self.capacity:
            # evict LFU (LRU among ties)
            evict_key = next(iter(self.freq_to_keys[self.min_freq]))
            del self.freq_to_keys[self.min_freq][evict_key]
            del self.key_to_val[evict_key]
            del self.key_to_freq[evict_key]

        self.key_to_val[key] = value
        self.key_to_freq[key] = 1
        self.freq_to_keys[1][key] = None
        self.min_freq = 1

    def _increase_freq(self, key):
        freq = self.key_to_freq[key]
        self.key_to_freq[key] = freq + 1

        del self.freq_to_keys[freq][key]
        if not self.freq_to_keys[freq]:
            del self.freq_to_keys[freq]
            if self.min_freq == freq:
                self.min_freq += 1

        self.freq_to_keys[freq + 1][key] = None


from collections import OrderedDict


# === CONTAINS DUPLICATE WITHIN K ===
def contains_nearby_duplicate(nums: List[int], k: int) -> bool:
    """Check if duplicate exists within k indices"""
    seen = {}

    for i, num in enumerate(nums):
        if num in seen and i - seen[num] <= k:
            return True
        seen[num] = i

    return False


# === CONTAINS DUPLICATE WITHIN K AND T ===
def contains_nearby_almost_duplicate(nums: List[int], k: int, t: int) -> bool:
    """Check if |nums[i] - nums[j]| <= t and |i - j| <= k"""
    if t < 0:
        return False

    buckets = {}
    width = t + 1

    for i, num in enumerate(nums):
        bucket = num // width

        if bucket in buckets:
            return True
        if bucket - 1 in buckets and num - buckets[bucket - 1] <= t:
            return True
        if bucket + 1 in buckets and buckets[bucket + 1] - num <= t:
            return True

        buckets[bucket] = num

        if i >= k:
            del buckets[nums[i - k] // width]

    return False


# === INTERSECTION OF TWO ARRAYS ===
def intersection(nums1: List[int], nums2: List[int]) -> List[int]:
    """Find intersection (unique elements)"""
    return list(set(nums1) & set(nums2))


def intersect(nums1: List[int], nums2: List[int]) -> List[int]:
    """Find intersection (with duplicates)"""
    count = Counter(nums1)
    result = []

    for num in nums2:
        if count[num] > 0:
            result.append(num)
            count[num] -= 1

    return result


# === ISOMORPHIC STRINGS ===
def is_isomorphic(s: str, t: str) -> bool:
    """Check if s and t are isomorphic"""
    if len(s) != len(t):
        return False

    s_to_t = {}
    t_to_s = {}

    for c1, c2 in zip(s, t):
        if c1 in s_to_t and s_to_t[c1] != c2:
            return False
        if c2 in t_to_s and t_to_s[c2] != c1:
            return False
        s_to_t[c1] = c2
        t_to_s[c2] = c1

    return True


# === WORD PATTERN ===
def word_pattern(pattern: str, s: str) -> bool:
    """Check if string follows pattern"""
    words = s.split()
    if len(pattern) != len(words):
        return False

    p_to_w = {}
    w_to_p = {}

    for p, w in zip(pattern, words):
        if p in p_to_w and p_to_w[p] != w:
            return False
        if w in w_to_p and w_to_p[w] != p:
            return False
        p_to_w[p] = w
        w_to_p[w] = p

    return True


# === LONGEST CONSECUTIVE SEQUENCE ===
def longest_consecutive(nums: List[int]) -> int:
    """Find longest consecutive sequence"""
    num_set = set(nums)
    max_len = 0

    for num in num_set:
        # only start from sequence beginning
        if num - 1 not in num_set:
            curr = num
            length = 1

            while curr + 1 in num_set:
                curr += 1
                length += 1

            max_len = max(max_len, length)

    return max_len


# === MAJORITY ELEMENT ===
def majority_element(nums: List[int]) -> int:
    """Find element appearing > n/2 times"""
    # Boyer-Moore voting
    candidate = nums[0]
    count = 1

    for num in nums[1:]:
        if count == 0:
            candidate = num
            count = 1
        elif num == candidate:
            count += 1
        else:
            count -= 1

    return candidate


# === DESIGN HASHMAP ===
class MyHashMap:
    """Simple HashMap implementation"""

    def __init__(self):
        self.size = 1000
        self.buckets = [[] for _ in range(self.size)]

    def _hash(self, key: int) -> int:
        return key % self.size

    def put(self, key: int, value: int) -> None:
        idx = self._hash(key)
        for i, (k, v) in enumerate(self.buckets[idx]):
            if k == key:
                self.buckets[idx][i] = (key, value)
                return
        self.buckets[idx].append((key, value))

    def get(self, key: int) -> int:
        idx = self._hash(key)
        for k, v in self.buckets[idx]:
            if k == key:
                return v
        return -1

    def remove(self, key: int) -> None:
        idx = self._hash(key)
        self.buckets[idx] = [(k, v) for k, v in self.buckets[idx] if k != key]


# === DESIGN HASHSET ===
class MyHashSet:
    """Simple HashSet implementation"""

    def __init__(self):
        self.size = 1000
        self.buckets = [[] for _ in range(self.size)]

    def _hash(self, key: int) -> int:
        return key % self.size

    def add(self, key: int) -> None:
        idx = self._hash(key)
        if key not in self.buckets[idx]:
            self.buckets[idx].append(key)

    def remove(self, key: int) -> None:
        idx = self._hash(key)
        if key in self.buckets[idx]:
            self.buckets[idx].remove(key)

    def contains(self, key: int) -> bool:
        idx = self._hash(key)
        return key in self.buckets[idx]
