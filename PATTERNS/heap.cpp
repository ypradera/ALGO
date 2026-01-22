/*
 * HEAP / PRIORITY QUEUE PATTERNS
 * Use: Top-K, median, scheduling, merge K sorted
 * Time: O(log n) insert/delete, O(1) peek
 */

#include <vector>
#include <queue>
#include <functional>
#include <algorithm>

using namespace std;

// === BASIC HEAP OPERATIONS ===
void heapDemo() {
    // min-heap
    priority_queue<int, vector<int>, greater<int>> minHeap;

    // max-heap (default)
    priority_queue<int> maxHeap;

    // push O(log n)
    minHeap.push(5);
    maxHeap.push(5);

    // top O(1)
    int minTop = minHeap.top();
    int maxTop = maxHeap.top();

    // pop O(log n)
    minHeap.pop();
    maxHeap.pop();
}

// === KTH LARGEST ELEMENT ===
int findKthLargest(vector<int>& nums, int k) {
    priority_queue<int, vector<int>, greater<int>> minHeap;

    for (int num : nums) {
        minHeap.push(num);
        if (minHeap.size() > k) {
            minHeap.pop();
        }
    }

    return minHeap.top();
}

// Quickselect O(n) average
int findKthLargestQuickselect(vector<int>& nums, int k) {
    int n = nums.size();
    k = n - k;  // convert to kth smallest

    function<int(int, int)> quickselect = [&](int lo, int hi) -> int {
        int pivot = nums[hi];
        int i = lo;

        for (int j = lo; j < hi; j++) {
            if (nums[j] < pivot) {
                swap(nums[i], nums[j]);
                i++;
            }
        }
        swap(nums[i], nums[hi]);

        if (i == k) return nums[i];
        if (i < k) return quickselect(i + 1, hi);
        return quickselect(lo, i - 1);
    };

    return quickselect(0, n - 1);
}

// === TOP K FREQUENT ELEMENTS ===
vector<int> topKFrequent(vector<int>& nums, int k) {
    unordered_map<int, int> count;
    for (int num : nums) count[num]++;

    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> minHeap;

    for (auto& [num, freq] : count) {
        minHeap.push({freq, num});
        if (minHeap.size() > k) {
            minHeap.pop();
        }
    }

    vector<int> result;
    while (!minHeap.empty()) {
        result.push_back(minHeap.top().second);
        minHeap.pop();
    }

    return result;
}

#include <unordered_map>

// === MERGE K SORTED LISTS ===
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

ListNode* mergeKLists(vector<ListNode*>& lists) {
    auto cmp = [](ListNode* a, ListNode* b) { return a->val > b->val; };
    priority_queue<ListNode*, vector<ListNode*>, decltype(cmp)> minHeap(cmp);

    for (ListNode* lst : lists) {
        if (lst) minHeap.push(lst);
    }

    ListNode dummy(0);
    ListNode* curr = &dummy;

    while (!minHeap.empty()) {
        ListNode* node = minHeap.top();
        minHeap.pop();
        curr->next = node;
        curr = curr->next;

        if (node->next) {
            minHeap.push(node->next);
        }
    }

    return dummy.next;
}

// === MERGE K SORTED ARRAYS ===
vector<int> mergeKArrays(vector<vector<int>>& arrays) {
    // (value, array_idx, element_idx)
    priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, greater<>> minHeap;

    for (int i = 0; i < arrays.size(); i++) {
        if (!arrays[i].empty()) {
            minHeap.push({arrays[i][0], i, 0});
        }
    }

    vector<int> result;
    while (!minHeap.empty()) {
        auto [val, arrIdx, elemIdx] = minHeap.top();
        minHeap.pop();
        result.push_back(val);

        if (elemIdx + 1 < arrays[arrIdx].size()) {
            minHeap.push({arrays[arrIdx][elemIdx + 1], arrIdx, elemIdx + 1});
        }
    }

    return result;
}

// === FIND MEDIAN FROM DATA STREAM ===
class MedianFinder {
    priority_queue<int> maxHeap;  // smaller half
    priority_queue<int, vector<int>, greater<int>> minHeap;  // larger half

public:
    void addNum(int num) {
        maxHeap.push(num);
        minHeap.push(maxHeap.top());
        maxHeap.pop();

        if (minHeap.size() > maxHeap.size()) {
            maxHeap.push(minHeap.top());
            minHeap.pop();
        }
    }

    double findMedian() {
        if (maxHeap.size() > minHeap.size()) {
            return maxHeap.top();
        }
        return (maxHeap.top() + minHeap.top()) / 2.0;
    }
};

// === TASK SCHEDULER ===
int leastInterval(vector<char>& tasks, int n) {
    vector<int> count(26, 0);
    for (char task : tasks) count[task - 'A']++;

    priority_queue<int> maxHeap;
    for (int c : count) {
        if (c > 0) maxHeap.push(c);
    }

    int time = 0;
    queue<pair<int, int>> cooldown;  // (count, available_time)

    while (!maxHeap.empty() || !cooldown.empty()) {
        time++;

        if (!maxHeap.empty()) {
            int cnt = maxHeap.top() - 1;
            maxHeap.pop();
            if (cnt > 0) {
                cooldown.push({cnt, time + n});
            }
        }

        if (!cooldown.empty() && cooldown.front().second == time) {
            maxHeap.push(cooldown.front().first);
            cooldown.pop();
        }
    }

    return time;
}

// === K CLOSEST POINTS ===
vector<vector<int>> kClosest(vector<vector<int>>& points, int k) {
    auto cmp = [](vector<int>& a, vector<int>& b) {
        return a[0]*a[0] + a[1]*a[1] < b[0]*b[0] + b[1]*b[1];
    };
    priority_queue<vector<int>, vector<vector<int>>, decltype(cmp)> maxHeap(cmp);

    for (auto& point : points) {
        maxHeap.push(point);
        if (maxHeap.size() > k) {
            maxHeap.pop();
        }
    }

    vector<vector<int>> result;
    while (!maxHeap.empty()) {
        result.push_back(maxHeap.top());
        maxHeap.pop();
    }

    return result;
}

// === REORGANIZE STRING ===
string reorganizeString(string& s) {
    vector<int> count(26, 0);
    for (char c : s) count[c - 'a']++;

    priority_queue<pair<int, char>> maxHeap;
    for (int i = 0; i < 26; i++) {
        if (count[i] > 0) {
            maxHeap.push({count[i], 'a' + i});
        }
    }

    string result;
    pair<int, char> prev = {0, '#'};

    while (!maxHeap.empty()) {
        auto [cnt, ch] = maxHeap.top();
        maxHeap.pop();
        result += ch;

        if (prev.first > 0) {
            maxHeap.push(prev);
        }

        prev = {cnt - 1, ch};
    }

    return result.size() == s.size() ? result : "";
}

// === SMALLEST RANGE COVERING K LISTS ===
vector<int> smallestRange(vector<vector<int>>& nums) {
    // (value, list_idx, element_idx)
    priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, greater<>> minHeap;

    int maxVal = INT_MIN;
    for (int i = 0; i < nums.size(); i++) {
        minHeap.push({nums[i][0], i, 0});
        maxVal = max(maxVal, nums[i][0]);
    }

    vector<int> result = {INT_MIN / 2, INT_MAX / 2};

    while (minHeap.size() == nums.size()) {
        auto [minVal, listIdx, elemIdx] = minHeap.top();
        minHeap.pop();

        if (maxVal - minVal < result[1] - result[0]) {
            result = {minVal, maxVal};
        }

        if (elemIdx + 1 < nums[listIdx].size()) {
            int nextVal = nums[listIdx][elemIdx + 1];
            minHeap.push({nextVal, listIdx, elemIdx + 1});
            maxVal = max(maxVal, nextVal);
        }
    }

    return result;
}

// === IPO (maximize capital) ===
int findMaximizedCapital(int k, int w, vector<int>& profits, vector<int>& capital) {
    int n = profits.size();
    vector<pair<int, int>> projects;
    for (int i = 0; i < n; i++) {
        projects.push_back({capital[i], profits[i]});
    }
    sort(projects.begin(), projects.end());

    priority_queue<int> maxHeap;
    int i = 0;

    for (int j = 0; j < k; j++) {
        while (i < n && projects[i].first <= w) {
            maxHeap.push(projects[i].second);
            i++;
        }

        if (maxHeap.empty()) break;

        w += maxHeap.top();
        maxHeap.pop();
    }

    return w;
}
