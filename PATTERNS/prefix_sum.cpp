/*
 * PREFIX SUM PATTERN
 * Use: Range sum queries, subarray sums, 2D range queries
 * Time: O(n) build, O(1) query, Space: O(n)
 */

#include <vector>
#include <unordered_map>

using namespace std;

// === 1D PREFIX SUM ===
vector<long long> buildPrefixSum(vector<int>& arr) {
    int n = arr.size();
    vector<long long> prefix(n + 1, 0);

    for (int i = 0; i < n; i++) {
        prefix[i + 1] = prefix[i] + arr[i];
    }

    return prefix;
}

long long rangeSum(vector<long long>& prefix, int left, int right) {
    return prefix[right + 1] - prefix[left];
}

// === SUBARRAY SUM EQUALS K ===
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

// === PRODUCT EXCEPT SELF ===
vector<int> productExceptSelf(vector<int>& arr) {
    int n = arr.size();
    vector<int> result(n, 1);

    // left products
    int leftProd = 1;
    for (int i = 0; i < n; i++) {
        result[i] = leftProd;
        leftProd *= arr[i];
    }

    // right products
    int rightProd = 1;
    for (int i = n - 1; i >= 0; i--) {
        result[i] *= rightProd;
        rightProd *= arr[i];
    }

    return result;
}

// === 2D PREFIX SUM ===
class PrefixSum2D {
public:
    vector<vector<long long>> prefix;

    PrefixSum2D(vector<vector<int>>& matrix) {
        if (matrix.empty() || matrix[0].empty()) return;

        int m = matrix.size(), n = matrix[0].size();
        prefix.assign(m + 1, vector<long long>(n + 1, 0));

        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                prefix[i + 1][j + 1] = matrix[i][j]
                    + prefix[i][j + 1]
                    + prefix[i + 1][j]
                    - prefix[i][j];
            }
        }
    }

    long long query(int r1, int c1, int r2, int c2) {
        return prefix[r2 + 1][c2 + 1]
             - prefix[r1][c2 + 1]
             - prefix[r2 + 1][c1]
             + prefix[r1][c1];
    }
};

// === DIFFERENCE ARRAY ===
vector<int> buildDifferenceArray(vector<int>& arr) {
    int n = arr.size();
    vector<int> diff(n);

    diff[0] = arr[0];
    for (int i = 1; i < n; i++) {
        diff[i] = arr[i] - arr[i - 1];
    }

    return diff;
}

void rangeAdd(vector<int>& diff, int left, int right, int val) {
    diff[left] += val;
    if (right + 1 < diff.size()) {
        diff[right + 1] -= val;
    }
}

vector<int> reconstructFromDiff(vector<int>& diff) {
    int n = diff.size();
    vector<int> arr(n);

    arr[0] = diff[0];
    for (int i = 1; i < n; i++) {
        arr[i] = arr[i - 1] + diff[i];
    }

    return arr;
}

// === RANGE UPDATE QUERIES ===
vector<int> processRangeUpdates(int n, vector<vector<int>>& updates) {
    vector<int> diff(n + 1, 0);

    for (auto& update : updates) {
        int left = update[0], right = update[1], val = update[2];
        diff[left] += val;
        diff[right + 1] -= val;
    }

    vector<int> result(n);
    result[0] = diff[0];
    for (int i = 1; i < n; i++) {
        result[i] = result[i - 1] + diff[i];
    }

    return result;
}

// === RUNNING SUM ===
vector<int> runningSum(vector<int>& arr) {
    vector<int> result;
    int total = 0;

    for (int num : arr) {
        total += num;
        result.push_back(total);
    }

    return result;
}

// === PIVOT INDEX ===
int pivotIndex(vector<int>& arr) {
    long long total = 0;
    for (int num : arr) total += num;

    long long leftSum = 0;
    for (int i = 0; i < arr.size(); i++) {
        if (leftSum == total - leftSum - arr[i]) {
            return i;
        }
        leftSum += arr[i];
    }

    return -1;
}

// === SUBARRAY SUM DIVISIBLE BY K ===
int subarraysDivByK(vector<int>& arr, int k) {
    unordered_map<int, int> remainderCount;
    remainderCount[0] = 1;

    int count = 0;
    int prefixSum = 0;

    for (int num : arr) {
        prefixSum += num;
        int remainder = ((prefixSum % k) + k) % k;  // handle negative

        count += remainderCount[remainder];
        remainderCount[remainder]++;
    }

    return count;
}

// === LONGEST SUBARRAY WITH SUM K ===
int longestSubarraySumK(vector<int>& arr, int k) {
    unordered_map<long long, int> prefixIdx;
    prefixIdx[0] = -1;

    long long prefixSum = 0;
    int maxLen = 0;

    for (int i = 0; i < arr.size(); i++) {
        prefixSum += arr[i];

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
int findMaxEqual01(vector<int>& arr) {
    unordered_map<int, int> prefixIdx;
    prefixIdx[0] = -1;

    int count = 0;
    int maxLen = 0;

    for (int i = 0; i < arr.size(); i++) {
        count += (arr[i] == 1) ? 1 : -1;

        if (prefixIdx.count(count)) {
            maxLen = max(maxLen, i - prefixIdx[count]);
        } else {
            prefixIdx[count] = i;
        }
    }

    return maxLen;
}
