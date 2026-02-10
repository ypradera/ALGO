/*
 * STATE MACHINE (FSM)
 * Use: Protocol parsing, UI flow, device control, communication handlers
 *
 * Features:
 * - Table-driven FSM
 * - Hierarchical state machine (HSM)
 * - Event-driven transitions
 *
 * Time: O(1) state transitions
 * Space: O(states * events) for transition table
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* ============================================
 * SIMPLE TABLE-DRIVEN FSM
 * ============================================ */

// State and event definitions
typedef enum {
    STATE_IDLE,
    STATE_CONNECTING,
    STATE_CONNECTED,
    STATE_ERROR,
    STATE_COUNT
} State;

typedef enum {
    EVENT_CONNECT,
    EVENT_DISCONNECT,
    EVENT_DATA_RECEIVED,
    EVENT_TIMEOUT,
    EVENT_ERROR,
    EVENT_COUNT
} Event;

// Transition table entry
typedef struct {
    State next_state;
    void (*action)(void);
} Transition;

// Action functions
void action_start_connect(void) { /* Start connection */ }
void action_complete_connect(void) { /* Connection complete */ }
void action_process_data(void) { /* Process received data */ }
void action_disconnect(void) { /* Clean disconnect */ }
void action_handle_error(void) { /* Error handling */ }
void action_retry(void) { /* Retry connection */ }
void action_none(void) { /* No action */ }

// Transition table [current_state][event] -> {next_state, action}
static const Transition transition_table[STATE_COUNT][EVENT_COUNT] = {
    // STATE_IDLE
    {
        {STATE_CONNECTING, action_start_connect},   // CONNECT
        {STATE_IDLE, action_none},                  // DISCONNECT
        {STATE_IDLE, action_none},                  // DATA_RECEIVED
        {STATE_IDLE, action_none},                  // TIMEOUT
        {STATE_ERROR, action_handle_error}          // ERROR
    },
    // STATE_CONNECTING
    {
        {STATE_CONNECTED, action_complete_connect}, // CONNECT (success)
        {STATE_IDLE, action_disconnect},            // DISCONNECT
        {STATE_CONNECTING, action_none},            // DATA_RECEIVED
        {STATE_IDLE, action_retry},                 // TIMEOUT
        {STATE_ERROR, action_handle_error}          // ERROR
    },
    // STATE_CONNECTED
    {
        {STATE_CONNECTED, action_none},             // CONNECT
        {STATE_IDLE, action_disconnect},            // DISCONNECT
        {STATE_CONNECTED, action_process_data},     // DATA_RECEIVED
        {STATE_CONNECTED, action_none},             // TIMEOUT
        {STATE_ERROR, action_handle_error}          // ERROR
    },
    // STATE_ERROR
    {
        {STATE_CONNECTING, action_start_connect},   // CONNECT (retry)
        {STATE_IDLE, action_disconnect},            // DISCONNECT
        {STATE_ERROR, action_none},                 // DATA_RECEIVED
        {STATE_IDLE, action_retry},                 // TIMEOUT
        {STATE_ERROR, action_none}                  // ERROR
    }
};

// FSM context
typedef struct {
    State current_state;
    uint32_t state_entry_time;
    uint32_t event_count;
} FSM;

void fsm_init(FSM *fsm) {
    fsm->current_state = STATE_IDLE;
    fsm->state_entry_time = 0;
    fsm->event_count = 0;
}

void fsm_process_event(FSM *fsm, Event event) {
    if (event >= EVENT_COUNT) return;

    const Transition *trans = &transition_table[fsm->current_state][event];

    // Execute action
    if (trans->action) {
        trans->action();
    }

    // Update state
    if (fsm->current_state != trans->next_state) {
        fsm->current_state = trans->next_state;
        // fsm->state_entry_time = get_system_time();
    }

    fsm->event_count++;
}

State fsm_get_state(FSM *fsm) {
    return fsm->current_state;
}


/* ============================================
 * EVENT-DRIVEN FSM WITH GUARDS
 * ============================================ */

typedef bool (*GuardFunc)(void *context);
typedef void (*ActionFunc)(void *context);

typedef struct {
    Event event;
    State next_state;
    GuardFunc guard;      // NULL = always true
    ActionFunc action;    // NULL = no action
} TransitionRule;

#define MAX_TRANSITIONS_PER_STATE   8

typedef struct {
    State state;
    TransitionRule transitions[MAX_TRANSITIONS_PER_STATE];
    uint8_t num_transitions;
    ActionFunc on_enter;  // called when entering state
    ActionFunc on_exit;   // called when leaving state
} StateConfig;

typedef struct {
    State current_state;
    const StateConfig *states;
    uint8_t num_states;
    void *context;
} EventDrivenFSM;

void ed_fsm_init(EventDrivenFSM *fsm, const StateConfig *states, uint8_t num_states, void *context) {
    fsm->current_state = states[0].state;
    fsm->states = states;
    fsm->num_states = num_states;
    fsm->context = context;

    // Call on_enter for initial state
    if (states[0].on_enter) {
        states[0].on_enter(context);
    }
}

bool ed_fsm_process(EventDrivenFSM *fsm, Event event) {
    // Find current state config
    const StateConfig *current = NULL;
    for (int i = 0; i < fsm->num_states; i++) {
        if (fsm->states[i].state == fsm->current_state) {
            current = &fsm->states[i];
            break;
        }
    }

    if (!current) return false;

    // Find matching transition
    for (int i = 0; i < current->num_transitions; i++) {
        const TransitionRule *rule = &current->transitions[i];

        if (rule->event == event) {
            // Check guard condition
            if (rule->guard && !rule->guard(fsm->context)) {
                continue;  // guard failed, try next rule
            }

            // Execute transition
            if (fsm->current_state != rule->next_state) {
                // Exit current state
                if (current->on_exit) {
                    current->on_exit(fsm->context);
                }

                // Execute action
                if (rule->action) {
                    rule->action(fsm->context);
                }

                // Enter new state
                fsm->current_state = rule->next_state;

                // Find new state config and call on_enter
                for (int j = 0; j < fsm->num_states; j++) {
                    if (fsm->states[j].state == rule->next_state) {
                        if (fsm->states[j].on_enter) {
                            fsm->states[j].on_enter(fsm->context);
                        }
                        break;
                    }
                }
            } else {
                // Same state, just execute action
                if (rule->action) {
                    rule->action(fsm->context);
                }
            }

            return true;
        }
    }

    return false;  // no matching transition
}


/* ============================================
 * HIERARCHICAL STATE MACHINE (HSM)
 * ============================================ */

typedef enum {
    HSM_STATE_TOP,
    HSM_STATE_MAIN,
    HSM_STATE_MAIN_IDLE,
    HSM_STATE_MAIN_ACTIVE,
    HSM_STATE_MAIN_ACTIVE_PROCESSING,
    HSM_STATE_MAIN_ACTIVE_WAITING,
    HSM_STATE_ERROR_HANDLER,
    HSM_STATE_COUNT_MAX
} HSMState;

typedef struct HSMStateHandler {
    HSMState state;
    HSMState parent;
    void (*on_enter)(void *ctx);
    void (*on_exit)(void *ctx);
    HSMState (*on_event)(void *ctx, Event event);
} HSMStateHandler;

typedef struct {
    HSMState current_state;
    const HSMStateHandler *handlers;
    uint8_t num_handlers;
    void *context;
} HSM;

const HSMStateHandler* hsm_find_handler(HSM *hsm, HSMState state) {
    for (int i = 0; i < hsm->num_handlers; i++) {
        if (hsm->handlers[i].state == state) {
            return &hsm->handlers[i];
        }
    }
    return NULL;
}

void hsm_transition(HSM *hsm, HSMState target) {
    // Find common ancestor and execute exit/enter chain
    // (simplified - full implementation would trace paths)

    const HSMStateHandler *current = hsm_find_handler(hsm, hsm->current_state);
    const HSMStateHandler *next = hsm_find_handler(hsm, target);

    if (current && current->on_exit) {
        current->on_exit(hsm->context);
    }

    hsm->current_state = target;

    if (next && next->on_enter) {
        next->on_enter(hsm->context);
    }
}

void hsm_dispatch(HSM *hsm, Event event) {
    HSMState state = hsm->current_state;

    // Walk up hierarchy until event is handled
    while (state != HSM_STATE_TOP) {
        const HSMStateHandler *handler = hsm_find_handler(hsm, state);
        if (!handler) break;

        if (handler->on_event) {
            HSMState result = handler->on_event(hsm->context, event);
            if (result != state) {
                hsm_transition(hsm, result);
                return;
            }
        }

        state = handler->parent;  // try parent state
    }
}


/* ============================================
 * PROTOCOL PARSER FSM (UART frame parsing)
 * ============================================ */

typedef enum {
    PARSE_WAIT_START,
    PARSE_READ_LENGTH,
    PARSE_READ_TYPE,
    PARSE_READ_DATA,
    PARSE_READ_CHECKSUM
} ParseState;

#define FRAME_MAX_SIZE  64
#define FRAME_START     0x7E

typedef struct {
    ParseState state;
    uint8_t buffer[FRAME_MAX_SIZE];
    uint8_t index;
    uint8_t length;
    uint8_t type;
    uint8_t checksum;
} FrameParser;

void frame_parser_init(FrameParser *parser) {
    parser->state = PARSE_WAIT_START;
    parser->index = 0;
    parser->checksum = 0;
}

// Returns true when complete frame is received
bool frame_parser_process(FrameParser *parser, uint8_t byte) {
    switch (parser->state) {
        case PARSE_WAIT_START:
            if (byte == FRAME_START) {
                parser->state = PARSE_READ_LENGTH;
                parser->checksum = byte;
            }
            break;

        case PARSE_READ_LENGTH:
            parser->length = byte;
            parser->checksum ^= byte;
            if (byte > 0 && byte <= FRAME_MAX_SIZE) {
                parser->state = PARSE_READ_TYPE;
            } else {
                parser->state = PARSE_WAIT_START;  // invalid length
            }
            break;

        case PARSE_READ_TYPE:
            parser->type = byte;
            parser->checksum ^= byte;
            parser->index = 0;
            if (parser->length > 1) {
                parser->state = PARSE_READ_DATA;
            } else {
                parser->state = PARSE_READ_CHECKSUM;
            }
            break;

        case PARSE_READ_DATA:
            parser->buffer[parser->index++] = byte;
            parser->checksum ^= byte;
            if (parser->index >= parser->length - 1) {
                parser->state = PARSE_READ_CHECKSUM;
            }
            break;

        case PARSE_READ_CHECKSUM:
            parser->state = PARSE_WAIT_START;
            if (parser->checksum == byte) {
                return true;  // valid frame
            }
            break;
    }

    return false;
}
