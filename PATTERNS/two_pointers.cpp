/*
 * TWO POINTERS PATTERN
 * Use: Sorted arrays, finding pairs, partitioning
 * Time: O(n) typically, Space: O(1)
 */

#include <vector>
#include <algorithm>
#include <string>
#include <cmath>

using namespace std;

// === OPPOSITE DIRECTION (sorted array) ===
pair<int, int> twoSumSorted(vector<int>& arr, int target) {
    int left = 0, right = arr.size() - 1;

    while (left < right) {
        int sum = arr[left] + arr[right];

        if (sum == target) {
            return {left, right};
        } else if (sum < target) {
            left++;
        } else {
            right--;
        }
    }

    return {-1, -1};
}

vector<vector<int>> threeSum(vector<int>& arr, int target = 0) {
    sort(arr.begin(), arr.end());
    vector<vector<int>> result;
    int n = arr.size();

    for (int i = 0; i < n - 2; i++) {
        if (i > 0 && arr[i] == arr[i - 1]) continue;

        int left = i + 1, right = n - 1;

        while (left < right) {
            int sum = arr[i] + arr[left] + arr[right];

            if (sum == target) {
                result.push_back({arr[i], arr[left], arr[right]});
                left++;
                right--;
                while (left < right && arr[left] == arr[left - 1]) left++;
                while (left < right && arr[right] == arr[right + 1]) right--;
            } else if (sum < target) {
                left++;
            } else {
                right--;
            }
        }
    }

    return result;
}

int containerWithMostWater(vector<int>& heights) {
    int left = 0, right = heights.size() - 1;
    int maxArea = 0;

    while (left < right) {
        int width = right - left;
        int height = min(heights[left], heights[right]);
        maxArea = max(maxArea, width * height);

        if (heights[left] < heights[right]) {
            left++;
        } else {
            right--;
        }
    }

    return maxArea;
}

// === SAME DIRECTION ===
int removeDuplicates(vector<int>& arr) {
    if (arr.empty()) return 0;

    int write = 1;
    for (int read = 1; read < arr.size(); read++) {
        if (arr[read] != arr[read - 1]) {
            arr[write++] = arr[read];
        }
    }

    return write;
}

void moveZeros(vector<int>& arr) {
    int write = 0;

    for (int read = 0; read < arr.size(); read++) {
        if (arr[read] != 0) {
            swap(arr[write], arr[read]);
            write++;
        }
    }
}

int removeElement(vector<int>& arr, int val) {
    int write = 0;

    for (int read = 0; read < arr.size(); read++) {
        if (arr[read] != val) {
            arr[write++] = arr[read];
        }
    }

    return write;
}

// === DUTCH NATIONAL FLAG (3-way partition) ===
void sortColors(vector<int>& arr) {
    int low = 0, mid = 0, high = arr.size() - 1;

    while (mid <= high) {
        if (arr[mid] == 0) {
            swap(arr[low], arr[mid]);
            low++;
            mid++;
        } else if (arr[mid] == 1) {
            mid++;
        } else {
            swap(arr[mid], arr[high]);
            high--;
        }
    }
}

void partitionAroundPivot(vector<int>& arr, int pivot) {
    int write = 0;

    for (int read = 0; read < arr.size(); read++) {
        if (arr[read] < pivot) {
            swap(arr[write], arr[read]);
            write++;
        }
    }
}

// === MERGE TWO SORTED ARRAYS ===
vector<int> mergeSorted(vector<int>& arr1, vector<int>& arr2) {
    vector<int> result;
    int i = 0, j = 0;

    while (i < arr1.size() && j < arr2.size()) {
        if (arr1[i] <= arr2[j]) {
            result.push_back(arr1[i++]);
        } else {
            result.push_back(arr2[j++]);
        }
    }

    while (i < arr1.size()) result.push_back(arr1[i++]);
    while (j < arr2.size()) result.push_back(arr2[j++]);

    return result;
}

// === PALINDROME CHECK ===
bool isPalindrome(string& s) {
    int left = 0, right = s.size() - 1;

    while (left < right) {
        while (left < right && !isalnum(s[left])) left++;
        while (left < right && !isalnum(s[right])) right--;

        if (tolower(s[left]) != tolower(s[right])) {
            return false;
        }

        left++;
        right--;
    }

    return true;
}

// === TRAPPING RAIN WATER ===
int trapWater(vector<int>& heights) {
    if (heights.empty()) return 0;

    int left = 0, right = heights.size() - 1;
    int leftMax = heights[left], rightMax = heights[right];
    int water = 0;

    while (left < right) {
        if (leftMax < rightMax) {
            left++;
            leftMax = max(leftMax, heights[left]);
            water += leftMax - heights[left];
        } else {
            right--;
            rightMax = max(rightMax, heights[right]);
            water += rightMax - heights[right];
        }
    }

    return water;
}

// === SQUARES OF SORTED ARRAY ===
vector<int> sortedSquares(vector<int>& arr) {
    int n = arr.size();
    vector<int> result(n);
    int left = 0, right = n - 1;
    int pos = n - 1;

    while (left <= right) {
        if (abs(arr[left]) > abs(arr[right])) {
            result[pos--] = arr[left] * arr[left];
            left++;
        } else {
            result[pos--] = arr[right] * arr[right];
            right--;
        }
    }

    return result;
}

// === REVERSE STRING ===
void reverseString(vector<char>& s) {
    int left = 0, right = s.size() - 1;

    while (left < right) {
        swap(s[left], s[right]);
        left++;
        right--;
    }
}

// === VALID PALINDROME II (remove at most one char) ===
bool validPalindromeII(string& s) {
    auto isPalin = [&](int l, int r) {
        while (l < r) {
            if (s[l] != s[r]) return false;
            l++;
            r--;
        }
        return true;
    };

    int left = 0, right = s.size() - 1;
    while (left < right) {
        if (s[left] != s[right]) {
            return isPalin(left + 1, right) || isPalin(left, right - 1);
        }
        left++;
        right--;
    }

    return true;
}
