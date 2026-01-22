/*
 * LINKED LIST PATTERNS
 * Use: In-place reversal, merge, reorder
 * Time: O(n), Space: O(1)
 */

typedef struct ListNode {
    int val;
    struct ListNode *next;
} ListNode;

// === REVERSE LINKED LIST ===
ListNode* reverse_list(ListNode *head) {
    ListNode *prev = NULL, *curr = head;

    while (curr) {
        ListNode *next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    return prev;
}

// === REVERSE BETWEEN POSITIONS [left, right] (1-indexed) ===
ListNode* reverse_between(ListNode *head, int left, int right) {
    if (!head || left == right) return head;

    ListNode dummy = {0, head};
    ListNode *prev = &dummy;

    // move to position before left
    for (int i = 1; i < left; i++) {
        prev = prev->next;
    }

    // reverse from left to right
    ListNode *curr = prev->next;
    for (int i = 0; i < right - left; i++) {
        ListNode *next = curr->next;
        curr->next = next->next;
        next->next = prev->next;
        prev->next = next;
    }

    return dummy.next;
}

// === MERGE TWO SORTED LISTS ===
ListNode* merge_two_lists(ListNode *l1, ListNode *l2) {
    ListNode dummy = {0, NULL};
    ListNode *tail = &dummy;

    while (l1 && l2) {
        if (l1->val <= l2->val) {
            tail->next = l1;
            l1 = l1->next;
        } else {
            tail->next = l2;
            l2 = l2->next;
        }
        tail = tail->next;
    }
    tail->next = l1 ? l1 : l2;

    return dummy.next;
}

// === REORDER LIST (L0→Ln→L1→Ln-1→...) ===
void reorder_list(ListNode *head) {
    if (!head || !head->next) return;

    // 1. find middle
    ListNode *slow = head, *fast = head;
    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // 2. reverse second half
    ListNode *second = slow->next;
    slow->next = NULL;
    second = reverse_list(second);

    // 3. merge alternating
    ListNode *first = head;
    while (second) {
        ListNode *tmp1 = first->next;
        ListNode *tmp2 = second->next;
        first->next = second;
        second->next = tmp1;
        first = tmp1;
        second = tmp2;
    }
}

// === DELETE NTH NODE FROM END ===
ListNode* remove_nth_from_end(ListNode *head, int n) {
    ListNode dummy = {0, head};
    ListNode *fast = &dummy, *slow = &dummy;

    // move fast n+1 steps ahead
    for (int i = 0; i <= n; i++) {
        fast = fast->next;
    }

    // move both until fast reaches end
    while (fast) {
        fast = fast->next;
        slow = slow->next;
    }

    // delete slow->next
    slow->next = slow->next->next;

    return dummy.next;
}

// === DETECT AND REMOVE LOOP ===
void remove_loop(ListNode *head) {
    ListNode *slow = head, *fast = head;

    // detect
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) break;
    }

    if (!fast || !fast->next) return;  // no loop

    // find loop start
    slow = head;
    if (slow == fast) {
        // loop at head
        while (fast->next != slow) {
            fast = fast->next;
        }
    } else {
        while (slow->next != fast->next) {
            slow = slow->next;
            fast = fast->next;
        }
    }
    fast->next = NULL;  // remove loop
}
