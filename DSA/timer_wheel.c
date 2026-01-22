/*
 * TIMER WHEEL IMPLEMENTATIONS
 * Use: Efficient timer management, timeout handling, scheduling
 *
 * Features:
 * - Simple timer wheel (single level)
 * - Hierarchical timer wheel (multi-level)
 * - Deadline scheduler
 * - Soft timers for cooperative multitasking
 *
 * Time: O(1) insert/delete, O(1) tick processing (amortized)
 * Space: O(n) for n timers
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* ============================================
 * SIMPLE TIMER WHEEL (Single Level)
 * Good for short timeouts with uniform distribution
 * ============================================ */

#define WHEEL_SIZE      64      // Must be power of 2
#define WHEEL_MASK      (WHEEL_SIZE - 1)
#define MAX_TIMERS      32

typedef void (*TimerCallback)(void *arg);

typedef struct TimerNode {
    uint32_t expiry;            // Absolute expiry tick
    uint32_t period;            // For periodic timers (0 = one-shot)
    TimerCallback callback;
    void *arg;
    int8_t next;                // Index of next timer in slot (-1 = end)
    bool active;
} TimerNode;

typedef struct {
    TimerNode timers[MAX_TIMERS];
    int8_t slots[WHEEL_SIZE];   // Head of each slot's linked list
    uint32_t current_tick;
    uint16_t current_slot;
} TimerWheel;

void timer_wheel_init(TimerWheel *tw) {
    tw->current_tick = 0;
    tw->current_slot = 0;

    for (int i = 0; i < WHEEL_SIZE; i++) {
        tw->slots[i] = -1;
    }

    for (int i = 0; i < MAX_TIMERS; i++) {
        tw->timers[i].active = false;
        tw->timers[i].next = -1;
    }
}

// Internal: Add timer to a slot
static void timer_wheel_add_to_slot(TimerWheel *tw, int8_t timer_id, uint16_t slot) {
    tw->timers[timer_id].next = tw->slots[slot];
    tw->slots[slot] = timer_id;
}

// Internal: Remove timer from a slot
static void timer_wheel_remove_from_slot(TimerWheel *tw, int8_t timer_id, uint16_t slot) {
    int8_t *prev = &tw->slots[slot];
    while (*prev != -1) {
        if (*prev == timer_id) {
            *prev = tw->timers[timer_id].next;
            tw->timers[timer_id].next = -1;
            return;
        }
        prev = &tw->timers[*prev].next;
    }
}

// Returns timer ID or -1 on failure
int timer_wheel_add(TimerWheel *tw, uint32_t delay, uint32_t period,
                    TimerCallback callback, void *arg) {
    // Find free timer slot
    int timer_id = -1;
    for (int i = 0; i < MAX_TIMERS; i++) {
        if (!tw->timers[i].active) {
            timer_id = i;
            break;
        }
    }
    if (timer_id < 0) return -1;

    // Calculate expiry and target slot
    uint32_t expiry = tw->current_tick + delay;
    uint16_t slot = expiry & WHEEL_MASK;

    // Initialize timer
    tw->timers[timer_id].expiry = expiry;
    tw->timers[timer_id].period = period;
    tw->timers[timer_id].callback = callback;
    tw->timers[timer_id].arg = arg;
    tw->timers[timer_id].active = true;

    // Add to slot
    timer_wheel_add_to_slot(tw, timer_id, slot);

    return timer_id;
}

bool timer_wheel_cancel(TimerWheel *tw, int timer_id) {
    if (timer_id < 0 || timer_id >= MAX_TIMERS) return false;
    if (!tw->timers[timer_id].active) return false;

    uint16_t slot = tw->timers[timer_id].expiry & WHEEL_MASK;
    timer_wheel_remove_from_slot(tw, timer_id, slot);
    tw->timers[timer_id].active = false;

    return true;
}

// Advance time and process expired timers
void timer_wheel_tick(TimerWheel *tw) {
    tw->current_tick++;
    tw->current_slot = tw->current_tick & WHEEL_MASK;

    int8_t timer_id = tw->slots[tw->current_slot];
    tw->slots[tw->current_slot] = -1;  // Clear slot

    while (timer_id >= 0) {
        int8_t next = tw->timers[timer_id].next;
        TimerNode *timer = &tw->timers[timer_id];
        timer->next = -1;

        // Check if actually expired (handle wrap-around)
        if (timer->active && timer->expiry <= tw->current_tick) {
            // Execute callback
            if (timer->callback) {
                timer->callback(timer->arg);
            }

            // Re-schedule periodic timers
            if (timer->period > 0 && timer->active) {
                timer->expiry = tw->current_tick + timer->period;
                uint16_t new_slot = timer->expiry & WHEEL_MASK;
                timer_wheel_add_to_slot(tw, timer_id, new_slot);
            } else {
                timer->active = false;
            }
        } else if (timer->active) {
            // Timer hasn't expired yet (wrap around), re-add
            timer_wheel_add_to_slot(tw, timer_id, tw->current_slot);
        }

        timer_id = next;
    }
}


/* ============================================
 * HIERARCHICAL TIMER WHEEL
 * Handles larger time ranges efficiently
 * ============================================ */

#define HW_LEVELS       3
#define HW_BITS         6           // Bits per level
#define HW_SIZE         (1 << HW_BITS)  // 64 slots per level
#define HW_MASK         (HW_SIZE - 1)
#define HW_MAX_TIMERS   64

typedef struct HWTimerNode {
    uint32_t expiry;
    uint32_t period;
    TimerCallback callback;
    void *arg;
    int8_t next;
    uint8_t level;
    bool active;
} HWTimerNode;

typedef struct {
    HWTimerNode timers[HW_MAX_TIMERS];
    int8_t slots[HW_LEVELS][HW_SIZE];
    uint32_t current_tick;
} HierarchicalWheel;

void hwheel_init(HierarchicalWheel *hw) {
    hw->current_tick = 0;

    for (int l = 0; l < HW_LEVELS; l++) {
        for (int s = 0; s < HW_SIZE; s++) {
            hw->slots[l][s] = -1;
        }
    }

    for (int i = 0; i < HW_MAX_TIMERS; i++) {
        hw->timers[i].active = false;
        hw->timers[i].next = -1;
    }
}

// Determine which level a timer belongs to
static int hwheel_get_level(uint32_t delay) {
    if (delay < HW_SIZE) return 0;
    if (delay < (HW_SIZE << HW_BITS)) return 1;
    return 2;
}

// Get slot within a level
static uint16_t hwheel_get_slot(uint32_t tick, int level) {
    return (tick >> (level * HW_BITS)) & HW_MASK;
}

static void hwheel_add_to_slot(HierarchicalWheel *hw, int8_t timer_id,
                                int level, uint16_t slot) {
    hw->timers[timer_id].level = level;
    hw->timers[timer_id].next = hw->slots[level][slot];
    hw->slots[level][slot] = timer_id;
}

int hwheel_add(HierarchicalWheel *hw, uint32_t delay, uint32_t period,
               TimerCallback callback, void *arg) {
    int timer_id = -1;
    for (int i = 0; i < HW_MAX_TIMERS; i++) {
        if (!hw->timers[i].active) {
            timer_id = i;
            break;
        }
    }
    if (timer_id < 0) return -1;

    uint32_t expiry = hw->current_tick + delay;
    int level = hwheel_get_level(delay);
    uint16_t slot = hwheel_get_slot(expiry, level);

    hw->timers[timer_id].expiry = expiry;
    hw->timers[timer_id].period = period;
    hw->timers[timer_id].callback = callback;
    hw->timers[timer_id].arg = arg;
    hw->timers[timer_id].active = true;

    hwheel_add_to_slot(hw, timer_id, level, slot);

    return timer_id;
}

// Cascade timers from higher levels when needed
static void hwheel_cascade(HierarchicalWheel *hw, int level) {
    if (level >= HW_LEVELS) return;

    uint16_t slot = hwheel_get_slot(hw->current_tick, level);
    int8_t timer_id = hw->slots[level][slot];
    hw->slots[level][slot] = -1;

    while (timer_id >= 0) {
        int8_t next = hw->timers[timer_id].next;
        HWTimerNode *timer = &hw->timers[timer_id];
        timer->next = -1;

        if (timer->active) {
            // Recalculate level and slot
            uint32_t remaining = timer->expiry - hw->current_tick;
            int new_level = hwheel_get_level(remaining);
            uint16_t new_slot = hwheel_get_slot(timer->expiry, new_level);
            hwheel_add_to_slot(hw, timer_id, new_level, new_slot);
        }

        timer_id = next;
    }
}

void hwheel_tick(HierarchicalWheel *hw) {
    hw->current_tick++;

    // Cascade from higher levels if needed
    for (int l = 1; l < HW_LEVELS; l++) {
        if ((hw->current_tick & ((1 << (l * HW_BITS)) - 1)) == 0) {
            hwheel_cascade(hw, l);
        }
    }

    // Process expired timers in level 0
    uint16_t slot = hw->current_tick & HW_MASK;
    int8_t timer_id = hw->slots[0][slot];
    hw->slots[0][slot] = -1;

    while (timer_id >= 0) {
        int8_t next = hw->timers[timer_id].next;
        HWTimerNode *timer = &hw->timers[timer_id];
        timer->next = -1;

        if (timer->active && timer->expiry <= hw->current_tick) {
            if (timer->callback) {
                timer->callback(timer->arg);
            }

            if (timer->period > 0 && timer->active) {
                timer->expiry = hw->current_tick + timer->period;
                int level = hwheel_get_level(timer->period);
                uint16_t new_slot = hwheel_get_slot(timer->expiry, level);
                hwheel_add_to_slot(hw, timer_id, level, new_slot);
            } else {
                timer->active = false;
            }
        }

        timer_id = next;
    }
}


/* ============================================
 * DEADLINE SCHEDULER
 * Run tasks by deadline
 * ============================================ */

#define MAX_DEADLINE_TASKS  16

typedef void (*DeadlineTask)(void);

typedef struct {
    uint32_t deadline;
    uint32_t wcet;          // Worst-case execution time
    DeadlineTask task;
    bool active;
} DeadlineEntry;

typedef struct {
    DeadlineEntry tasks[MAX_DEADLINE_TASKS];
    uint32_t current_time;
} DeadlineScheduler;

void deadline_sched_init(DeadlineScheduler *ds) {
    ds->current_time = 0;
    for (int i = 0; i < MAX_DEADLINE_TASKS; i++) {
        ds->tasks[i].active = false;
    }
}

int deadline_sched_add(DeadlineScheduler *ds, uint32_t deadline,
                       uint32_t wcet, DeadlineTask task) {
    int slot = -1;
    for (int i = 0; i < MAX_DEADLINE_TASKS; i++) {
        if (!ds->tasks[i].active) {
            slot = i;
            break;
        }
    }
    if (slot < 0) return -1;

    ds->tasks[slot].deadline = deadline;
    ds->tasks[slot].wcet = wcet;
    ds->tasks[slot].task = task;
    ds->tasks[slot].active = true;

    return slot;
}

// Find task with earliest deadline
int deadline_sched_get_next(DeadlineScheduler *ds) {
    int best = -1;
    uint32_t earliest = UINT32_MAX;

    for (int i = 0; i < MAX_DEADLINE_TASKS; i++) {
        if (ds->tasks[i].active && ds->tasks[i].deadline < earliest) {
            earliest = ds->tasks[i].deadline;
            best = i;
        }
    }

    return best;
}

// Run highest priority task
bool deadline_sched_run_one(DeadlineScheduler *ds) {
    int task_id = deadline_sched_get_next(ds);
    if (task_id < 0) return false;

    DeadlineEntry *task = &ds->tasks[task_id];

    // Check for deadline miss
    if (ds->current_time > task->deadline) {
        // Handle deadline miss (could add callback here)
        task->active = false;
        return false;
    }

    // Execute task
    if (task->task) {
        task->task();
    }

    task->active = false;
    return true;
}


/* ============================================
 * SOFT TIMER MANAGER
 * For cooperative multitasking
 * ============================================ */

#define MAX_SOFT_TIMERS     16

typedef enum {
    SOFT_TIMER_STOPPED,
    SOFT_TIMER_RUNNING,
    SOFT_TIMER_EXPIRED
} SoftTimerState;

typedef struct {
    uint32_t interval;      // Timer interval
    uint32_t counter;       // Current countdown
    TimerCallback callback;
    void *arg;
    SoftTimerState state;
    bool auto_reload;       // Auto-restart on expiry
} SoftTimer;

typedef struct {
    SoftTimer timers[MAX_SOFT_TIMERS];
} SoftTimerManager;

void soft_timer_init(SoftTimerManager *stm) {
    for (int i = 0; i < MAX_SOFT_TIMERS; i++) {
        stm->timers[i].state = SOFT_TIMER_STOPPED;
    }
}

int soft_timer_create(SoftTimerManager *stm, uint32_t interval,
                      bool auto_reload, TimerCallback callback, void *arg) {
    for (int i = 0; i < MAX_SOFT_TIMERS; i++) {
        if (stm->timers[i].state == SOFT_TIMER_STOPPED &&
            stm->timers[i].callback == NULL) {
            stm->timers[i].interval = interval;
            stm->timers[i].counter = interval;
            stm->timers[i].callback = callback;
            stm->timers[i].arg = arg;
            stm->timers[i].auto_reload = auto_reload;
            return i;
        }
    }
    return -1;
}

void soft_timer_start(SoftTimerManager *stm, int timer_id) {
    if (timer_id < 0 || timer_id >= MAX_SOFT_TIMERS) return;
    stm->timers[timer_id].counter = stm->timers[timer_id].interval;
    stm->timers[timer_id].state = SOFT_TIMER_RUNNING;
}

void soft_timer_stop(SoftTimerManager *stm, int timer_id) {
    if (timer_id < 0 || timer_id >= MAX_SOFT_TIMERS) return;
    stm->timers[timer_id].state = SOFT_TIMER_STOPPED;
}

void soft_timer_reset(SoftTimerManager *stm, int timer_id) {
    if (timer_id < 0 || timer_id >= MAX_SOFT_TIMERS) return;
    stm->timers[timer_id].counter = stm->timers[timer_id].interval;
}

// Call this from main loop at fixed rate
void soft_timer_tick(SoftTimerManager *stm, uint32_t elapsed_ticks) {
    for (int i = 0; i < MAX_SOFT_TIMERS; i++) {
        SoftTimer *timer = &stm->timers[i];

        if (timer->state == SOFT_TIMER_RUNNING) {
            if (timer->counter <= elapsed_ticks) {
                timer->state = SOFT_TIMER_EXPIRED;

                if (timer->callback) {
                    timer->callback(timer->arg);
                }

                if (timer->auto_reload) {
                    timer->counter = timer->interval;
                    timer->state = SOFT_TIMER_RUNNING;
                }
            } else {
                timer->counter -= elapsed_ticks;
            }
        }
    }
}

bool soft_timer_is_expired(SoftTimerManager *stm, int timer_id) {
    if (timer_id < 0 || timer_id >= MAX_SOFT_TIMERS) return false;
    return stm->timers[timer_id].state == SOFT_TIMER_EXPIRED;
}


/* ============================================
 * WATCHDOG PATTERN
 * Software watchdog timer
 * ============================================ */

typedef void (*WatchdogHandler)(uint8_t channel);

#define MAX_WATCHDOG_CHANNELS   8

typedef struct {
    uint32_t timeout;
    uint32_t counter;
    bool enabled;
    bool expired;
} WatchdogChannel;

typedef struct {
    WatchdogChannel channels[MAX_WATCHDOG_CHANNELS];
    WatchdogHandler handler;
} SoftwareWatchdog;

void watchdog_init(SoftwareWatchdog *wd, WatchdogHandler handler) {
    wd->handler = handler;
    for (int i = 0; i < MAX_WATCHDOG_CHANNELS; i++) {
        wd->channels[i].enabled = false;
        wd->channels[i].expired = false;
    }
}

void watchdog_enable(SoftwareWatchdog *wd, uint8_t channel, uint32_t timeout) {
    if (channel >= MAX_WATCHDOG_CHANNELS) return;
    wd->channels[channel].timeout = timeout;
    wd->channels[channel].counter = timeout;
    wd->channels[channel].enabled = true;
    wd->channels[channel].expired = false;
}

void watchdog_disable(SoftwareWatchdog *wd, uint8_t channel) {
    if (channel >= MAX_WATCHDOG_CHANNELS) return;
    wd->channels[channel].enabled = false;
}

void watchdog_kick(SoftwareWatchdog *wd, uint8_t channel) {
    if (channel >= MAX_WATCHDOG_CHANNELS) return;
    if (wd->channels[channel].enabled) {
        wd->channels[channel].counter = wd->channels[channel].timeout;
        wd->channels[channel].expired = false;
    }
}

void watchdog_tick(SoftwareWatchdog *wd, uint32_t elapsed) {
    for (int i = 0; i < MAX_WATCHDOG_CHANNELS; i++) {
        WatchdogChannel *ch = &wd->channels[i];
        if (ch->enabled && !ch->expired) {
            if (ch->counter <= elapsed) {
                ch->expired = true;
                if (wd->handler) {
                    wd->handler(i);
                }
            } else {
                ch->counter -= elapsed;
            }
        }
    }
}


/* ============================================
 * TIMEOUT HELPER
 * Non-blocking timeout checks
 * ============================================ */

typedef struct {
    uint32_t start_time;
    uint32_t duration;
    bool running;
} Timeout;

void timeout_init(Timeout *to) {
    to->running = false;
}

void timeout_start(Timeout *to, uint32_t current_time, uint32_t duration) {
    to->start_time = current_time;
    to->duration = duration;
    to->running = true;
}

void timeout_stop(Timeout *to) {
    to->running = false;
}

bool timeout_is_expired(Timeout *to, uint32_t current_time) {
    if (!to->running) return false;
    return (current_time - to->start_time) >= to->duration;
}

uint32_t timeout_remaining(Timeout *to, uint32_t current_time) {
    if (!to->running) return 0;
    uint32_t elapsed = current_time - to->start_time;
    if (elapsed >= to->duration) return 0;
    return to->duration - elapsed;
}

