/*
 * FAST & SLOW POINTERS (Floyd's Cycle Detection)
 * Use: Cycle detection, finding middle, linked list problems
 * Time: O(n), Space: O(1)
 */

// === LINKED LIST NODE ===
typedef struct ListNode {
    int val;
    struct ListNode *next;
} ListNode;

// === DETECT CYCLE ===
int has_cycle(ListNode *head) {
    ListNode *slow = head, *fast = head;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;

        if (slow == fast) {
            return 1;  // cycle detected
        }
    }
    return 0;  // no cycle
}

// === FIND CYCLE START ===
ListNode* find_cycle_start(ListNode *head) {
    ListNode *slow = head, *fast = head;

    // detect cycle
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;

        if (slow == fast) {
            break;
        }
    }

    if (!fast || !fast->next) {
        return NULL;  // no cycle
    }

    // find start: reset slow to head
    slow = head;
    while (slow != fast) {
        slow = slow->next;
        fast = fast->next;
    }
    return slow;  // cycle start
}

// === FIND MIDDLE NODE ===
ListNode* find_middle(ListNode *head) {
    ListNode *slow = head, *fast = head;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;  // middle (or second middle if even length)
}

// === FIND MIDDLE (first middle if even) ===
ListNode* find_first_middle(ListNode *head) {
    ListNode *slow = head, *fast = head;

    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

// === ARRAY CYCLE (e.g., Find Duplicate Number) ===
// Array where each element points to next index
int find_duplicate(int *nums, int n) {
    int slow = nums[0], fast = nums[0];

    // phase 1: find intersection
    do {
        slow = nums[slow];
        fast = nums[nums[fast]];
    } while (slow != fast);

    // phase 2: find entrance
    slow = nums[0];
    while (slow != fast) {
        slow = nums[slow];
        fast = nums[fast];
    }
    return slow;
}

// === HAPPY NUMBER (digit sum cycle) ===
int sum_of_squares(int n) {
    int sum = 0;
    while (n) {
        int digit = n % 10;
        sum += digit * digit;
        n /= 10;
    }
    return sum;
}

int is_happy(int n) {
    int slow = n, fast = n;

    do {
        slow = sum_of_squares(slow);
        fast = sum_of_squares(sum_of_squares(fast));
    } while (slow != fast);

    return slow == 1;
}
