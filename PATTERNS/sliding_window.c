/*
 * SLIDING WINDOW PATTERN
 * Use: Contiguous subarray/substring problems
 * Time: O(n), Space: O(1) or O(k) for frequency maps
 */

#include <string.h>

#define MAX_N 100000
#define CHAR_SET 256  // ASCII character set size

// === FIXED-SIZE WINDOW ===
// Find max sum of subarray of size k
int max_sum_fixed_window(int *arr, int n, int k) {
    if (n < k) return -1;

    // compute first window
    int window_sum = 0;
    for (int i = 0; i < k; i++) {
        window_sum += arr[i];
    }

    int max_sum = window_sum;

    // slide window: add right, remove left
    for (int i = k; i < n; i++) {
        window_sum += arr[i] - arr[i - k];
        if (window_sum > max_sum) {
            max_sum = window_sum;
        }
    }

    return max_sum;
}

// === VARIABLE-SIZE WINDOW (shrinkable) ===
// Find longest subarray with sum <= target
int longest_subarray_sum_at_most(int *arr, int n, long long target) {
    int left = 0;
    long long sum = 0;
    int best = 0;

    for (int right = 0; right < n; right++) {
        // expand: include right element
        sum += arr[right];

        // shrink: while window is invalid
        while (left <= right && sum > target) {
            sum -= arr[left];
            left++;
        }

        // update answer with valid window
        int len = right - left + 1;
        if (len > best) best = len;
    }

    return best;
}

// === VARIABLE-SIZE WINDOW (non-shrinkable) ===
// Find max length where window never shrinks past best
// Optimization: window size only grows or stays same
int longest_subarray_optimized(int *arr, int n, long long target) {
    int left = 0;
    long long sum = 0;

    for (int right = 0; right < n; right++) {
        sum += arr[right];

        if (sum > target) {
            // slide window (don't shrink, just move)
            sum -= arr[left];
            left++;
        }
    }

    return n - left;  // final window size is the answer
}

// === SMALLEST WINDOW WITH SUM >= TARGET ===
int min_subarray_sum_at_least(int *arr, int n, int target) {
    int left = 0;
    long long sum = 0;
    int min_len = n + 1;  // impossible value

    for (int right = 0; right < n; right++) {
        sum += arr[right];

        // shrink while valid to find minimum
        while (sum >= target) {
            int len = right - left + 1;
            if (len < min_len) min_len = len;
            sum -= arr[left];
            left++;
        }
    }

    return (min_len == n + 1) ? 0 : min_len;
}

// === WINDOW WITH FREQUENCY COUNT (string) ===
// Longest substring with at most k distinct characters
int longest_k_distinct(char *s, int k) {
    int freq[CHAR_SET] = {0};
    int distinct = 0;
    int left = 0;
    int best = 0;
    int n = strlen(s);

    for (int right = 0; right < n; right++) {
        // expand: add s[right]
        if (freq[(unsigned char)s[right]] == 0) {
            distinct++;
        }
        freq[(unsigned char)s[right]]++;

        // shrink: while too many distinct chars
        while (distinct > k) {
            freq[(unsigned char)s[left]]--;
            if (freq[(unsigned char)s[left]] == 0) {
                distinct--;
            }
            left++;
        }

        int len = right - left + 1;
        if (len > best) best = len;
    }

    return best;
}

// === LONGEST SUBSTRING WITHOUT REPEATING ===
int longest_no_repeat(char *s) {
    int last_seen[CHAR_SET];
    memset(last_seen, -1, sizeof(last_seen));

    int left = 0;
    int best = 0;
    int n = strlen(s);

    for (int right = 0; right < n; right++) {
        unsigned char c = s[right];

        // if char was seen in current window, shrink
        if (last_seen[c] >= left) {
            left = last_seen[c] + 1;
        }

        last_seen[c] = right;

        int len = right - left + 1;
        if (len > best) best = len;
    }

    return best;
}

// === MINIMUM WINDOW SUBSTRING ===
// Find smallest window in s containing all chars of pattern
int min_window_substring(char *s, char *pattern, int *result_start) {
    int need[CHAR_SET] = {0};
    int have[CHAR_SET] = {0};
    int required = 0;
    int formed = 0;

    // count required characters
    for (int i = 0; pattern[i]; i++) {
        if (need[(unsigned char)pattern[i]] == 0) {
            required++;
        }
        need[(unsigned char)pattern[i]]++;
    }

    int left = 0;
    int min_len = -1;
    *result_start = 0;
    int n = strlen(s);

    for (int right = 0; right < n; right++) {
        unsigned char c = s[right];
        have[c]++;

        if (need[c] > 0 && have[c] == need[c]) {
            formed++;
        }

        // shrink while valid
        while (formed == required) {
            int len = right - left + 1;
            if (min_len == -1 || len < min_len) {
                min_len = len;
                *result_start = left;
            }

            unsigned char lc = s[left];
            have[lc]--;
            if (need[lc] > 0 && have[lc] < need[lc]) {
                formed--;
            }
            left++;
        }
    }

    return min_len;
}

// === SLIDING WINDOW MAXIMUM (using deque - see stack_queue.c) ===
// For each window of size k, find the maximum
// Uses monotonic deque - O(n) time
void sliding_window_max(int *arr, int n, int k, int *result) {
    int deque[MAX_N];  // stores indices
    int front = 0, back = 0;

    for (int i = 0; i < n; i++) {
        // remove elements outside window
        while (front < back && deque[front] <= i - k) {
            front++;
        }

        // remove smaller elements (maintain decreasing order)
        while (front < back && arr[deque[back - 1]] <= arr[i]) {
            back--;
        }

        deque[back++] = i;

        // record result after first k elements
        if (i >= k - 1) {
            result[i - k + 1] = arr[deque[front]];
        }
    }
}

// === COUNT SUBARRAYS WITH SUM EQUALS K ===
// For O(n) hashmap version, see hashmap.c
// This is O(n^2) version without external dependencies
int count_subarrays_sum_k(int *arr, int n, int k) {
    int count = 0;

    for (int left = 0; left < n; left++) {
        long long sum = 0;
        for (int right = left; right < n; right++) {
            sum += arr[right];
            if (sum == k) {
                count++;
            }
        }
    }

    return count;
}

// === TEMPLATE: Generic sliding window pattern ===
/*
int sliding_window_template(int *arr, int n, <params>) {
    int left = 0;
    <state> state = initial_state;
    int best = initial_answer;

    for (int right = 0; right < n; right++) {
        // 1. EXPAND: add arr[right] to window
        update_state(&state, arr[right]);

        // 2. SHRINK: while window is invalid
        while (left <= right && is_invalid(state)) {
            remove_from_state(&state, arr[left]);
            left++;
        }

        // 3. UPDATE: record answer from valid window [left..right]
        int current = calculate_answer(left, right, state);
        if (is_better(current, best)) {
            best = current;
        }
    }

    return best;
}
*/
