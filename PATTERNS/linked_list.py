"""
LINKED LIST PATTERN
Use: In-place reversal, merge, reorder, cycle detection
Time: O(n), Space: O(1) for most operations
"""

from typing import Optional, List


class ListNode:
    def __init__(self, val=0, next=None):
        self.val = val
        self.next = next


# === REVERSE LINKED LIST ===
def reverse_list(head: Optional[ListNode]) -> Optional[ListNode]:
    """Reverse entire linked list iteratively"""
    prev = None
    curr = head

    while curr:
        next_node = curr.next
        curr.next = prev
        prev = curr
        curr = next_node

    return prev


def reverse_list_recursive(head: Optional[ListNode]) -> Optional[ListNode]:
    """Reverse linked list recursively"""
    if not head or not head.next:
        return head

    new_head = reverse_list_recursive(head.next)
    head.next.next = head
    head.next = None

    return new_head


# === REVERSE SUBLIST ===
def reverse_between(head: Optional[ListNode], left: int, right: int) -> Optional[ListNode]:
    """Reverse nodes from position left to right (1-indexed)"""
    if not head or left == right:
        return head

    dummy = ListNode(0, head)
    prev = dummy

    # move to node before left
    for _ in range(left - 1):
        prev = prev.next

    # reverse sublist
    curr = prev.next
    for _ in range(right - left):
        next_node = curr.next
        curr.next = next_node.next
        next_node.next = prev.next
        prev.next = next_node

    return dummy.next


# === REVERSE IN K-GROUPS ===
def reverse_k_group(head: Optional[ListNode], k: int) -> Optional[ListNode]:
    """Reverse nodes in groups of k"""
    # count nodes
    count = 0
    curr = head
    while curr:
        count += 1
        curr = curr.next

    dummy = ListNode(0, head)
    prev_group = dummy

    while count >= k:
        curr = prev_group.next
        next_node = curr.next

        for _ in range(k - 1):
            curr.next = next_node.next
            next_node.next = prev_group.next
            prev_group.next = next_node
            next_node = curr.next

        prev_group = curr
        count -= k

    return dummy.next


# === MERGE TWO SORTED LISTS ===
def merge_two_lists(l1: Optional[ListNode], l2: Optional[ListNode]) -> Optional[ListNode]:
    """Merge two sorted linked lists"""
    dummy = ListNode()
    curr = dummy

    while l1 and l2:
        if l1.val <= l2.val:
            curr.next = l1
            l1 = l1.next
        else:
            curr.next = l2
            l2 = l2.next
        curr = curr.next

    curr.next = l1 or l2
    return dummy.next


# === MERGE K SORTED LISTS ===
def merge_k_lists(lists: List[Optional[ListNode]]) -> Optional[ListNode]:
    """Merge k sorted lists using divide and conquer"""
    if not lists:
        return None

    while len(lists) > 1:
        merged = []
        for i in range(0, len(lists), 2):
            l1 = lists[i]
            l2 = lists[i + 1] if i + 1 < len(lists) else None
            merged.append(merge_two_lists(l1, l2))
        lists = merged

    return lists[0]


# === FIND MIDDLE ===
def find_middle(head: Optional[ListNode]) -> Optional[ListNode]:
    """Find middle node (second middle if even)"""
    slow = fast = head

    while fast and fast.next:
        slow = slow.next
        fast = fast.next.next

    return slow


# === DETECT CYCLE ===
def has_cycle(head: Optional[ListNode]) -> bool:
    """Detect if list has cycle"""
    slow = fast = head

    while fast and fast.next:
        slow = slow.next
        fast = fast.next.next
        if slow == fast:
            return True

    return False


def detect_cycle(head: Optional[ListNode]) -> Optional[ListNode]:
    """Return node where cycle begins, None if no cycle"""
    slow = fast = head

    while fast and fast.next:
        slow = slow.next
        fast = fast.next.next
        if slow == fast:
            slow = head
            while slow != fast:
                slow = slow.next
                fast = fast.next
            return slow

    return None


# === REMOVE NTH FROM END ===
def remove_nth_from_end(head: Optional[ListNode], n: int) -> Optional[ListNode]:
    """Remove nth node from end"""
    dummy = ListNode(0, head)
    slow = fast = dummy

    for _ in range(n + 1):
        fast = fast.next

    while fast:
        slow = slow.next
        fast = fast.next

    slow.next = slow.next.next
    return dummy.next


# === REORDER LIST ===
def reorder_list(head: Optional[ListNode]) -> None:
    """Reorder: L0 -> Ln -> L1 -> Ln-1 -> ..."""
    if not head or not head.next:
        return

    # find middle
    slow = fast = head
    while fast.next and fast.next.next:
        slow = slow.next
        fast = fast.next.next

    # reverse second half
    second = reverse_list(slow.next)
    slow.next = None
    first = head

    # merge alternating
    while second:
        tmp1, tmp2 = first.next, second.next
        first.next = second
        second.next = tmp1
        first = tmp1
        second = tmp2


# === PALINDROME CHECK ===
def is_palindrome(head: Optional[ListNode]) -> bool:
    """Check if list is palindrome"""
    if not head or not head.next:
        return True

    # find middle
    slow = fast = head
    while fast.next and fast.next.next:
        slow = slow.next
        fast = fast.next.next

    # reverse and compare
    second = reverse_list(slow.next)
    first = head

    while second:
        if first.val != second.val:
            return False
        first = first.next
        second = second.next

    return True


# === ADD TWO NUMBERS ===
def add_two_numbers(l1: Optional[ListNode], l2: Optional[ListNode]) -> Optional[ListNode]:
    """Add two numbers represented as linked lists (reverse order)"""
    dummy = ListNode()
    curr = dummy
    carry = 0

    while l1 or l2 or carry:
        val = carry
        if l1:
            val += l1.val
            l1 = l1.next
        if l2:
            val += l2.val
            l2 = l2.next

        carry = val // 10
        curr.next = ListNode(val % 10)
        curr = curr.next

    return dummy.next


# === INTERSECTION OF TWO LISTS ===
def get_intersection(headA: Optional[ListNode], headB: Optional[ListNode]) -> Optional[ListNode]:
    """Find intersection node of two lists"""
    if not headA or not headB:
        return None

    a, b = headA, headB

    while a != b:
        a = a.next if a else headB
        b = b.next if b else headA

    return a


# === COPY LIST WITH RANDOM POINTER ===
class RandomNode:
    def __init__(self, val=0, next=None, random=None):
        self.val = val
        self.next = next
        self.random = random


def copy_random_list(head: Optional[RandomNode]) -> Optional[RandomNode]:
    """Deep copy list with random pointers (O(1) space)"""
    if not head:
        return None

    # interleave copies
    curr = head
    while curr:
        copy = RandomNode(curr.val, curr.next)
        curr.next = copy
        curr = copy.next

    # set random pointers
    curr = head
    while curr:
        if curr.random:
            curr.next.random = curr.random.next
        curr = curr.next.next

    # separate lists
    dummy = ListNode()
    copy_curr = dummy
    curr = head

    while curr:
        copy_curr.next = curr.next
        curr.next = curr.next.next
        copy_curr = copy_curr.next
        curr = curr.next

    return dummy.next


# === ROTATE LIST ===
def rotate_right(head: Optional[ListNode], k: int) -> Optional[ListNode]:
    """Rotate list to the right by k places"""
    if not head or not head.next or k == 0:
        return head

    # get length and tail
    length = 1
    tail = head
    while tail.next:
        tail = tail.next
        length += 1

    k = k % length
    if k == 0:
        return head

    # find new tail
    new_tail = head
    for _ in range(length - k - 1):
        new_tail = new_tail.next

    new_head = new_tail.next
    new_tail.next = None
    tail.next = head

    return new_head
