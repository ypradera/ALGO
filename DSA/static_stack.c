/*
 * STATIC STACK
 * Use: Expression parsing, undo operations, call stack simulation, DFS
 *
 * Features:
 * - Fixed-size, no dynamic allocation
 * - LIFO (Last-In-First-Out)
 *
 * Time: O(1) all operations
 * Space: O(n) stack size
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* ============================================
 * BASIC STATIC STACK (integers)
 * ============================================ */

#define STACK_SIZE      64

typedef struct {
    int32_t data[STACK_SIZE];
    int16_t top;  // -1 when empty
} StaticStack;

void stack_init(StaticStack *s) {
    s->top = -1;
}

bool stack_is_empty(StaticStack *s) {
    return s->top < 0;
}

bool stack_is_full(StaticStack *s) {
    return s->top >= STACK_SIZE - 1;
}

uint16_t stack_count(StaticStack *s) {
    return s->top + 1;
}

bool stack_push(StaticStack *s, int32_t value) {
    if (stack_is_full(s)) {
        return false;
    }

    s->data[++s->top] = value;
    return true;
}

bool stack_pop(StaticStack *s, int32_t *value) {
    if (stack_is_empty(s)) {
        return false;
    }

    *value = s->data[s->top--];
    return true;
}

bool stack_peek(StaticStack *s, int32_t *value) {
    if (stack_is_empty(s)) {
        return false;
    }

    *value = s->data[s->top];
    return true;
}

void stack_clear(StaticStack *s) {
    s->top = -1;
}


/* ============================================
 * BYTE STACK (for parsing)
 * ============================================ */

#define BYTE_STACK_SIZE     128

typedef struct {
    uint8_t data[BYTE_STACK_SIZE];
    int16_t top;
} ByteStack;

void byte_stack_init(ByteStack *s) {
    s->top = -1;
}

bool byte_stack_push(ByteStack *s, uint8_t value) {
    if (s->top >= BYTE_STACK_SIZE - 1) return false;
    s->data[++s->top] = value;
    return true;
}

bool byte_stack_pop(ByteStack *s, uint8_t *value) {
    if (s->top < 0) return false;
    *value = s->data[s->top--];
    return true;
}

bool byte_stack_peek(ByteStack *s, uint8_t *value) {
    if (s->top < 0) return false;
    *value = s->data[s->top];
    return true;
}


/* ============================================
 * POINTER STACK (for callback chains)
 * ============================================ */

#define PTR_STACK_SIZE      16

typedef void (*FunctionPtr)(void);

typedef struct {
    FunctionPtr callbacks[PTR_STACK_SIZE];
    int8_t top;
} CallbackStack;

void callback_stack_init(CallbackStack *s) {
    s->top = -1;
}

bool callback_stack_push(CallbackStack *s, FunctionPtr func) {
    if (s->top >= PTR_STACK_SIZE - 1) return false;
    s->callbacks[++s->top] = func;
    return true;
}

bool callback_stack_pop(CallbackStack *s, FunctionPtr *func) {
    if (s->top < 0) return false;
    *func = s->callbacks[s->top--];
    return true;
}

// Execute all callbacks and clear stack
void callback_stack_execute_all(CallbackStack *s) {
    while (s->top >= 0) {
        FunctionPtr func = s->callbacks[s->top--];
        if (func) func();
    }
}


/* ============================================
 * MIN/MAX STACK (O(1) min/max query)
 * ============================================ */

typedef struct {
    int32_t value;
    int32_t min;
    int32_t max;
} MinMaxEntry;

typedef struct {
    MinMaxEntry data[STACK_SIZE];
    int16_t top;
} MinMaxStack;

void minmax_stack_init(MinMaxStack *s) {
    s->top = -1;
}

bool minmax_stack_push(MinMaxStack *s, int32_t value) {
    if (s->top >= STACK_SIZE - 1) return false;

    s->top++;
    s->data[s->top].value = value;

    if (s->top == 0) {
        s->data[s->top].min = value;
        s->data[s->top].max = value;
    } else {
        int32_t prev_min = s->data[s->top - 1].min;
        int32_t prev_max = s->data[s->top - 1].max;
        s->data[s->top].min = (value < prev_min) ? value : prev_min;
        s->data[s->top].max = (value > prev_max) ? value : prev_max;
    }

    return true;
}

bool minmax_stack_pop(MinMaxStack *s, int32_t *value) {
    if (s->top < 0) return false;
    *value = s->data[s->top--].value;
    return true;
}

bool minmax_stack_get_min(MinMaxStack *s, int32_t *min) {
    if (s->top < 0) return false;
    *min = s->data[s->top].min;
    return true;
}

bool minmax_stack_get_max(MinMaxStack *s, int32_t *max) {
    if (s->top < 0) return false;
    *max = s->data[s->top].max;
    return true;
}


/* ============================================
 * UNDO STACK (for state management)
 * ============================================ */

#define UNDO_STACK_SIZE     32
#define STATE_SIZE          64

typedef struct {
    uint8_t state[STATE_SIZE];
} StateSnapshot;

typedef struct {
    StateSnapshot snapshots[UNDO_STACK_SIZE];
    int8_t top;
} UndoStack;

void undo_stack_init(UndoStack *s) {
    s->top = -1;
}

bool undo_stack_save(UndoStack *s, const void *state, uint16_t size) {
    if (s->top >= UNDO_STACK_SIZE - 1 || size > STATE_SIZE) {
        return false;
    }

    s->top++;
    memcpy(s->snapshots[s->top].state, state, size);
    return true;
}

bool undo_stack_restore(UndoStack *s, void *state, uint16_t size) {
    if (s->top < 0 || size > STATE_SIZE) {
        return false;
    }

    memcpy(state, s->snapshots[s->top--].state, size);
    return true;
}

bool undo_stack_peek(UndoStack *s, void *state, uint16_t size) {
    if (s->top < 0 || size > STATE_SIZE) {
        return false;
    }

    memcpy(state, s->snapshots[s->top].state, size);
    return true;
}


/* ============================================
 * EXPRESSION EVALUATOR (postfix)
 * ============================================ */

int32_t evaluate_postfix(const char *expr) {
    StaticStack stack;
    stack_init(&stack);

    for (int i = 0; expr[i] != '\0'; i++) {
        char c = expr[i];

        if (c >= '0' && c <= '9') {
            // Parse number
            int32_t num = 0;
            while (expr[i] >= '0' && expr[i] <= '9') {
                num = num * 10 + (expr[i] - '0');
                i++;
            }
            i--;  // adjust for loop increment
            stack_push(&stack, num);
        }
        else if (c == '+' || c == '-' || c == '*' || c == '/') {
            int32_t b, a;
            stack_pop(&stack, &b);
            stack_pop(&stack, &a);

            int32_t result;
            switch (c) {
                case '+': result = a + b; break;
                case '-': result = a - b; break;
                case '*': result = a * b; break;
                case '/': result = (b != 0) ? a / b : 0; break;
                default: result = 0;
            }

            stack_push(&stack, result);
        }
        // Skip spaces
    }

    int32_t result;
    stack_pop(&stack, &result);
    return result;
}


/* ============================================
 * BRACKET MATCHING
 * ============================================ */

bool is_brackets_balanced(const char *str) {
    ByteStack stack;
    byte_stack_init(&stack);

    for (int i = 0; str[i] != '\0'; i++) {
        char c = str[i];

        if (c == '(' || c == '[' || c == '{') {
            byte_stack_push(&stack, (uint8_t)c);
        }
        else if (c == ')' || c == ']' || c == '}') {
            if (stack.top < 0) return false;

            uint8_t top;
            byte_stack_pop(&stack, &top);

            if ((c == ')' && top != '(') ||
                (c == ']' && top != '[') ||
                (c == '}' && top != '{')) {
                return false;
            }
        }
    }

    return stack.top < 0;
}


/* ============================================
 * GENERIC MACRO-BASED STACK
 * ============================================ */

#define DEFINE_STATIC_STACK(name, type, size) \
    typedef struct { \
        type data[size]; \
        int16_t top; \
    } name##_Stack; \
    \
    static inline void name##_init(name##_Stack *s) { \
        s->top = -1; \
    } \
    \
    static inline bool name##_is_empty(name##_Stack *s) { \
        return s->top < 0; \
    } \
    \
    static inline bool name##_push(name##_Stack *s, type value) { \
        if (s->top >= size - 1) return false; \
        s->data[++s->top] = value; \
        return true; \
    } \
    \
    static inline bool name##_pop(name##_Stack *s, type *value) { \
        if (s->top < 0) return false; \
        *value = s->data[s->top--]; \
        return true; \
    } \
    \
    static inline bool name##_peek(name##_Stack *s, type *value) { \
        if (s->top < 0) return false; \
        *value = s->data[s->top]; \
        return true; \
    }

// Example usage:
// DEFINE_STATIC_STACK(float, float, 32)
// float_Stack my_stack;
// float_init(&my_stack);
