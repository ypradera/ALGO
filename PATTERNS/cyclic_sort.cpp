/*
 * CYCLIC SORT PATTERN
 * Use: Arrays with values in range [1, n] or [0, n-1]
 * Time: O(n), Space: O(1)
 */

#include <vector>
#include <algorithm>

using namespace std;

// === CYCLIC SORT (values 1 to n) ===
void cyclicSort(vector<int>& nums) {
    int i = 0;
    int n = nums.size();

    while (i < n) {
        int correctIdx = nums[i] - 1;

        if (nums[i] != nums[correctIdx]) {
            swap(nums[i], nums[correctIdx]);
        } else {
            i++;
        }
    }
}

// === CYCLIC SORT (values 0 to n-1) ===
void cyclicSortZeroBased(vector<int>& nums) {
    int i = 0;
    int n = nums.size();

    while (i < n) {
        int correctIdx = nums[i];

        if (correctIdx >= 0 && correctIdx < n && nums[i] != nums[correctIdx]) {
            swap(nums[i], nums[correctIdx]);
        } else {
            i++;
        }
    }
}

// === FIND MISSING NUMBER (0 to n, one missing) ===
int findMissingNumber(vector<int>& nums) {
    int n = nums.size();
    int i = 0;

    while (i < n) {
        if (nums[i] < n && nums[i] != nums[nums[i]]) {
            swap(nums[i], nums[nums[i]]);
        } else {
            i++;
        }
    }

    for (i = 0; i < n; i++) {
        if (nums[i] != i) {
            return i;
        }
    }

    return n;
}

int findMissingNumberXOR(vector<int>& nums) {
    int n = nums.size();
    int result = n;

    for (int i = 0; i < n; i++) {
        result ^= i ^ nums[i];
    }

    return result;
}

// === FIND ALL MISSING NUMBERS ===
vector<int> findAllMissing(vector<int>& nums) {
    int i = 0;
    int n = nums.size();

    while (i < n) {
        int correctIdx = nums[i] - 1;

        if (correctIdx >= 0 && correctIdx < n && nums[i] != nums[correctIdx]) {
            swap(nums[i], nums[correctIdx]);
        } else {
            i++;
        }
    }

    vector<int> missing;
    for (i = 0; i < n; i++) {
        if (nums[i] != i + 1) {
            missing.push_back(i + 1);
        }
    }

    return missing;
}

// === FIND DUPLICATE NUMBER ===
int findDuplicate(vector<int>& nums) {
    // Floyd's cycle detection
    int slow = nums[0];
    int fast = nums[0];

    do {
        slow = nums[slow];
        fast = nums[nums[fast]];
    } while (slow != fast);

    slow = nums[0];
    while (slow != fast) {
        slow = nums[slow];
        fast = nums[fast];
    }

    return slow;
}

int findDuplicateCyclic(vector<int>& nums) {
    int i = 0;
    int n = nums.size();

    while (i < n) {
        if (nums[i] != i + 1) {
            int correctIdx = nums[i] - 1;
            if (nums[i] != nums[correctIdx]) {
                swap(nums[i], nums[correctIdx]);
            } else {
                return nums[i];
            }
        } else {
            i++;
        }
    }

    return -1;
}

// === FIND ALL DUPLICATES ===
vector<int> findAllDuplicates(vector<int>& nums) {
    int i = 0;
    int n = nums.size();

    while (i < n) {
        int correctIdx = nums[i] - 1;

        if (nums[i] != nums[correctIdx]) {
            swap(nums[i], nums[correctIdx]);
        } else {
            i++;
        }
    }

    vector<int> duplicates;
    for (i = 0; i < n; i++) {
        if (nums[i] != i + 1) {
            duplicates.push_back(nums[i]);
        }
    }

    return duplicates;
}

vector<int> findAllDuplicatesMarking(vector<int>& nums) {
    vector<int> result;

    for (int num : nums) {
        int idx = abs(num) - 1;
        if (nums[idx] < 0) {
            result.push_back(abs(num));
        } else {
            nums[idx] = -nums[idx];
        }
    }

    return result;
}

// === FIND CORRUPT PAIR (one missing, one duplicate) ===
vector<int> findCorruptPair(vector<int>& nums) {
    int i = 0;
    int n = nums.size();

    while (i < n) {
        int correctIdx = nums[i] - 1;

        if (nums[i] != nums[correctIdx]) {
            swap(nums[i], nums[correctIdx]);
        } else {
            i++;
        }
    }

    for (i = 0; i < n; i++) {
        if (nums[i] != i + 1) {
            return {nums[i], i + 1};  // {duplicate, missing}
        }
    }

    return {};
}

// === FIRST MISSING POSITIVE ===
int firstMissingPositive(vector<int>& nums) {
    int n = nums.size();
    int i = 0;

    while (i < n) {
        int correctIdx = nums[i] - 1;

        if (nums[i] > 0 && nums[i] <= n && nums[i] != nums[correctIdx]) {
            swap(nums[i], nums[correctIdx]);
        } else {
            i++;
        }
    }

    for (i = 0; i < n; i++) {
        if (nums[i] != i + 1) {
            return i + 1;
        }
    }

    return n + 1;
}

// === KTH MISSING POSITIVE NUMBER ===
int findKthPositive(vector<int>& arr, int k) {
    int lo = 0, hi = arr.size();

    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        int missing = arr[mid] - (mid + 1);

        if (missing < k) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }

    return lo + k;
}

int findKthPositiveLinear(vector<int>& arr, int k) {
    int missing = 0;
    int current = 1;
    int i = 0;

    while (missing < k) {
        if (i < arr.size() && arr[i] == current) {
            i++;
        } else {
            missing++;
            if (missing == k) return current;
        }
        current++;
    }

    return current;
}

// === SMALLEST MISSING NON-NEGATIVE ===
int smallestMissingNonNegative(vector<int>& nums) {
    int n = nums.size();
    int i = 0;

    while (i < n) {
        int correctIdx = nums[i];

        if (correctIdx >= 0 && correctIdx < n && nums[i] != nums[correctIdx]) {
            swap(nums[i], nums[correctIdx]);
        } else {
            i++;
        }
    }

    for (i = 0; i < n; i++) {
        if (nums[i] != i) {
            return i;
        }
    }

    return n;
}
