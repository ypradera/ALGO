"""
STACK & QUEUE PATTERNS
Use: Monotonic stack, next greater element, sliding window max
Time: O(n), Space: O(n)
"""

from typing import List, Optional
from collections import deque


# === MONOTONIC STACK (next greater element) ===
def next_greater_element(arr: List[int]) -> List[int]:
    """For each element, find next greater element to the right"""
    n = len(arr)
    result = [-1] * n
    stack = []  # stores indices

    for i in range(n):
        while stack and arr[stack[-1]] < arr[i]:
            idx = stack.pop()
            result[idx] = arr[i]
        stack.append(i)

    return result


def next_smaller_element(arr: List[int]) -> List[int]:
    """For each element, find next smaller element to the right"""
    n = len(arr)
    result = [-1] * n
    stack = []

    for i in range(n):
        while stack and arr[stack[-1]] > arr[i]:
            idx = stack.pop()
            result[idx] = arr[i]
        stack.append(i)

    return result


def previous_greater_element(arr: List[int]) -> List[int]:
    """For each element, find previous greater element to the left"""
    n = len(arr)
    result = [-1] * n
    stack = []

    for i in range(n):
        while stack and arr[stack[-1]] <= arr[i]:
            stack.pop()
        if stack:
            result[i] = arr[stack[-1]]
        stack.append(i)

    return result


# === DAILY TEMPERATURES ===
def daily_temperatures(temps: List[int]) -> List[int]:
    """Days until warmer temperature"""
    n = len(temps)
    result = [0] * n
    stack = []

    for i in range(n):
        while stack and temps[stack[-1]] < temps[i]:
            idx = stack.pop()
            result[idx] = i - idx
        stack.append(i)

    return result


# === LARGEST RECTANGLE IN HISTOGRAM ===
def largest_rectangle_histogram(heights: List[int]) -> int:
    """Find largest rectangle area in histogram"""
    stack = []  # stores indices
    max_area = 0
    heights = heights + [0]  # sentinel

    for i, h in enumerate(heights):
        while stack and heights[stack[-1]] > h:
            height = heights[stack.pop()]
            width = i if not stack else i - stack[-1] - 1
            max_area = max(max_area, height * width)
        stack.append(i)

    return max_area


# === MAXIMAL RECTANGLE IN MATRIX ===
def maximal_rectangle(matrix: List[List[str]]) -> int:
    """Find largest rectangle of 1s in binary matrix"""
    if not matrix or not matrix[0]:
        return 0

    m, n = len(matrix), len(matrix[0])
    heights = [0] * n
    max_area = 0

    for i in range(m):
        for j in range(n):
            heights[j] = heights[j] + 1 if matrix[i][j] == '1' else 0

        max_area = max(max_area, largest_rectangle_histogram(heights))

    return max_area


# === TRAPPING RAIN WATER (using stack) ===
def trap_water_stack(heights: List[int]) -> int:
    """Calculate trapped rainwater using monotonic stack"""
    stack = []
    water = 0

    for i, h in enumerate(heights):
        while stack and heights[stack[-1]] < h:
            bottom = heights[stack.pop()]
            if stack:
                width = i - stack[-1] - 1
                height = min(h, heights[stack[-1]]) - bottom
                water += width * height
        stack.append(i)

    return water


# === SLIDING WINDOW MAXIMUM (monotonic deque) ===
def sliding_window_max(arr: List[int], k: int) -> List[int]:
    """Find maximum in each window of size k"""
    dq = deque()  # stores indices, decreasing order
    result = []

    for i in range(len(arr)):
        # remove elements outside window
        while dq and dq[0] <= i - k:
            dq.popleft()

        # remove smaller elements
        while dq and arr[dq[-1]] <= arr[i]:
            dq.pop()

        dq.append(i)

        if i >= k - 1:
            result.append(arr[dq[0]])

    return result


# === VALID PARENTHESES ===
def is_valid_parentheses(s: str) -> bool:
    """Check if parentheses are balanced"""
    stack = []
    mapping = {')': '(', '}': '{', ']': '['}

    for char in s:
        if char in mapping:
            if not stack or stack.pop() != mapping[char]:
                return False
        else:
            stack.append(char)

    return len(stack) == 0


def longest_valid_parentheses(s: str) -> int:
    """Find length of longest valid parentheses substring"""
    stack = [-1]  # stores indices
    max_len = 0

    for i, char in enumerate(s):
        if char == '(':
            stack.append(i)
        else:
            stack.pop()
            if not stack:
                stack.append(i)
            else:
                max_len = max(max_len, i - stack[-1])

    return max_len


# === MIN STACK ===
class MinStack:
    """Stack that supports O(1) getMin"""

    def __init__(self):
        self.stack = []
        self.min_stack = []

    def push(self, val: int) -> None:
        self.stack.append(val)
        if not self.min_stack or val <= self.min_stack[-1]:
            self.min_stack.append(val)

    def pop(self) -> None:
        if self.stack.pop() == self.min_stack[-1]:
            self.min_stack.pop()

    def top(self) -> int:
        return self.stack[-1]

    def get_min(self) -> int:
        return self.min_stack[-1]


# === QUEUE USING STACKS ===
class MyQueue:
    """Queue implemented using two stacks"""

    def __init__(self):
        self.stack_in = []
        self.stack_out = []

    def push(self, x: int) -> None:
        self.stack_in.append(x)

    def pop(self) -> int:
        self._transfer()
        return self.stack_out.pop()

    def peek(self) -> int:
        self._transfer()
        return self.stack_out[-1]

    def empty(self) -> bool:
        return not self.stack_in and not self.stack_out

    def _transfer(self) -> None:
        if not self.stack_out:
            while self.stack_in:
                self.stack_out.append(self.stack_in.pop())


# === STACK USING QUEUES ===
class MyStack:
    """Stack implemented using single queue"""

    def __init__(self):
        self.queue = deque()

    def push(self, x: int) -> None:
        self.queue.append(x)
        # rotate to put new element at front
        for _ in range(len(self.queue) - 1):
            self.queue.append(self.queue.popleft())

    def pop(self) -> int:
        return self.queue.popleft()

    def top(self) -> int:
        return self.queue[0]

    def empty(self) -> bool:
        return len(self.queue) == 0


# === EVALUATE REVERSE POLISH NOTATION ===
def eval_rpn(tokens: List[str]) -> int:
    """Evaluate reverse polish notation expression"""
    stack = []
    ops = {
        '+': lambda a, b: a + b,
        '-': lambda a, b: a - b,
        '*': lambda a, b: a * b,
        '/': lambda a, b: int(a / b),
    }

    for token in tokens:
        if token in ops:
            b, a = stack.pop(), stack.pop()
            stack.append(ops[token](a, b))
        else:
            stack.append(int(token))

    return stack[0]


# === DECODE STRING ===
def decode_string(s: str) -> str:
    """Decode string like "3[a2[c]]" -> "accaccacc" """
    stack = []
    curr_str = ""
    curr_num = 0

    for char in s:
        if char.isdigit():
            curr_num = curr_num * 10 + int(char)
        elif char == '[':
            stack.append((curr_str, curr_num))
            curr_str = ""
            curr_num = 0
        elif char == ']':
            prev_str, num = stack.pop()
            curr_str = prev_str + curr_str * num
        else:
            curr_str += char

    return curr_str


# === BASIC CALCULATOR ===
def basic_calculator(s: str) -> int:
    """Evaluate expression with +, -, (, )"""
    stack = []
    result = 0
    num = 0
    sign = 1

    for char in s:
        if char.isdigit():
            num = num * 10 + int(char)
        elif char == '+':
            result += sign * num
            num = 0
            sign = 1
        elif char == '-':
            result += sign * num
            num = 0
            sign = -1
        elif char == '(':
            stack.append(result)
            stack.append(sign)
            result = 0
            sign = 1
        elif char == ')':
            result += sign * num
            num = 0
            result *= stack.pop()  # sign
            result += stack.pop()  # previous result

    return result + sign * num
