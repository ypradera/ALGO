/*
 * MERGE INTERVALS PATTERN
 * Use: Scheduling, range problems, overlap detection
 * Time: O(n log n) for sorting, Space: O(n)
 */

#include <vector>
#include <algorithm>
#include <queue>

using namespace std;

// === MERGE OVERLAPPING INTERVALS ===
vector<vector<int>> mergeIntervals(vector<vector<int>>& intervals) {
    if (intervals.empty()) return {};

    sort(intervals.begin(), intervals.end());
    vector<vector<int>> merged;
    merged.push_back(intervals[0]);

    for (int i = 1; i < intervals.size(); i++) {
        if (intervals[i][0] <= merged.back()[1]) {
            merged.back()[1] = max(merged.back()[1], intervals[i][1]);
        } else {
            merged.push_back(intervals[i]);
        }
    }

    return merged;
}

// === INSERT INTERVAL ===
vector<vector<int>> insertInterval(vector<vector<int>>& intervals, vector<int>& newInterval) {
    vector<vector<int>> result;
    int i = 0;
    int n = intervals.size();

    // add all intervals before newInterval
    while (i < n && intervals[i][1] < newInterval[0]) {
        result.push_back(intervals[i]);
        i++;
    }

    // merge overlapping intervals
    while (i < n && intervals[i][0] <= newInterval[1]) {
        newInterval[0] = min(newInterval[0], intervals[i][0]);
        newInterval[1] = max(newInterval[1], intervals[i][1]);
        i++;
    }
    result.push_back(newInterval);

    // add remaining intervals
    while (i < n) {
        result.push_back(intervals[i]);
        i++;
    }

    return result;
}

// === INTERVAL INTERSECTION ===
vector<vector<int>> intervalIntersection(vector<vector<int>>& first, vector<vector<int>>& second) {
    vector<vector<int>> result;
    int i = 0, j = 0;

    while (i < first.size() && j < second.size()) {
        int start = max(first[i][0], second[j][0]);
        int end = min(first[i][1], second[j][1]);

        if (start <= end) {
            result.push_back({start, end});
        }

        if (first[i][1] < second[j][1]) {
            i++;
        } else {
            j++;
        }
    }

    return result;
}

// === MEETING ROOMS I (can attend all?) ===
bool canAttendMeetings(vector<vector<int>>& intervals) {
    sort(intervals.begin(), intervals.end());

    for (int i = 1; i < intervals.size(); i++) {
        if (intervals[i][0] < intervals[i-1][1]) {
            return false;
        }
    }

    return true;
}

// === MEETING ROOMS II (min rooms needed) ===
int minMeetingRooms(vector<vector<int>>& intervals) {
    if (intervals.empty()) return 0;

    sort(intervals.begin(), intervals.end());
    priority_queue<int, vector<int>, greater<int>> endTimes;

    for (auto& interval : intervals) {
        if (!endTimes.empty() && endTimes.top() <= interval[0]) {
            endTimes.pop();
        }
        endTimes.push(interval[1]);
    }

    return endTimes.size();
}

int minMeetingRoomsSweep(vector<vector<int>>& intervals) {
    vector<pair<int, int>> events;

    for (auto& interval : intervals) {
        events.push_back({interval[0], 1});   // start
        events.push_back({interval[1], -1});  // end
    }

    sort(events.begin(), events.end());

    int rooms = 0, maxRooms = 0;
    for (auto& [time, delta] : events) {
        rooms += delta;
        maxRooms = max(maxRooms, rooms);
    }

    return maxRooms;
}

// === NON-OVERLAPPING INTERVALS (min removals) ===
int eraseOverlapIntervals(vector<vector<int>>& intervals) {
    if (intervals.empty()) return 0;

    sort(intervals.begin(), intervals.end(), [](auto& a, auto& b) {
        return a[1] < b[1];
    });

    int count = 0;
    int prevEnd = INT_MIN;

    for (auto& interval : intervals) {
        if (interval[0] >= prevEnd) {
            prevEnd = interval[1];
        } else {
            count++;
        }
    }

    return count;
}

// === MAX OVERLAP COUNT ===
int maxOverlap(vector<vector<int>>& intervals) {
    vector<pair<int, int>> events;

    for (auto& interval : intervals) {
        events.push_back({interval[0], 1});
        events.push_back({interval[1], -1});
    }

    sort(events.begin(), events.end());

    int current = 0, maxCount = 0;
    for (auto& [time, delta] : events) {
        current += delta;
        maxCount = max(maxCount, current);
    }

    return maxCount;
}

// === REMOVE COVERED INTERVALS ===
int removeCoveredIntervals(vector<vector<int>>& intervals) {
    sort(intervals.begin(), intervals.end(), [](auto& a, auto& b) {
        return a[0] == b[0] ? a[1] > b[1] : a[0] < b[0];
    });

    int count = 0;
    int prevEnd = 0;

    for (auto& interval : intervals) {
        if (interval[1] > prevEnd) {
            count++;
            prevEnd = interval[1];
        }
    }

    return count;
}

// === MINIMUM ARROWS TO BURST BALLOONS ===
int findMinArrowShots(vector<vector<int>>& points) {
    if (points.empty()) return 0;

    sort(points.begin(), points.end(), [](auto& a, auto& b) {
        return a[1] < b[1];
    });

    int arrows = 1;
    int end = points[0][1];

    for (int i = 1; i < points.size(); i++) {
        if (points[i][0] > end) {
            arrows++;
            end = points[i][1];
        }
    }

    return arrows;
}

// === CAR POOLING ===
bool carPooling(vector<vector<int>>& trips, int capacity) {
    vector<pair<int, int>> events;

    for (auto& trip : trips) {
        events.push_back({trip[1], trip[0]});   // pickup
        events.push_back({trip[2], -trip[0]});  // dropoff
    }

    sort(events.begin(), events.end());

    int current = 0;
    for (auto& [time, delta] : events) {
        current += delta;
        if (current > capacity) return false;
    }

    return true;
}

// === SUMMARY RANGES ===
vector<string> summaryRanges(vector<int>& nums) {
    if (nums.empty()) return {};

    vector<string> result;
    int start = nums[0];

    for (int i = 1; i <= nums.size(); i++) {
        if (i == nums.size() || nums[i] != nums[i-1] + 1) {
            if (start == nums[i-1]) {
                result.push_back(to_string(start));
            } else {
                result.push_back(to_string(start) + "->" + to_string(nums[i-1]));
            }
            if (i < nums.size()) start = nums[i];
        }
    }

    return result;
}

// === PARTITION LABELS ===
vector<int> partitionLabels(string& s) {
    vector<int> last(26, 0);
    for (int i = 0; i < s.size(); i++) {
        last[s[i] - 'a'] = i;
    }

    vector<int> result;
    int start = 0, end = 0;

    for (int i = 0; i < s.size(); i++) {
        end = max(end, last[s[i] - 'a']);
        if (i == end) {
            result.push_back(end - start + 1);
            start = i + 1;
        }
    }

    return result;
}

// === MY CALENDAR ===
class MyCalendar {
    vector<pair<int, int>> bookings;

public:
    bool book(int start, int end) {
        for (auto& [s, e] : bookings) {
            if (start < e && end > s) {
                return false;
            }
        }
        bookings.push_back({start, end});
        return true;
    }
};
