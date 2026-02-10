/*
 * HASHMAP / HASHTABLE PATTERNS
 * Use: Two sum, frequency counting, caching, grouping
 * Time: O(1) average operations, Space: O(n)
 */

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <algorithm>
#include <list>

using namespace std;

// === TWO SUM ===
vector<int> twoSum(vector<int>& nums, int target) {
    unordered_map<int, int> seen;

    for (int i = 0; i < nums.size(); i++) {
        int complement = target - nums[i];
        if (seen.count(complement)) {
            return {seen[complement], i};
        }
        seen[nums[i]] = i;
    }

    return {};
}

// === SUBARRAY SUM EQUALS K ===
int subarraySumK(vector<int>& nums, int k) {
    unordered_map<long long, int> prefixCount;
    prefixCount[0] = 1;

    int count = 0;
    long long prefixSum = 0;

    for (int num : nums) {
        prefixSum += num;
        count += prefixCount[prefixSum - k];
        prefixCount[prefixSum]++;
    }

    return count;
}

// === LONGEST SUBARRAY WITH SUM K ===
int longestSubarraySumK(vector<int>& nums, int k) {
    unordered_map<long long, int> prefixIdx;
    prefixIdx[0] = -1;

    long long prefixSum = 0;
    int maxLen = 0;

    for (int i = 0; i < nums.size(); i++) {
        prefixSum += nums[i];

        if (prefixIdx.count(prefixSum - k)) {
            maxLen = max(maxLen, i - prefixIdx[prefixSum - k]);
        }

        if (!prefixIdx.count(prefixSum)) {
            prefixIdx[prefixSum] = i;
        }
    }

    return maxLen;
}

// === CONTIGUOUS ARRAY (equal 0s and 1s) ===
int findMaxLength(vector<int>& nums) {
    unordered_map<int, int> prefixIdx;
    prefixIdx[0] = -1;

    int count = 0;
    int maxLen = 0;

    for (int i = 0; i < nums.size(); i++) {
        count += nums[i] == 1 ? 1 : -1;

        if (prefixIdx.count(count)) {
            maxLen = max(maxLen, i - prefixIdx[count]);
        } else {
            prefixIdx[count] = i;
        }
    }

    return maxLen;
}

// === GROUP ANAGRAMS ===
vector<vector<string>> groupAnagrams(vector<string>& strs) {
    unordered_map<string, vector<string>> groups;

    for (string& s : strs) {
        string key = s;
        sort(key.begin(), key.end());
        groups[key].push_back(s);
    }

    vector<vector<string>> result;
    for (auto& [key, group] : groups) {
        result.push_back(group);
    }

    return result;
}

// === VALID ANAGRAM ===
bool isAnagram(string& s, string& t) {
    if (s.size() != t.size()) return false;

    vector<int> count(26, 0);
    for (int i = 0; i < s.size(); i++) {
        count[s[i] - 'a']++;
        count[t[i] - 'a']--;
    }

    for (int c : count) {
        if (c != 0) return false;
    }

    return true;
}

// === FIRST UNIQUE CHARACTER ===
int firstUniqChar(string& s) {
    vector<int> count(26, 0);

    for (char c : s) count[c - 'a']++;

    for (int i = 0; i < s.size(); i++) {
        if (count[s[i] - 'a'] == 1) return i;
    }

    return -1;
}

// === LRU CACHE ===
class LRUCache {
    int capacity;
    list<pair<int, int>> cache;  // (key, value)
    unordered_map<int, list<pair<int, int>>::iterator> map;

public:
    LRUCache(int cap) : capacity(cap) {}

    int get(int key) {
        if (!map.count(key)) return -1;

        // move to front
        cache.splice(cache.begin(), cache, map[key]);
        return map[key]->second;
    }

    void put(int key, int value) {
        if (map.count(key)) {
            cache.splice(cache.begin(), cache, map[key]);
            map[key]->second = value;
            return;
        }

        if (cache.size() == capacity) {
            map.erase(cache.back().first);
            cache.pop_back();
        }

        cache.push_front({key, value});
        map[key] = cache.begin();
    }
};

// === CONTAINS DUPLICATE WITHIN K ===
bool containsNearbyDuplicate(vector<int>& nums, int k) {
    unordered_map<int, int> seen;

    for (int i = 0; i < nums.size(); i++) {
        if (seen.count(nums[i]) && i - seen[nums[i]] <= k) {
            return true;
        }
        seen[nums[i]] = i;
    }

    return false;
}

// === INTERSECTION OF TWO ARRAYS ===
vector<int> intersection(vector<int>& nums1, vector<int>& nums2) {
    unordered_set<int> set1(nums1.begin(), nums1.end());
    unordered_set<int> result;

    for (int num : nums2) {
        if (set1.count(num)) {
            result.insert(num);
        }
    }

    return vector<int>(result.begin(), result.end());
}

vector<int> intersect(vector<int>& nums1, vector<int>& nums2) {
    unordered_map<int, int> count;
    for (int num : nums1) count[num]++;

    vector<int> result;
    for (int num : nums2) {
        if (count[num] > 0) {
            result.push_back(num);
            count[num]--;
        }
    }

    return result;
}

// === ISOMORPHIC STRINGS ===
bool isIsomorphic(string& s, string& t) {
    if (s.size() != t.size()) return false;

    unordered_map<char, char> sToT, tToS;

    for (int i = 0; i < s.size(); i++) {
        if (sToT.count(s[i]) && sToT[s[i]] != t[i]) return false;
        if (tToS.count(t[i]) && tToS[t[i]] != s[i]) return false;
        sToT[s[i]] = t[i];
        tToS[t[i]] = s[i];
    }

    return true;
}

// === WORD PATTERN ===
bool wordPattern(string& pattern, string& s) {
    vector<string> words;
    string word;
    for (char c : s) {
        if (c == ' ') {
            words.push_back(word);
            word = "";
        } else {
            word += c;
        }
    }
    words.push_back(word);

    if (pattern.size() != words.size()) return false;

    unordered_map<char, string> pToW;
    unordered_map<string, char> wToP;

    for (int i = 0; i < pattern.size(); i++) {
        if (pToW.count(pattern[i]) && pToW[pattern[i]] != words[i]) return false;
        if (wToP.count(words[i]) && wToP[words[i]] != pattern[i]) return false;
        pToW[pattern[i]] = words[i];
        wToP[words[i]] = pattern[i];
    }

    return true;
}

// === LONGEST CONSECUTIVE SEQUENCE ===
int longestConsecutive(vector<int>& nums) {
    unordered_set<int> numSet(nums.begin(), nums.end());
    int maxLen = 0;

    for (int num : numSet) {
        if (!numSet.count(num - 1)) {
            int curr = num;
            int length = 1;

            while (numSet.count(curr + 1)) {
                curr++;
                length++;
            }

            maxLen = max(maxLen, length);
        }
    }

    return maxLen;
}

// === MAJORITY ELEMENT (Boyer-Moore) ===
int majorityElement(vector<int>& nums) {
    int candidate = nums[0];
    int count = 1;

    for (int i = 1; i < nums.size(); i++) {
        if (count == 0) {
            candidate = nums[i];
            count = 1;
        } else if (nums[i] == candidate) {
            count++;
        } else {
            count--;
        }
    }

    return candidate;
}

// === DESIGN HASHMAP ===
class MyHashMap {
    static const int SIZE = 1000;
    vector<list<pair<int, int>>> buckets;

    int hash(int key) { return key % SIZE; }

public:
    MyHashMap() : buckets(SIZE) {}

    void put(int key, int value) {
        int idx = hash(key);
        for (auto& [k, v] : buckets[idx]) {
            if (k == key) {
                v = value;
                return;
            }
        }
        buckets[idx].push_back({key, value});
    }

    int get(int key) {
        int idx = hash(key);
        for (auto& [k, v] : buckets[idx]) {
            if (k == key) return v;
        }
        return -1;
    }

    void remove(int key) {
        int idx = hash(key);
        buckets[idx].remove_if([key](auto& p) { return p.first == key; });
    }
};

// === DESIGN HASHSET ===
class MyHashSet {
    static const int SIZE = 1000;
    vector<list<int>> buckets;

    int hash(int key) { return key % SIZE; }

public:
    MyHashSet() : buckets(SIZE) {}

    void add(int key) {
        int idx = hash(key);
        for (int k : buckets[idx]) {
            if (k == key) return;
        }
        buckets[idx].push_back(key);
    }

    void remove(int key) {
        int idx = hash(key);
        buckets[idx].remove(key);
    }

    bool contains(int key) {
        int idx = hash(key);
        for (int k : buckets[idx]) {
            if (k == key) return true;
        }
        return false;
    }
};
