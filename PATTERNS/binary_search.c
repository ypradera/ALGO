/*
 * BINARY SEARCH PATTERN
 * Use: Sorted arrays, search space reduction, finding boundaries
 * Time: O(log n), Space: O(1)
 */

// === STANDARD BINARY SEARCH ===
int binary_search(int *arr, int n, int target) {
    int left = 0, right = n - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;  // avoid overflow

        if (arr[mid] == target) {
            return mid;
        } else if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return -1;  // not found
}

// === LOWER BOUND (first element >= target) ===
int lower_bound(int *arr, int n, int target) {
    int left = 0, right = n;

    while (left < right) {
        int mid = left + (right - left) / 2;

        if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return left;  // insertion point
}

// === UPPER BOUND (first element > target) ===
int upper_bound(int *arr, int n, int target) {
    int left = 0, right = n;

    while (left < right) {
        int mid = left + (right - left) / 2;

        if (arr[mid] <= target) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return left;
}

// === SEARCH ON ANSWER (e.g., minimum capacity to ship packages) ===
// When answer is in a range [lo, hi] and you need min valid answer
int search_on_answer(int lo, int hi) {
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;

        if (is_valid(mid)) {  // <-- define your condition
            hi = mid;         // try smaller
        } else {
            lo = mid + 1;     // need larger
        }
    }
    return lo;
}
