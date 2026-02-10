/*
 * Trees - Interview Problems for Embedded Systems
 *
 * ============================================================================
 *                    BEGINNER'S GUIDE: TREES
 * ============================================================================
 *
 * WHAT IS A TREE?
 * ---------------
 * A tree is a hierarchical data structure with nodes connected by edges.
 * Unlike linked lists (linear), trees branch out.
 *
 * TERMINOLOGY:
 *   - Root: The topmost node (no parent)
 *   - Parent: Node with children below it
 *   - Child: Node connected below a parent
 *   - Leaf: Node with no children
 *   - Height: Longest path from root to leaf
 *   - Depth: Distance from root to a node
 *
 * VISUAL REPRESENTATION (Binary Tree):
 *
 *              [10]         <- Root (level 0)
 *             /    \
 *          [5]      [15]    <- Level 1
 *         /   \    /    \
 *       [3]  [7] [12]  [20] <- Level 2 (Leaves)
 *
 * TYPES OF TREES:
 *   1. BINARY TREE: Each node has at most 2 children
 *   2. BINARY SEARCH TREE (BST): Left < Parent < Right
 *   3. AVL TREE: Self-balancing BST
 *   4. HEAP: Complete binary tree with heap property
 *   5. TRIE: For string storage/search
 *
 * WHY TREES IN EMBEDDED?
 *   - Hierarchical data (file systems, menus)
 *   - Priority queues (heaps for task scheduling)
 *   - Fast lookup (BST for configuration storage)
 *   - Expression parsing (syntax trees)
 *   - Decision trees (state machines)
 *
 * ============================================================================
 * BIG-O COMPLEXITY (Binary Search Tree)
 * ============================================================================
 *
 * Operation      | Average  | Worst    | Notes
 * ---------------|----------|----------|----------------------------------
 * Search         | O(log n) | O(n)     | Worst case: degenerate (linked list)
 * Insert         | O(log n) | O(n)     | Worst case: inserting sorted data
 * Delete         | O(log n) | O(n)     | Worst case: degenerate tree
 * Find Min/Max   | O(log n) | O(n)     | Go left/right until leaf
 * Traversal      | O(n)     | O(n)     | Must visit all nodes
 *
 * For balanced trees (AVL, Red-Black): All operations O(log n) guaranteed
 *
 * ============================================================================
 * COMMON INTERVIEW PROBLEMS
 * ============================================================================
 *
 * #  | Problem                          | Difficulty | Key Concept
 * ---|----------------------------------|------------|------------------------
 * 1  | Inorder/Preorder/Postorder       | Easy       | Recursion, stack
 * 2  | Level Order Traversal            | Easy       | BFS with queue
 * 3  | Maximum Depth                    | Easy       | Recursion
 * 4  | Validate BST                     | Medium     | Range checking
 * 5  | Lowest Common Ancestor           | Medium     | Recursion or parent ptrs
 * 6  | Serialize/Deserialize            | Medium     | Preorder + markers
 * 7  | BST to Sorted Doubly LL          | Medium     | Inorder traversal
 * 8  | Construct from Traversals        | Medium     | Recursion + indexing
 * 9  | Is Balanced?                     | Easy       | Check heights
 * 10 | Path Sum                         | Easy       | DFS
 *
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

/* ============================================================================
 * BINARY TREE NODE STRUCTURE
 * ============================================================================
 *
 * VISUAL:
 *          +--------+
 *          | parent |  (optional, makes some operations easier)
 *          +--------+
 *               |
 *          +--------+
 *          |  data  |
 *          +--------+
 *         /          \
 *   +--------+    +--------+
 *   |  left  |    | right  |
 *   +--------+    +--------+
 *
 * ============================================================================ */

typedef struct TreeNode {
    int data;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

/* ============================================================================
 * UTILITY FUNCTIONS
 * ============================================================================ */

TreeNode* createTreeNode(int data) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if (node) {
        node->data = data;
        node->left = NULL;
        node->right = NULL;
    }
    return node;
}

void freeTree(TreeNode* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

/* ============================================================================
 * 1. TREE TRAVERSALS
 * Time: O(n), Space: O(h) where h = height (for recursion stack)
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * "Traversal" = visiting every node in a specific order.
 *
 * THREE MAIN TYPES (for binary trees):
 *
 *        [1]
 *       /   \
 *     [2]   [3]
 *     / \
 *   [4] [5]
 *
 *   INORDER (Left, Root, Right):   4, 2, 5, 1, 3
 *     "Go left, visit me, go right"
 *     For BST: gives sorted order!
 *
 *   PREORDER (Root, Left, Right):  1, 2, 4, 5, 3
 *     "Visit me, go left, go right"
 *     Used for: copying tree, prefix expression
 *
 *   POSTORDER (Left, Right, Root): 4, 5, 2, 3, 1
 *     "Go left, go right, visit me"
 *     Used for: deleting tree, postfix expression
 *
 * LEVEL ORDER (BFS):              1, 2, 3, 4, 5
 *     Visit level by level, left to right
 *     Uses queue instead of recursion
 *
 * MEMORY TRICK:
 *   The name tells you when to print the ROOT:
 *   - INorder:   print IN the middle (left, ROOT, right)
 *   - PREorder:  print BEFORE (ROOT, left, right)
 *   - POSTorder: print AFTER (left, right, ROOT)
 * ============================================================================ */

// INORDER: Left -> Root -> Right
void inorderTraversal(TreeNode* root) {
    if (!root) return;

    inorderTraversal(root->left);   // 1. Visit left subtree
    printf("%d ", root->data);       // 2. Visit root (print)
    inorderTraversal(root->right);  // 3. Visit right subtree
}

// PREORDER: Root -> Left -> Right
void preorderTraversal(TreeNode* root) {
    if (!root) return;

    printf("%d ", root->data);       // 1. Visit root (print)
    preorderTraversal(root->left);   // 2. Visit left subtree
    preorderTraversal(root->right);  // 3. Visit right subtree
}

// POSTORDER: Left -> Right -> Root
void postorderTraversal(TreeNode* root) {
    if (!root) return;

    postorderTraversal(root->left);  // 1. Visit left subtree
    postorderTraversal(root->right); // 2. Visit right subtree
    printf("%d ", root->data);       // 3. Visit root (print)
}

/* ============================================================================
 * 2. LEVEL ORDER TRAVERSAL (BFS)
 * Time: O(n), Space: O(w) where w = max width of tree
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * Visit nodes level by level, left to right.
 *
 * Uses a QUEUE:
 *   1. Add root to queue
 *   2. While queue not empty:
 *      a. Remove front node, print it
 *      b. Add its children to queue
 *
 * VISUAL:
 *        [1]       Level 0
 *       /   \
 *     [2]   [3]    Level 1
 *     / \
 *   [4] [5]        Level 2
 *
 *   Queue evolution:
 *   [1]         -> print 1, add 2,3
 *   [2,3]       -> print 2, add 4,5
 *   [3,4,5]     -> print 3 (no children)
 *   [4,5]       -> print 4
 *   [5]         -> print 5
 *   []          -> done
 *
 *   Output: 1 2 3 4 5
 * ============================================================================ */

// Simple queue for level order traversal
#define MAX_QUEUE_SIZE 100

void levelOrderTraversal(TreeNode* root) {
    if (!root) return;

    TreeNode* queue[MAX_QUEUE_SIZE];
    int front = 0, rear = 0;

    // Enqueue root
    queue[rear++] = root;

    while (front < rear) {
        // Dequeue and print
        TreeNode* node = queue[front++];
        printf("%d ", node->data);

        // Enqueue children
        if (node->left) queue[rear++] = node->left;
        if (node->right) queue[rear++] = node->right;
    }
}

/* ============================================================================
 * 3. MAXIMUM DEPTH (Height of Tree)
 * Time: O(n), Space: O(h)
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * Maximum depth = longest path from root to any leaf.
 *
 * RECURSIVE INSIGHT:
 * Height of tree = 1 + max(height of left, height of right)
 * Base case: empty tree has height 0
 *
 *        [1]           height = 1 + max(2, 1) = 3
 *       /   \
 *     [2]   [3]        left height = 2, right height = 1
 *     / \
 *   [4] [5]
 * ============================================================================ */

int maxDepth(TreeNode* root) {
    if (!root) return 0;

    int leftDepth = maxDepth(root->left);
    int rightDepth = maxDepth(root->right);

    return 1 + (leftDepth > rightDepth ? leftDepth : rightDepth);
}

/* ============================================================================
 * 4. BINARY SEARCH TREE (BST) OPERATIONS
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * BST Property: For every node:
 *   - ALL nodes in left subtree have SMALLER values
 *   - ALL nodes in right subtree have LARGER values
 *
 *        [10]
 *       /    \
 *     [5]    [15]
 *    /  \   /   \
 *  [3] [7][12] [20]
 *
 *   5 < 10 ✓  (left < root)
 *   15 > 10 ✓ (right > root)
 *   3,7 < 10 ✓ (entire left subtree < root)
 *   12,20 > 10 ✓ (entire right subtree > root)
 *
 * WHY BST?
 *   - Search is O(log n) average (like binary search in array)
 *   - In-order traversal gives sorted order
 *   - Efficient insertion/deletion
 * ============================================================================ */

// INSERT into BST
TreeNode* bstInsert(TreeNode* root, int data) {
    // Base case: empty tree, create new node
    if (!root) return createTreeNode(data);

    // Recursive case: go left or right
    if (data < root->data) {
        root->left = bstInsert(root->left, data);
    } else if (data > root->data) {
        root->right = bstInsert(root->right, data);
    }
    // If data == root->data, we don't insert duplicates (or handle differently)

    return root;
}

// SEARCH in BST
TreeNode* bstSearch(TreeNode* root, int data) {
    // Base case: empty or found
    if (!root || root->data == data) return root;

    // Go left or right based on value
    if (data < root->data) {
        return bstSearch(root->left, data);
    } else {
        return bstSearch(root->right, data);
    }
}

// FIND MINIMUM (leftmost node)
TreeNode* bstFindMin(TreeNode* root) {
    if (!root) return NULL;
    while (root->left) {
        root = root->left;
    }
    return root;
}

// FIND MAXIMUM (rightmost node)
TreeNode* bstFindMax(TreeNode* root) {
    if (!root) return NULL;
    while (root->right) {
        root = root->right;
    }
    return root;
}

/* ============================================================================
 * 5. VALIDATE BST
 * Time: O(n), Space: O(h)
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * Check if a binary tree is a valid BST.
 *
 * COMMON MISTAKE:
 * Just checking node->left < node < node->right is NOT enough!
 *
 *        [10]
 *       /    \
 *     [5]    [15]
 *            /
 *          [6]     <- INVALID! 6 < 10 but it's in right subtree!
 *
 * CORRECT APPROACH:
 * Track valid range for each node.
 * Left subtree: all values must be < parent
 * Right subtree: all values must be > parent
 * ============================================================================ */

bool validateBSTHelper(TreeNode* node, long min, long max) {
    if (!node) return true;

    // Check if current node violates range
    if (node->data <= min || node->data >= max) {
        return false;
    }

    // Left subtree: max is current node's value
    // Right subtree: min is current node's value
    return validateBSTHelper(node->left, min, node->data) &&
           validateBSTHelper(node->right, node->data, max);
}

bool isValidBST(TreeNode* root) {
    return validateBSTHelper(root, LONG_MIN, LONG_MAX);
}

/* ============================================================================
 * 6. LOWEST COMMON ANCESTOR (LCA)
 * Time: O(n), Space: O(h)
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * LCA of nodes p and q is the deepest node that is ancestor of both.
 *
 *        [3]
 *       /   \
 *     [5]   [1]
 *    /  \  /   \
 *  [6] [2][0] [8]
 *      / \
 *    [7] [4]
 *
 *   LCA(5, 1) = 3  (both are children of 3)
 *   LCA(5, 4) = 5  (5 is ancestor of 4)
 *   LCA(6, 4) = 5  (5 is ancestor of both)
 *
 * APPROACH:
 * Recursively search left and right subtrees.
 * If we find p in left and q in right (or vice versa), current node is LCA.
 * If both found on same side, LCA is in that subtree.
 * ============================================================================ */

TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
    if (!root || root == p || root == q) return root;

    TreeNode* left = lowestCommonAncestor(root->left, p, q);
    TreeNode* right = lowestCommonAncestor(root->right, p, q);

    // If both sides return non-null, current node is LCA
    if (left && right) return root;

    // Otherwise, LCA is in whichever side found something
    return left ? left : right;
}

/* LCA for BST is simpler - use BST property */
TreeNode* lcaBST(TreeNode* root, int p, int q) {
    if (!root) return NULL;

    // If both smaller, go left
    if (p < root->data && q < root->data) {
        return lcaBST(root->left, p, q);
    }

    // If both larger, go right
    if (p > root->data && q > root->data) {
        return lcaBST(root->right, p, q);
    }

    // Split point - this is the LCA
    return root;
}

/* ============================================================================
 * 7. CHECK IF BALANCED
 * Time: O(n), Space: O(h)
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * A tree is "height-balanced" if for every node, the heights of left and
 * right subtrees differ by at most 1.
 *
 * Balanced:          Unbalanced:
 *      [1]               [1]
 *     /   \             /
 *   [2]   [3]         [2]
 *   / \              /
 * [4] [5]          [3]
 *
 * WHY IT MATTERS:
 * Balanced trees guarantee O(log n) operations.
 * Unbalanced trees can degrade to O(n) (like a linked list).
 * ============================================================================ */

int checkBalanceHelper(TreeNode* root, bool* isBalanced) {
    if (!root) return 0;

    int leftHeight = checkBalanceHelper(root->left, isBalanced);
    int rightHeight = checkBalanceHelper(root->right, isBalanced);

    if (abs(leftHeight - rightHeight) > 1) {
        *isBalanced = false;
    }

    return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}

bool isBalanced(TreeNode* root) {
    bool balanced = true;
    checkBalanceHelper(root, &balanced);
    return balanced;
}

/* ============================================================================
 * 8. HEAP (Priority Queue)
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * A HEAP is a complete binary tree where:
 *   - MIN HEAP: Parent is smaller than or equal to children
 *   - MAX HEAP: Parent is larger than or equal to children
 *
 * MIN HEAP example:
 *          [1]
 *         /   \
 *       [3]   [2]
 *       / \
 *     [6] [5]
 *
 * Root always has min (or max) value!
 *
 * WHY HEAPS IN EMBEDDED?
 *   - Task scheduling (priority queues)
 *   - Event-driven systems
 *   - Timer management
 *   - O(1) to get highest priority item
 *   - O(log n) to add/remove items
 *
 * ARRAY REPRESENTATION:
 * Heaps are typically stored as arrays for efficiency:
 *   - Parent of i: (i-1)/2
 *   - Left child of i: 2*i + 1
 *   - Right child of i: 2*i + 2
 *
 *   Array: [1, 3, 2, 6, 5]
 *   Index:  0  1  2  3  4
 *
 *              [1]         index 0
 *             /   \
 *           [3]   [2]      index 1, 2
 *           / \
 *         [6] [5]          index 3, 4
 * ============================================================================ */

#define MAX_HEAP_SIZE 100

typedef struct {
    int data[MAX_HEAP_SIZE];
    int size;
} MinHeap;

void heap_init(MinHeap* heap) {
    heap->size = 0;
}

void heap_swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Move element UP to maintain heap property (after insert)
void heap_bubbleUp(MinHeap* heap, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (heap->data[parent] > heap->data[index]) {
            heap_swap(&heap->data[parent], &heap->data[index]);
            index = parent;
        } else {
            break;
        }
    }
}

// Move element DOWN to maintain heap property (after remove)
void heap_bubbleDown(MinHeap* heap, int index) {
    while (true) {
        int smallest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < heap->size && heap->data[left] < heap->data[smallest]) {
            smallest = left;
        }
        if (right < heap->size && heap->data[right] < heap->data[smallest]) {
            smallest = right;
        }

        if (smallest != index) {
            heap_swap(&heap->data[index], &heap->data[smallest]);
            index = smallest;
        } else {
            break;
        }
    }
}

// INSERT: Add element and bubble up - O(log n)
bool heap_insert(MinHeap* heap, int data) {
    if (heap->size >= MAX_HEAP_SIZE) return false;

    heap->data[heap->size] = data;
    heap_bubbleUp(heap, heap->size);
    heap->size++;
    return true;
}

// PEEK: Get minimum without removing - O(1)
int heap_peek(MinHeap* heap) {
    if (heap->size == 0) return INT_MAX;
    return heap->data[0];
}

// EXTRACT MIN: Remove and return minimum - O(log n)
int heap_extractMin(MinHeap* heap) {
    if (heap->size == 0) return INT_MAX;

    int min = heap->data[0];
    heap->data[0] = heap->data[heap->size - 1];
    heap->size--;
    heap_bubbleDown(heap, 0);

    return min;
}

void heap_print(MinHeap* heap) {
    printf("Heap: [");
    for (int i = 0; i < heap->size; i++) {
        printf("%d", heap->data[i]);
        if (i < heap->size - 1) printf(", ");
    }
    printf("] (min = %d)\n", heap->size > 0 ? heap->data[0] : -1);
}

/* ============================================================================
 * DEMO FUNCTIONS
 * ============================================================================ */

void demo_traversals(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|                    TREE TRAVERSALS                                 |\n");
    printf("+====================================================================+\n");

    /*
     * Build this tree:
     *        [1]
     *       /   \
     *     [2]   [3]
     *    /   \
     *  [4]   [5]
     */
    TreeNode* root = createTreeNode(1);
    root->left = createTreeNode(2);
    root->right = createTreeNode(3);
    root->left->left = createTreeNode(4);
    root->left->right = createTreeNode(5);

    printf("\n       [1]      \n");
    printf("      /   \\    \n");
    printf("    [2]   [3]  \n");
    printf("    / \\       \n");
    printf("  [4] [5]      \n\n");

    printf("Inorder (L, Root, R):   ");
    inorderTraversal(root);
    printf("\n");

    printf("Preorder (Root, L, R):  ");
    preorderTraversal(root);
    printf("\n");

    printf("Postorder (L, R, Root): ");
    postorderTraversal(root);
    printf("\n");

    printf("Level Order (BFS):      ");
    levelOrderTraversal(root);
    printf("\n");

    printf("\n[MEMORY TRICK]\n");
    printf("  INorder   = print IN the middle\n");
    printf("  PREorder  = print at the PRE (beginning)\n");
    printf("  POSTorder = print at the POST (end)\n");

    freeTree(root);
}

void demo_bst(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|                BINARY SEARCH TREE (BST)                            |\n");
    printf("+====================================================================+\n");

    TreeNode* root = NULL;

    printf("\n[DEMO] Inserting: 10, 5, 15, 3, 7, 12, 20\n");
    int values[] = {10, 5, 15, 3, 7, 12, 20};
    for (int i = 0; i < 7; i++) {
        root = bstInsert(root, values[i]);
    }

    printf("\n       [10]       \n");
    printf("      /    \\     \n");
    printf("    [5]    [15]   \n");
    printf("    / \\   /   \\ \n");
    printf("  [3] [7][12] [20]\n\n");

    printf("Inorder traversal (sorted!): ");
    inorderTraversal(root);
    printf("\n");

    printf("\n[DEMO] Search operations\n");
    printf("  Search 7:  %s\n", bstSearch(root, 7) ? "FOUND" : "NOT FOUND");
    printf("  Search 99: %s\n", bstSearch(root, 99) ? "FOUND" : "NOT FOUND");

    printf("  Minimum:   %d\n", bstFindMin(root)->data);
    printf("  Maximum:   %d\n", bstFindMax(root)->data);

    printf("\n[DEMO] Validate BST\n");
    printf("  Is valid BST: %s\n", isValidBST(root) ? "YES" : "NO");

    printf("\n[DEMO] Height and balance\n");
    printf("  Tree height: %d\n", maxDepth(root));
    printf("  Is balanced: %s\n", isBalanced(root) ? "YES" : "NO");

    printf("\n[DEMO] Lowest Common Ancestor (BST)\n");
    printf("  LCA(3, 7) = %d (should be 5)\n", lcaBST(root, 3, 7)->data);
    printf("  LCA(3, 20) = %d (should be 10)\n", lcaBST(root, 3, 20)->data);

    freeTree(root);
}

void demo_heap(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|                    MIN HEAP (Priority Queue)                       |\n");
    printf("+====================================================================+\n");

    printf("\n[WHY HEAPS IN EMBEDDED?]\n");
    printf("  - Task scheduler: always run highest priority task\n");
    printf("  - Timer management: get next expiring timer\n");
    printf("  - Event queues: process most urgent event\n");
    printf("  - O(1) peek, O(log n) insert/remove\n");

    MinHeap heap;
    heap_init(&heap);

    printf("\n[DEMO] Inserting: 5, 3, 8, 1, 2\n");
    heap_insert(&heap, 5);
    printf("  Insert 5: ");
    heap_print(&heap);

    heap_insert(&heap, 3);
    printf("  Insert 3: ");
    heap_print(&heap);

    heap_insert(&heap, 8);
    printf("  Insert 8: ");
    heap_print(&heap);

    heap_insert(&heap, 1);
    printf("  Insert 1: ");
    heap_print(&heap);

    heap_insert(&heap, 2);
    printf("  Insert 2: ");
    heap_print(&heap);

    printf("\n[DEMO] Extract minimum (3 times)\n");
    for (int i = 0; i < 3; i++) {
        int min = heap_extractMin(&heap);
        printf("  Extracted %d: ", min);
        heap_print(&heap);
    }

    printf("\n[HEAP ARRAY REPRESENTATION]\n");
    printf("  Parent of index i:      (i-1)/2\n");
    printf("  Left child of index i:  2*i + 1\n");
    printf("  Right child of index i: 2*i + 2\n");
}

void demo_embedded(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|            EMBEDDED SYSTEMS CONSIDERATIONS                         |\n");
    printf("+====================================================================+\n");

    printf("\n[WHEN TO USE TREES IN EMBEDDED]\n");
    printf("  1. Priority queues (heaps) - task scheduling\n");
    printf("  2. Fast lookup - configuration storage (BST)\n");
    printf("  3. Hierarchical data - file systems, menus\n");
    printf("  4. Expression parsing - command interpreters\n");

    printf("\n[MEMORY CONSIDERATIONS]\n");
    printf("  TreeNode size: %zu bytes\n", sizeof(TreeNode));
    printf("  For n nodes: n * sizeof(TreeNode) bytes\n");
    printf("  Consider: static allocation, memory pools\n");

    printf("\n[RECURSION WARNING]\n");
    printf("  Tree operations are often recursive\n");
    printf("  Deep trees = deep recursion = stack overflow!\n");
    printf("  For embedded: consider iterative versions with explicit stack\n");
    printf("  Or limit tree depth based on available stack\n");

    printf("\n[INTERVIEW QUESTIONS]\n");
    printf("  Q: 'When would you use a tree vs a hash table?'\n");
    printf("  A: Tree when you need sorted order or range queries.\n");
    printf("     Hash when you just need fast lookup by key.\n");
    printf("\n");
    printf("  Q: 'How would you implement a priority queue for RTOS?'\n");
    printf("  A: Min-heap. O(1) to get highest priority, O(log n) to insert.\n");
    printf("     Array-based for cache efficiency and no pointer overhead.\n");
}

/* ============================================================================
 * MAIN
 * ============================================================================ */

int main(void) {
    printf("+====================================================================+\n");
    printf("|          TREES - EMBEDDED SYSTEMS INTERVIEW GUIDE                  |\n");
    printf("+====================================================================+\n");

    demo_traversals();
    demo_bst();
    demo_heap();
    demo_embedded();

    printf("\n");
    printf("+====================================================================+\n");
    printf("|                    SUMMARY                                         |\n");
    printf("+====================================================================+\n");
    printf("| Key concepts:                                                      |\n");
    printf("|   - Traversals: inorder (sorted), preorder, postorder, level      |\n");
    printf("|   - BST: left < root < right, O(log n) average                    |\n");
    printf("|   - Heap: O(1) peek min/max, O(log n) insert/remove               |\n");
    printf("|                                                                    |\n");
    printf("| Embedded focus:                                                    |\n");
    printf("|   - Heaps for priority queues (task scheduling)                   |\n");
    printf("|   - Watch recursion depth (stack overflow)                        |\n");
    printf("|   - Array-based heap is cache-friendly                            |\n");
    printf("+====================================================================+\n");

    return 0;
}
