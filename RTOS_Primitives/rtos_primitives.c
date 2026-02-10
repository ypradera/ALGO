/*
 * RTOS Primitives - Interview Topics for Embedded Systems
 *
 * ============================================================================
 *                    BEGINNER'S GUIDE: RTOS PRIMITIVES
 * ============================================================================
 *
 * WHAT IS AN RTOS?
 * ----------------
 * Real-Time Operating System - manages multiple tasks with timing guarantees.
 * Unlike desktop OS, RTOS focuses on DETERMINISM (predictable timing).
 *
 * KEY CONCEPTS:
 *   - Tasks (Threads): Independent units of execution
 *   - Scheduler: Decides which task runs when
 *   - Priority: Higher priority tasks run first
 *   - Preemption: Higher priority task can interrupt lower
 *
 * WHY RTOS IN EMBEDDED?
 *   - Manage complexity (multiple concurrent activities)
 *   - Meet timing requirements (real-time deadlines)
 *   - Modular code structure
 *   - Resource sharing between tasks
 *
 * COMMON RTOS: FreeRTOS, Zephyr, VxWorks, ThreadX, uC/OS
 *
 * ============================================================================
 * RTOS PRIMITIVES OVERVIEW
 * ============================================================================
 *
 * PRIMITIVE        | PURPOSE                      | TYPICAL USE
 * -----------------|------------------------------|---------------------------
 * Semaphore        | Signaling, resource counting | ISR->Task notification
 * Mutex            | Mutual exclusion             | Protect shared resources
 * Queue            | Task-to-task communication   | Data passing
 * Event Flags      | Multiple condition waiting   | Complex synchronization
 * Timer            | Delayed/periodic execution   | Timeouts, polling
 *
 * ============================================================================
 * COMMON INTERVIEW QUESTIONS
 * ============================================================================
 *
 * 1. "What's the difference between a semaphore and mutex?"
 * 2. "What is priority inversion and how do you solve it?"
 * 3. "How do you communicate between an ISR and a task?"
 * 4. "What is a deadlock and how do you prevent it?"
 * 5. "Explain the producer-consumer problem"
 * 6. "What scheduling algorithms do you know?"
 *
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* ============================================================================
 * 1. SEMAPHORE
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * A semaphore is like a counter that controls access to resources.
 *
 * TWO TYPES:
 *
 * BINARY SEMAPHORE (0 or 1):
 *   - Like a flag: "signal happened" or "not yet"
 *   - Used for: ISR to task notification
 *
 *   ISR:  semaphore_give()   // Signal "data ready"
 *   Task: semaphore_take()   // Wait for signal, then process
 *
 * COUNTING SEMAPHORE (0 to N):
 *   - Counts available resources
 *   - Used for: Pool of resources (buffers, connections)
 *
 *   Example: 3 UART ports available
 *   semaphore = 3
 *   Task A takes one: semaphore = 2
 *   Task B takes one: semaphore = 1
 *   Task C takes one: semaphore = 0
 *   Task D tries to take: BLOCKED (waits until one is released)
 *
 * OPERATIONS:
 *   - take (wait, pend, P): Decrement. If 0, block until >0
 *   - give (signal, post, V): Increment. Wake up waiting task
 *
 * VISUAL:
 *
 *   Binary Semaphore:
 *     ISR ----[give]----> [Semaphore] ----[take]----> Task
 *                              |
 *                         count: 0 or 1
 *
 *   Counting Semaphore:
 *     Producer --[give]--> [Semaphore] <--[take]-- Consumer
 *                              |
 *                         count: 0 to N
 * ============================================================================ */

typedef struct {
    int count;
    int maxCount;       // For counting semaphore
    // In real RTOS: waiting task queue here
} Semaphore;

void semaphore_init(Semaphore* sem, int initialCount, int maxCount) {
    sem->count = initialCount;
    sem->maxCount = maxCount;
}

// Binary semaphore helper
void semaphore_initBinary(Semaphore* sem) {
    semaphore_init(sem, 0, 1);
}

/*
 * semaphore_take - Acquire the semaphore (decrement)
 *
 * Returns: true if acquired, false if would block
 * In real RTOS: would block if count is 0
 */
bool semaphore_take(Semaphore* sem) {
    if (sem->count > 0) {
        sem->count--;
        return true;
    }
    // In real RTOS: block here and wait
    return false;  // Would block
}

/*
 * semaphore_give - Release the semaphore (increment)
 *
 * In real RTOS: wakes up waiting task if any
 */
bool semaphore_give(Semaphore* sem) {
    if (sem->count < sem->maxCount) {
        sem->count++;
        // In real RTOS: wake up highest priority waiting task
        return true;
    }
    return false;  // Already at max
}

void demo_semaphore(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|                    SEMAPHORE                                       |\n");
    printf("+====================================================================+\n");

    printf("\n[CONCEPT]\n");
    printf("  Semaphore = counter that controls resource access\n");
    printf("  take(): decrement, block if 0\n");
    printf("  give(): increment, wake up waiters\n");

    printf("\n[DEMO] Binary Semaphore (ISR -> Task signaling)\n");
    Semaphore binSem;
    semaphore_initBinary(&binSem);

    printf("  Initial count: %d\n", binSem.count);
    printf("  Task tries to take (would block): %s\n",
           semaphore_take(&binSem) ? "got it" : "blocked");
    printf("  ISR gives semaphore...\n");
    semaphore_give(&binSem);
    printf("  Count after give: %d\n", binSem.count);
    printf("  Task takes: %s\n",
           semaphore_take(&binSem) ? "got it!" : "blocked");
    printf("  Count after take: %d\n", binSem.count);

    printf("\n[DEMO] Counting Semaphore (Resource pool)\n");
    Semaphore countSem;
    semaphore_init(&countSem, 3, 3);  // 3 resources available

    printf("  Resources available: %d\n", countSem.count);
    printf("  Task A takes: %s (remaining: %d)\n",
           semaphore_take(&countSem) ? "OK" : "blocked", countSem.count);
    printf("  Task B takes: %s (remaining: %d)\n",
           semaphore_take(&countSem) ? "OK" : "blocked", countSem.count);
    printf("  Task C takes: %s (remaining: %d)\n",
           semaphore_take(&countSem) ? "OK" : "blocked", countSem.count);
    printf("  Task D takes: %s (remaining: %d)\n",
           semaphore_take(&countSem) ? "OK" : "BLOCKED!", countSem.count);
    printf("  Task A releases...\n");
    semaphore_give(&countSem);
    printf("  Task D can now take: %s\n",
           semaphore_take(&countSem) ? "OK" : "blocked");

    printf("\n[INTERVIEW TIP]\n");
    printf("  'Semaphores are for SIGNALING, mutexes are for LOCKING'\n");
}

/* ============================================================================
 * 2. MUTEX (Mutual Exclusion)
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * A mutex is like a lock on a door - only one task can hold it at a time.
 * Used to protect shared resources from concurrent access.
 *
 * SEMAPHORE VS MUTEX:
 *   SEMAPHORE:
 *     - Can be given by any task (signaling)
 *     - No ownership
 *     - Binary: signaling; Counting: resource pool
 *
 *   MUTEX:
 *     - Only owner can release
 *     - Has ownership (the task that locked it)
 *     - Often has priority inheritance (prevents priority inversion)
 *
 * CRITICAL SECTION:
 *   mutex_lock();
 *   // Critical section - only one task at a time
 *   shared_variable++;
 *   mutex_unlock();
 *
 * PRIORITY INVERSION:
 *   Problem: High priority task waits for low priority task holding mutex,
 *            while medium priority task runs (blocking high priority!)
 *
 *   Solution: PRIORITY INHERITANCE
 *     Low priority task temporarily gets high priority while holding mutex
 *
 * VISUAL:
 *   Task A: [===lock===|  critical section  |===unlock===]
 *   Task B: [          |blocked|            |  continues  ]
 *           time ------------------------------------------->
 * ============================================================================ */

typedef struct {
    bool locked;
    int ownerTaskId;    // Which task holds the mutex
    int originalPriority;  // For priority inheritance
    // In real RTOS: waiting task queue
} Mutex;

void mutex_init(Mutex* mtx) {
    mtx->locked = false;
    mtx->ownerTaskId = -1;
    mtx->originalPriority = -1;
}

bool mutex_lock(Mutex* mtx, int taskId) {
    if (!mtx->locked) {
        mtx->locked = true;
        mtx->ownerTaskId = taskId;
        return true;
    }
    // In real RTOS: block and wait, implement priority inheritance
    return false;
}

bool mutex_unlock(Mutex* mtx, int taskId) {
    // Only owner can unlock!
    if (mtx->locked && mtx->ownerTaskId == taskId) {
        mtx->locked = false;
        mtx->ownerTaskId = -1;
        // In real RTOS: restore priority, wake up waiting task
        return true;
    }
    return false;
}

void demo_mutex(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|                    MUTEX                                           |\n");
    printf("+====================================================================+\n");

    printf("\n[CONCEPT]\n");
    printf("  Mutex = Lock with ownership\n");
    printf("  Only the locking task can unlock\n");
    printf("  Used to protect shared resources\n");

    printf("\n[DEMO] Mutex protecting shared resource\n");
    Mutex mtx;
    mutex_init(&mtx);

    int sharedCounter = 0;

    printf("  Shared counter = %d\n", sharedCounter);
    printf("  Task 1 locks mutex: %s\n",
           mutex_lock(&mtx, 1) ? "OK" : "blocked");
    printf("  Task 1 increments counter...\n");
    sharedCounter++;
    printf("  Task 2 tries to lock: %s\n",
           mutex_lock(&mtx, 2) ? "OK" : "BLOCKED (waiting for Task 1)");
    printf("  Task 2 (incorrectly) tries to unlock: %s\n",
           mutex_unlock(&mtx, 2) ? "OK" : "FAILED (not owner!)");
    printf("  Task 1 unlocks: %s\n",
           mutex_unlock(&mtx, 1) ? "OK" : "failed");
    printf("  Task 2 can now lock: %s\n",
           mutex_lock(&mtx, 2) ? "OK" : "blocked");
    sharedCounter++;
    printf("  Final counter = %d\n", sharedCounter);
    mutex_unlock(&mtx, 2);

    printf("\n[PRIORITY INVERSION]\n");
    printf("  Problem: High priority task waits for low priority mutex holder\n");
    printf("           Medium priority task preempts low priority -> blocks high!\n");
    printf("  Solution: Priority Inheritance\n");
    printf("           Low priority task gets boosted while holding mutex\n");

    printf("\n[INTERVIEW TIP]\n");
    printf("  Q: 'Mutex vs Semaphore?'\n");
    printf("  A: Mutex has OWNERSHIP. Only owner can release.\n");
    printf("     Mutex often has priority inheritance.\n");
    printf("     Use mutex for locking, semaphore for signaling.\n");
}

/* ============================================================================
 * 3. MESSAGE QUEUE
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * A queue that passes messages (data) between tasks safely.
 *
 * VISUAL:
 *   Producer Task              Queue                Consumer Task
 *        |                  [_][_][_][_]                  |
 *        |------ send() ---->  |   |   <---- recv() ------|
 *                              |   |
 *                           front  rear
 *
 * OPERATIONS:
 *   - send (enqueue): Add message to back. Block if full.
 *   - receive (dequeue): Remove from front. Block if empty.
 *
 * WHY QUEUES?
 *   - Decouples producer and consumer speeds
 *   - Thread-safe data passing
 *   - Buffering for bursty data
 *
 * COMMON USE CASES:
 *   - ISR sends sensor data to processing task
 *   - Command queue for state machine
 *   - Log messages to logging task
 * ============================================================================ */

#define QUEUE_SIZE 8
#define MSG_SIZE sizeof(int)

typedef struct {
    int buffer[QUEUE_SIZE];
    int head;           // Read from here
    int tail;           // Write here
    int count;
    // In real RTOS: semaphores for blocking
} MessageQueue;

void queue_init(MessageQueue* q) {
    q->head = 0;
    q->tail = 0;
    q->count = 0;
}

bool queue_send(MessageQueue* q, int msg) {
    if (q->count >= QUEUE_SIZE) {
        return false;  // Queue full, would block in real RTOS
    }
    q->buffer[q->tail] = msg;
    q->tail = (q->tail + 1) % QUEUE_SIZE;
    q->count++;
    return true;
}

bool queue_receive(MessageQueue* q, int* msg) {
    if (q->count == 0) {
        return false;  // Queue empty, would block in real RTOS
    }
    *msg = q->buffer[q->head];
    q->head = (q->head + 1) % QUEUE_SIZE;
    q->count--;
    return true;
}

void demo_queue(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|                    MESSAGE QUEUE                                   |\n");
    printf("+====================================================================+\n");

    printf("\n[CONCEPT]\n");
    printf("  Queue = FIFO buffer for passing data between tasks\n");
    printf("  send(): add to back (block if full)\n");
    printf("  receive(): remove from front (block if empty)\n");

    printf("\n[DEMO] Producer-Consumer with queue\n");
    MessageQueue q;
    queue_init(&q);

    printf("  Producer sends: 10, 20, 30\n");
    queue_send(&q, 10);
    queue_send(&q, 20);
    queue_send(&q, 30);
    printf("  Queue count: %d\n", q.count);

    printf("  Consumer receives:\n");
    int msg;
    while (queue_receive(&q, &msg)) {
        printf("    Received: %d\n", msg);
    }
    printf("  Queue count: %d\n", q.count);

    printf("\n[ISR -> TASK PATTERN]\n");
    printf("  ISR:  queue_send(&sensorQueue, adcValue);\n");
    printf("  Task: while(1) {\n");
    printf("          queue_receive(&sensorQueue, &value, WAIT_FOREVER);\n");
    printf("          process(value);\n");
    printf("        }\n");

    printf("\n[INTERVIEW TIP]\n");
    printf("  Q: 'How do you pass data from ISR to task?'\n");
    printf("  A: Use a message queue. ISR sends, task receives.\n");
    printf("     Queue provides thread-safe buffering.\n");
}

/* ============================================================================
 * 4. EVENT FLAGS (Event Groups)
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * A set of bits that tasks can wait on, with AND/OR conditions.
 *
 * Like multiple binary semaphores combined:
 *   Bit 0: Data ready
 *   Bit 1: Connection established
 *   Bit 2: Button pressed
 *
 * OPERATIONS:
 *   - set: Set one or more bits
 *   - clear: Clear one or more bits
 *   - wait: Wait for bits with AND/OR condition
 *
 * EXAMPLE:
 *   Wait for (DATA_READY AND CONNECTION_OK) OR TIMEOUT:
 *   eventflags_wait(DATA_READY | CONNECTION_OK, WAIT_ALL);
 *
 * VISUAL:
 *   Event Flags: [0][0][1][0][1][0][0][1]
 *                           ^       ^
 *                        bit 4   bit 0
 *
 *   Task waiting for bit 0 AND bit 4:
 *   - Bit 0 is set: not unblocked yet
 *   - Bit 4 is set: NOW unblocked!
 * ============================================================================ */

typedef struct {
    unsigned int flags;
    // In real RTOS: waiting task queue with wait conditions
} EventGroup;

void eventGroup_init(EventGroup* eg) {
    eg->flags = 0;
}

void eventGroup_set(EventGroup* eg, unsigned int bitsToSet) {
    eg->flags |= bitsToSet;
    // In real RTOS: check if any waiting task should be woken
}

void eventGroup_clear(EventGroup* eg, unsigned int bitsToClear) {
    eg->flags &= ~bitsToClear;
}

// Wait for ALL specified bits to be set
bool eventGroup_waitAll(EventGroup* eg, unsigned int bitsToWait) {
    return (eg->flags & bitsToWait) == bitsToWait;
}

// Wait for ANY of specified bits to be set
bool eventGroup_waitAny(EventGroup* eg, unsigned int bitsToWait) {
    return (eg->flags & bitsToWait) != 0;
}

void demo_eventFlags(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|                    EVENT FLAGS                                     |\n");
    printf("+====================================================================+\n");

    printf("\n[CONCEPT]\n");
    printf("  Event flags = Bitmask for complex synchronization\n");
    printf("  Wait for multiple conditions with AND/OR logic\n");

    #define DATA_READY    (1 << 0)
    #define CONN_OK       (1 << 1)
    #define BUTTON_PRESS  (1 << 2)

    EventGroup eg;
    eventGroup_init(&eg);

    printf("\n[DEMO] Wait for (DATA_READY AND CONN_OK)\n");
    printf("  Initial flags: 0x%02X\n", eg.flags);

    printf("  Set DATA_READY...\n");
    eventGroup_set(&eg, DATA_READY);
    printf("  Flags: 0x%02X, wait satisfied? %s\n",
           eg.flags, eventGroup_waitAll(&eg, DATA_READY | CONN_OK) ? "YES" : "NO");

    printf("  Set CONN_OK...\n");
    eventGroup_set(&eg, CONN_OK);
    printf("  Flags: 0x%02X, wait satisfied? %s\n",
           eg.flags, eventGroup_waitAll(&eg, DATA_READY | CONN_OK) ? "YES" : "NO");

    printf("\n[DEMO] Wait for (DATA_READY OR BUTTON_PRESS)\n");
    eventGroup_clear(&eg, DATA_READY | CONN_OK | BUTTON_PRESS);
    printf("  Flags cleared: 0x%02X\n", eg.flags);
    printf("  Set BUTTON_PRESS...\n");
    eventGroup_set(&eg, BUTTON_PRESS);
    printf("  Wait ANY satisfied? %s\n",
           eventGroup_waitAny(&eg, DATA_READY | BUTTON_PRESS) ? "YES" : "NO");

    printf("\n[USE CASES]\n");
    printf("  - Wait for multiple initialization steps\n");
    printf("  - Complex state machine triggers\n");
    printf("  - Replacing multiple semaphores\n");
}

/* ============================================================================
 * 5. DEADLOCK
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * Deadlock = Two or more tasks waiting for each other forever.
 *
 * CLASSIC EXAMPLE (Dining Philosophers):
 *   Task A: lock(mutex1); lock(mutex2); ...
 *   Task B: lock(mutex2); lock(mutex1); ...
 *
 *   Scenario:
 *   1. Task A locks mutex1
 *   2. Task B locks mutex2
 *   3. Task A waits for mutex2 (held by B)
 *   4. Task B waits for mutex1 (held by A)
 *   --> DEADLOCK! Both wait forever.
 *
 * FOUR CONDITIONS (all must be true for deadlock):
 *   1. Mutual Exclusion: Resources can't be shared
 *   2. Hold and Wait: Task holds resource while waiting for another
 *   3. No Preemption: Can't forcibly take resource
 *   4. Circular Wait: A waits for B, B waits for A
 *
 * PREVENTION:
 *   1. Lock ordering: Always acquire mutexes in same order
 *   2. Timeout: Don't wait forever, fail after timeout
 *   3. Try-lock: Don't block, return immediately if can't lock
 *   4. Single lock: Use one mutex for all related resources
 * ============================================================================ */

void demo_deadlock(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|                    DEADLOCK                                        |\n");
    printf("+====================================================================+\n");

    printf("\n[WHAT IS DEADLOCK?]\n");
    printf("  Two or more tasks waiting for each other forever.\n");
    printf("\n");
    printf("  Task A: lock(mutex1); lock(mutex2);  // Holds 1, wants 2\n");
    printf("  Task B: lock(mutex2); lock(mutex1);  // Holds 2, wants 1\n");
    printf("  --> DEADLOCK!\n");

    printf("\n[FOUR CONDITIONS FOR DEADLOCK]\n");
    printf("  1. Mutual Exclusion: Resource can't be shared\n");
    printf("  2. Hold and Wait: Holding one, waiting for another\n");
    printf("  3. No Preemption: Can't forcibly take\n");
    printf("  4. Circular Wait: A->B->A\n");

    printf("\n[PREVENTION STRATEGIES]\n");
    printf("  1. LOCK ORDERING (most common)\n");
    printf("     Always acquire mutexes in same order:\n");
    printf("     Task A: lock(mutex1); lock(mutex2);\n");
    printf("     Task B: lock(mutex1); lock(mutex2);  // Same order!\n");
    printf("\n");
    printf("  2. TIMEOUT\n");
    printf("     if (mutex_lock_timeout(&mtx, 100ms) == TIMEOUT) {\n");
    printf("       // Handle failure, don't wait forever\n");
    printf("     }\n");
    printf("\n");
    printf("  3. TRY-LOCK\n");
    printf("     if (!mutex_trylock(&mtx)) {\n");
    printf("       // Couldn't lock, release other resources, retry\n");
    printf("     }\n");

    printf("\n[INTERVIEW TIP]\n");
    printf("  Q: 'How do you prevent deadlock?'\n");
    printf("  A: Lock ordering is simplest - always acquire in same order.\n");
    printf("     Also use timeouts for robustness.\n");
}

/* ============================================================================
 * 6. SCHEDULING ALGORITHMS
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * The scheduler decides which task runs when.
 *
 * PREEMPTIVE vs COOPERATIVE:
 *   - Preemptive: OS can interrupt running task
 *   - Cooperative: Task must voluntarily yield
 *
 * COMMON ALGORITHMS:
 *
 * 1. PRIORITY-BASED PREEMPTIVE (most common in RTOS)
 *    - Highest priority ready task runs
 *    - When higher priority becomes ready, it preempts current
 *
 * 2. ROUND-ROBIN (for equal priority tasks)
 *    - Each task gets a time slice
 *    - After time slice, next task runs
 *
 * 3. RATE MONOTONIC (RMS)
 *    - Higher rate (shorter period) = higher priority
 *    - For periodic tasks with deadlines
 *
 * 4. EARLIEST DEADLINE FIRST (EDF)
 *    - Task with earliest deadline runs first
 *    - Optimal but more complex
 * ============================================================================ */

typedef struct {
    int taskId;
    const char* name;
    int priority;       // Higher number = higher priority
    int burstTime;      // Time to complete
    int remainingTime;
    bool isReady;
} TaskInfo;

void demo_scheduling(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|                    SCHEDULING ALGORITHMS                           |\n");
    printf("+====================================================================+\n");

    printf("\n[PREEMPTIVE vs COOPERATIVE]\n");
    printf("  Preemptive: Higher priority task can interrupt lower\n");
    printf("  Cooperative: Task must call yield() or blocking function\n");
    printf("  Most RTOS are preemptive for real-time guarantees\n");

    printf("\n[COMMON ALGORITHMS]\n");
    printf("\n  1. PRIORITY-BASED PREEMPTIVE (FreeRTOS, etc.)\n");
    printf("     - Highest priority READY task runs\n");
    printf("     - Preempts immediately when higher priority becomes ready\n");

    TaskInfo tasks[] = {
        {1, "Sensor", 3, 2, 2, true},
        {2, "Display", 2, 3, 3, true},
        {3, "Logging", 1, 4, 4, true}
    };

    printf("     Tasks: ");
    for (int i = 0; i < 3; i++) {
        printf("%s(pri=%d) ", tasks[i].name, tasks[i].priority);
    }
    printf("\n     Execution order: Sensor -> Display -> Logging\n");

    printf("\n  2. ROUND-ROBIN (for equal priority)\n");
    printf("     - Each task gets time slice (e.g., 10ms)\n");
    printf("     - After slice, next equal-priority task runs\n");
    printf("     - Fair sharing among equal tasks\n");

    printf("\n  3. RATE MONOTONIC (RMS)\n");
    printf("     - For periodic tasks\n");
    printf("     - Shorter period = higher priority\n");
    printf("     - Task running every 10ms has higher priority than 50ms\n");

    printf("\n  4. EARLIEST DEADLINE FIRST (EDF)\n");
    printf("     - Dynamic priority based on deadline\n");
    printf("     - Optimal CPU utilization\n");
    printf("     - More complex to implement\n");

    printf("\n[INTERVIEW TIP]\n");
    printf("  Q: 'How does FreeRTOS scheduling work?'\n");
    printf("  A: Priority-based preemptive. Highest priority ready task runs.\n");
    printf("     Equal priority tasks use round-robin with time slicing.\n");
    printf("     configUSE_PREEMPTION and configUSE_TIME_SLICING control this.\n");
}

/* ============================================================================
 * 7. COMMON INTERVIEW PATTERNS
 * ============================================================================ */

void demo_interviewPatterns(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|            COMMON INTERVIEW PATTERNS                               |\n");
    printf("+====================================================================+\n");

    printf("\n[PATTERN 1: ISR -> Task Communication]\n");
    printf("  Problem: ISR can't call blocking functions, must be fast\n");
    printf("  Solution:\n");
    printf("    ISR {\n");
    printf("      data = read_sensor();\n");
    printf("      queue_send(&dataQueue, data);  // Non-blocking\n");
    printf("    }\n");
    printf("    Task {\n");
    printf("      while(1) {\n");
    printf("        queue_receive(&dataQueue, &data, WAIT_FOREVER);\n");
    printf("        process(data);\n");
    printf("      }\n");
    printf("    }\n");

    printf("\n[PATTERN 2: Deferred Interrupt Processing]\n");
    printf("  Problem: ISR takes too long\n");
    printf("  Solution:\n");
    printf("    ISR {\n");
    printf("      disable_interrupt();\n");
    printf("      semaphore_give(&isrSem);  // Signal task\n");
    printf("    }\n");
    printf("    Task (high priority) {\n");
    printf("      semaphore_take(&isrSem);  // Wait for ISR\n");
    printf("      do_heavy_processing();    // Time-consuming work\n");
    printf("      enable_interrupt();\n");
    printf("    }\n");

    printf("\n[PATTERN 3: Protecting Shared Resource]\n");
    printf("    mutex_lock(&resourceMutex);\n");
    printf("    shared_data++;               // Critical section\n");
    printf("    mutex_unlock(&resourceMutex);\n");

    printf("\n[PATTERN 4: Producer-Consumer]\n");
    printf("    // Producer task\n");
    printf("    while(1) {\n");
    printf("      data = produce();\n");
    printf("      queue_send(&buffer, data);\n");
    printf("    }\n");
    printf("    // Consumer task\n");
    printf("    while(1) {\n");
    printf("      queue_receive(&buffer, &data);\n");
    printf("      consume(data);\n");
    printf("    }\n");

    printf("\n[COMMON MISTAKES]\n");
    printf("  1. Calling blocking functions from ISR\n");
    printf("  2. Not protecting shared data with mutex\n");
    printf("  3. Holding mutex too long\n");
    printf("  4. Wrong lock ordering (deadlock)\n");
    printf("  5. Forgetting to release mutex (resource leak)\n");
}

/* ============================================================================
 * MAIN
 * ============================================================================ */

int main(void) {
    printf("+====================================================================+\n");
    printf("|     RTOS PRIMITIVES - EMBEDDED SYSTEMS INTERVIEW GUIDE             |\n");
    printf("+====================================================================+\n");

    demo_semaphore();
    demo_mutex();
    demo_queue();
    demo_eventFlags();
    demo_deadlock();
    demo_scheduling();
    demo_interviewPatterns();

    printf("\n");
    printf("+====================================================================+\n");
    printf("|                    SUMMARY                                         |\n");
    printf("+====================================================================+\n");
    printf("| Key primitives:                                                    |\n");
    printf("|   - Semaphore: Signaling (ISR->Task), counting resources          |\n");
    printf("|   - Mutex: Locking with ownership, priority inheritance           |\n");
    printf("|   - Queue: Thread-safe data passing                                |\n");
    printf("|   - Event flags: Complex multi-condition synchronization          |\n");
    printf("|                                                                    |\n");
    printf("| Must-know topics:                                                  |\n");
    printf("|   - Semaphore vs Mutex (signaling vs locking)                     |\n");
    printf("|   - Priority inversion and inheritance                             |\n");
    printf("|   - Deadlock conditions and prevention                             |\n");
    printf("|   - ISR to task communication patterns                             |\n");
    printf("+====================================================================+\n");

    return 0;
}
