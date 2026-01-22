/*
 * DYNAMIC PROGRAMMING PATTERNS
 * Use: Optimization, counting, decision problems
 * Time: O(n*m) typical, Space: O(n) with optimization
 */

#include <vector>
#include <string>
#include <algorithm>
#include <climits>

using namespace std;

// === 1D DP: CLIMBING STAIRS ===
int climbStairs(int n) {
    if (n <= 2) return n;

    int prev2 = 1, prev1 = 2;
    for (int i = 3; i <= n; i++) {
        int curr = prev1 + prev2;
        prev2 = prev1;
        prev1 = curr;
    }

    return prev1;
}

int fibonacci(int n) {
    if (n <= 1) return n;

    int prev2 = 0, prev1 = 1;
    for (int i = 2; i <= n; i++) {
        int curr = prev1 + prev2;
        prev2 = prev1;
        prev1 = curr;
    }

    return prev1;
}

// === HOUSE ROBBER ===
int rob(vector<int>& nums) {
    if (nums.empty()) return 0;
    if (nums.size() == 1) return nums[0];

    int prev2 = 0, prev1 = nums[0];
    for (int i = 1; i < nums.size(); i++) {
        int curr = max(prev1, prev2 + nums[i]);
        prev2 = prev1;
        prev1 = curr;
    }

    return prev1;
}

int robCircular(vector<int>& nums) {
    int n = nums.size();
    if (n == 1) return nums[0];

    auto robLinear = [](vector<int>& arr, int start, int end) {
        int prev2 = 0, prev1 = 0;
        for (int i = start; i <= end; i++) {
            int curr = max(prev1, prev2 + arr[i]);
            prev2 = prev1;
            prev1 = curr;
        }
        return prev1;
    };

    return max(robLinear(nums, 0, n - 2), robLinear(nums, 1, n - 1));
}

// === COIN CHANGE ===
int coinChange(vector<int>& coins, int amount) {
    vector<int> dp(amount + 1, INT_MAX);
    dp[0] = 0;

    for (int i = 1; i <= amount; i++) {
        for (int coin : coins) {
            if (coin <= i && dp[i - coin] != INT_MAX) {
                dp[i] = min(dp[i], dp[i - coin] + 1);
            }
        }
    }

    return dp[amount] == INT_MAX ? -1 : dp[amount];
}

int coinChangeWays(vector<int>& coins, int amount) {
    vector<int> dp(amount + 1, 0);
    dp[0] = 1;

    for (int coin : coins) {
        for (int i = coin; i <= amount; i++) {
            dp[i] += dp[i - coin];
        }
    }

    return dp[amount];
}

// === 0/1 KNAPSACK ===
int knapsack01(vector<int>& weights, vector<int>& values, int capacity) {
    int n = weights.size();
    vector<int> dp(capacity + 1, 0);

    for (int i = 0; i < n; i++) {
        for (int w = capacity; w >= weights[i]; w--) {
            dp[w] = max(dp[w], dp[w - weights[i]] + values[i]);
        }
    }

    return dp[capacity];
}

// === UNBOUNDED KNAPSACK ===
int knapsackUnbounded(vector<int>& weights, vector<int>& values, int capacity) {
    vector<int> dp(capacity + 1, 0);

    for (int w = 1; w <= capacity; w++) {
        for (int i = 0; i < weights.size(); i++) {
            if (weights[i] <= w) {
                dp[w] = max(dp[w], dp[w - weights[i]] + values[i]);
            }
        }
    }

    return dp[capacity];
}

// === LONGEST COMMON SUBSEQUENCE ===
int lcs(string& s1, string& s2) {
    int m = s1.size(), n = s2.size();
    vector<vector<int>> dp(2, vector<int>(n + 1, 0));

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (s1[i-1] == s2[j-1]) {
                dp[i % 2][j] = dp[(i-1) % 2][j-1] + 1;
            } else {
                dp[i % 2][j] = max(dp[(i-1) % 2][j], dp[i % 2][j-1]);
            }
        }
    }

    return dp[m % 2][n];
}

// === LONGEST INCREASING SUBSEQUENCE ===
int lis(vector<int>& nums) {
    vector<int> tails;

    for (int num : nums) {
        auto it = lower_bound(tails.begin(), tails.end(), num);
        if (it == tails.end()) {
            tails.push_back(num);
        } else {
            *it = num;
        }
    }

    return tails.size();
}

// === EDIT DISTANCE ===
int editDistance(string& word1, string& word2) {
    int m = word1.size(), n = word2.size();
    vector<int> dp(n + 1);

    for (int j = 0; j <= n; j++) dp[j] = j;

    for (int i = 1; i <= m; i++) {
        int prev = dp[0];
        dp[0] = i;
        for (int j = 1; j <= n; j++) {
            int temp = dp[j];
            if (word1[i-1] == word2[j-1]) {
                dp[j] = prev;
            } else {
                dp[j] = 1 + min({prev, dp[j], dp[j-1]});
            }
            prev = temp;
        }
    }

    return dp[n];
}

// === UNIQUE PATHS ===
int uniquePaths(int m, int n) {
    vector<int> dp(n, 1);

    for (int i = 1; i < m; i++) {
        for (int j = 1; j < n; j++) {
            dp[j] += dp[j-1];
        }
    }

    return dp[n-1];
}

int uniquePathsObstacles(vector<vector<int>>& grid) {
    int m = grid.size(), n = grid[0].size();
    vector<int> dp(n, 0);
    dp[0] = grid[0][0] == 0 ? 1 : 0;

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (grid[i][j] == 1) {
                dp[j] = 0;
            } else if (j > 0) {
                dp[j] += dp[j-1];
            }
        }
    }

    return dp[n-1];
}

// === MINIMUM PATH SUM ===
int minPathSum(vector<vector<int>>& grid) {
    int m = grid.size(), n = grid[0].size();
    vector<int> dp(n, INT_MAX);
    dp[0] = 0;

    for (int i = 0; i < m; i++) {
        dp[0] += grid[i][0];
        for (int j = 1; j < n; j++) {
            dp[j] = min(dp[j], dp[j-1]) + grid[i][j];
        }
    }

    return dp[n-1];
}

// === LONGEST PALINDROMIC SUBSEQUENCE ===
int longestPalindromeSubseq(string& s) {
    int n = s.size();
    vector<int> dp(n, 1);

    for (int i = n - 2; i >= 0; i--) {
        int prev = 0;
        for (int j = i + 1; j < n; j++) {
            int temp = dp[j];
            if (s[i] == s[j]) {
                dp[j] = prev + 2;
            } else {
                dp[j] = max(dp[j], dp[j-1]);
            }
            prev = temp;
        }
    }

    return dp[n-1];
}

// === PARTITION EQUAL SUBSET SUM ===
bool canPartition(vector<int>& nums) {
    int total = 0;
    for (int num : nums) total += num;

    if (total % 2) return false;

    int target = total / 2;
    vector<bool> dp(target + 1, false);
    dp[0] = true;

    for (int num : nums) {
        for (int j = target; j >= num; j--) {
            dp[j] = dp[j] || dp[j - num];
        }
    }

    return dp[target];
}

// === WORD BREAK ===
bool wordBreak(string& s, vector<string>& wordDict) {
    unordered_set<string> wordSet(wordDict.begin(), wordDict.end());
    int n = s.size();
    vector<bool> dp(n + 1, false);
    dp[0] = true;

    for (int i = 1; i <= n; i++) {
        for (int j = 0; j < i; j++) {
            if (dp[j] && wordSet.count(s.substr(j, i - j))) {
                dp[i] = true;
                break;
            }
        }
    }

    return dp[n];
}

#include <unordered_set>

// === MAXIMUM SUBARRAY (Kadane's) ===
int maxSubarray(vector<int>& nums) {
    int maxSum = nums[0], currSum = nums[0];

    for (int i = 1; i < nums.size(); i++) {
        currSum = max(nums[i], currSum + nums[i]);
        maxSum = max(maxSum, currSum);
    }

    return maxSum;
}

// === DECODE WAYS ===
int numDecodings(string& s) {
    if (s.empty() || s[0] == '0') return 0;

    int n = s.size();
    int prev2 = 1, prev1 = 1;

    for (int i = 1; i < n; i++) {
        int curr = 0;
        if (s[i] != '0') curr = prev1;
        if (s[i-1] == '1' || (s[i-1] == '2' && s[i] <= '6')) {
            curr += prev2;
        }
        prev2 = prev1;
        prev1 = curr;
    }

    return prev1;
}

// === BUY AND SELL STOCK ===
int maxProfitOneTransaction(vector<int>& prices) {
    int minPrice = INT_MAX, maxProfit = 0;

    for (int price : prices) {
        minPrice = min(minPrice, price);
        maxProfit = max(maxProfit, price - minPrice);
    }

    return maxProfit;
}

int maxProfitUnlimited(vector<int>& prices) {
    int profit = 0;
    for (int i = 1; i < prices.size(); i++) {
        if (prices[i] > prices[i-1]) {
            profit += prices[i] - prices[i-1];
        }
    }
    return profit;
}
