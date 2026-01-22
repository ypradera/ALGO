/*
 * TREE TRAVERSAL PATTERNS
 * Use: DFS, BFS, path problems, tree construction
 * Time: O(n), Space: O(h) for DFS, O(w) for BFS
 */

#include <vector>
#include <queue>
#include <stack>
#include <unordered_map>
#include <climits>
#include <string>
#include <sstream>

using namespace std;

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode* l, TreeNode* r) : val(x), left(l), right(r) {}
};

// === DFS - RECURSIVE ===
void inorderRecursive(TreeNode* root, vector<int>& result) {
    if (!root) return;
    inorderRecursive(root->left, result);
    result.push_back(root->val);
    inorderRecursive(root->right, result);
}

void preorderRecursive(TreeNode* root, vector<int>& result) {
    if (!root) return;
    result.push_back(root->val);
    preorderRecursive(root->left, result);
    preorderRecursive(root->right, result);
}

void postorderRecursive(TreeNode* root, vector<int>& result) {
    if (!root) return;
    postorderRecursive(root->left, result);
    postorderRecursive(root->right, result);
    result.push_back(root->val);
}

// === DFS - ITERATIVE ===
vector<int> inorderIterative(TreeNode* root) {
    vector<int> result;
    stack<TreeNode*> st;
    TreeNode* curr = root;

    while (curr || !st.empty()) {
        while (curr) {
            st.push(curr);
            curr = curr->left;
        }
        curr = st.top();
        st.pop();
        result.push_back(curr->val);
        curr = curr->right;
    }

    return result;
}

vector<int> preorderIterative(TreeNode* root) {
    if (!root) return {};

    vector<int> result;
    stack<TreeNode*> st;
    st.push(root);

    while (!st.empty()) {
        TreeNode* node = st.top();
        st.pop();
        result.push_back(node->val);
        if (node->right) st.push(node->right);
        if (node->left) st.push(node->left);
    }

    return result;
}

vector<int> postorderIterative(TreeNode* root) {
    if (!root) return {};

    vector<int> result;
    stack<TreeNode*> st;
    st.push(root);

    while (!st.empty()) {
        TreeNode* node = st.top();
        st.pop();
        result.push_back(node->val);
        if (node->left) st.push(node->left);
        if (node->right) st.push(node->right);
    }

    reverse(result.begin(), result.end());
    return result;
}

// === MORRIS TRAVERSAL (O(1) space) ===
vector<int> morrisInorder(TreeNode* root) {
    vector<int> result;
    TreeNode* curr = root;

    while (curr) {
        if (!curr->left) {
            result.push_back(curr->val);
            curr = curr->right;
        } else {
            TreeNode* pred = curr->left;
            while (pred->right && pred->right != curr) {
                pred = pred->right;
            }

            if (!pred->right) {
                pred->right = curr;
                curr = curr->left;
            } else {
                pred->right = nullptr;
                result.push_back(curr->val);
                curr = curr->right;
            }
        }
    }

    return result;
}

// === BFS - LEVEL ORDER ===
vector<vector<int>> levelOrder(TreeNode* root) {
    if (!root) return {};

    vector<vector<int>> result;
    queue<TreeNode*> q;
    q.push(root);

    while (!q.empty()) {
        vector<int> level;
        int size = q.size();

        for (int i = 0; i < size; i++) {
            TreeNode* node = q.front();
            q.pop();
            level.push_back(node->val);
            if (node->left) q.push(node->left);
            if (node->right) q.push(node->right);
        }

        result.push_back(level);
    }

    return result;
}

vector<vector<int>> zigzagLevelOrder(TreeNode* root) {
    if (!root) return {};

    vector<vector<int>> result;
    queue<TreeNode*> q;
    q.push(root);
    bool leftToRight = true;

    while (!q.empty()) {
        vector<int> level;
        int size = q.size();

        for (int i = 0; i < size; i++) {
            TreeNode* node = q.front();
            q.pop();
            level.push_back(node->val);
            if (node->left) q.push(node->left);
            if (node->right) q.push(node->right);
        }

        if (!leftToRight) {
            reverse(level.begin(), level.end());
        }
        result.push_back(level);
        leftToRight = !leftToRight;
    }

    return result;
}

// === TREE DEPTH ===
int maxDepth(TreeNode* root) {
    if (!root) return 0;
    return 1 + max(maxDepth(root->left), maxDepth(root->right));
}

int minDepth(TreeNode* root) {
    if (!root) return 0;
    if (!root->left) return 1 + minDepth(root->right);
    if (!root->right) return 1 + minDepth(root->left);
    return 1 + min(minDepth(root->left), minDepth(root->right));
}

// === VALIDATE BST ===
bool isValidBST(TreeNode* root, long minVal = LONG_MIN, long maxVal = LONG_MAX) {
    if (!root) return true;
    if (root->val <= minVal || root->val >= maxVal) return false;
    return isValidBST(root->left, minVal, root->val) &&
           isValidBST(root->right, root->val, maxVal);
}

// === LOWEST COMMON ANCESTOR ===
TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
    if (!root || root == p || root == q) return root;

    TreeNode* left = lowestCommonAncestor(root->left, p, q);
    TreeNode* right = lowestCommonAncestor(root->right, p, q);

    if (left && right) return root;
    return left ? left : right;
}

TreeNode* lcaBST(TreeNode* root, TreeNode* p, TreeNode* q) {
    while (root) {
        if (p->val < root->val && q->val < root->val) {
            root = root->left;
        } else if (p->val > root->val && q->val > root->val) {
            root = root->right;
        } else {
            return root;
        }
    }
    return nullptr;
}

// === PATH SUM ===
bool hasPathSum(TreeNode* root, int target) {
    if (!root) return false;
    if (!root->left && !root->right) return root->val == target;
    return hasPathSum(root->left, target - root->val) ||
           hasPathSum(root->right, target - root->val);
}

void pathSumHelper(TreeNode* root, int remaining, vector<int>& path, vector<vector<int>>& result) {
    if (!root) return;
    path.push_back(root->val);
    if (!root->left && !root->right && remaining == root->val) {
        result.push_back(path);
    }
    pathSumHelper(root->left, remaining - root->val, path, result);
    pathSumHelper(root->right, remaining - root->val, path, result);
    path.pop_back();
}

vector<vector<int>> pathSumAll(TreeNode* root, int target) {
    vector<vector<int>> result;
    vector<int> path;
    pathSumHelper(root, target, path, result);
    return result;
}

// === DIAMETER OF TREE ===
int diameterHelper(TreeNode* root, int& maxDiameter) {
    if (!root) return 0;
    int left = diameterHelper(root->left, maxDiameter);
    int right = diameterHelper(root->right, maxDiameter);
    maxDiameter = max(maxDiameter, left + right);
    return 1 + max(left, right);
}

int diameter(TreeNode* root) {
    int maxDiameter = 0;
    diameterHelper(root, maxDiameter);
    return maxDiameter;
}

// === INVERT TREE ===
TreeNode* invertTree(TreeNode* root) {
    if (!root) return nullptr;
    swap(root->left, root->right);
    invertTree(root->left);
    invertTree(root->right);
    return root;
}

// === FLATTEN TO LINKED LIST ===
void flatten(TreeNode* root) {
    TreeNode* curr = root;
    while (curr) {
        if (curr->left) {
            TreeNode* pred = curr->left;
            while (pred->right) {
                pred = pred->right;
            }
            pred->right = curr->right;
            curr->right = curr->left;
            curr->left = nullptr;
        }
        curr = curr->right;
    }
}

// === SERIALIZE / DESERIALIZE ===
string serialize(TreeNode* root) {
    if (!root) return "null,";
    return to_string(root->val) + "," +
           serialize(root->left) +
           serialize(root->right);
}

TreeNode* deserializeHelper(istringstream& ss) {
    string val;
    getline(ss, val, ',');
    if (val == "null") return nullptr;
    TreeNode* node = new TreeNode(stoi(val));
    node->left = deserializeHelper(ss);
    node->right = deserializeHelper(ss);
    return node;
}

TreeNode* deserialize(string data) {
    istringstream ss(data);
    return deserializeHelper(ss);
}

// === CONSTRUCT FROM TRAVERSALS ===
TreeNode* buildFromPreorderInorder(vector<int>& preorder, vector<int>& inorder) {
    unordered_map<int, int> inorderIdx;
    for (int i = 0; i < inorder.size(); i++) {
        inorderIdx[inorder[i]] = i;
    }

    int preIdx = 0;

    function<TreeNode*(int, int)> build = [&](int left, int right) -> TreeNode* {
        if (left > right) return nullptr;
        int val = preorder[preIdx++];
        TreeNode* node = new TreeNode(val);
        int mid = inorderIdx[val];
        node->left = build(left, mid - 1);
        node->right = build(mid + 1, right);
        return node;
    };

    return build(0, inorder.size() - 1);
}
