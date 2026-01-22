/*
 * LINKED LIST PATTERN
 * Use: In-place reversal, merge, reorder, cycle detection
 * Time: O(n), Space: O(1) for most operations
 */

#include <vector>

using namespace std;

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode* n) : val(x), next(n) {}
};

// === REVERSE LINKED LIST ===
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

ListNode* reverseListRecursive(ListNode* head) {
    if (!head || !head->next) return head;

    ListNode* newHead = reverseListRecursive(head->next);
    head->next->next = head;
    head->next = nullptr;

    return newHead;
}

// === REVERSE SUBLIST ===
ListNode* reverseBetween(ListNode* head, int left, int right) {
    if (!head || left == right) return head;

    ListNode dummy(0, head);
    ListNode* prev = &dummy;

    for (int i = 0; i < left - 1; i++) {
        prev = prev->next;
    }

    ListNode* curr = prev->next;
    for (int i = 0; i < right - left; i++) {
        ListNode* nextNode = curr->next;
        curr->next = nextNode->next;
        nextNode->next = prev->next;
        prev->next = nextNode;
    }

    return dummy.next;
}

// === REVERSE IN K-GROUPS ===
ListNode* reverseKGroup(ListNode* head, int k) {
    // count nodes
    int count = 0;
    ListNode* curr = head;
    while (curr) {
        count++;
        curr = curr->next;
    }

    ListNode dummy(0, head);
    ListNode* prevGroup = &dummy;

    while (count >= k) {
        curr = prevGroup->next;
        ListNode* nextNode = curr->next;

        for (int i = 0; i < k - 1; i++) {
            curr->next = nextNode->next;
            nextNode->next = prevGroup->next;
            prevGroup->next = nextNode;
            nextNode = curr->next;
        }

        prevGroup = curr;
        count -= k;
    }

    return dummy.next;
}

// === MERGE TWO SORTED LISTS ===
ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
    ListNode dummy(0);
    ListNode* curr = &dummy;

    while (l1 && l2) {
        if (l1->val <= l2->val) {
            curr->next = l1;
            l1 = l1->next;
        } else {
            curr->next = l2;
            l2 = l2->next;
        }
        curr = curr->next;
    }

    curr->next = l1 ? l1 : l2;
    return dummy.next;
}

// === MERGE K SORTED LISTS ===
ListNode* mergeKLists(vector<ListNode*>& lists) {
    if (lists.empty()) return nullptr;

    while (lists.size() > 1) {
        vector<ListNode*> merged;
        for (int i = 0; i < lists.size(); i += 2) {
            ListNode* l1 = lists[i];
            ListNode* l2 = (i + 1 < lists.size()) ? lists[i + 1] : nullptr;
            merged.push_back(mergeTwoLists(l1, l2));
        }
        lists = merged;
    }

    return lists[0];
}

// === FIND MIDDLE ===
ListNode* findMiddle(ListNode* head) {
    ListNode* slow = head;
    ListNode* fast = head;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    return slow;
}

// === DETECT CYCLE ===
bool hasCycle(ListNode* head) {
    ListNode* slow = head;
    ListNode* fast = head;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) return true;
    }

    return false;
}

ListNode* detectCycle(ListNode* head) {
    ListNode* slow = head;
    ListNode* fast = head;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) {
            slow = head;
            while (slow != fast) {
                slow = slow->next;
                fast = fast->next;
            }
            return slow;
        }
    }

    return nullptr;
}

// === REMOVE NTH FROM END ===
ListNode* removeNthFromEnd(ListNode* head, int n) {
    ListNode dummy(0, head);
    ListNode* slow = &dummy;
    ListNode* fast = &dummy;

    for (int i = 0; i <= n; i++) {
        fast = fast->next;
    }

    while (fast) {
        slow = slow->next;
        fast = fast->next;
    }

    slow->next = slow->next->next;
    return dummy.next;
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

// === PALINDROME CHECK ===
bool isPalindrome(ListNode* head) {
    if (!head || !head->next) return true;

    ListNode* slow = head;
    ListNode* fast = head;
    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    ListNode* second = reverseList(slow->next);
    ListNode* first = head;

    while (second) {
        if (first->val != second->val) return false;
        first = first->next;
        second = second->next;
    }

    return true;
}

// === ADD TWO NUMBERS ===
ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
    ListNode dummy(0);
    ListNode* curr = &dummy;
    int carry = 0;

    while (l1 || l2 || carry) {
        int val = carry;
        if (l1) {
            val += l1->val;
            l1 = l1->next;
        }
        if (l2) {
            val += l2->val;
            l2 = l2->next;
        }

        carry = val / 10;
        curr->next = new ListNode(val % 10);
        curr = curr->next;
    }

    return dummy.next;
}

// === INTERSECTION OF TWO LISTS ===
ListNode* getIntersection(ListNode* headA, ListNode* headB) {
    if (!headA || !headB) return nullptr;

    ListNode* a = headA;
    ListNode* b = headB;

    while (a != b) {
        a = a ? a->next : headB;
        b = b ? b->next : headA;
    }

    return a;
}

// === ROTATE LIST ===
ListNode* rotateRight(ListNode* head, int k) {
    if (!head || !head->next || k == 0) return head;

    // get length and tail
    int length = 1;
    ListNode* tail = head;
    while (tail->next) {
        tail = tail->next;
        length++;
    }

    k = k % length;
    if (k == 0) return head;

    // find new tail
    ListNode* newTail = head;
    for (int i = 0; i < length - k - 1; i++) {
        newTail = newTail->next;
    }

    ListNode* newHead = newTail->next;
    newTail->next = nullptr;
    tail->next = head;

    return newHead;
}

// === SWAP NODES IN PAIRS ===
ListNode* swapPairs(ListNode* head) {
    ListNode dummy(0, head);
    ListNode* prev = &dummy;

    while (prev->next && prev->next->next) {
        ListNode* first = prev->next;
        ListNode* second = first->next;

        first->next = second->next;
        second->next = first;
        prev->next = second;

        prev = first;
    }

    return dummy.next;
}
