/*
 * PREFIX SUM PATTERN
 * Use: Range queries, subarray sums, 2D queries
 * Time: O(n) build, O(1) query, Space: O(n)
 */

#include <string.h>

// === 1D PREFIX SUM ===
// Build: prefix[i] = sum of arr[0..i-1]
void build_prefix_sum(int *arr, int n, int *prefix) {
    prefix[0] = 0;
    for (int i = 0; i < n; i++) {
        prefix[i + 1] = prefix[i] + arr[i];
    }
}

// Query: sum of arr[left..right] inclusive
int range_sum(int *prefix, int left, int right) {
    return prefix[right + 1] - prefix[left];
}

// === SUBARRAY SUM EQUALS K ===
// Using hash map (here simplified with array for small values)
#define OFFSET 10000  // for negative sums
int subarray_sum_equals_k(int *nums, int n, int k) {
    static int count_map[20001];  // static for embedded
    memset(count_map, 0, sizeof(count_map));

    count_map[0 + OFFSET] = 1;  // empty prefix
    int sum = 0, result = 0;

    for (int i = 0; i < n; i++) {
        sum += nums[i];
        int target = sum - k;
        if (target >= -OFFSET && target <= OFFSET) {
            result += count_map[target + OFFSET];
        }
        count_map[sum + OFFSET]++;
    }
    return result;
}

// === 2D PREFIX SUM ===
#define MAX_N 100
int prefix2d[MAX_N + 1][MAX_N + 1];

void build_2d_prefix(int matrix[MAX_N][MAX_N], int rows, int cols) {
    memset(prefix2d, 0, sizeof(prefix2d));

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            prefix2d[i + 1][j + 1] = matrix[i][j]
                                   + prefix2d[i][j + 1]
                                   + prefix2d[i + 1][j]
                                   - prefix2d[i][j];
        }
    }
}

// Query: sum of submatrix from (r1,c1) to (r2,c2) inclusive
int range_sum_2d(int r1, int c1, int r2, int c2) {
    return prefix2d[r2 + 1][c2 + 1]
         - prefix2d[r1][c2 + 1]
         - prefix2d[r2 + 1][c1]
         + prefix2d[r1][c1];
}

// === DIFFERENCE ARRAY (Range updates) ===
// Efficient for multiple range updates, single final query
int diff[MAX_N + 1];

void range_add(int left, int right, int val, int n) {
    diff[left] += val;
    if (right + 1 < n) {
        diff[right + 1] -= val;
    }
}

void apply_diff(int *arr, int n) {
    arr[0] = diff[0];
    for (int i = 1; i < n; i++) {
        arr[i] = arr[i - 1] + diff[i];
    }
}
