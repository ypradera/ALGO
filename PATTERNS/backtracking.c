/*
 * BACKTRACKING PATTERNS
 * Use: Permutations, combinations, subsets, constraint satisfaction
 * Embedded focus: static arrays, careful recursion depth
 */

#include <string.h>

#define MAX_N 20

// === SUBSETS (Power Set) ===
int subsets_result[1 << MAX_N][MAX_N];
int subsets_sizes[1 << MAX_N];
int subsets_count;

void generate_subsets(int *nums, int n, int *current, int curr_size, int start) {
    // store current subset
    memcpy(subsets_result[subsets_count], current, curr_size * sizeof(int));
    subsets_sizes[subsets_count] = curr_size;
    subsets_count++;

    for (int i = start; i < n; i++) {
        current[curr_size] = nums[i];
        generate_subsets(nums, n, current, curr_size + 1, i + 1);
    }
}

void subsets(int *nums, int n) {
    int current[MAX_N];
    subsets_count = 0;
    generate_subsets(nums, n, current, 0, 0);
}

// === SUBSETS (Iterative with Bitmask) ===
void subsets_iterative(int *nums, int n) {
    subsets_count = 0;

    for (int mask = 0; mask < (1 << n); mask++) {
        int size = 0;
        for (int i = 0; i < n; i++) {
            if (mask & (1 << i)) {
                subsets_result[subsets_count][size++] = nums[i];
            }
        }
        subsets_sizes[subsets_count] = size;
        subsets_count++;
    }
}

// === PERMUTATIONS ===
int perm_result[MAX_N * MAX_N][MAX_N];  // n! permutations
int perm_count;

void swap(int *a, int *b) {
    int tmp = *a; *a = *b; *b = tmp;
}

void generate_permutations(int *nums, int n, int start) {
    if (start == n) {
        memcpy(perm_result[perm_count++], nums, n * sizeof(int));
        return;
    }

    for (int i = start; i < n; i++) {
        swap(&nums[start], &nums[i]);
        generate_permutations(nums, n, start + 1);
        swap(&nums[start], &nums[i]);  // backtrack
    }
}

void permutations(int *nums, int n) {
    perm_count = 0;
    generate_permutations(nums, n, 0);
}

// === COMBINATIONS (n choose k) ===
int comb_result[MAX_N * MAX_N][MAX_N];
int comb_count;

void generate_combinations(int n, int k, int *current, int curr_size, int start) {
    if (curr_size == k) {
        memcpy(comb_result[comb_count++], current, k * sizeof(int));
        return;
    }

    // pruning: need (k - curr_size) more elements
    for (int i = start; i <= n - (k - curr_size) + 1; i++) {
        current[curr_size] = i;
        generate_combinations(n, k, current, curr_size + 1, i + 1);
    }
}

void combinations(int n, int k) {
    int current[MAX_N];
    comb_count = 0;
    generate_combinations(n, k, current, 0, 1);
}

// === N-QUEENS ===
int queens_solutions[100][MAX_N];  // solutions[i][row] = column
int queens_count;
int cols[MAX_N];       // columns used
int diag1[2 * MAX_N];  // main diagonals (row - col + n)
int diag2[2 * MAX_N];  // anti diagonals (row + col)

void solve_n_queens(int n, int row, int *placement) {
    if (row == n) {
        memcpy(queens_solutions[queens_count++], placement, n * sizeof(int));
        return;
    }

    for (int col = 0; col < n; col++) {
        int d1 = row - col + n;
        int d2 = row + col;

        if (!cols[col] && !diag1[d1] && !diag2[d2]) {
            placement[row] = col;
            cols[col] = diag1[d1] = diag2[d2] = 1;

            solve_n_queens(n, row + 1, placement);

            cols[col] = diag1[d1] = diag2[d2] = 0;  // backtrack
        }
    }
}

void n_queens(int n) {
    int placement[MAX_N];
    queens_count = 0;
    memset(cols, 0, sizeof(cols));
    memset(diag1, 0, sizeof(diag1));
    memset(diag2, 0, sizeof(diag2));
    solve_n_queens(n, 0, placement);
}

// === WORD SEARCH IN GRID ===
int word_search_rows, word_search_cols;
char word_grid[MAX_N][MAX_N];
int word_visited[MAX_N][MAX_N];
int dr[] = {-1, 1, 0, 0};
int dc[] = {0, 0, -1, 1};

int word_search_dfs(int r, int c, const char *word, int idx) {
    if (word[idx] == '\0') return 1;  // found

    if (r < 0 || r >= word_search_rows ||
        c < 0 || c >= word_search_cols ||
        word_visited[r][c] ||
        word_grid[r][c] != word[idx]) {
        return 0;
    }

    word_visited[r][c] = 1;

    for (int d = 0; d < 4; d++) {
        if (word_search_dfs(r + dr[d], c + dc[d], word, idx + 1)) {
            word_visited[r][c] = 0;
            return 1;
        }
    }

    word_visited[r][c] = 0;  // backtrack
    return 0;
}

int word_search(const char *word) {
    memset(word_visited, 0, sizeof(word_visited));

    for (int r = 0; r < word_search_rows; r++) {
        for (int c = 0; c < word_search_cols; c++) {
            if (word_search_dfs(r, c, word, 0)) {
                return 1;
            }
        }
    }
    return 0;
}

// === SUDOKU SOLVER ===
int sudoku_board[9][9];

int is_valid_sudoku(int row, int col, int num) {
    // check row and column
    for (int i = 0; i < 9; i++) {
        if (sudoku_board[row][i] == num) return 0;
        if (sudoku_board[i][col] == num) return 0;
    }

    // check 3x3 box
    int box_r = (row / 3) * 3;
    int box_c = (col / 3) * 3;
    for (int r = box_r; r < box_r + 3; r++) {
        for (int c = box_c; c < box_c + 3; c++) {
            if (sudoku_board[r][c] == num) return 0;
        }
    }

    return 1;
}

int solve_sudoku(void) {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (sudoku_board[r][c] == 0) {
                for (int num = 1; num <= 9; num++) {
                    if (is_valid_sudoku(r, c, num)) {
                        sudoku_board[r][c] = num;
                        if (solve_sudoku()) return 1;
                        sudoku_board[r][c] = 0;  // backtrack
                    }
                }
                return 0;  // no valid number
            }
        }
    }
    return 1;  // solved
}
