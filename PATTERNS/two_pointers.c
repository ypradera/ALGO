/*
 * TWO POINTERS PATTERN
 * Use: Sorted arrays, pair finding, partitioning
 * Time: O(n), Space: O(1)
 */

// === OPPOSITE DIRECTION (e.g., Two Sum in sorted array) ===
int left = 0, right = n - 1;

while (left < right) {
    int sum = arr[left] + arr[right];

    if (sum == target) {
        // found pair
        return 1;
    } else if (sum < target) {
        left++;
    } else {
        right--;
    }
}

// === SAME DIRECTION (e.g., Remove duplicates in-place) ===
int slow = 0;

for (int fast = 1; fast < n; fast++) {
    if (arr[fast] != arr[slow]) {
        slow++;
        arr[slow] = arr[fast];
    }
}
// new length = slow + 1

// === PARTITION (e.g., Dutch National Flag / Sort colors) ===
int low = 0, mid = 0, high = n - 1;

while (mid <= high) {
    if (arr[mid] == 0) {
        // swap arr[low] and arr[mid]
        int tmp = arr[low]; arr[low] = arr[mid]; arr[mid] = tmp;
        low++;
        mid++;
    } else if (arr[mid] == 1) {
        mid++;
    } else {  // arr[mid] == 2
        // swap arr[mid] and arr[high]
        int tmp = arr[mid]; arr[mid] = arr[high]; arr[high] = tmp;
        high--;
    }
}
