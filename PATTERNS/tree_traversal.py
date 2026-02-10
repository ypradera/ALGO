"""
TREE TRAVERSAL PATTERNS
Use: DFS, BFS, path problems, tree construction
Time: O(n), Space: O(h) for DFS, O(w) for BFS
"""

from typing import List, Optional
from collections import deque


class TreeNode:
    def __init__(self, val=0, left=None, right=None):
        self.val = val
        self.left = left
        self.right = right


# === DFS - RECURSIVE ===
def inorder_recursive(root: Optional[TreeNode]) -> List[int]:
    """Left -> Root -> Right"""
    result = []

    def dfs(node):
        if not node:
            return
        dfs(node.left)
        result.append(node.val)
        dfs(node.right)

    dfs(root)
    return result


def preorder_recursive(root: Optional[TreeNode]) -> List[int]:
    """Root -> Left -> Right"""
    result = []

    def dfs(node):
        if not node:
            return
        result.append(node.val)
        dfs(node.left)
        dfs(node.right)

    dfs(root)
    return result


def postorder_recursive(root: Optional[TreeNode]) -> List[int]:
    """Left -> Right -> Root"""
    result = []

    def dfs(node):
        if not node:
            return
        dfs(node.left)
        dfs(node.right)
        result.append(node.val)

    dfs(root)
    return result


# === DFS - ITERATIVE ===
def inorder_iterative(root: Optional[TreeNode]) -> List[int]:
    """Inorder using stack"""
    result = []
    stack = []
    curr = root

    while curr or stack:
        while curr:
            stack.append(curr)
            curr = curr.left
        curr = stack.pop()
        result.append(curr.val)
        curr = curr.right

    return result


def preorder_iterative(root: Optional[TreeNode]) -> List[int]:
    """Preorder using stack"""
    if not root:
        return []

    result = []
    stack = [root]

    while stack:
        node = stack.pop()
        result.append(node.val)
        if node.right:
            stack.append(node.right)
        if node.left:
            stack.append(node.left)

    return result


def postorder_iterative(root: Optional[TreeNode]) -> List[int]:
    """Postorder using two stacks"""
    if not root:
        return []

    result = []
    stack = [root]

    while stack:
        node = stack.pop()
        result.append(node.val)
        if node.left:
            stack.append(node.left)
        if node.right:
            stack.append(node.right)

    return result[::-1]


# === MORRIS TRAVERSAL (O(1) space) ===
def morris_inorder(root: Optional[TreeNode]) -> List[int]:
    """Inorder traversal with O(1) space"""
    result = []
    curr = root

    while curr:
        if not curr.left:
            result.append(curr.val)
            curr = curr.right
        else:
            # find inorder predecessor
            pred = curr.left
            while pred.right and pred.right != curr:
                pred = pred.right

            if not pred.right:
                # create thread
                pred.right = curr
                curr = curr.left
            else:
                # remove thread
                pred.right = None
                result.append(curr.val)
                curr = curr.right

    return result


# === BFS - LEVEL ORDER ===
def level_order(root: Optional[TreeNode]) -> List[List[int]]:
    """Level order traversal"""
    if not root:
        return []

    result = []
    queue = deque([root])

    while queue:
        level = []
        for _ in range(len(queue)):
            node = queue.popleft()
            level.append(node.val)
            if node.left:
                queue.append(node.left)
            if node.right:
                queue.append(node.right)
        result.append(level)

    return result


def zigzag_level_order(root: Optional[TreeNode]) -> List[List[int]]:
    """Zigzag level order traversal"""
    if not root:
        return []

    result = []
    queue = deque([root])
    left_to_right = True

    while queue:
        level = []
        for _ in range(len(queue)):
            node = queue.popleft()
            level.append(node.val)
            if node.left:
                queue.append(node.left)
            if node.right:
                queue.append(node.right)

        if not left_to_right:
            level.reverse()
        result.append(level)
        left_to_right = not left_to_right

    return result


# === TREE DEPTH ===
def max_depth(root: Optional[TreeNode]) -> int:
    """Maximum depth of tree"""
    if not root:
        return 0
    return 1 + max(max_depth(root.left), max_depth(root.right))


def min_depth(root: Optional[TreeNode]) -> int:
    """Minimum depth (to leaf)"""
    if not root:
        return 0
    if not root.left:
        return 1 + min_depth(root.right)
    if not root.right:
        return 1 + min_depth(root.left)
    return 1 + min(min_depth(root.left), min_depth(root.right))


# === VALIDATE BST ===
def is_valid_bst(root: Optional[TreeNode]) -> bool:
    """Check if valid binary search tree"""
    def validate(node, min_val, max_val):
        if not node:
            return True
        if node.val <= min_val or node.val >= max_val:
            return False
        return (validate(node.left, min_val, node.val) and
                validate(node.right, node.val, max_val))

    return validate(root, float('-inf'), float('inf'))


# === LOWEST COMMON ANCESTOR ===
def lowest_common_ancestor(root: TreeNode, p: TreeNode, q: TreeNode) -> TreeNode:
    """Find LCA of two nodes in binary tree"""
    if not root or root == p or root == q:
        return root

    left = lowest_common_ancestor(root.left, p, q)
    right = lowest_common_ancestor(root.right, p, q)

    if left and right:
        return root
    return left or right


def lca_bst(root: TreeNode, p: TreeNode, q: TreeNode) -> TreeNode:
    """Find LCA in BST"""
    while root:
        if p.val < root.val and q.val < root.val:
            root = root.left
        elif p.val > root.val and q.val > root.val:
            root = root.right
        else:
            return root
    return None


# === PATH SUM ===
def has_path_sum(root: Optional[TreeNode], target: int) -> bool:
    """Check if root-to-leaf path with target sum exists"""
    if not root:
        return False
    if not root.left and not root.right:
        return root.val == target
    return (has_path_sum(root.left, target - root.val) or
            has_path_sum(root.right, target - root.val))


def path_sum_all(root: Optional[TreeNode], target: int) -> List[List[int]]:
    """Find all root-to-leaf paths with target sum"""
    result = []

    def dfs(node, remaining, path):
        if not node:
            return
        path.append(node.val)
        if not node.left and not node.right and remaining == node.val:
            result.append(path[:])
        dfs(node.left, remaining - node.val, path)
        dfs(node.right, remaining - node.val, path)
        path.pop()

    dfs(root, target, [])
    return result


# === DIAMETER OF TREE ===
def diameter(root: Optional[TreeNode]) -> int:
    """Longest path between any two nodes"""
    max_diameter = 0

    def depth(node):
        nonlocal max_diameter
        if not node:
            return 0
        left = depth(node.left)
        right = depth(node.right)
        max_diameter = max(max_diameter, left + right)
        return 1 + max(left, right)

    depth(root)
    return max_diameter


# === INVERT TREE ===
def invert_tree(root: Optional[TreeNode]) -> Optional[TreeNode]:
    """Invert binary tree"""
    if not root:
        return None
    root.left, root.right = invert_tree(root.right), invert_tree(root.left)
    return root


# === FLATTEN TO LINKED LIST ===
def flatten(root: Optional[TreeNode]) -> None:
    """Flatten tree to linked list (preorder)"""
    curr = root
    while curr:
        if curr.left:
            # find rightmost of left subtree
            pred = curr.left
            while pred.right:
                pred = pred.right
            pred.right = curr.right
            curr.right = curr.left
            curr.left = None
        curr = curr.right


# === SERIALIZE / DESERIALIZE ===
def serialize(root: Optional[TreeNode]) -> str:
    """Serialize tree to string"""
    result = []

    def dfs(node):
        if not node:
            result.append("null")
            return
        result.append(str(node.val))
        dfs(node.left)
        dfs(node.right)

    dfs(root)
    return ",".join(result)


def deserialize(data: str) -> Optional[TreeNode]:
    """Deserialize string to tree"""
    values = iter(data.split(","))

    def dfs():
        val = next(values)
        if val == "null":
            return None
        node = TreeNode(int(val))
        node.left = dfs()
        node.right = dfs()
        return node

    return dfs()


# === CONSTRUCT FROM TRAVERSALS ===
def build_from_preorder_inorder(preorder: List[int], inorder: List[int]) -> Optional[TreeNode]:
    """Build tree from preorder and inorder traversals"""
    if not preorder:
        return None

    inorder_idx = {val: i for i, val in enumerate(inorder)}
    pre_iter = iter(preorder)

    def build(left, right):
        if left > right:
            return None
        val = next(pre_iter)
        node = TreeNode(val)
        mid = inorder_idx[val]
        node.left = build(left, mid - 1)
        node.right = build(mid + 1, right)
        return node

    return build(0, len(inorder) - 1)
