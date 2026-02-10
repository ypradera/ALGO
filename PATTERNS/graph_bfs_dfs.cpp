/*
 * GRAPH BFS/DFS PATTERNS
 * Use: Traversal, shortest path, cycle detection, topological sort
 * Time: O(V + E), Space: O(V)
 */

#include <vector>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>

using namespace std;

// === ADJACENCY LIST REPRESENTATION ===
vector<vector<int>> buildAdjacencyList(vector<vector<int>>& edges, int n, bool directed = false) {
    vector<vector<int>> graph(n);

    for (auto& edge : edges) {
        graph[edge[0]].push_back(edge[1]);
        if (!directed) {
            graph[edge[1]].push_back(edge[0]);
        }
    }

    return graph;
}

// === BFS TRAVERSAL ===
vector<int> bfs(vector<vector<int>>& graph, int start) {
    vector<bool> visited(graph.size(), false);
    queue<int> q;
    vector<int> order;

    visited[start] = true;
    q.push(start);

    while (!q.empty()) {
        int node = q.front();
        q.pop();
        order.push_back(node);

        for (int neighbor : graph[node]) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                q.push(neighbor);
            }
        }
    }

    return order;
}

// === DFS TRAVERSAL (iterative) ===
vector<int> dfsIterative(vector<vector<int>>& graph, int start) {
    vector<bool> visited(graph.size(), false);
    stack<int> st;
    vector<int> order;

    st.push(start);

    while (!st.empty()) {
        int node = st.top();
        st.pop();

        if (visited[node]) continue;
        visited[node] = true;
        order.push_back(node);

        for (int neighbor : graph[node]) {
            if (!visited[neighbor]) {
                st.push(neighbor);
            }
        }
    }

    return order;
}

// === DFS TRAVERSAL (recursive) ===
void dfsHelper(vector<vector<int>>& graph, int node, vector<bool>& visited, vector<int>& order) {
    visited[node] = true;
    order.push_back(node);
    for (int neighbor : graph[node]) {
        if (!visited[neighbor]) {
            dfsHelper(graph, neighbor, visited, order);
        }
    }
}

vector<int> dfsRecursive(vector<vector<int>>& graph, int start) {
    vector<bool> visited(graph.size(), false);
    vector<int> order;
    dfsHelper(graph, start, visited, order);
    return order;
}

// === SHORTEST PATH (unweighted) ===
int shortestPathBFS(vector<vector<int>>& graph, int start, int end) {
    if (start == end) return 0;

    vector<bool> visited(graph.size(), false);
    queue<pair<int, int>> q;  // (node, distance)

    visited[start] = true;
    q.push({start, 0});

    while (!q.empty()) {
        auto [node, dist] = q.front();
        q.pop();

        for (int neighbor : graph[node]) {
            if (neighbor == end) return dist + 1;
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                q.push({neighbor, dist + 1});
            }
        }
    }

    return -1;
}

// === CYCLE DETECTION (undirected) ===
bool hasCycleUndirected(vector<vector<int>>& graph) {
    int n = graph.size();
    vector<bool> visited(n, false);

    function<bool(int, int)> dfs = [&](int node, int parent) -> bool {
        visited[node] = true;
        for (int neighbor : graph[node]) {
            if (!visited[neighbor]) {
                if (dfs(neighbor, node)) return true;
            } else if (neighbor != parent) {
                return true;
            }
        }
        return false;
    };

    for (int i = 0; i < n; i++) {
        if (!visited[i] && dfs(i, -1)) {
            return true;
        }
    }

    return false;
}

// === CYCLE DETECTION (directed) ===
bool hasCycleDirected(vector<vector<int>>& graph) {
    int n = graph.size();
    vector<int> color(n, 0);  // 0=white, 1=gray, 2=black

    function<bool(int)> dfs = [&](int node) -> bool {
        color[node] = 1;
        for (int neighbor : graph[node]) {
            if (color[neighbor] == 1) return true;  // back edge
            if (color[neighbor] == 0 && dfs(neighbor)) return true;
        }
        color[node] = 2;
        return false;
    };

    for (int i = 0; i < n; i++) {
        if (color[i] == 0 && dfs(i)) {
            return true;
        }
    }

    return false;
}

// === TOPOLOGICAL SORT (Kahn's BFS) ===
vector<int> topologicalSortBFS(vector<vector<int>>& graph) {
    int n = graph.size();
    vector<int> inDegree(n, 0);

    for (int node = 0; node < n; node++) {
        for (int neighbor : graph[node]) {
            inDegree[neighbor]++;
        }
    }

    queue<int> q;
    for (int i = 0; i < n; i++) {
        if (inDegree[i] == 0) q.push(i);
    }

    vector<int> order;
    while (!q.empty()) {
        int node = q.front();
        q.pop();
        order.push_back(node);

        for (int neighbor : graph[node]) {
            if (--inDegree[neighbor] == 0) {
                q.push(neighbor);
            }
        }
    }

    return order.size() == n ? order : vector<int>();  // empty if cycle
}

// === TOPOLOGICAL SORT (DFS) ===
vector<int> topologicalSortDFS(vector<vector<int>>& graph) {
    int n = graph.size();
    vector<bool> visited(n, false);
    stack<int> st;

    function<void(int)> dfs = [&](int node) {
        visited[node] = true;
        for (int neighbor : graph[node]) {
            if (!visited[neighbor]) {
                dfs(neighbor);
            }
        }
        st.push(node);
    };

    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            dfs(i);
        }
    }

    vector<int> order;
    while (!st.empty()) {
        order.push_back(st.top());
        st.pop();
    }

    return order;
}

// === CONNECTED COMPONENTS ===
int countComponents(vector<vector<int>>& graph) {
    int n = graph.size();
    vector<bool> visited(n, false);
    int count = 0;

    function<void(int)> dfs = [&](int node) {
        visited[node] = true;
        for (int neighbor : graph[node]) {
            if (!visited[neighbor]) {
                dfs(neighbor);
            }
        }
    };

    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            dfs(i);
            count++;
        }
    }

    return count;
}

// === BIPARTITE CHECK ===
bool isBipartite(vector<vector<int>>& graph) {
    int n = graph.size();
    vector<int> color(n, -1);

    function<bool(int)> bfs = [&](int start) -> bool {
        queue<int> q;
        q.push(start);
        color[start] = 0;

        while (!q.empty()) {
            int node = q.front();
            q.pop();

            for (int neighbor : graph[node]) {
                if (color[neighbor] == -1) {
                    color[neighbor] = 1 - color[node];
                    q.push(neighbor);
                } else if (color[neighbor] == color[node]) {
                    return false;
                }
            }
        }
        return true;
    };

    for (int i = 0; i < n; i++) {
        if (color[i] == -1 && !bfs(i)) {
            return false;
        }
    }

    return true;
}

// === NUMBER OF ISLANDS (grid) ===
int numIslands(vector<vector<char>>& grid) {
    if (grid.empty()) return 0;

    int m = grid.size(), n = grid[0].size();
    int count = 0;
    int dirs[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

    function<void(int, int)> dfs = [&](int r, int c) {
        if (r < 0 || r >= m || c < 0 || c >= n || grid[r][c] != '1') return;
        grid[r][c] = '0';
        for (auto& d : dirs) {
            dfs(r + d[0], c + d[1]);
        }
    };

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (grid[i][j] == '1') {
                dfs(i, j);
                count++;
            }
        }
    }

    return count;
}

// === COURSE SCHEDULE ===
bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
    vector<vector<int>> graph(numCourses);
    for (auto& p : prerequisites) {
        graph[p[1]].push_back(p[0]);
    }

    return topologicalSortBFS(graph).size() == numCourses;
}

// === CLONE GRAPH ===
class Node {
public:
    int val;
    vector<Node*> neighbors;
    Node(int v) : val(v) {}
};

Node* cloneGraph(Node* node) {
    if (!node) return nullptr;

    unordered_map<Node*, Node*> visited;

    function<Node*(Node*)> dfs = [&](Node* n) -> Node* {
        if (visited.count(n)) return visited[n];

        Node* clone = new Node(n->val);
        visited[n] = clone;

        for (Node* neighbor : n->neighbors) {
            clone->neighbors.push_back(dfs(neighbor));
        }

        return clone;
    };

    return dfs(node);
}
