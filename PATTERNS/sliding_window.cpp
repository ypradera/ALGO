/*
 * SLIDING WINDOW PATTERN
 * Use: Contiguous subarray/substring problems
 * Time: O(n), Space: O(1) or O(k) for frequency maps
 */

#include <vector>
#include <string>
#include <unordered_map>
#include <deque>
#include <algorithm>
#include <climits>

using namespace std;

// === FIXED-SIZE WINDOW ===
int maxSumFixedWindow(vector<int>& arr, int k) {
    int n = arr.size();
    if (n < k) return -1;

    int windowSum = 0;
    for (int i = 0; i < k; i++) {
        windowSum += arr[i];
    }

    int maxSum = windowSum;
    for (int i = k; i < n; i++) {
        windowSum += arr[i] - arr[i - k];
        maxSum = max(maxSum, windowSum);
    }

    return maxSum;
}

// === VARIABLE-SIZE WINDOW (shrinkable) ===
int longestSubarraySumAtMost(vector<int>& arr, long long target) {
    int left = 0;
    long long sum = 0;
    int best = 0;

    for (int right = 0; right < arr.size(); right++) {
        sum += arr[right];

        while (left <= right && sum > target) {
            sum -= arr[left];
            left++;
        }

        best = max(best, right - left + 1);
    }

    return best;
}

// === VARIABLE-SIZE WINDOW (non-shrinkable optimization) ===
int longestSubarrayOptimized(vector<int>& arr, long long target) {
    int left = 0;
    long long sum = 0;
    int n = arr.size();

    for (int right = 0; right < n; right++) {
        sum += arr[right];

        if (sum > target) {
            sum -= arr[left];
            left++;
        }
    }

    return n - left;
}

// === SMALLEST WINDOW WITH SUM >= TARGET ===
int minSubarraySumAtLeast(vector<int>& arr, int target) {
    int left = 0;
    long long sum = 0;
    int minLen = INT_MAX;
    int n = arr.size();

    for (int right = 0; right < n; right++) {
        sum += arr[right];

        while (sum >= target) {
            minLen = min(minLen, right - left + 1);
            sum -= arr[left];
            left++;
        }
    }

    return minLen == INT_MAX ? 0 : minLen;
}

// === LONGEST SUBSTRING WITH K DISTINCT CHARS ===
int longestKDistinct(string& s, int k) {
    unordered_map<char, int> freq;
    int left = 0;
    int best = 0;
    int n = s.size();

    for (int right = 0; right < n; right++) {
        freq[s[right]]++;

        while (freq.size() > k) {
            freq[s[left]]--;
            if (freq[s[left]] == 0) {
                freq.erase(s[left]);
            }
            left++;
        }

        best = max(best, right - left + 1);
    }

    return best;
}

// === LONGEST SUBSTRING WITHOUT REPEATING ===
int longestNoRepeat(string& s) {
    unordered_map<char, int> lastSeen;
    int left = 0;
    int best = 0;
    int n = s.size();

    for (int right = 0; right < n; right++) {
        char c = s[right];

        if (lastSeen.count(c) && lastSeen[c] >= left) {
            left = lastSeen[c] + 1;
        }

        lastSeen[c] = right;
        best = max(best, right - left + 1);
    }

    return best;
}

// === MINIMUM WINDOW SUBSTRING ===
string minWindowSubstring(string& s, string& pattern) {
    unordered_map<char, int> need, have;
    for (char c : pattern) need[c]++;

    int required = need.size();
    int formed = 0;
    int left = 0;
    int minLen = INT_MAX;
    int resultStart = 0;

    for (int right = 0; right < s.size(); right++) {
        char c = s[right];
        have[c]++;

        if (need.count(c) && have[c] == need[c]) {
            formed++;
        }

        while (formed == required) {
            if (right - left + 1 < minLen) {
                minLen = right - left + 1;
                resultStart = left;
            }

            char lc = s[left];
            have[lc]--;
            if (need.count(lc) && have[lc] < need[lc]) {
                formed--;
            }
            left++;
        }
    }

    return minLen == INT_MAX ? "" : s.substr(resultStart, minLen);
}

// === SLIDING WINDOW MAXIMUM (monotonic deque) ===
vector<int> slidingWindowMax(vector<int>& arr, int k) {
    deque<int> dq;  // stores indices
    vector<int> result;
    int n = arr.size();

    for (int i = 0; i < n; i++) {
        // remove elements outside window
        while (!dq.empty() && dq.front() <= i - k) {
            dq.pop_front();
        }

        // remove smaller elements
        while (!dq.empty() && arr[dq.back()] <= arr[i]) {
            dq.pop_back();
        }

        dq.push_back(i);

        if (i >= k - 1) {
            result.push_back(arr[dq.front()]);
        }
    }

    return result;
}

// === COUNT SUBARRAYS WITH SUM EQUALS K ===
int countSubarraysSumK(vector<int>& arr, int k) {
    unordered_map<long long, int> prefixCount;
    prefixCount[0] = 1;

    int count = 0;
    long long prefixSum = 0;

    for (int num : arr) {
        prefixSum += num;
        count += prefixCount[prefixSum - k];
        prefixCount[prefixSum]++;
    }

    return count;
}

// === MAX CONSECUTIVE ONES WITH K FLIPS ===
int maxOnesWithKFlips(vector<int>& arr, int k) {
    int left = 0;
    int zeros = 0;
    int n = arr.size();

    for (int right = 0; right < n; right++) {
        if (arr[right] == 0) {
            zeros++;
        }

        if (zeros > k) {
            if (arr[left] == 0) {
                zeros--;
            }
            left++;
        }
    }

    return n - left;
}

// === FRUITS INTO BASKETS (at most 2 types) ===
int totalFruit(vector<int>& fruits) {
    unordered_map<int, int> basket;
    int left = 0;
    int maxFruits = 0;

    for (int right = 0; right < fruits.size(); right++) {
        basket[fruits[right]]++;

        while (basket.size() > 2) {
            basket[fruits[left]]--;
            if (basket[fruits[left]] == 0) {
                basket.erase(fruits[left]);
            }
            left++;
        }

        maxFruits = max(maxFruits, right - left + 1);
    }

    return maxFruits;
}
