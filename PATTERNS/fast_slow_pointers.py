"""
FAST & SLOW POINTERS (FLOYD'S CYCLE DETECTION)
Use: Cycle detection, finding middle, linked list problems
Time: O(n), Space: O(1)
"""

from typing import Optional


class ListNode:
    def __init__(self, val=0, next=None):
        self.val = val
        self.next = next


# === CYCLE DETECTION ===
def has_cycle(head: Optional[ListNode]) -> bool:
    """Detect if linked list has a cycle"""
    if not head or not head.next:
        return False

    slow = head
    fast = head

    while fast and fast.next:
        slow = slow.next
        fast = fast.next.next

        if slow == fast:
            return True

    return False


def find_cycle_start(head: Optional[ListNode]) -> Optional[ListNode]:
    """Find the node where cycle begins"""
    if not head or not head.next:
        return None

    slow = fast = head

    # detect cycle
    while fast and fast.next:
        slow = slow.next
        fast = fast.next.next

        if slow == fast:
            break
    else:
        return None  # no cycle

    # find start: move slow to head, advance both by 1
    slow = head
    while slow != fast:
        slow = slow.next
        fast = fast.next

    return slow


def cycle_length(head: Optional[ListNode]) -> int:
    """Find length of cycle (0 if no cycle)"""
    if not head or not head.next:
        return 0

    slow = fast = head

    while fast and fast.next:
        slow = slow.next
        fast = fast.next.next

        if slow == fast:
            # count cycle length
            length = 1
            current = slow.next
            while current != slow:
                length += 1
                current = current.next
            return length

    return 0


# === FIND MIDDLE ===
def find_middle(head: Optional[ListNode]) -> Optional[ListNode]:
    """Find middle node (second middle if even length)"""
    if not head:
        return None

    slow = fast = head

    while fast and fast.next:
        slow = slow.next
        fast = fast.next.next

    return slow


def find_middle_first(head: Optional[ListNode]) -> Optional[ListNode]:
    """Find middle node (first middle if even length)"""
    if not head:
        return None

    slow = fast = head

    while fast.next and fast.next.next:
        slow = slow.next
        fast = fast.next.next

    return slow


# === HAPPY NUMBER ===
def is_happy_number(n: int) -> bool:
    """Check if number is happy (sum of squares reaches 1)"""
    def get_next(num: int) -> int:
        total = 0
        while num:
            digit = num % 10
            total += digit * digit
            num //= 10
        return total

    slow = n
    fast = get_next(n)

    while fast != 1 and slow != fast:
        slow = get_next(slow)
        fast = get_next(get_next(fast))

    return fast == 1


# === FIND DUPLICATE (array as linked list) ===
def find_duplicate(nums: list) -> int:
    """
    Find duplicate in array where nums[i] in [1, n].
    Treat array as linked list: i -> nums[i]
    """
    # phase 1: find intersection
    slow = fast = nums[0]

    while True:
        slow = nums[slow]
        fast = nums[nums[fast]]
        if slow == fast:
            break

    # phase 2: find entrance
    slow = nums[0]
    while slow != fast:
        slow = nums[slow]
        fast = nums[fast]

    return slow


# === PALINDROME CHECK ===
def is_palindrome(head: Optional[ListNode]) -> bool:
    """Check if linked list is palindrome using O(1) space"""
    if not head or not head.next:
        return True

    # find middle
    slow = fast = head
    while fast.next and fast.next.next:
        slow = slow.next
        fast = fast.next.next

    # reverse second half
    second_half = reverse_list(slow.next)

    # compare halves
    first = head
    second = second_half
    result = True

    while second:
        if first.val != second.val:
            result = False
            break
        first = first.next
        second = second.next

    # restore list (optional)
    slow.next = reverse_list(second_half)

    return result


def reverse_list(head: Optional[ListNode]) -> Optional[ListNode]:
    """Helper: reverse linked list"""
    prev = None
    curr = head

    while curr:
        next_node = curr.next
        curr.next = prev
        prev = curr
        curr = next_node

    return prev


# === REORDER LIST ===
def reorder_list(head: Optional[ListNode]) -> None:
    """Reorder: L0 -> Ln -> L1 -> Ln-1 -> L2 -> Ln-2 -> ..."""
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


# === NTH NODE FROM END ===
def remove_nth_from_end(head: Optional[ListNode], n: int) -> Optional[ListNode]:
    """Remove nth node from end using two pointers"""
    dummy = ListNode(0, head)
    slow = fast = dummy

    # advance fast by n+1
    for _ in range(n + 1):
        fast = fast.next

    # advance both until fast reaches end
    while fast:
        slow = slow.next
        fast = fast.next

    # remove node
    slow.next = slow.next.next

    return dummy.next


# === CIRCULAR ARRAY LOOP ===
def circular_array_loop(nums: list) -> bool:
    """Detect cycle in circular array (all same direction)"""
    n = len(nums)

    def next_idx(i: int) -> int:
        return (i + nums[i]) % n

    for i in range(n):
        if nums[i] == 0:
            continue

        slow = fast = i
        # ensure same direction
        while nums[slow] * nums[next_idx(slow)] > 0 and \
              nums[fast] * nums[next_idx(fast)] > 0 and \
              nums[next_idx(fast)] * nums[next_idx(next_idx(fast))] > 0:

            slow = next_idx(slow)
            fast = next_idx(next_idx(fast))

            if slow == fast:
                # check cycle length > 1
                if slow == next_idx(slow):
                    break
                return True

        # mark visited nodes
        j = i
        while nums[j] * nums[next_idx(j)] > 0:
            temp = j
            j = next_idx(j)
            nums[temp] = 0

    return False
