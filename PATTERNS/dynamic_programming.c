/*
 * DYNAMIC PROGRAMMING PATTERNS
 * Use: Optimization, counting, decision problems
 * Embedded focus: 1D/2D arrays, space optimization
 */

#include <string.h>
#include <limits.h>

#define MAX_N 1000
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// === 1D DP: FIBONACCI / CLIMBING STAIRS ===
int climb_stairs(int n) {
    if (n <= 2) return n;

    int prev2 = 1, prev1 = 2;
    for (int i = 3; i <= n; i++) {
        int curr = prev1 + prev2;
        prev2 = prev1;
        prev1 = curr;
    }
    return prev1;
}

// === 1D DP: HOUSE ROBBER ===
int rob(int *nums, int n) {
    if (n == 0) return 0;
    if (n == 1) return nums[0];

    int prev2 = nums[0];
    int prev1 = MAX(nums[0], nums[1]);

    for (int i = 2; i < n; i++) {
        int curr = MAX(prev1, prev2 + nums[i]);
        prev2 = prev1;
        prev1 = curr;
    }
    return prev1;
}

// === 1D DP: COIN CHANGE (Minimum coins) ===
int coin_change(int *coins, int num_coins, int amount) {
    int dp[MAX_N + 1];

    for (int i = 0; i <= amount; i++) {
        dp[i] = amount + 1;  // "infinity"
    }
    dp[0] = 0;

    for (int i = 1; i <= amount; i++) {
        for (int j = 0; j < num_coins; j++) {
            if (coins[j] <= i) {
                dp[i] = MIN(dp[i], dp[i - coins[j]] + 1);
            }
        }
    }

    return dp[amount] > amount ? -1 : dp[amount];
}

// === 2D DP: LONGEST COMMON SUBSEQUENCE ===
int lcs(const char *s1, const char *s2) {
    int m = strlen(s1), n = strlen(s2);
    int dp[2][MAX_N + 1];  // space optimized
    memset(dp, 0, sizeof(dp));

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (s1[i - 1] == s2[j - 1]) {
                dp[i % 2][j] = dp[(i - 1) % 2][j - 1] + 1;
            } else {
                dp[i % 2][j] = MAX(dp[(i - 1) % 2][j], dp[i % 2][j - 1]);
            }
        }
    }

    return dp[m % 2][n];
}

// === 2D DP: EDIT DISTANCE ===
int edit_distance(const char *s1, const char *s2) {
    int m = strlen(s1), n = strlen(s2);
    int dp[2][MAX_N + 1];

    // base case
    for (int j = 0; j <= n; j++) dp[0][j] = j;

    for (int i = 1; i <= m; i++) {
        dp[i % 2][0] = i;
        for (int j = 1; j <= n; j++) {
            if (s1[i - 1] == s2[j - 1]) {
                dp[i % 2][j] = dp[(i - 1) % 2][j - 1];
            } else {
                dp[i % 2][j] = 1 + MIN(dp[(i - 1) % 2][j - 1],  // replace
                                   MIN(dp[(i - 1) % 2][j],       // delete
                                       dp[i % 2][j - 1]));       // insert
            }
        }
    }

    return dp[m % 2][n];
}

// === 2D DP: 0/1 KNAPSACK ===
int knapsack(int *weights, int *values, int n, int capacity) {
    int dp[2][MAX_N + 1];
    memset(dp, 0, sizeof(dp));

    for (int i = 1; i <= n; i++) {
        for (int w = 0; w <= capacity; w++) {
            dp[i % 2][w] = dp[(i - 1) % 2][w];  // don't take
            if (weights[i - 1] <= w) {
                int take = values[i - 1] + dp[(i - 1) % 2][w - weights[i - 1]];
                dp[i % 2][w] = MAX(dp[i % 2][w], take);
            }
        }
    }

    return dp[n % 2][capacity];
}

// === 1D DP: LONGEST INCREASING SUBSEQUENCE ===
int lis(int *nums, int n) {
    int dp[MAX_N];  // dp[i] = smallest ending element for LIS of length i+1
    int len = 0;

    for (int i = 0; i < n; i++) {
        // binary search for position
        int lo = 0, hi = len;
        while (lo < hi) {
            int mid = (lo + hi) / 2;
            if (dp[mid] < nums[i]) {
                lo = mid + 1;
            } else {
                hi = mid;
            }
        }
        dp[lo] = nums[i];
        if (lo == len) len++;
    }

    return len;
}

// === GRID DP: UNIQUE PATHS ===
int unique_paths(int m, int n) {
    int dp[MAX_N];

    for (int j = 0; j < n; j++) dp[j] = 1;

    for (int i = 1; i < m; i++) {
        for (int j = 1; j < n; j++) {
            dp[j] += dp[j - 1];
        }
    }

    return dp[n - 1];
}

// === GRID DP: MINIMUM PATH SUM ===
int min_path_sum(int grid[][MAX_N], int m, int n) {
    int dp[MAX_N];

    dp[0] = grid[0][0];
    for (int j = 1; j < n; j++) {
        dp[j] = dp[j - 1] + grid[0][j];
    }

    for (int i = 1; i < m; i++) {
        dp[0] += grid[i][0];
        for (int j = 1; j < n; j++) {
            dp[j] = MIN(dp[j], dp[j - 1]) + grid[i][j];
        }
    }

    return dp[n - 1];
}

// === INTERVAL DP: MATRIX CHAIN MULTIPLICATION ===
int matrix_chain(int *dims, int n) {
    // dims[i-1] x dims[i] is matrix i
    int dp[MAX_N][MAX_N];
    memset(dp, 0, sizeof(dp));

    for (int len = 2; len < n; len++) {
        for (int i = 1; i < n - len + 1; i++) {
            int j = i + len - 1;
            dp[i][j] = INT_MAX;
            for (int k = i; k < j; k++) {
                int cost = dp[i][k] + dp[k + 1][j] +
                          dims[i - 1] * dims[k] * dims[j];
                dp[i][j] = MIN(dp[i][j], cost);
            }
        }
    }

    return dp[1][n - 1];
}
