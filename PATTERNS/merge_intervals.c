/*
 * MERGE INTERVALS PATTERN
 * Use: Overlapping intervals, scheduling, calendar conflicts
 * Time: O(n log n), Space: O(n)
 */

#include <stdlib.h>
#include <string.h>

#define MAX_INTERVALS 1000
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct {
    int start;
    int end;
} Interval;

// === SORT INTERVALS BY START ===
int compare_by_start(const void *a, const void *b) {
    return ((Interval *)a)->start - ((Interval *)b)->start;
}

int compare_by_end(const void *a, const void *b) {
    return ((Interval *)a)->end - ((Interval *)b)->end;
}

// === MERGE OVERLAPPING INTERVALS ===
int merge_intervals(Interval *intervals, int n, Interval *result) {
    if (n == 0) return 0;

    qsort(intervals, n, sizeof(Interval), compare_by_start);

    int count = 0;
    result[count++] = intervals[0];

    for (int i = 1; i < n; i++) {
        if (intervals[i].start <= result[count - 1].end) {
            // overlapping - merge
            result[count - 1].end = MAX(result[count - 1].end, intervals[i].end);
        } else {
            // non-overlapping - add new
            result[count++] = intervals[i];
        }
    }

    return count;
}

// === INSERT INTERVAL ===
int insert_interval(Interval *intervals, int n, Interval new_interval,
                    Interval *result) {
    int count = 0;
    int i = 0;

    // add all intervals before new_interval
    while (i < n && intervals[i].end < new_interval.start) {
        result[count++] = intervals[i++];
    }

    // merge overlapping intervals with new_interval
    while (i < n && intervals[i].start <= new_interval.end) {
        new_interval.start = MIN(new_interval.start, intervals[i].start);
        new_interval.end = MAX(new_interval.end, intervals[i].end);
        i++;
    }
    result[count++] = new_interval;

    // add remaining intervals
    while (i < n) {
        result[count++] = intervals[i++];
    }

    return count;
}

// === CHECK OVERLAPPING ===
int has_overlap(Interval *intervals, int n) {
    qsort(intervals, n, sizeof(Interval), compare_by_start);

    for (int i = 1; i < n; i++) {
        if (intervals[i].start < intervals[i - 1].end) {
            return 1;  // overlap found
        }
    }
    return 0;
}

// === MINIMUM MEETING ROOMS (Maximum Overlapping) ===
int min_meeting_rooms(Interval *intervals, int n) {
    int starts[MAX_INTERVALS], ends[MAX_INTERVALS];

    for (int i = 0; i < n; i++) {
        starts[i] = intervals[i].start;
        ends[i] = intervals[i].end;
    }

    // sort both arrays
    qsort(starts, n, sizeof(int), compare_by_start);
    qsort(ends, n, sizeof(int), compare_by_end);

    int rooms = 0, max_rooms = 0;
    int s = 0, e = 0;

    while (s < n) {
        if (starts[s] < ends[e]) {
            rooms++;
            s++;
        } else {
            rooms--;
            e++;
        }
        if (rooms > max_rooms) max_rooms = rooms;
    }

    return max_rooms;
}

// === INTERVAL INTERSECTION ===
int interval_intersection(Interval *a, int a_len, Interval *b, int b_len,
                          Interval *result) {
    int i = 0, j = 0, count = 0;

    while (i < a_len && j < b_len) {
        int start = MAX(a[i].start, b[j].start);
        int end = MIN(a[i].end, b[j].end);

        if (start <= end) {
            result[count].start = start;
            result[count].end = end;
            count++;
        }

        // move pointer with smaller end
        if (a[i].end < b[j].end) {
            i++;
        } else {
            j++;
        }
    }

    return count;
}

// === REMOVE COVERED INTERVALS ===
int remove_covered_intervals(Interval *intervals, int n) {
    // sort by start ascending, then by end descending
    qsort(intervals, n, sizeof(Interval), compare_by_start);

    int count = 0;
    int max_end = 0;

    for (int i = 0; i < n; i++) {
        if (intervals[i].end > max_end) {
            count++;
            max_end = intervals[i].end;
        }
        // else: interval is covered by previous one
    }

    return count;
}

// === NON-OVERLAPPING INTERVALS (Min removals) ===
int erase_overlap_intervals(Interval *intervals, int n) {
    if (n == 0) return 0;

    qsort(intervals, n, sizeof(Interval), compare_by_end);

    int count = 0;
    int prev_end = intervals[0].end;

    for (int i = 1; i < n; i++) {
        if (intervals[i].start < prev_end) {
            count++;  // remove this interval
        } else {
            prev_end = intervals[i].end;
        }
    }

    return count;
}

// === EMPLOYEE FREE TIME ===
// Given schedules for each employee, find common free time
typedef struct {
    Interval *schedule;
    int size;
} EmployeeSchedule;

int employee_free_time(EmployeeSchedule *schedules, int num_employees,
                       Interval *result) {
    // Flatten and merge all intervals
    Interval all[MAX_INTERVALS];
    int total = 0;

    for (int e = 0; e < num_employees; e++) {
        for (int i = 0; i < schedules[e].size; i++) {
            all[total++] = schedules[e].schedule[i];
        }
    }

    // Merge intervals
    Interval merged[MAX_INTERVALS];
    int merged_count = merge_intervals(all, total, merged);

    // Find gaps
    int count = 0;
    for (int i = 1; i < merged_count; i++) {
        result[count].start = merged[i - 1].end;
        result[count].end = merged[i].start;
        count++;
    }

    return count;
}
