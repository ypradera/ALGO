/*
 * BINARY SEARCH PATTERN
 * Use: Sorted arrays, search space reduction, optimization problems
 * Time: O(log n), Space: O(1)
 */

#include <vector>
#include <algorithm>
#include <functional>
#include <cmath>

using namespace std;

// === STANDARD BINARY SEARCH ===
int binarySearch(vector<int>& arr, int target) {
    int lo = 0, hi = arr.size() - 1;

    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;

        if (arr[mid] == target) {
            return mid;
        } else if (arr[mid] < target) {
            lo = mid + 1;
        } else {
            hi = mid - 1;
        }
    }

    return -1;
}

// === LOWER BOUND (first >= target) ===
int lowerBound(vector<int>& arr, int target) {
    int lo = 0, hi = arr.size();

    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;

        if (arr[mid] < target) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }

    return lo;
}

// === UPPER BOUND (first > target) ===
int upperBound(vector<int>& arr, int target) {
    int lo = 0, hi = arr.size();

    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;

        if (arr[mid] <= target) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }

    return lo;
}

// === FIND FIRST AND LAST OCCURRENCE ===
vector<int> searchRange(vector<int>& arr, int target) {
    int first = lowerBound(arr, target);

    if (first == arr.size() || arr[first] != target) {
        return {-1, -1};
    }

    int last = upperBound(arr, target) - 1;
    return {first, last};
}

// === SEARCH IN ROTATED SORTED ARRAY ===
int searchRotated(vector<int>& arr, int target) {
    int lo = 0, hi = arr.size() - 1;

    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;

        if (arr[mid] == target) {
            return mid;
        }

        // left half is sorted
        if (arr[lo] <= arr[mid]) {
            if (arr[lo] <= target && target < arr[mid]) {
                hi = mid - 1;
            } else {
                lo = mid + 1;
            }
        }
        // right half is sorted
        else {
            if (arr[mid] < target && target <= arr[hi]) {
                lo = mid + 1;
            } else {
                hi = mid - 1;
            }
        }
    }

    return -1;
}

// === FIND MINIMUM IN ROTATED ARRAY ===
int findMinRotated(vector<int>& arr) {
    int lo = 0, hi = arr.size() - 1;

    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;

        if (arr[mid] > arr[hi]) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }

    return arr[lo];
}

// === FIND PEAK ELEMENT ===
int findPeak(vector<int>& arr) {
    int lo = 0, hi = arr.size() - 1;

    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;

        if (arr[mid] < arr[mid + 1]) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }

    return lo;
}

// === SEARCH ON ANSWER (minimization) ===
int searchOnAnswerMin(int lo, int hi, function<bool(int)> isValid) {
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;

        if (isValid(mid)) {
            hi = mid;
        } else {
            lo = mid + 1;
        }
    }

    return lo;
}

// === SEARCH ON ANSWER (maximization) ===
int searchOnAnswerMax(int lo, int hi, function<bool(int)> isValid) {
    while (lo < hi) {
        int mid = lo + (hi - lo + 1) / 2;  // bias right

        if (isValid(mid)) {
            lo = mid;
        } else {
            hi = mid - 1;
        }
    }

    return lo;
}

// === KOKO EATING BANANAS ===
int minEatingSpeed(vector<int>& piles, int h) {
    auto canFinish = [&](int speed) {
        long long hours = 0;
        for (int pile : piles) {
            hours += (pile + speed - 1) / speed;
        }
        return hours <= h;
    };

    int lo = 1, hi = *max_element(piles.begin(), piles.end());

    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;

        if (canFinish(mid)) {
            hi = mid;
        } else {
            lo = mid + 1;
        }
    }

    return lo;
}

// === SPLIT ARRAY LARGEST SUM ===
int splitArray(vector<int>& arr, int k) {
    auto canSplit = [&](int maxSum) {
        int count = 1;
        long long currSum = 0;

        for (int num : arr) {
            if (currSum + num > maxSum) {
                count++;
                currSum = num;
            } else {
                currSum += num;
            }
        }

        return count <= k;
    };

    int lo = *max_element(arr.begin(), arr.end());
    long long hi = 0;
    for (int num : arr) hi += num;

    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;

        if (canSplit(mid)) {
            hi = mid;
        } else {
            lo = mid + 1;
        }
    }

    return lo;
}

// === SQRT(X) ===
int sqrtInt(int x) {
    if (x < 2) return x;

    long lo = 1, hi = x / 2;

    while (lo <= hi) {
        long mid = lo + (hi - lo) / 2;
        long sq = mid * mid;

        if (sq == x) {
            return mid;
        } else if (sq < x) {
            lo = mid + 1;
        } else {
            hi = mid - 1;
        }
    }

    return hi;
}

// === BINARY SEARCH ON FLOATS ===
double sqrtFloat(double x, double epsilon = 1e-9) {
    double lo = 0.0, hi = max(1.0, x);

    while (hi - lo > epsilon) {
        double mid = (lo + hi) / 2;

        if (mid * mid < x) {
            lo = mid;
        } else {
            hi = mid;
        }
    }

    return lo;
}

// === SEARCH 2D MATRIX ===
bool searchMatrix(vector<vector<int>>& matrix, int target) {
    if (matrix.empty() || matrix[0].empty()) return false;

    int m = matrix.size(), n = matrix[0].size();
    int lo = 0, hi = m * n - 1;

    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        int val = matrix[mid / n][mid % n];

        if (val == target) {
            return true;
        } else if (val < target) {
            lo = mid + 1;
        } else {
            hi = mid - 1;
        }
    }

    return false;
}

// === FIND KTH SMALLEST IN SORTED MATRIX ===
int kthSmallest(vector<vector<int>>& matrix, int k) {
    int n = matrix.size();
    int lo = matrix[0][0], hi = matrix[n-1][n-1];

    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;

        // count elements <= mid
        int count = 0;
        int col = n - 1;
        for (int row = 0; row < n; row++) {
            while (col >= 0 && matrix[row][col] > mid) {
                col--;
            }
            count += col + 1;
        }

        if (count < k) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }

    return lo;
}
