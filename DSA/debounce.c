/*
 * DEBOUNCE ALGORITHMS
 * Use: Button/switch inputs, digital signal conditioning, contact bounce removal
 *
 * Features:
 * - Simple delay debounce
 * - Counter-based debounce
 * - Shift register debounce
 * - Schmitt trigger style
 *
 * Time: O(1) per sample
 * Space: O(1) per input
 */

#include <stdint.h>
#include <stdbool.h>

/* ============================================
 * SIMPLE TIMER-BASED DEBOUNCE
 * ============================================ */

#define DEBOUNCE_DELAY_MS   50

typedef struct {
    bool current_state;
    bool last_raw_state;
    uint32_t last_change_time;
} DebounceSimple;

void debounce_simple_init(DebounceSimple *db) {
    db->current_state = false;
    db->last_raw_state = false;
    db->last_change_time = 0;
}

// Call this periodically with current time in ms
bool debounce_simple_update(DebounceSimple *db, bool raw_input, uint32_t current_time) {
    if (raw_input != db->last_raw_state) {
        db->last_change_time = current_time;
        db->last_raw_state = raw_input;
    }

    if ((current_time - db->last_change_time) >= DEBOUNCE_DELAY_MS) {
        db->current_state = db->last_raw_state;
    }

    return db->current_state;
}


/* ============================================
 * COUNTER-BASED DEBOUNCE
 * Requires N consecutive same readings
 * ============================================ */

#define DEBOUNCE_COUNT_THRESHOLD    10

typedef struct {
    bool current_state;
    uint8_t counter;
} DebounceCounter;

void debounce_counter_init(DebounceCounter *db, bool initial_state) {
    db->current_state = initial_state;
    db->counter = 0;
}

// Call this at fixed interval (e.g., every 1ms)
bool debounce_counter_update(DebounceCounter *db, bool raw_input) {
    if (raw_input == db->current_state) {
        db->counter = 0;
    } else {
        db->counter++;

        if (db->counter >= DEBOUNCE_COUNT_THRESHOLD) {
            db->current_state = raw_input;
            db->counter = 0;
        }
    }

    return db->current_state;
}


/* ============================================
 * SHIFT REGISTER DEBOUNCE (Ganssle method)
 * Very robust, pattern-based
 * ============================================ */

#define SHIFT_DEBOUNCE_MASK     0xFF  // 8 samples
#define SHIFT_DEBOUNCE_ON       0xFF  // All 1s = stable high
#define SHIFT_DEBOUNCE_OFF      0x00  // All 0s = stable low

typedef struct {
    bool current_state;
    uint8_t shift_register;
} DebounceShift;

void debounce_shift_init(DebounceShift *db, bool initial_state) {
    db->current_state = initial_state;
    db->shift_register = initial_state ? SHIFT_DEBOUNCE_ON : SHIFT_DEBOUNCE_OFF;
}

// Call at fixed rate (e.g., every 5ms for 8 samples = 40ms debounce)
bool debounce_shift_update(DebounceShift *db, bool raw_input) {
    // Shift in new sample
    db->shift_register = (db->shift_register << 1) | (raw_input ? 1 : 0);

    // Check for stable states
    uint8_t masked = db->shift_register & SHIFT_DEBOUNCE_MASK;

    if (masked == SHIFT_DEBOUNCE_ON) {
        db->current_state = true;
    } else if (masked == SHIFT_DEBOUNCE_OFF) {
        db->current_state = false;
    }
    // Otherwise keep previous state

    return db->current_state;
}


/* ============================================
 * INTEGRATING DEBOUNCE (charge/discharge)
 * Hysteresis-like behavior
 * ============================================ */

#define INTEGRATOR_MAX      100
#define INTEGRATOR_HIGH     75
#define INTEGRATOR_LOW      25
#define INTEGRATOR_STEP     10

typedef struct {
    bool current_state;
    int16_t integrator;
} DebounceIntegrator;

void debounce_integrator_init(DebounceIntegrator *db, bool initial_state) {
    db->current_state = initial_state;
    db->integrator = initial_state ? INTEGRATOR_MAX : 0;
}

bool debounce_integrator_update(DebounceIntegrator *db, bool raw_input) {
    // Charge or discharge integrator
    if (raw_input) {
        db->integrator += INTEGRATOR_STEP;
        if (db->integrator > INTEGRATOR_MAX) {
            db->integrator = INTEGRATOR_MAX;
        }
    } else {
        db->integrator -= INTEGRATOR_STEP;
        if (db->integrator < 0) {
            db->integrator = 0;
        }
    }

    // Hysteresis thresholds
    if (db->integrator >= INTEGRATOR_HIGH) {
        db->current_state = true;
    } else if (db->integrator <= INTEGRATOR_LOW) {
        db->current_state = false;
    }

    return db->current_state;
}


/* ============================================
 * BUTTON WITH PRESS/RELEASE/HOLD DETECTION
 * ============================================ */

#define HOLD_TIME_MS        1000
#define REPEAT_TIME_MS      200
#define REPEAT_DELAY_MS     500

typedef enum {
    BUTTON_IDLE,
    BUTTON_PRESSED,
    BUTTON_HELD,
    BUTTON_RELEASED
} ButtonEvent;

typedef struct {
    DebounceShift debounce;
    bool prev_state;
    uint32_t press_time;
    uint32_t last_repeat_time;
    bool held_reported;
} ButtonState;

void button_init(ButtonState *btn) {
    debounce_shift_init(&btn->debounce, false);
    btn->prev_state = false;
    btn->press_time = 0;
    btn->last_repeat_time = 0;
    btn->held_reported = false;
}

ButtonEvent button_update(ButtonState *btn, bool raw_input, uint32_t current_time) {
    bool current = debounce_shift_update(&btn->debounce, raw_input);
    ButtonEvent event = BUTTON_IDLE;

    if (current && !btn->prev_state) {
        // Just pressed
        event = BUTTON_PRESSED;
        btn->press_time = current_time;
        btn->held_reported = false;
    }
    else if (!current && btn->prev_state) {
        // Just released
        event = BUTTON_RELEASED;
    }
    else if (current && btn->prev_state) {
        // Still held
        if (!btn->held_reported &&
            (current_time - btn->press_time) >= HOLD_TIME_MS) {
            event = BUTTON_HELD;
            btn->held_reported = true;
            btn->last_repeat_time = current_time;
        }
    }

    btn->prev_state = current;
    return event;
}

bool button_is_pressed(ButtonState *btn) {
    return btn->debounce.current_state;
}

bool button_is_held(ButtonState *btn) {
    return btn->held_reported;
}


/* ============================================
 * MULTI-BUTTON DEBOUNCER (8 buttons)
 * ============================================ */

typedef struct {
    uint8_t current_state;      // Bit per button
    uint8_t shift_reg[8];       // Shift register per button
} MultiButtonDebounce;

void multi_button_init(MultiButtonDebounce *mbd) {
    mbd->current_state = 0;
    for (int i = 0; i < 8; i++) {
        mbd->shift_reg[i] = 0;
    }
}

// raw_inputs: bit 0 = button 0, etc.
uint8_t multi_button_update(MultiButtonDebounce *mbd, uint8_t raw_inputs) {
    for (int i = 0; i < 8; i++) {
        bool bit = (raw_inputs >> i) & 1;

        mbd->shift_reg[i] = (mbd->shift_reg[i] << 1) | bit;

        if (mbd->shift_reg[i] == 0xFF) {
            mbd->current_state |= (1 << i);
        } else if (mbd->shift_reg[i] == 0x00) {
            mbd->current_state &= ~(1 << i);
        }
    }

    return mbd->current_state;
}

// Get changed buttons (XOR of previous and current)
uint8_t multi_button_get_changed(MultiButtonDebounce *mbd, uint8_t prev_state) {
    return mbd->current_state ^ prev_state;
}


/* ============================================
 * ROTARY ENCODER DEBOUNCE
 * ============================================ */

typedef struct {
    int8_t position;
    uint8_t prev_state;
    DebounceShift debounce_a;
    DebounceShift debounce_b;
} RotaryEncoder;

void encoder_init(RotaryEncoder *enc) {
    enc->position = 0;
    enc->prev_state = 0;
    debounce_shift_init(&enc->debounce_a, false);
    debounce_shift_init(&enc->debounce_b, false);
}

// Returns delta: +1 for CW, -1 for CCW, 0 for no change
int8_t encoder_update(RotaryEncoder *enc, bool raw_a, bool raw_b) {
    bool a = debounce_shift_update(&enc->debounce_a, raw_a);
    bool b = debounce_shift_update(&enc->debounce_b, raw_b);

    uint8_t current_state = (a ? 2 : 0) | (b ? 1 : 0);
    int8_t delta = 0;

    // Gray code state machine
    // 00 -> 01 -> 11 -> 10 -> 00 (CW)
    // 00 -> 10 -> 11 -> 01 -> 00 (CCW)

    static const int8_t encoder_table[16] = {
        0, -1, 1, 0,
        1, 0, 0, -1,
        -1, 0, 0, 1,
        0, 1, -1, 0
    };

    uint8_t index = (enc->prev_state << 2) | current_state;
    delta = encoder_table[index];

    enc->prev_state = current_state;
    enc->position += delta;

    return delta;
}

int8_t encoder_get_position(RotaryEncoder *enc) {
    return enc->position;
}

void encoder_reset_position(RotaryEncoder *enc) {
    enc->position = 0;
}


/* ============================================
 * ANALOG INPUT DEADBAND (noise suppression)
 * ============================================ */

typedef struct {
    int16_t current_value;
    int16_t deadband;
} DeadbandFilter;

void deadband_init(DeadbandFilter *db, int16_t deadband) {
    db->current_value = 0;
    db->deadband = deadband;
}

int16_t deadband_update(DeadbandFilter *db, int16_t raw_input) {
    int16_t diff = raw_input - db->current_value;

    if (diff > db->deadband || diff < -db->deadband) {
        db->current_value = raw_input;
    }

    return db->current_value;
}
