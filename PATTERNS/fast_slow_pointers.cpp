/*
 * FAST & SLOW POINTERS (FLOYD'S CYCLE DETECTION)
 * Use: Cycle detection, finding middle, linked list problems
 * Time: O(n), Space: O(1)
 */

#include <vector>

using namespace std;

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

// === CYCLE DETECTION ===
bool hasCycle(ListNode* head) {
    if (!head || !head->next) return false;

    ListNode* slow = head;
    ListNode* fast = head;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;

        if (slow == fast) {
            return true;
        }
    }

    return false;
}

ListNode* findCycleStart(ListNode* head) {
    if (!head || !head->next) return nullptr;

    ListNode* slow = head;
    ListNode* fast = head;

    // detect cycle
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;

        if (slow == fast) {
            break;
        }
    }

    if (!fast || !fast->next) return nullptr;  // no cycle

    // find start
    slow = head;
    while (slow != fast) {
        slow = slow->next;
        fast = fast->next;
    }

    return slow;
}

int cycleLength(ListNode* head) {
    if (!head || !head->next) return 0;

    ListNode* slow = head;
    ListNode* fast = head;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;

        if (slow == fast) {
            int length = 1;
            ListNode* curr = slow->next;
            while (curr != slow) {
                length++;
                curr = curr->next;
            }
            return length;
        }
    }

    return 0;
}

// === FIND MIDDLE ===
ListNode* findMiddle(ListNode* head) {
    if (!head) return nullptr;

    ListNode* slow = head;
    ListNode* fast = head;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    return slow;
}

ListNode* findMiddleFirst(ListNode* head) {
    if (!head) return nullptr;

    ListNode* slow = head;
    ListNode* fast = head;

    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    return slow;
}

// === HAPPY NUMBER ===
int getNext(int num) {
    int total = 0;
    while (num) {
        int digit = num % 10;
        total += digit * digit;
        num /= 10;
    }
    return total;
}

bool isHappyNumber(int n) {
    int slow = n;
    int fast = getNext(n);

    while (fast != 1 && slow != fast) {
        slow = getNext(slow);
        fast = getNext(getNext(fast));
    }

    return fast == 1;
}

// === FIND DUPLICATE (array as linked list) ===
int findDuplicate(vector<int>& nums) {
    // phase 1: find intersection
    int slow = nums[0];
    int fast = nums[0];

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

// === REVERSE LIST HELPER ===
ListNode* reverseList(ListNode* head) {
    ListNode* prev = nullptr;
    ListNode* curr = head;

    while (curr) {
        ListNode* next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }

    return prev;
}

// === PALINDROME CHECK ===
bool isPalindrome(ListNode* head) {
    if (!head || !head->next) return true;

    // find middle
    ListNode* slow = head;
    ListNode* fast = head;
    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // reverse second half
    ListNode* secondHalf = reverseList(slow->next);

    // compare halves
    ListNode* first = head;
    ListNode* second = secondHalf;
    bool result = true;

    while (second) {
        if (first->val != second->val) {
            result = false;
            break;
        }
        first = first->next;
        second = second->next;
    }

    // restore list (optional)
    slow->next = reverseList(secondHalf);

    return result;
}

// === REORDER LIST ===
void reorderList(ListNode* head) {
    if (!head || !head->next) return;

    // find middle
    ListNode* slow = head;
    ListNode* fast = head;
    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // reverse second half
    ListNode* second = reverseList(slow->next);
    slow->next = nullptr;
    ListNode* first = head;

    // merge alternating
    while (second) {
        ListNode* tmp1 = first->next;
        ListNode* tmp2 = second->next;
        first->next = second;
        second->next = tmp1;
        first = tmp1;
        second = tmp2;
    }
}

// === NTH NODE FROM END ===
ListNode* removeNthFromEnd(ListNode* head, int n) {
    ListNode dummy(0);
    dummy.next = head;
    ListNode* slow = &dummy;
    ListNode* fast = &dummy;

    // advance fast by n+1
    for (int i = 0; i <= n; i++) {
        fast = fast->next;
    }

    // advance both until fast reaches end
    while (fast) {
        slow = slow->next;
        fast = fast->next;
    }

    // remove node
    ListNode* toDelete = slow->next;
    slow->next = slow->next->next;
    delete toDelete;

    return dummy.next;
}

// === CIRCULAR ARRAY LOOP ===
bool circularArrayLoop(vector<int>& nums) {
    int n = nums.size();

    auto nextIdx = [&](int i) {
        return ((i + nums[i]) % n + n) % n;
    };

    for (int i = 0; i < n; i++) {
        if (nums[i] == 0) continue;

        int slow = i, fast = i;
        // ensure same direction
        while (nums[slow] * nums[nextIdx(slow)] > 0 &&
               nums[fast] * nums[nextIdx(fast)] > 0 &&
               nums[nextIdx(fast)] * nums[nextIdx(nextIdx(fast))] > 0) {

            slow = nextIdx(slow);
            fast = nextIdx(nextIdx(fast));

            if (slow == fast) {
                // check cycle length > 1
                if (slow == nextIdx(slow)) break;
                return true;
            }
        }

        // mark visited nodes
        int j = i;
        int sign = nums[i];
        while (nums[j] * sign > 0) {
            int next = nextIdx(j);
            nums[j] = 0;
            j = next;
        }
    }

    return false;
}
