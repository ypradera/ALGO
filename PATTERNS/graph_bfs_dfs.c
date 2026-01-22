/*
 * GRAPH BFS/DFS PATTERNS
 * Use: Traversal, shortest path, connected components, topological sort
 * Embedded focus: adjacency list with static allocation
 */

#include <string.h>

#define MAX_VERTICES 1000
#define MAX_EDGES    5000

// === ADJACENCY LIST (Static Allocation) ===
typedef struct {
    int adj[MAX_EDGES];      // destination vertices
    int head[MAX_VERTICES];  // head[v] = first edge index for vertex v
    int next[MAX_EDGES];     // next edge in list
    int edge_count;
    int vertex_count;
} Graph;

void graph_init(Graph *g, int n) {
    g->vertex_count = n;
    g->edge_count = 0;
    memset(g->head, -1, sizeof(g->head));
}

void graph_add_edge(Graph *g, int from, int to) {
    int idx = g->edge_count++;
    g->adj[idx] = to;
    g->next[idx] = g->head[from];
    g->head[from] = idx;
}

// === BFS (Shortest Path in Unweighted Graph) ===
int bfs_shortest_path(Graph *g, int start, int end, int *dist) {
    int queue[MAX_VERTICES];
    int front = 0, back = 0;

    memset(dist, -1, g->vertex_count * sizeof(int));
    dist[start] = 0;
    queue[back++] = start;

    while (front < back) {
        int u = queue[front++];

        if (u == end) return dist[end];

        for (int e = g->head[u]; e != -1; e = g->next[e]) {
            int v = g->adj[e];
            if (dist[v] == -1) {
                dist[v] = dist[u] + 1;
                queue[back++] = v;
            }
        }
    }
    return -1;  // not reachable
}

// === DFS (Recursive) ===
int visited[MAX_VERTICES];

void dfs_recursive(Graph *g, int u) {
    visited[u] = 1;
    // process vertex u

    for (int e = g->head[u]; e != -1; e = g->next[e]) {
        int v = g->adj[e];
        if (!visited[v]) {
            dfs_recursive(g, v);
        }
    }
}

// === DFS (Iterative with Stack) ===
void dfs_iterative(Graph *g, int start) {
    int stack[MAX_VERTICES];
    int top = -1;

    memset(visited, 0, sizeof(visited));
    stack[++top] = start;

    while (top >= 0) {
        int u = stack[top--];

        if (visited[u]) continue;
        visited[u] = 1;
        // process vertex u

        for (int e = g->head[u]; e != -1; e = g->next[e]) {
            int v = g->adj[e];
            if (!visited[v]) {
                stack[++top] = v;
            }
        }
    }
}

// === CONNECTED COMPONENTS ===
int count_components(Graph *g) {
    memset(visited, 0, sizeof(visited));
    int count = 0;

    for (int i = 0; i < g->vertex_count; i++) {
        if (!visited[i]) {
            dfs_recursive(g, i);
            count++;
        }
    }
    return count;
}

// === CYCLE DETECTION (Directed Graph) ===
#define WHITE 0
#define GRAY  1
#define BLACK 2

int color[MAX_VERTICES];

int has_cycle_dfs(Graph *g, int u) {
    color[u] = GRAY;

    for (int e = g->head[u]; e != -1; e = g->next[e]) {
        int v = g->adj[e];
        if (color[v] == GRAY) return 1;  // back edge = cycle
        if (color[v] == WHITE && has_cycle_dfs(g, v)) return 1;
    }

    color[u] = BLACK;
    return 0;
}

int has_cycle(Graph *g) {
    memset(color, WHITE, sizeof(color));

    for (int i = 0; i < g->vertex_count; i++) {
        if (color[i] == WHITE && has_cycle_dfs(g, i)) {
            return 1;
        }
    }
    return 0;
}

// === TOPOLOGICAL SORT (Kahn's Algorithm - BFS) ===
int topological_sort(Graph *g, int *result) {
    int in_degree[MAX_VERTICES] = {0};
    int queue[MAX_VERTICES];
    int front = 0, back = 0;
    int idx = 0;

    // calculate in-degrees
    for (int u = 0; u < g->vertex_count; u++) {
        for (int e = g->head[u]; e != -1; e = g->next[e]) {
            in_degree[g->adj[e]]++;
        }
    }

    // enqueue vertices with in-degree 0
    for (int i = 0; i < g->vertex_count; i++) {
        if (in_degree[i] == 0) {
            queue[back++] = i;
        }
    }

    while (front < back) {
        int u = queue[front++];
        result[idx++] = u;

        for (int e = g->head[u]; e != -1; e = g->next[e]) {
            int v = g->adj[e];
            if (--in_degree[v] == 0) {
                queue[back++] = v;
            }
        }
    }

    return idx == g->vertex_count ? 1 : 0;  // 0 if cycle exists
}

// === GRID BFS (2D Matrix) ===
int dr[] = {-1, 1, 0, 0};  // up, down, left, right
int dc[] = {0, 0, -1, 1};

int grid_bfs(int grid[][MAX_VERTICES], int rows, int cols,
             int start_r, int start_c, int end_r, int end_c) {

    int dist[MAX_VERTICES][MAX_VERTICES];
    memset(dist, -1, sizeof(dist));

    int queue[MAX_VERTICES * 2];  // pairs: r, c
    int front = 0, back = 0;

    queue[back++] = start_r;
    queue[back++] = start_c;
    dist[start_r][start_c] = 0;

    while (front < back) {
        int r = queue[front++];
        int c = queue[front++];

        if (r == end_r && c == end_c) {
            return dist[r][c];
        }

        for (int d = 0; d < 4; d++) {
            int nr = r + dr[d];
            int nc = c + dc[d];

            if (nr >= 0 && nr < rows && nc >= 0 && nc < cols &&
                grid[nr][nc] != 1 && dist[nr][nc] == -1) {  // 1 = obstacle
                dist[nr][nc] = dist[r][c] + 1;
                queue[back++] = nr;
                queue[back++] = nc;
            }
        }
    }
    return -1;
}
