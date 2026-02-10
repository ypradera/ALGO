/*
 * STACK & QUEUE PATTERNS
 * Use: Monotonic stack, next greater element, sliding window max
 * Time: O(n), Space: O(n)
 */

#include <vector>
#include <stack>
#include <queue>
#include <deque>
#include <string>
#include <unordered_map>

using namespace std;

// === MONOTONIC STACK (next greater element) ===
vector<int> nextGreaterElement(vector<int>& arr) {
    int n = arr.size();
    vector<int> result(n, -1);
    stack<int> st;  // stores indices

    for (int i = 0; i < n; i++) {
        while (!st.empty() && arr[st.top()] < arr[i]) {
            result[st.top()] = arr[i];
            st.pop();
        }
        st.push(i);
    }

    return result;
}

vector<int> nextSmallerElement(vector<int>& arr) {
    int n = arr.size();
    vector<int> result(n, -1);
    stack<int> st;

    for (int i = 0; i < n; i++) {
        while (!st.empty() && arr[st.top()] > arr[i]) {
            result[st.top()] = arr[i];
            st.pop();
        }
        st.push(i);
    }

    return result;
}

vector<int> previousGreaterElement(vector<int>& arr) {
    int n = arr.size();
    vector<int> result(n, -1);
    stack<int> st;

    for (int i = 0; i < n; i++) {
        while (!st.empty() && arr[st.top()] <= arr[i]) {
            st.pop();
        }
        if (!st.empty()) {
            result[i] = arr[st.top()];
        }
        st.push(i);
    }

    return result;
}

// === DAILY TEMPERATURES ===
vector<int> dailyTemperatures(vector<int>& temps) {
    int n = temps.size();
    vector<int> result(n, 0);
    stack<int> st;

    for (int i = 0; i < n; i++) {
        while (!st.empty() && temps[st.top()] < temps[i]) {
            int idx = st.top();
            st.pop();
            result[idx] = i - idx;
        }
        st.push(i);
    }

    return result;
}

// === LARGEST RECTANGLE IN HISTOGRAM ===
int largestRectangleHistogram(vector<int>& heights) {
    stack<int> st;
    int maxArea = 0;
    heights.push_back(0);  // sentinel
    int n = heights.size();

    for (int i = 0; i < n; i++) {
        while (!st.empty() && heights[st.top()] > heights[i]) {
            int height = heights[st.top()];
            st.pop();
            int width = st.empty() ? i : i - st.top() - 1;
            maxArea = max(maxArea, height * width);
        }
        st.push(i);
    }

    heights.pop_back();  // restore
    return maxArea;
}

// === TRAPPING RAIN WATER (using stack) ===
int trapWaterStack(vector<int>& heights) {
    stack<int> st;
    int water = 0;
    int n = heights.size();

    for (int i = 0; i < n; i++) {
        while (!st.empty() && heights[st.top()] < heights[i]) {
            int bottom = heights[st.top()];
            st.pop();
            if (!st.empty()) {
                int width = i - st.top() - 1;
                int height = min(heights[i], heights[st.top()]) - bottom;
                water += width * height;
            }
        }
        st.push(i);
    }

    return water;
}

// === SLIDING WINDOW MAXIMUM (monotonic deque) ===
vector<int> slidingWindowMax(vector<int>& arr, int k) {
    deque<int> dq;  // stores indices
    vector<int> result;
    int n = arr.size();

    for (int i = 0; i < n; i++) {
        while (!dq.empty() && dq.front() <= i - k) {
            dq.pop_front();
        }

        while (!dq.empty() && arr[dq.back()] <= arr[i]) {
            dq.pop_back();
        }

        dq.push_back(i);

        if (i >= k - 1) {
            result.push_back(arr[dq.front()]);
        }
    }

    return result;
}

// === VALID PARENTHESES ===
bool isValidParentheses(string& s) {
    stack<char> st;
    unordered_map<char, char> mapping = {{')', '('}, {'}', '{'}, {']', '['}};

    for (char c : s) {
        if (mapping.count(c)) {
            if (st.empty() || st.top() != mapping[c]) {
                return false;
            }
            st.pop();
        } else {
            st.push(c);
        }
    }

    return st.empty();
}

int longestValidParentheses(string& s) {
    stack<int> st;
    st.push(-1);
    int maxLen = 0;

    for (int i = 0; i < s.size(); i++) {
        if (s[i] == '(') {
            st.push(i);
        } else {
            st.pop();
            if (st.empty()) {
                st.push(i);
            } else {
                maxLen = max(maxLen, i - st.top());
            }
        }
    }

    return maxLen;
}

// === MIN STACK ===
class MinStack {
    stack<pair<int, int>> st;  // (value, min at this point)

public:
    void push(int val) {
        int minVal = st.empty() ? val : min(val, st.top().second);
        st.push({val, minVal});
    }

    void pop() {
        st.pop();
    }

    int top() {
        return st.top().first;
    }

    int getMin() {
        return st.top().second;
    }
};

// === QUEUE USING STACKS ===
class MyQueue {
    stack<int> stackIn, stackOut;

    void transfer() {
        if (stackOut.empty()) {
            while (!stackIn.empty()) {
                stackOut.push(stackIn.top());
                stackIn.pop();
            }
        }
    }

public:
    void push(int x) {
        stackIn.push(x);
    }

    int pop() {
        transfer();
        int val = stackOut.top();
        stackOut.pop();
        return val;
    }

    int peek() {
        transfer();
        return stackOut.top();
    }

    bool empty() {
        return stackIn.empty() && stackOut.empty();
    }
};

// === STACK USING QUEUES ===
class MyStack {
    queue<int> q;

public:
    void push(int x) {
        q.push(x);
        for (int i = 0; i < q.size() - 1; i++) {
            q.push(q.front());
            q.pop();
        }
    }

    int pop() {
        int val = q.front();
        q.pop();
        return val;
    }

    int top() {
        return q.front();
    }

    bool empty() {
        return q.empty();
    }
};

// === EVALUATE REVERSE POLISH NOTATION ===
int evalRPN(vector<string>& tokens) {
    stack<int> st;

    for (string& token : tokens) {
        if (token == "+" || token == "-" || token == "*" || token == "/") {
            int b = st.top(); st.pop();
            int a = st.top(); st.pop();

            if (token == "+") st.push(a + b);
            else if (token == "-") st.push(a - b);
            else if (token == "*") st.push(a * b);
            else st.push(a / b);
        } else {
            st.push(stoi(token));
        }
    }

    return st.top();
}

// === DECODE STRING ===
string decodeString(string& s) {
    stack<pair<string, int>> st;
    string currStr = "";
    int currNum = 0;

    for (char c : s) {
        if (isdigit(c)) {
            currNum = currNum * 10 + (c - '0');
        } else if (c == '[') {
            st.push({currStr, currNum});
            currStr = "";
            currNum = 0;
        } else if (c == ']') {
            auto [prevStr, num] = st.top();
            st.pop();
            string repeated = "";
            for (int i = 0; i < num; i++) {
                repeated += currStr;
            }
            currStr = prevStr + repeated;
        } else {
            currStr += c;
        }
    }

    return currStr;
}

// === BASIC CALCULATOR ===
int basicCalculator(string& s) {
    stack<int> st;
    int result = 0;
    int num = 0;
    int sign = 1;

    for (char c : s) {
        if (isdigit(c)) {
            num = num * 10 + (c - '0');
        } else if (c == '+') {
            result += sign * num;
            num = 0;
            sign = 1;
        } else if (c == '-') {
            result += sign * num;
            num = 0;
            sign = -1;
        } else if (c == '(') {
            st.push(result);
            st.push(sign);
            result = 0;
            sign = 1;
        } else if (c == ')') {
            result += sign * num;
            num = 0;
            result *= st.top(); st.pop();
            result += st.top(); st.pop();
        }
    }

    return result + sign * num;
}
