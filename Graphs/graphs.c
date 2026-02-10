/*
 * Graphs - Interview Problems for Embedded Systems
 *
 * ============================================================================
 *                    BEGINNER'S GUIDE: GRAPHS
 * ============================================================================
 *
 * WHAT IS A GRAPH?
 * ----------------
 * A graph is a collection of NODES (vertices) connected by EDGES.
 * Unlike trees, graphs can have cycles and multiple paths between nodes.
 *
 * VISUAL REPRESENTATION:
 *
 *     [0] ---- [1]              Undirected graph
 *      |      / |               (edges go both ways)
 *      |    /   |
 *     [2] ---- [3]
 *
 *     [0] ---> [1]              Directed graph
 *      |      / |               (edges have direction)
 *      v    v   v
 *     [2] <--- [3]
 *
 * TERMINOLOGY:
 *   - Vertex (Node): A point in the graph
 *   - Edge: Connection between two vertices
 *   - Directed: Edges have direction (A->B is different from B->A)
 *   - Undirected: Edges go both ways
 *   - Weighted: Edges have values (cost, distance)
 *   - Path: Sequence of vertices connected by edges
 *   - Cycle: Path that starts and ends at same vertex
 *   - Connected: Every vertex is reachable from every other vertex
 *   - Degree: Number of edges connected to a vertex
 *
 * GRAPH VS TREE:
 *   - Tree is a special graph with no cycles and single root
 *   - Graph can have cycles, multiple paths, no hierarchy
 *
 * ============================================================================
 * GRAPH REPRESENTATION
 * ============================================================================
 *
 * 1. ADJACENCY MATRIX:
 *    2D array where matrix[i][j] = 1 if edge from i to j
 *
 *    Graph:      Matrix:
 *    0 -- 1        0  1  2  3
 *    |    |     0 [0, 1, 1, 0]
 *    2 -- 3     1 [1, 0, 0, 1]
 *               2 [1, 0, 0, 1]
 *               3 [0, 1, 1, 0]
 *
 *    PROS: O(1) edge lookup
 *    CONS: O(V²) space, even for sparse graphs
 *
 * 2. ADJACENCY LIST:
 *    Array of lists, each containing neighbors
 *
 *    0: [1, 2]
 *    1: [0, 3]
 *    2: [0, 3]
 *    3: [1, 2]
 *
 *    PROS: O(V+E) space, efficient for sparse graphs
 *    CONS: O(degree) edge lookup
 *
 * FOR EMBEDDED: Adjacency list usually preferred (less memory)
 *
 * ============================================================================
 * GRAPHS IN EMBEDDED SYSTEMS
 * ============================================================================
 *
 * 1. STATE MACHINES
 *    - States are vertices, transitions are edges
 *    - Used in: Protocol handlers, UI navigation, device control
 *
 * 2. TASK DEPENDENCIES
 *    - Tasks are vertices, dependencies are edges
 *    - Topological sort for execution order
 *
 * 3. NETWORK TOPOLOGY
 *    - Devices are vertices, connections are edges
 *    - Routing, shortest path algorithms
 *
 * 4. SENSOR NETWORKS
 *    - Sensors are vertices, communication links are edges
 *    - Finding optimal communication paths
 *
 * 5. PCB ROUTING
 *    - Pins are vertices, traces are edges
 *    - Finding shortest paths without crossing
 *
 * ============================================================================
 * BIG-O COMPLEXITY
 * ============================================================================
 *
 * V = vertices, E = edges
 *
 * Operation          | Adj Matrix | Adj List
 * -------------------|------------|----------
 * Add vertex         | O(V²)      | O(1)
 * Add edge           | O(1)       | O(1)
 * Remove edge        | O(1)       | O(E)
 * Check edge exists  | O(1)       | O(degree)
 * Find neighbors     | O(V)       | O(degree)
 * Space              | O(V²)      | O(V+E)
 * BFS/DFS            | O(V²)      | O(V+E)
 *
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_VERTICES 10

/* ============================================================================
 * GRAPH REPRESENTATION: ADJACENCY MATRIX
 * ============================================================================ */

typedef struct {
    int matrix[MAX_VERTICES][MAX_VERTICES];
    int numVertices;
} GraphMatrix;

void graphMatrix_init(GraphMatrix* g, int vertices) {
    g->numVertices = vertices;
    for (int i = 0; i < vertices; i++) {
        for (int j = 0; j < vertices; j++) {
            g->matrix[i][j] = 0;
        }
    }
}

void graphMatrix_addEdge(GraphMatrix* g, int src, int dest, bool directed) {
    g->matrix[src][dest] = 1;
    if (!directed) {
        g->matrix[dest][src] = 1;
    }
}

void graphMatrix_print(GraphMatrix* g) {
    printf("Adjacency Matrix:\n   ");
    for (int i = 0; i < g->numVertices; i++) {
        printf("%d ", i);
    }
    printf("\n");
    for (int i = 0; i < g->numVertices; i++) {
        printf("%d: ", i);
        for (int j = 0; j < g->numVertices; j++) {
            printf("%d ", g->matrix[i][j]);
        }
        printf("\n");
    }
}

/* ============================================================================
 * GRAPH REPRESENTATION: ADJACENCY LIST
 * ============================================================================ */

typedef struct AdjListNode {
    int dest;
    int weight;     // For weighted graphs
    struct AdjListNode* next;
} AdjListNode;

typedef struct {
    AdjListNode* head[MAX_VERTICES];
    int numVertices;
} GraphList;

void graphList_init(GraphList* g, int vertices) {
    g->numVertices = vertices;
    for (int i = 0; i < vertices; i++) {
        g->head[i] = NULL;
    }
}

AdjListNode* createAdjNode(int dest, int weight) {
    AdjListNode* node = (AdjListNode*)malloc(sizeof(AdjListNode));
    if (node) {
        node->dest = dest;
        node->weight = weight;
        node->next = NULL;
    }
    return node;
}

void graphList_addEdge(GraphList* g, int src, int dest, int weight, bool directed) {
    // Add edge src -> dest
    AdjListNode* node = createAdjNode(dest, weight);
    node->next = g->head[src];
    g->head[src] = node;

    // For undirected, also add dest -> src
    if (!directed) {
        node = createAdjNode(src, weight);
        node->next = g->head[dest];
        g->head[dest] = node;
    }
}

void graphList_print(GraphList* g) {
    printf("Adjacency List:\n");
    for (int i = 0; i < g->numVertices; i++) {
        printf("%d: ", i);
        AdjListNode* curr = g->head[i];
        while (curr) {
            printf("-> %d ", curr->dest);
            curr = curr->next;
        }
        printf("\n");
    }
}

void graphList_free(GraphList* g) {
    for (int i = 0; i < g->numVertices; i++) {
        AdjListNode* curr = g->head[i];
        while (curr) {
            AdjListNode* temp = curr;
            curr = curr->next;
            free(temp);
        }
    }
}

/* ============================================================================
 * 1. BREADTH-FIRST SEARCH (BFS)
 * Time: O(V+E), Space: O(V)
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * BFS explores graph level by level, like ripples in water.
 * Uses a QUEUE (FIFO).
 *
 * VISUAL:
 *     [0] ---- [1] ---- [4]
 *      |        |
 *     [2] ---- [3]
 *
 *   Starting from 0:
 *   Level 0: Visit 0
 *   Level 1: Visit 1, 2 (neighbors of 0)
 *   Level 2: Visit 3, 4 (neighbors of 1, 2)
 *
 *   Order: 0, 1, 2, 3, 4
 *
 * APPLICATIONS:
 *   - Shortest path in unweighted graph
 *   - Level order traversal
 *   - Finding connected components
 *   - Testing if graph is bipartite
 *
 * EMBEDDED USE:
 *   - Network broadcast (reach all nodes)
 *   - Finding shortest route in mesh network
 * ============================================================================ */

void bfs(GraphList* g, int start) {
    bool visited[MAX_VERTICES] = {false};
    int queue[MAX_VERTICES];
    int front = 0, rear = 0;

    // Start with initial vertex
    visited[start] = true;
    queue[rear++] = start;

    printf("BFS from vertex %d: ", start);

    while (front < rear) {
        // Dequeue vertex
        int current = queue[front++];
        printf("%d ", current);

        // Visit all unvisited neighbors
        AdjListNode* neighbor = g->head[current];
        while (neighbor) {
            if (!visited[neighbor->dest]) {
                visited[neighbor->dest] = true;
                queue[rear++] = neighbor->dest;
            }
            neighbor = neighbor->next;
        }
    }
    printf("\n");
}

/* ============================================================================
 * 2. DEPTH-FIRST SEARCH (DFS)
 * Time: O(V+E), Space: O(V)
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * DFS explores as far as possible before backtracking.
 * Uses a STACK (or recursion).
 *
 * VISUAL (same graph):
 *     [0] ---- [1] ---- [4]
 *      |        |
 *     [2] ---- [3]
 *
 *   Starting from 0:
 *   Visit 0 -> go to neighbor 1
 *   Visit 1 -> go to neighbor 3
 *   Visit 3 -> go to neighbor 2
 *   Visit 2 -> already visited 0, backtrack
 *   Back to 1 -> go to neighbor 4
 *   Visit 4 -> done
 *
 *   Order: 0, 1, 3, 2, 4 (or 0, 2, 3, 1, 4 depending on neighbor order)
 *
 * APPLICATIONS:
 *   - Cycle detection
 *   - Topological sorting
 *   - Path finding
 *   - Solving mazes
 *
 * EMBEDDED USE:
 *   - Exploring all states in state machine
 *   - Detecting deadlock cycles
 * ============================================================================ */

void dfsRecursive(GraphList* g, int vertex, bool visited[]) {
    visited[vertex] = true;
    printf("%d ", vertex);

    AdjListNode* neighbor = g->head[vertex];
    while (neighbor) {
        if (!visited[neighbor->dest]) {
            dfsRecursive(g, neighbor->dest, visited);
        }
        neighbor = neighbor->next;
    }
}

void dfs(GraphList* g, int start) {
    bool visited[MAX_VERTICES] = {false};
    printf("DFS from vertex %d: ", start);
    dfsRecursive(g, start, visited);
    printf("\n");
}

// Iterative DFS (uses explicit stack - better for embedded)
void dfsIterative(GraphList* g, int start) {
    bool visited[MAX_VERTICES] = {false};
    int stack[MAX_VERTICES];
    int top = 0;

    stack[top++] = start;

    printf("DFS (iterative) from vertex %d: ", start);

    while (top > 0) {
        int current = stack[--top];

        if (!visited[current]) {
            visited[current] = true;
            printf("%d ", current);

            // Add neighbors to stack
            AdjListNode* neighbor = g->head[current];
            while (neighbor) {
                if (!visited[neighbor->dest]) {
                    stack[top++] = neighbor->dest;
                }
                neighbor = neighbor->next;
            }
        }
    }
    printf("\n");
}

/* ============================================================================
 * 3. CYCLE DETECTION
 * Time: O(V+E), Space: O(V)
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * A cycle is a path that starts and ends at the same vertex.
 *
 * For UNDIRECTED graph:
 *   Use DFS. If we find an already-visited vertex (not parent), there's a cycle.
 *
 * For DIRECTED graph:
 *   Use DFS with "recursion stack". If we find vertex that's currently in
 *   the recursion stack, there's a cycle.
 *
 * EMBEDDED USE:
 *   - Detecting deadlocks in resource allocation
 *   - Validating state machine doesn't have infinite loops
 * ============================================================================ */

bool hasCycleUndirectedDFS(GraphList* g, int vertex, bool visited[], int parent) {
    visited[vertex] = true;

    AdjListNode* neighbor = g->head[vertex];
    while (neighbor) {
        if (!visited[neighbor->dest]) {
            if (hasCycleUndirectedDFS(g, neighbor->dest, visited, vertex)) {
                return true;
            }
        } else if (neighbor->dest != parent) {
            // Found visited vertex that's not parent = cycle!
            return true;
        }
        neighbor = neighbor->next;
    }
    return false;
}

bool hasCycleUndirected(GraphList* g) {
    bool visited[MAX_VERTICES] = {false};

    for (int i = 0; i < g->numVertices; i++) {
        if (!visited[i]) {
            if (hasCycleUndirectedDFS(g, i, visited, -1)) {
                return true;
            }
        }
    }
    return false;
}

/* ============================================================================
 * 4. TOPOLOGICAL SORT
 * Time: O(V+E), Space: O(V)
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * Linear ordering of vertices where for every edge u->v, u comes before v.
 * Only works on DIRECTED ACYCLIC GRAPHS (DAG).
 *
 * VISUAL:
 *     [5] --> [2] --> [3]
 *      |       |
 *      v       v
 *     [0] <-- [4] --> [1]
 *
 *   Valid topological orders:
 *   5, 4, 2, 3, 1, 0
 *   5, 4, 0, 2, 3, 1
 *   etc.
 *
 * EMBEDDED USE:
 *   - Task scheduling (run tasks in dependency order)
 *   - Build systems (compile files in correct order)
 *   - Initialization sequence (init peripherals in correct order)
 * ============================================================================ */

void topologicalSortDFS(GraphList* g, int vertex, bool visited[], int stack[], int* stackIdx) {
    visited[vertex] = true;

    AdjListNode* neighbor = g->head[vertex];
    while (neighbor) {
        if (!visited[neighbor->dest]) {
            topologicalSortDFS(g, neighbor->dest, visited, stack, stackIdx);
        }
        neighbor = neighbor->next;
    }

    // Push to stack after visiting all descendants
    stack[(*stackIdx)++] = vertex;
}

void topologicalSort(GraphList* g) {
    bool visited[MAX_VERTICES] = {false};
    int stack[MAX_VERTICES];
    int stackIdx = 0;

    // Visit all vertices
    for (int i = 0; i < g->numVertices; i++) {
        if (!visited[i]) {
            topologicalSortDFS(g, i, visited, stack, &stackIdx);
        }
    }

    // Print stack in reverse order
    printf("Topological Sort: ");
    for (int i = stackIdx - 1; i >= 0; i--) {
        printf("%d ", stack[i]);
    }
    printf("\n");
}

/* ============================================================================
 * 5. SHORTEST PATH (Dijkstra's Algorithm)
 * Time: O(V²) with matrix, O(E log V) with heap
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * Find shortest path from source to all other vertices in weighted graph.
 *
 * ALGORITHM:
 * 1. Set distance to source = 0, all others = infinity
 * 2. Pick unvisited vertex with smallest distance
 * 3. Update distances to its neighbors
 * 4. Mark as visited, repeat until all visited
 *
 * EMBEDDED USE:
 *   - Network routing
 *   - Finding optimal sensor communication paths
 * ============================================================================ */

int minDistance(int dist[], bool visited[], int V) {
    int min = INT_MAX, minIdx = -1;
    for (int v = 0; v < V; v++) {
        if (!visited[v] && dist[v] < min) {
            min = dist[v];
            minIdx = v;
        }
    }
    return minIdx;
}

void dijkstra(GraphList* g, int src) {
    int dist[MAX_VERTICES];
    bool visited[MAX_VERTICES] = {false};

    // Initialize distances
    for (int i = 0; i < g->numVertices; i++) {
        dist[i] = INT_MAX;
    }
    dist[src] = 0;

    for (int count = 0; count < g->numVertices - 1; count++) {
        int u = minDistance(dist, visited, g->numVertices);
        if (u == -1) break;

        visited[u] = true;

        // Update distances of neighbors
        AdjListNode* neighbor = g->head[u];
        while (neighbor) {
            int v = neighbor->dest;
            int weight = neighbor->weight;
            if (!visited[v] && dist[u] != INT_MAX &&
                dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
            }
            neighbor = neighbor->next;
        }
    }

    // Print results
    printf("Shortest distances from vertex %d:\n", src);
    for (int i = 0; i < g->numVertices; i++) {
        if (dist[i] == INT_MAX) {
            printf("  To %d: INF\n", i);
        } else {
            printf("  To %d: %d\n", i, dist[i]);
        }
    }
}

/* ============================================================================
 * 6. STATE MACHINE IMPLEMENTATION
 * Common embedded pattern using graphs
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * A state machine is a graph where:
 *   - Vertices = states
 *   - Edges = transitions (triggered by events)
 *
 * EXAMPLE: Simple LED controller
 *   States: OFF, ON, BLINKING
 *   Events: BUTTON_PRESS, TIMEOUT
 *
 *         BUTTON_PRESS
 *    [OFF] <-----------> [ON]
 *      |                   |
 *      |  LONG_PRESS       | DOUBLE_PRESS
 *      v                   v
 *         [BLINKING]
 * ============================================================================ */

typedef enum {
    STATE_OFF,
    STATE_ON,
    STATE_BLINKING,
    NUM_STATES
} LedState;

typedef enum {
    EVENT_BUTTON_PRESS,
    EVENT_LONG_PRESS,
    EVENT_DOUBLE_PRESS,
    EVENT_TIMEOUT,
    NUM_EVENTS
} LedEvent;

const char* stateNames[] = {"OFF", "ON", "BLINKING"};
const char* eventNames[] = {"BUTTON_PRESS", "LONG_PRESS", "DOUBLE_PRESS", "TIMEOUT"};

// State transition table (can be thought of as adjacency matrix)
// transition[currentState][event] = nextState
// -1 means no transition
int transitions[NUM_STATES][NUM_EVENTS] = {
    // BUTTON_PRESS  LONG_PRESS   DOUBLE_PRESS  TIMEOUT
    {  STATE_ON,     STATE_BLINKING, -1,        -1      },  // STATE_OFF
    {  STATE_OFF,    -1,             STATE_BLINKING, -1 },  // STATE_ON
    {  STATE_OFF,    -1,             -1,        STATE_OFF}   // STATE_BLINKING
};

typedef struct {
    LedState currentState;
    void (*onEnter[NUM_STATES])(void);  // Entry actions
    void (*onExit[NUM_STATES])(void);   // Exit actions
} StateMachine;

void led_off_enter(void) { printf("  [Action] LED turned OFF\n"); }
void led_on_enter(void) { printf("  [Action] LED turned ON\n"); }
void led_blink_enter(void) { printf("  [Action] LED blinking started\n"); }

void stateMachine_init(StateMachine* sm) {
    sm->currentState = STATE_OFF;
    sm->onEnter[STATE_OFF] = led_off_enter;
    sm->onEnter[STATE_ON] = led_on_enter;
    sm->onEnter[STATE_BLINKING] = led_blink_enter;
}

void stateMachine_processEvent(StateMachine* sm, LedEvent event) {
    int nextState = transitions[sm->currentState][event];

    printf("  Event: %s in state %s -> ",
           eventNames[event], stateNames[sm->currentState]);

    if (nextState == -1) {
        printf("No transition (ignored)\n");
        return;
    }

    printf("State %s\n", stateNames[nextState]);

    // Execute entry action
    if (sm->onEnter[nextState]) {
        sm->onEnter[nextState]();
    }

    sm->currentState = nextState;
}

/* ============================================================================
 * DEMO FUNCTIONS
 * ============================================================================ */

void demo_representation(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|            GRAPH REPRESENTATIONS                                   |\n");
    printf("+====================================================================+\n");

    printf("\n[DEMO] Graph:\n");
    printf("    [0] ---- [1]\n");
    printf("     |        |\n");
    printf("    [2] ---- [3]\n");

    printf("\n[Adjacency Matrix]\n");
    GraphMatrix gm;
    graphMatrix_init(&gm, 4);
    graphMatrix_addEdge(&gm, 0, 1, false);
    graphMatrix_addEdge(&gm, 0, 2, false);
    graphMatrix_addEdge(&gm, 1, 3, false);
    graphMatrix_addEdge(&gm, 2, 3, false);
    graphMatrix_print(&gm);

    printf("\n[Adjacency List]\n");
    GraphList gl;
    graphList_init(&gl, 4);
    graphList_addEdge(&gl, 0, 1, 1, false);
    graphList_addEdge(&gl, 0, 2, 1, false);
    graphList_addEdge(&gl, 1, 3, 1, false);
    graphList_addEdge(&gl, 2, 3, 1, false);
    graphList_print(&gl);

    printf("\n[COMPARISON]\n");
    printf("  Matrix: O(V²) space, O(1) edge lookup\n");
    printf("  List:   O(V+E) space, O(degree) edge lookup\n");
    printf("  For embedded: List usually preferred (less memory)\n");

    graphList_free(&gl);
}

void demo_traversals(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|            BFS AND DFS TRAVERSALS                                  |\n");
    printf("+====================================================================+\n");

    GraphList g;
    graphList_init(&g, 5);

    // Create graph:
    // 0 -- 1 -- 4
    // |    |
    // 2 -- 3
    graphList_addEdge(&g, 0, 1, 1, false);
    graphList_addEdge(&g, 0, 2, 1, false);
    graphList_addEdge(&g, 1, 3, 1, false);
    graphList_addEdge(&g, 1, 4, 1, false);
    graphList_addEdge(&g, 2, 3, 1, false);

    printf("\n[DEMO] Graph:\n");
    printf("    [0] ---- [1] ---- [4]\n");
    printf("     |        |\n");
    printf("    [2] ---- [3]\n\n");

    bfs(&g, 0);
    dfs(&g, 0);
    dfsIterative(&g, 0);

    printf("\n[KEY DIFFERENCE]\n");
    printf("  BFS: Level by level (uses queue)\n");
    printf("  DFS: Go deep first (uses stack/recursion)\n");
    printf("  BFS gives shortest path in unweighted graph!\n");

    graphList_free(&g);
}

void demo_algorithms(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|            GRAPH ALGORITHMS                                        |\n");
    printf("+====================================================================+\n");

    // Cycle detection
    printf("\n[DEMO] Cycle Detection\n");
    GraphList g1;
    graphList_init(&g1, 4);
    graphList_addEdge(&g1, 0, 1, 1, false);
    graphList_addEdge(&g1, 1, 2, 1, false);
    graphList_addEdge(&g1, 2, 3, 1, false);
    printf("  Graph without cycle (0-1-2-3): Has cycle? %s\n",
           hasCycleUndirected(&g1) ? "YES" : "NO");
    graphList_addEdge(&g1, 3, 0, 1, false);  // Add cycle
    printf("  After adding edge 3-0:         Has cycle? %s\n",
           hasCycleUndirected(&g1) ? "YES" : "NO");
    graphList_free(&g1);

    // Topological sort
    printf("\n[DEMO] Topological Sort (Task Dependencies)\n");
    printf("  Task graph (directed):\n");
    printf("    [5] --> [2] --> [3]\n");
    printf("     |       |\n");
    printf("     v       v\n");
    printf("    [0] <-- [4] --> [1]\n\n");

    GraphList g2;
    graphList_init(&g2, 6);
    graphList_addEdge(&g2, 5, 2, 1, true);
    graphList_addEdge(&g2, 5, 0, 1, true);
    graphList_addEdge(&g2, 4, 0, 1, true);
    graphList_addEdge(&g2, 4, 1, 1, true);
    graphList_addEdge(&g2, 2, 3, 1, true);
    topologicalSort(&g2);
    graphList_free(&g2);

    // Shortest path
    printf("\n[DEMO] Shortest Path (Dijkstra)\n");
    printf("  Weighted graph:\n");
    printf("    [0] --4-- [1]\n");
    printf("     |        |\n");
    printf("     1        2\n");
    printf("     |        |\n");
    printf("    [2] --3-- [3]\n\n");

    GraphList g3;
    graphList_init(&g3, 4);
    graphList_addEdge(&g3, 0, 1, 4, false);
    graphList_addEdge(&g3, 0, 2, 1, false);
    graphList_addEdge(&g3, 1, 3, 2, false);
    graphList_addEdge(&g3, 2, 3, 3, false);
    dijkstra(&g3, 0);
    graphList_free(&g3);
}

void demo_stateMachine(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|            STATE MACHINE (Graph in Embedded)                       |\n");
    printf("+====================================================================+\n");

    printf("\n[WHY STATE MACHINES IN EMBEDDED?]\n");
    printf("  - Clear structure for complex behavior\n");
    printf("  - Easy to test and debug\n");
    printf("  - Can be represented as graph\n");
    printf("  - Common in: protocols, UI, device control\n");

    printf("\n[DEMO] LED Controller State Machine\n");
    printf("  States: OFF, ON, BLINKING\n");
    printf("  Events: BUTTON_PRESS, LONG_PRESS, DOUBLE_PRESS, TIMEOUT\n\n");

    StateMachine sm;
    stateMachine_init(&sm);

    printf("Initial state: %s\n\n", stateNames[sm.currentState]);

    printf("Simulating events:\n");
    stateMachine_processEvent(&sm, EVENT_BUTTON_PRESS);
    stateMachine_processEvent(&sm, EVENT_DOUBLE_PRESS);
    stateMachine_processEvent(&sm, EVENT_TIMEOUT);
    stateMachine_processEvent(&sm, EVENT_LONG_PRESS);
}

/* ============================================================================
 * MAIN
 * ============================================================================ */

int main(void) {
    printf("+====================================================================+\n");
    printf("|         GRAPHS - EMBEDDED SYSTEMS INTERVIEW GUIDE                  |\n");
    printf("+====================================================================+\n");

    demo_representation();
    demo_traversals();
    demo_algorithms();
    demo_stateMachine();

    printf("\n");
    printf("+====================================================================+\n");
    printf("|                    SUMMARY                                         |\n");
    printf("+====================================================================+\n");
    printf("| Key algorithms:                                                    |\n");
    printf("|   - BFS: Level-order, shortest path (unweighted)                  |\n");
    printf("|   - DFS: Explore deeply, cycle detection                          |\n");
    printf("|   - Topological Sort: Task scheduling                              |\n");
    printf("|   - Dijkstra: Shortest path (weighted)                            |\n");
    printf("|                                                                    |\n");
    printf("| Embedded applications:                                             |\n");
    printf("|   - State machines (states = vertices, transitions = edges)       |\n");
    printf("|   - Task dependencies and scheduling                               |\n");
    printf("|   - Network routing                                                |\n");
    printf("+====================================================================+\n");

    return 0;
}
