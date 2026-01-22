/*
 * TREE TRAVERSAL PATTERNS
 * Use: Binary tree DFS/BFS, path problems, serialization
 * Embedded focus: iterative solutions, static allocation
 */

#include <stddef.h>
#include <limits.h>

#define MAX_NODES 1000

typedef struct TreeNode {
    int val;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

// === RECURSIVE DFS ===
void inorder_recursive(TreeNode *root, int *result, int *idx) {
    if (!root) return;
    inorder_recursive(root->left, result, idx);
    result[(*idx)++] = root->val;
    inorder_recursive(root->right, result, idx);
}

void preorder_recursive(TreeNode *root, int *result, int *idx) {
    if (!root) return;
    result[(*idx)++] = root->val;
    preorder_recursive(root->left, result, idx);
    preorder_recursive(root->right, result, idx);
}

void postorder_recursive(TreeNode *root, int *result, int *idx) {
    if (!root) return;
    postorder_recursive(root->left, result, idx);
    postorder_recursive(root->right, result, idx);
    result[(*idx)++] = root->val;
}

// === ITERATIVE INORDER (Morris Traversal - O(1) space) ===
void morris_inorder(TreeNode *root, int *result, int *idx) {
    TreeNode *curr = root;

    while (curr) {
        if (!curr->left) {
            result[(*idx)++] = curr->val;
            curr = curr->right;
        } else {
            // find inorder predecessor
            TreeNode *pred = curr->left;
            while (pred->right && pred->right != curr) {
                pred = pred->right;
            }

            if (!pred->right) {
                // make curr the right child of predecessor
                pred->right = curr;
                curr = curr->left;
            } else {
                // revert the link
                pred->right = NULL;
                result[(*idx)++] = curr->val;
                curr = curr->right;
            }
        }
    }
}

// === ITERATIVE WITH STACK ===
void inorder_iterative(TreeNode *root, int *result, int *idx) {
    TreeNode *stack[MAX_NODES];
    int top = -1;
    TreeNode *curr = root;

    while (curr || top >= 0) {
        while (curr) {
            stack[++top] = curr;
            curr = curr->left;
        }
        curr = stack[top--];
        result[(*idx)++] = curr->val;
        curr = curr->right;
    }
}

// === BFS LEVEL ORDER ===
void level_order(TreeNode *root, int result[][MAX_NODES], int *level_sizes, int *num_levels) {
    if (!root) { *num_levels = 0; return; }

    TreeNode *queue[MAX_NODES];
    int front = 0, back = 0;

    queue[back++] = root;
    *num_levels = 0;

    while (front < back) {
        int level_len = back - front;
        level_sizes[*num_levels] = level_len;

        for (int i = 0; i < level_len; i++) {
            TreeNode *node = queue[front++];
            result[*num_levels][i] = node->val;

            if (node->left) queue[back++] = node->left;
            if (node->right) queue[back++] = node->right;
        }
        (*num_levels)++;
    }
}

// === MAX DEPTH ===
int max_depth(TreeNode *root) {
    if (!root) return 0;
    int left = max_depth(root->left);
    int right = max_depth(root->right);
    return 1 + (left > right ? left : right);
}

// === PATH SUM ===
int has_path_sum(TreeNode *root, int target) {
    if (!root) return 0;

    target -= root->val;

    // leaf node check
    if (!root->left && !root->right) {
        return target == 0;
    }

    return has_path_sum(root->left, target) ||
           has_path_sum(root->right, target);
}

// === LOWEST COMMON ANCESTOR ===
TreeNode* lca(TreeNode *root, TreeNode *p, TreeNode *q) {
    if (!root || root == p || root == q) return root;

    TreeNode *left = lca(root->left, p, q);
    TreeNode *right = lca(root->right, p, q);

    if (left && right) return root;
    return left ? left : right;
}

// === VALIDATE BST ===
int is_valid_bst_helper(TreeNode *root, long min_val, long max_val) {
    if (!root) return 1;

    if (root->val <= min_val || root->val >= max_val) {
        return 0;
    }

    return is_valid_bst_helper(root->left, min_val, root->val) &&
           is_valid_bst_helper(root->right, root->val, max_val);
}

int is_valid_bst(TreeNode *root) {
    return is_valid_bst_helper(root, LONG_MIN, LONG_MAX);
}

// === SERIALIZE/DESERIALIZE (preorder with markers) ===
void serialize(TreeNode *root, int *data, int *idx) {
    if (!root) {
        data[(*idx)++] = INT_MIN;  // null marker
        return;
    }
    data[(*idx)++] = root->val;
    serialize(root->left, data, idx);
    serialize(root->right, data, idx);
}
