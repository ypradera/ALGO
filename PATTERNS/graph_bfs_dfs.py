"""
GRAPH BFS/DFS PATTERNS
Use: Traversal, shortest path, cycle detection, topological sort
Time: O(V + E), Space: O(V)
"""

from typing import List, Dict, Set, Tuple
from collections import deque, defaultdict


# === ADJACENCY LIST REPRESENTATION ===
def build_adjacency_list(edges: List[List[int]], n: int, directed: bool = False) -> Dict[int, List[int]]:
    """Build adjacency list from edge list"""
    graph = defaultdict(list)

    for u, v in edges:
        graph[u].append(v)
        if not directed:
            graph[v].append(u)

    return graph


def build_weighted_graph(edges: List[List[int]], directed: bool = False) -> Dict[int, List[Tuple[int, int]]]:
    """Build weighted adjacency list: edges = [[u, v, weight], ...]"""
    graph = defaultdict(list)

    for u, v, w in edges:
        graph[u].append((v, w))
        if not directed:
            graph[v].append((u, w))

    return graph


# === BFS TRAVERSAL ===
def bfs(graph: Dict[int, List[int]], start: int) -> List[int]:
    """BFS traversal from start node"""
    visited = {start}
    queue = deque([start])
    order = []

    while queue:
        node = queue.popleft()
        order.append(node)

        for neighbor in graph[node]:
            if neighbor not in visited:
                visited.add(neighbor)
                queue.append(neighbor)

    return order


# === DFS TRAVERSAL ===
def dfs_iterative(graph: Dict[int, List[int]], start: int) -> List[int]:
    """DFS traversal using stack"""
    visited = set()
    stack = [start]
    order = []

    while stack:
        node = stack.pop()
        if node in visited:
            continue
        visited.add(node)
        order.append(node)

        for neighbor in graph[node]:
            if neighbor not in visited:
                stack.append(neighbor)

    return order


def dfs_recursive(graph: Dict[int, List[int]], start: int) -> List[int]:
    """DFS traversal using recursion"""
    visited = set()
    order = []

    def dfs(node):
        visited.add(node)
        order.append(node)
        for neighbor in graph[node]:
            if neighbor not in visited:
                dfs(neighbor)

    dfs(start)
    return order


# === SHORTEST PATH (unweighted) ===
def shortest_path_bfs(graph: Dict[int, List[int]], start: int, end: int) -> int:
    """Find shortest path length using BFS"""
    if start == end:
        return 0

    visited = {start}
    queue = deque([(start, 0)])

    while queue:
        node, dist = queue.popleft()

        for neighbor in graph[node]:
            if neighbor == end:
                return dist + 1
            if neighbor not in visited:
                visited.add(neighbor)
                queue.append((neighbor, dist + 1))

    return -1  # no path


def shortest_path_with_path(graph: Dict[int, List[int]], start: int, end: int) -> List[int]:
    """Find shortest path with actual path"""
    if start == end:
        return [start]

    visited = {start}
    queue = deque([(start, [start])])

    while queue:
        node, path = queue.popleft()

        for neighbor in graph[node]:
            if neighbor == end:
                return path + [neighbor]
            if neighbor not in visited:
                visited.add(neighbor)
                queue.append((neighbor, path + [neighbor]))

    return []  # no path


# === CYCLE DETECTION (undirected) ===
def has_cycle_undirected(graph: Dict[int, List[int]], n: int) -> bool:
    """Detect cycle in undirected graph"""
    visited = set()

    def dfs(node, parent):
        visited.add(node)
        for neighbor in graph[node]:
            if neighbor not in visited:
                if dfs(neighbor, node):
                    return True
            elif neighbor != parent:
                return True
        return False

    for i in range(n):
        if i not in visited:
            if dfs(i, -1):
                return True

    return False


# === CYCLE DETECTION (directed) ===
def has_cycle_directed(graph: Dict[int, List[int]], n: int) -> bool:
    """Detect cycle in directed graph using colors"""
    WHITE, GRAY, BLACK = 0, 1, 2
    color = [WHITE] * n

    def dfs(node):
        color[node] = GRAY
        for neighbor in graph[node]:
            if color[neighbor] == GRAY:  # back edge
                return True
            if color[neighbor] == WHITE and dfs(neighbor):
                return True
        color[node] = BLACK
        return False

    for i in range(n):
        if color[i] == WHITE:
            if dfs(i):
                return True

    return False


# === TOPOLOGICAL SORT (Kahn's BFS) ===
def topological_sort_bfs(graph: Dict[int, List[int]], n: int) -> List[int]:
    """Topological sort using BFS (Kahn's algorithm)"""
    in_degree = [0] * n

    for node in graph:
        for neighbor in graph[node]:
            in_degree[neighbor] += 1

    queue = deque([i for i in range(n) if in_degree[i] == 0])
    order = []

    while queue:
        node = queue.popleft()
        order.append(node)

        for neighbor in graph[node]:
            in_degree[neighbor] -= 1
            if in_degree[neighbor] == 0:
                queue.append(neighbor)

    return order if len(order) == n else []  # empty if cycle


# === TOPOLOGICAL SORT (DFS) ===
def topological_sort_dfs(graph: Dict[int, List[int]], n: int) -> List[int]:
    """Topological sort using DFS"""
    visited = set()
    stack = []

    def dfs(node):
        visited.add(node)
        for neighbor in graph[node]:
            if neighbor not in visited:
                dfs(neighbor)
        stack.append(node)

    for i in range(n):
        if i not in visited:
            dfs(i)

    return stack[::-1]


# === CONNECTED COMPONENTS ===
def count_components(graph: Dict[int, List[int]], n: int) -> int:
    """Count connected components in undirected graph"""
    visited = set()
    count = 0

    def dfs(node):
        visited.add(node)
        for neighbor in graph[node]:
            if neighbor not in visited:
                dfs(neighbor)

    for i in range(n):
        if i not in visited:
            dfs(i)
            count += 1

    return count


# === BIPARTITE CHECK ===
def is_bipartite(graph: Dict[int, List[int]], n: int) -> bool:
    """Check if graph is bipartite (2-colorable)"""
    color = [-1] * n

    def bfs(start):
        queue = deque([start])
        color[start] = 0

        while queue:
            node = queue.popleft()
            for neighbor in graph[node]:
                if color[neighbor] == -1:
                    color[neighbor] = 1 - color[node]
                    queue.append(neighbor)
                elif color[neighbor] == color[node]:
                    return False
        return True

    for i in range(n):
        if color[i] == -1:
            if not bfs(i):
                return False

    return True


# === GRID BFS (matrix traversal) ===
def bfs_grid(grid: List[List[int]], start: Tuple[int, int]) -> int:
    """BFS on grid, count reachable cells"""
    if not grid:
        return 0

    m, n = len(grid), len(grid[0])
    directions = [(0, 1), (0, -1), (1, 0), (-1, 0)]

    visited = {start}
    queue = deque([start])
    count = 0

    while queue:
        r, c = queue.popleft()
        count += 1

        for dr, dc in directions:
            nr, nc = r + dr, c + dc
            if 0 <= nr < m and 0 <= nc < n and (nr, nc) not in visited and grid[nr][nc] == 1:
                visited.add((nr, nc))
                queue.append((nr, nc))

    return count


# === NUMBER OF ISLANDS ===
def num_islands(grid: List[List[str]]) -> int:
    """Count number of islands in grid"""
    if not grid:
        return 0

    m, n = len(grid), len(grid[0])
    count = 0

    def dfs(r, c):
        if r < 0 or r >= m or c < 0 or c >= n or grid[r][c] != '1':
            return
        grid[r][c] = '0'  # mark visited
        dfs(r + 1, c)
        dfs(r - 1, c)
        dfs(r, c + 1)
        dfs(r, c - 1)

    for i in range(m):
        for j in range(n):
            if grid[i][j] == '1':
                dfs(i, j)
                count += 1

    return count


# === CLONE GRAPH ===
class GraphNode:
    def __init__(self, val=0, neighbors=None):
        self.val = val
        self.neighbors = neighbors if neighbors else []


def clone_graph(node: GraphNode) -> GraphNode:
    """Deep clone a graph"""
    if not node:
        return None

    visited = {}

    def dfs(n):
        if n in visited:
            return visited[n]

        clone = GraphNode(n.val)
        visited[n] = clone

        for neighbor in n.neighbors:
            clone.neighbors.append(dfs(neighbor))

        return clone

    return dfs(node)


# === COURSE SCHEDULE (can finish?) ===
def can_finish(num_courses: int, prerequisites: List[List[int]]) -> bool:
    """Check if all courses can be completed (no cycle)"""
    graph = defaultdict(list)
    for course, prereq in prerequisites:
        graph[prereq].append(course)

    return len(topological_sort_bfs(graph, num_courses)) == num_courses


# === WORD LADDER ===
def ladder_length(begin_word: str, end_word: str, word_list: List[str]) -> int:
    """Shortest transformation sequence length"""
    word_set = set(word_list)
    if end_word not in word_set:
        return 0

    queue = deque([(begin_word, 1)])
    visited = {begin_word}

    while queue:
        word, length = queue.popleft()

        for i in range(len(word)):
            for c in 'abcdefghijklmnopqrstuvwxyz':
                new_word = word[:i] + c + word[i+1:]

                if new_word == end_word:
                    return length + 1

                if new_word in word_set and new_word not in visited:
                    visited.add(new_word)
                    queue.append((new_word, length + 1))

    return 0
