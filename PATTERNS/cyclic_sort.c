/*
 * CYCLIC SORT PATTERN
 * Use: Arrays with values in range [1, n] or [0, n-1]
 * Time: O(n), Space: O(1)
 */

#include <stdlib.h>

// === CYCLIC SORT (values 1 to n) ===
// Each value v should be at index v-1
void cyclic_sort(int *nums, int n) {
    int i = 0;
    while (i < n) {
        int correct_idx = nums[i] - 1;  // where nums[i] should be

        if (nums[i] != nums[correct_idx]) {
            // swap to correct position
            int tmp = nums[i];
            nums[i] = nums[correct_idx];
            nums[correct_idx] = tmp;
        } else {
            i++;
        }
    }
}

// === CYCLIC SORT (values 0 to n-1) ===
void cyclic_sort_zero_based(int *nums, int n) {
    int i = 0;
    while (i < n) {
        int correct_idx = nums[i];  // where nums[i] should be

        if (nums[i] >= 0 && nums[i] < n && nums[i] != nums[correct_idx]) {
            int tmp = nums[i];
            nums[i] = nums[correct_idx];
            nums[correct_idx] = tmp;
        } else {
            i++;
        }
    }
}

// === FIND MISSING NUMBER (0 to n, one missing) ===
int find_missing_number(int *nums, int n) {
    int i = 0;
    while (i < n) {
        if (nums[i] < n && nums[i] != nums[nums[i]]) {
            int tmp = nums[i];
            nums[i] = nums[tmp];
            nums[tmp] = tmp;
        } else {
            i++;
        }
    }

    // find where value != index
    for (i = 0; i < n; i++) {
        if (nums[i] != i) {
            return i;
        }
    }
    return n;  // all 0 to n-1 present, so n is missing
}

// === FIND ALL MISSING NUMBERS ===
int find_all_missing(int *nums, int n, int *result) {
    // cyclic sort first
    int i = 0;
    while (i < n) {
        int correct_idx = nums[i] - 1;
        if (nums[i] > 0 && nums[i] <= n && nums[i] != nums[correct_idx]) {
            int tmp = nums[i];
            nums[i] = nums[correct_idx];
            nums[correct_idx] = tmp;
        } else {
            i++;
        }
    }

    // collect missing numbers
    int count = 0;
    for (i = 0; i < n; i++) {
        if (nums[i] != i + 1) {
            result[count++] = i + 1;
        }
    }
    return count;
}

// === FIND DUPLICATE NUMBER ===
int find_duplicate(int *nums, int n) {
    int i = 0;
    while (i < n) {
        if (nums[i] != i + 1) {
            int correct_idx = nums[i] - 1;
            if (nums[i] != nums[correct_idx]) {
                int tmp = nums[i];
                nums[i] = nums[correct_idx];
                nums[correct_idx] = tmp;
            } else {
                return nums[i];  // duplicate found
            }
        } else {
            i++;
        }
    }
    return -1;
}

// === FIND ALL DUPLICATES ===
int find_all_duplicates(int *nums, int n, int *result) {
    int i = 0;
    while (i < n) {
        int correct_idx = nums[i] - 1;
        if (nums[i] != nums[correct_idx]) {
            int tmp = nums[i];
            nums[i] = nums[correct_idx];
            nums[correct_idx] = tmp;
        } else {
            i++;
        }
    }

    int count = 0;
    for (i = 0; i < n; i++) {
        if (nums[i] != i + 1) {
            result[count++] = nums[i];
        }
    }
    return count;
}

// === FIND CORRUPT PAIR (one missing, one duplicate) ===
void find_corrupt_pair(int *nums, int n, int *duplicate, int *missing) {
    int i = 0;
    while (i < n) {
        int correct_idx = nums[i] - 1;
        if (nums[i] != nums[correct_idx]) {
            int tmp = nums[i];
            nums[i] = nums[correct_idx];
            nums[correct_idx] = tmp;
        } else {
            i++;
        }
    }

    for (i = 0; i < n; i++) {
        if (nums[i] != i + 1) {
            *duplicate = nums[i];
            *missing = i + 1;
            return;
        }
    }
}

// === FIRST MISSING POSITIVE ===
int first_missing_positive(int *nums, int n) {
    // treat array as 1-indexed: value v at index v-1
    int i = 0;
    while (i < n) {
        int correct_idx = nums[i] - 1;
        // only consider values in range [1, n]
        if (nums[i] > 0 && nums[i] <= n && nums[i] != nums[correct_idx]) {
            int tmp = nums[i];
            nums[i] = nums[correct_idx];
            nums[correct_idx] = tmp;
        } else {
            i++;
        }
    }

    // find first position where value != index + 1
    for (i = 0; i < n; i++) {
        if (nums[i] != i + 1) {
            return i + 1;
        }
    }
    return n + 1;
}

// === KTH MISSING POSITIVE NUMBER ===
// Array is sorted, find kth missing positive
int find_kth_missing(int *arr, int n, int k) {
    int lo = 0, hi = n;

    while (lo < hi) {
        int mid = (lo + hi) / 2;
        // number of missing before index mid
        int missing = arr[mid] - (mid + 1);

        if (missing < k) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }

    // at position lo, we've passed (arr[lo-1] - lo) missing numbers
    // answer = lo + k (1-indexed position of kth missing)
    return lo + k;
}
