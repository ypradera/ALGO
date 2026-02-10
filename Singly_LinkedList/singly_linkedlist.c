/*
 * Singly Linked List - Common Interview Problems for Embedded Systems
 * Version: 1.0
 *
 * ============================================================================
 *                    BEGINNER'S GUIDE: WHAT IS A LINKED LIST?
 * ============================================================================
 *
 * If you've never seen a linked list before, start here!
 *
 * WHAT IS A LINKED LIST?
 * ----------------------
 * A linked list is a way to store a collection of items where each item
 * "points to" the next item, like a chain.
 *
 * Think of it like a scavenger hunt:
 *   - Each clue tells you where to find the next clue
 *   - You can only follow the clues in order
 *   - The last clue says "THE END" (NULL in programming)
 *
 * VISUAL REPRESENTATION:
 * ----------------------
 *
 *   HEAD                                                    NULL
 *    |                                                        |
 *    v                                                        v
 *  +------+------+     +------+------+     +------+------+
 *  | data | next |---->| data | next |---->| data | next |----> NULL
 *  |  10  |   --------->|  20  |   --------->|  30  |  X   |
 *  +------+------+     +------+------+     +------+------+
 *      Node 1              Node 2              Node 3
 *
 *   - "HEAD" is a pointer to the first node (our starting point)
 *   - Each box is a "NODE" containing:
 *       * "data" = the value we want to store (10, 20, 30)
 *       * "next" = pointer (arrow) to the next node
 *   - "NULL" means "nothing here" - it marks the end
 *
 * COMPARE TO AN ARRAY:
 * --------------------
 *   Array:  [10][20][30][40][50]    <- All items sit next to each other in memory
 *                                      Access any item instantly by index: arr[2]
 *
 *   Linked List:  10 -> 20 -> 30    <- Items can be ANYWHERE in memory
 *                                      Must follow arrows to find item #3
 *
 * WHY USE A LINKED LIST INSTEAD OF AN ARRAY?
 * ------------------------------------------
 *
 *   ARRAY PROBLEMS:
 *   - Fixed size (must know how many items ahead of time)
 *   - Inserting in middle is SLOW (must shift all items after)
 *   - Deleting from middle is SLOW (must shift all items after)
 *
 *   Example: Insert 15 between 10 and 20 in array [10,20,30,40,50]:
 *   Step 1: [10,20,30,40,50, _ ]  <- Need to make room!
 *   Step 2: [10,20,30,40, _ ,50]  <- Shift 50
 *   Step 3: [10,20,30, _ ,40,50]  <- Shift 40
 *   Step 4: [10,20, _ ,30,40,50]  <- Shift 30
 *   Step 5: [10, _ ,20,30,40,50]  <- Shift 20
 *   Step 6: [10,15,20,30,40,50]   <- Finally insert 15!
 *   That's 5 shifts! For a million items, that's a million shifts!
 *
 *   LINKED LIST SOLUTION:
 *   - Dynamic size (grows and shrinks as needed)
 *   - Insert anywhere = just change 2 pointers (instant!)
 *   - Delete anywhere = just change 1 pointer (instant!)
 *
 *   Example: Insert 15 between 10 and 20 in linked list:
 *   Before:  10 -> 20 -> 30
 *   Step 1:  Create new node with 15
 *   Step 2:  Point 15's "next" to 20
 *   Step 3:  Point 10's "next" to 15
 *   After:   10 -> 15 -> 20 -> 30
 *   Only 2 pointer changes! Same speed for a million items!
 *
 * THE TRADE-OFF:
 * --------------
 *   - Arrays:       Fast to ACCESS any item (arr[500] is instant)
 *                   Slow to INSERT/DELETE in middle
 *
 *   - Linked Lists: Slow to ACCESS (must walk through 500 arrows to reach item 500)
 *                   Fast to INSERT/DELETE once you're at the right spot
 *
 * TYPES OF LINKED LISTS:
 * ----------------------
 *   1. SINGLY LINKED (this file): Each node points to NEXT only
 *      10 -> 20 -> 30 -> NULL
 *      Can only go FORWARD
 *
 *   2. DOUBLY LINKED: Each node points to NEXT and PREVIOUS
 *      NULL <- 10 <-> 20 <-> 30 -> NULL
 *      Can go FORWARD and BACKWARD
 *
 *   3. CIRCULAR: Last node points back to first (no NULL)
 *      10 -> 20 -> 30 -+
 *      ^               |
 *      +---------------+
 *      Goes around forever!
 *
 * BASIC OPERATIONS YOU'LL LEARN:
 * ------------------------------
 *   1. CREATE a new node
 *   2. INSERT at beginning (easiest)
 *   3. INSERT at end (need to walk to end first)
 *   4. INSERT in middle (need to find the spot first)
 *   5. DELETE a node
 *   6. SEARCH for a value
 *   7. TRAVERSE (visit every node)
 *   8. REVERSE the list
 *
 * COMMON POINTER TERMINOLOGY:
 * ---------------------------
 *   head  = pointer to the FIRST node (if NULL, list is empty)
 *   tail  = pointer to the LAST node (optional, makes append O(1))
 *   curr  = "current" - pointer we move as we traverse
 *   prev  = "previous" - pointer to the node BEFORE curr
 *   next  = "next" - pointer to the node AFTER curr
 *   temp  = temporary pointer (to not lose a node while rewiring)
 *
 * GOLDEN RULE OF LINKED LISTS:
 * ----------------------------
 *   "ALWAYS save the next pointer BEFORE you change it!"
 *
 *   WRONG (you lose the rest of the list!):
 *     curr->next = new_node;           // Oops! We lost everything after curr!
 *     new_node->next = curr->next;     // curr->next is now new_node, not the old next!
 *
 *   CORRECT:
 *     new_node->next = curr->next;     // First, save where we're going
 *     curr->next = new_node;           // Now safe to redirect
 *
 * ============================================================================
 * BIG-O COMPLEXITY SUMMARY FOR LINKED LISTS
 * ============================================================================
 *
 * Operation              | Singly LL | Array/Vector | Notes
 * -----------------------|-----------|--------------|---------------------------
 * Access by index        | O(n)      | O(1)         | LL must traverse
 * Insert at head         | O(1)      | O(n)         | LL wins - no shifting
 * Insert at tail         | O(n)/O(1)*| O(1) amort.  | *O(1) if tail ptr kept
 * Insert at middle       | O(n)      | O(n)         | LL: traverse + O(1) insert
 * Delete at head         | O(1)      | O(n)         | LL wins - no shifting
 * Delete at tail         | O(n)      | O(1)         | LL must find prev node
 * Delete at middle       | O(n)      | O(n)         | Similar performance
 * Search                 | O(n)      | O(n)/O(logn) | Array can be sorted
 * Memory per element     | data+ptr  | data only    | LL has pointer overhead
 *
 * ============================================================================
 * WHEN TO USE LINKED LISTS IN EMBEDDED SYSTEMS
 * ============================================================================
 *
 * 1. DYNAMIC MEMORY POOLS / BUFFER MANAGEMENT
 *    - Free block lists in custom memory allocators
 *    - Packet buffer chains in network stacks
 *    - DMA descriptor chains
 *    Example: FreeRTOS uses LL for memory block management
 *
 * 2. TASK/EVENT QUEUES (RTOS)
 *    - Ready task lists (sorted by priority)
 *    - Timer lists (sorted by expiration time)
 *    - Deferred procedure call queues
 *    Example: Task scheduling in FreeRTOS, Zephyr RTOS
 *
 * 3. INTERRUPT HANDLING
 *    - ISR handler chains
 *    - Callback registration lists
 *    - Event subscriber lists
 *
 * 4. DEVICE DRIVER MANAGEMENT
 *    - Linked list of registered drivers
 *    - I/O request queues
 *    - Device enumeration lists
 *
 * 5. PROTOCOL STACKS
 *    - TCP/IP: packet fragment reassembly
 *    - CAN bus: message queues
 *    - USB: transfer descriptor lists
 *
 * 6. SENSOR DATA PIPELINES
 *    - Circular buffers with variable-size data
 *    - Timestamped sample chains
 *    - Filter chains (linked processing stages)
 *
 * ============================================================================
 * WHY LINKED LISTS OVER ARRAYS IN EMBEDDED?
 * ============================================================================
 *
 * ADVANTAGES:
 * - No need to know size at compile time
 * - O(1) insertion/deletion at known position (no shifting)
 * - No wasted pre-allocated memory
 * - Natural fit for queue/stack implementations
 * - Easy to splice/merge lists
 *
 * DISADVANTAGES:
 * - Pointer overhead (4-8 bytes per node)
 * - Poor cache locality (nodes scattered in memory)
 * - No random access - must traverse
 * - Dynamic allocation can fragment heap
 * - Risk of memory leaks if not managed properly
 *
 * EMBEDDED-SPECIFIC CONSIDERATIONS:
 * - Often use static node pools instead of malloc()
 * - Intrusive lists (node embedded in struct) save memory
 * - Single vs double linked depends on operations needed
 * - Consider cache line size for node layout
 *
 * ============================================================================
 * ADVANCED EMBEDDED TOPICS (INTERVIEW FAVORITES)
 * ============================================================================
 *
 * 1. INTRUSIVE VS NON-INTRUSIVE LINKED LISTS
 *    ----------------------------------------
 *    Non-intrusive (what we use here):
 *      struct Node { int data; Node* next; };  // Node wraps data
 *
 *    Intrusive (Linux kernel style):
 *      struct list_head { struct list_head *next, *prev; };
 *      struct my_device {
 *          int id;
 *          struct list_head list;  // Embedded inside the struct!
 *      };
 *
 *    WHY INTRUSIVE IS BETTER FOR EMBEDDED:
 *    - One allocation per object (not two: node + data)
 *    - Object can be on multiple lists simultaneously
 *    - No need for container_of() macro... wait, you DO need it:
 *      #define container_of(ptr, type, member) \
 *          ((type *)((char *)(ptr) - offsetof(type, member)))
 *    - Used in: Linux kernel, FreeRTOS, Zephyr, most RTOS
 *
 * 2. STATIC MEMORY POOLS (NO MALLOC)
 *    --------------------------------
 *    // Pre-allocate nodes at compile time
 *    #define MAX_NODES 32
 *    static Node node_pool[MAX_NODES];
 *    static Node* free_list = NULL;
 *
 *    void init_pool(void) {
 *        for (int i = 0; i < MAX_NODES - 1; i++)
 *            node_pool[i].next = &node_pool[i + 1];
 *        node_pool[MAX_NODES - 1].next = NULL;
 *        free_list = &node_pool[0];
 *    }
 *
 *    Node* pool_alloc(void) {
 *        Node* node = free_list;
 *        if (free_list) free_list = free_list->next;
 *        return node;  // NULL if pool exhausted
 *    }
 *
 *    void pool_free(Node* node) {
 *        node->next = free_list;
 *        free_list = node;
 *    }
 *
 *    WHY: Deterministic allocation time, no fragmentation, no heap
 *
 * 3. VOLATILE POINTERS (ISR SAFETY)
 *    -------------------------------
 *    When a list is modified in an ISR:
 *
 *    volatile Node* volatile head;  // Both pointer AND data volatile
 *
 *    // In ISR:
 *    void ISR_handler(void) {
 *        Node* new_node = pool_alloc();
 *        new_node->next = head;
 *        head = new_node;  // Must be atomic or protected!
 *    }
 *
 *    CRITICAL: Single pointer write is usually atomic on ARM,
 *    but read-modify-write is NOT. Use:
 *    - Disable interrupts (critical section)
 *    - ARM: LDREX/STREX instructions
 *    - Atomic built-ins: __atomic_store_n(), __atomic_load_n()
 *
 * 4. LOCK-FREE LISTS (ADVANCED)
 *    ---------------------------
 *    For multi-core or preemptive systems without disabling interrupts:
 *
 *    bool lock_free_push(Node** head, Node* new_node) {
 *        Node* old_head;
 *        do {
 *            old_head = __atomic_load_n(head, __ATOMIC_ACQUIRE);
 *            new_node->next = old_head;
 *        } while (!__atomic_compare_exchange_n(head, &old_head, new_node,
 *                  false, __ATOMIC_RELEASE, __ATOMIC_RELAXED));
 *        return true;
 *    }
 *
 *    INTERVIEW Q: "What is the ABA problem?"
 *    A: Thread 1 reads A, Thread 2 changes A->B->A, Thread 1's CAS succeeds
 *       but the list state is corrupted. Solution: tagged pointers or hazard ptrs
 *
 * 5. MEMORY ALIGNMENT & DMA
 *    -----------------------
 *    typedef struct __attribute__((aligned(4))) {
 *        uint32_t data;
 *        struct Node* next;
 *    } AlignedNode;
 *
 *    // For DMA descriptor chains (must be aligned to DMA requirements)
 *    typedef struct __attribute__((aligned(32))) {  // Cache line aligned
 *        uint32_t src_addr;
 *        uint32_t dst_addr;
 *        uint32_t length;
 *        struct DMA_Desc* next;
 *    } DMA_Descriptor;
 *
 * 6. CIRCULAR LINKED LISTS
 *    ----------------------
 *    Last node points to head. Common uses:
 *    - Round-robin schedulers
 *    - Ring buffers with variable-size entries
 *    - Continuous polling of devices
 *
 *    bool is_circular(Node* head) {
 *        // Same as cycle detection!
 *        // Floyd's algorithm works here too
 *    }
 *
 * 7. XOR LINKED LIST (MEMORY OPTIMIZATION)
 *    -------------------------------------
 *    Store XOR of prev and next instead of both pointers:
 *    node->ptr = prev_addr XOR next_addr
 *
 *    Traverse forward:  next = prev XOR node->ptr
 *    Traverse backward: prev = next XOR node->ptr
 *
 *    SAVES: 4-8 bytes per node (one pointer)
 *    COST:  More complex code, need two adjacent nodes to traverse
 *    INTERVIEW: Know the concept, rarely implemented in practice
 *
 * 8. COMMON BUGS & PITFALLS
 *    -----------------------
 *    a) Memory leaks: forgetting to free removed nodes
 *    b) Use-after-free: accessing freed nodes
 *    c) Dangling pointers: not updating all references
 *    d) Race conditions: ISR modifying list during traversal
 *    e) Stack overflow: recursive operations on long lists
 *    f) NULL dereference: not checking empty list / end of list
 *    g) Lost nodes: updating next before saving reference
 *
 *    DEFENSIVE PATTERN:
 *    void safe_delete(Node** head, Node* target) {
 *        if (!head || !*head || !target) return;  // Null checks
 *        // ... deletion logic
 *        target->next = NULL;  // Poison the deleted node
 *        target->data = 0xDEAD;  // Debug marker
 *        free(target);
 *    }
 *
 * 9. REAL INTERVIEW QUESTIONS
 *    -------------------------
 *    Q: "How would you implement a message queue for an RTOS?"
 *    A: Linked list with head/tail pointers, ISR-safe enqueue,
 *       priority ordering, static memory pool
 *
 *    Q: "Your linked list operations are slow. How do you debug?"
 *    A: Check for accidental O(n^2) loops, cache thrashing (use pool),
 *       measure with cycle counter, consider array if random access needed
 *
 *    Q: "How do you safely modify a list from both ISR and main loop?"
 *    A: Disable interrupts (short critical section), or use lock-free
 *       algorithms, or use a "pending" list that ISR writes to
 *
 *    Q: "Explain container_of macro"
 *    A: Given pointer to member, returns pointer to containing struct
 *       using offsetof() to calculate the base address
 *
 * ============================================================================
 * Key interview considerations for embedded systems:
 * - Memory constraints (avoid excessive malloc)
 * - No recursion when possible (stack limitations)
 * - In-place algorithms preferred
 * - Pointer manipulation skills are essential
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* ============================================================================
 * BIG-O FOR IMPLEMENTED ALGORITHMS
 * ============================================================================
 *
 * #  | Problem                      | Time       | Space    | Technique
 * ---|------------------------------|------------|----------|--------------------
 * 1  | Reverse Linked List          | O(n)       | O(1)     | Three pointers
 * 2  | Detect Cycle + Find Start    | O(n)       | O(1)     | Floyd's algorithm
 * 3  | Merge Two Sorted Lists       | O(n+m)     | O(1)     | Two pointers + dummy
 * 4  | Remove Nth From End          | O(n)       | O(1)     | Two pointers gap
 * 5  | Find Middle                  | O(n)       | O(1)     | Slow/fast pointers
 * 6  | Palindrome Check             | O(n)       | O(1)     | Reverse half in-place
 * 7  | Intersection of Two Lists    | O(n+m)     | O(1)     | Length diff or switch
 * 8  | Reverse K-Group              | O(n)       | O(1)     | Group reversal
 * 9  | Sort List (Merge Sort)       | O(n log n) | O(log n) | Divide & conquer
 * 10 | Copy with Random Pointer     | O(n)       | O(1)*    | Interleaving trick
 *
 * *Note: Problem 10 achieves O(1) extra space by interleaving nodes.
 *        A hash map solution would be O(n) space but simpler to implement.
 *
 * ============================================================================ */

/* ============================================================================
 * NODE STRUCTURE - THE BUILDING BLOCK OF A LINKED LIST
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * A "Node" is like a box with two compartments:
 *   +--------+--------+
 *   |  data  |  next  |
 *   | (value)|(pointer)|
 *   +--------+--------+
 *
 * - "data" holds the actual value we want to store (like a number)
 * - "next" holds the memory address of the next node (like an arrow pointing forward)
 *
 * When we write: node->next
 * We're saying: "Follow the arrow from this node to the next one"
 *
 * When we write: node->data
 * We're saying: "Give me the value stored in this node"
 *
 * MEMORY VISUALIZATION:
 *   Memory Address:  0x100        0x200        0x300
 *                   +------+     +------+     +------+
 *                   |  10  |     |  20  |     |  30  |
 *                   | 0x200|---->| 0x300|---->| NULL |
 *                   +------+     +------+     +------+
 *
 *   - Node at 0x100 has data=10, next points to address 0x200
 *   - Node at 0x200 has data=20, next points to address 0x300
 *   - Node at 0x300 has data=30, next is NULL (end of list)
 *
 * ============================================================================ */

typedef struct Node {
    int data;               // The value this node holds (could be any type)
    struct Node* next;      // Pointer to the next node (NULL if last node)
    struct Node* random;    // Extra pointer used for problem #10 (ignore for now)
} Node;
/*
 * WHY "struct Node*" and not just "Node*"?
 * At this point in the code, we're still DEFINING what "Node" is.
 * The typedef hasn't finished yet, so we must use "struct Node".
 * This is a "self-referential structure" - the node contains a pointer to
 * another node of the same type.
 */

/* ============================================================================
 * UTILITY FUNCTIONS - BASIC OPERATIONS EVERY LINKED LIST NEEDS
 * ============================================================================ */

/*
 * createNode - Allocate memory for a new node and initialize it
 *
 * BEGINNER EXPLANATION:
 * Think of malloc() like asking the computer: "Please give me a box big enough
 * to hold one Node."
 *
 * The computer responds with the address where it put the box, which we store
 * in "newNode" so we can find it later.
 *
 * STEP BY STEP:
 * 1. malloc(sizeof(Node)) - Ask for memory the size of one Node
 * 2. (Node*) - Tell C this memory will be used as a Node pointer
 * 3. If malloc succeeded (newNode is not NULL):
 *    a. Store the data value in the node
 *    b. Set next to NULL (this node points nowhere yet)
 *    c. Set random to NULL (not used in most problems)
 * 4. Return the pointer to this new node
 *
 * VISUAL:
 *   Before: We have nothing
 *   After:  newNode ---> +------+------+
 *                        |  42  | NULL |
 *                        +------+------+
 */
Node* createNode(int data) {
    // Ask operating system for memory to store one Node
    Node* newNode = (Node*)malloc(sizeof(Node));

    // Always check if malloc succeeded! (it returns NULL if out of memory)
    if (newNode) {
        newNode->data = data;      // Store the value
        newNode->next = NULL;      // New node doesn't point anywhere yet
        newNode->random = NULL;    // Not used in basic operations
    }
    return newNode;  // Return pointer to our new node (or NULL if failed)
}

/*
 * printList - Walk through the list and print each value
 *
 * BEGINNER EXPLANATION:
 * This is called "TRAVERSAL" - visiting every node in order.
 *
 * We use "head" as our moving pointer. Starting from the first node,
 * we keep following the "next" arrows until we hit NULL.
 *
 * VISUAL WALKTHROUGH for list: 10 -> 20 -> 30 -> NULL
 *
 *   Iteration 1: head points to 10, print "10 -> ", move head to next (20)
 *   Iteration 2: head points to 20, print "20 -> ", move head to next (30)
 *   Iteration 3: head points to 30, print "30 -> ", move head to next (NULL)
 *   Iteration 4: head is NULL, exit loop, print "NULL"
 *
 * NOTE: We're modifying "head" inside this function, but that's OK!
 * In C, parameters are passed by VALUE, so we're modifying a COPY of head,
 * not the original pointer in the calling function.
 */
void printList(Node* head) {
    printf("List: ");

    // Keep going while head is not NULL
    while (head) {                      // Same as: while (head != NULL)
        printf("%d -> ", head->data);   // Print current node's data
        head = head->next;              // Move to the next node
    }

    printf("NULL\n");  // Mark the end of the list
}

/*
 * freeList - Release all memory used by the list
 *
 * BEGINNER EXPLANATION:
 * Memory allocated with malloc() MUST be freed, or you have a "memory leak".
 * In embedded systems with limited RAM, this can crash your device!
 *
 * CRITICAL CONCEPT - Why we need "temp":
 *   If we just did: free(head); head = head->next;
 *   After free(head), the memory is gone! We can't access head->next anymore!
 *   This is "use-after-free" - a serious bug.
 *
 * CORRECT APPROACH:
 *   1. Save the next pointer BEFORE freeing
 *   2. Free the current node
 *   3. Move to the saved next pointer
 *
 * VISUAL WALKTHROUGH for list: 10 -> 20 -> 30 -> NULL
 *
 *   Iteration 1: temp=10's node, head moves to 20, free temp (10 is gone)
 *   Iteration 2: temp=20's node, head moves to 30, free temp (20 is gone)
 *   Iteration 3: temp=30's node, head moves to NULL, free temp (30 is gone)
 *   Iteration 4: head is NULL, exit loop. All memory freed!
 */
void freeList(Node* head) {
    Node* temp;     // Temporary pointer to hold node we're about to free

    while (head) {                  // While there are nodes to free
        temp = head;                // Save current node
        head = head->next;          // Move head to next BEFORE freeing
        free(temp);                 // Now safe to free the saved node
    }
    // After this loop, all nodes are freed and head is NULL
}

/*
 * createListFromArray - Build a linked list from an array of values
 *
 * BEGINNER EXPLANATION:
 * This helper function takes an array like [1, 2, 3, 4, 5] and creates:
 *   1 -> 2 -> 3 -> 4 -> 5 -> NULL
 *
 * VISUAL STEP-BY-STEP for arr = [10, 20, 30]:
 *
 *   Start: head = createNode(10)
 *          head ---> [10|NULL]
 *          current = head
 *
 *   i=1:   current->next = createNode(20)
 *          head ---> [10| *]----> [20|NULL]
 *          current = current->next (now points to 20's node)
 *
 *   i=2:   current->next = createNode(30)
 *          head ---> [10| *]----> [20| *]----> [30|NULL]
 *          current = current->next (now points to 30's node)
 *
 *   Return head (which still points to the first node)
 *
 * KEY INSIGHT:
 *   We use "current" to walk to the end as we build the list.
 *   "head" never moves - it always points to the first node.
 *   This way we can return "head" at the end to give access to the whole list.
 */
Node* createListFromArray(int arr[], int size) {
    // Edge case: empty array means empty list
    if (size == 0) return NULL;

    // Create the first node (this becomes our head)
    Node* head = createNode(arr[0]);

    // "current" is our moving pointer for building the list
    Node* current = head;

    // Create remaining nodes and link them
    for (int i = 1; i < size; i++) {
        current->next = createNode(arr[i]);  // Create new node and link it
        current = current->next;              // Move current to the new node
    }

    return head;  // Return pointer to first node
}

/* ============================================================================
 * 1. REVERSE LINKED LIST
 * Time: O(n), Space: O(1)
 * Critical for embedded: In-place, iterative (no stack overflow)
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * "Reversing" means changing the direction of all arrows.
 *
 * BEFORE:  1 -> 2 -> 3 -> 4 -> 5 -> NULL
 * AFTER:   NULL <- 1 <- 2 <- 3 <- 4 <- 5
 *          (which is: 5 -> 4 -> 3 -> 2 -> 1 -> NULL when read from new head)
 *
 * THE APPROACH - Use THREE pointers:
 *   - prev    = the node we just passed (starts as NULL)
 *   - current = the node we're at right now
 *   - next    = the node we're about to visit (saved before we change things)
 *
 * VISUAL WALKTHROUGH for list 1 -> 2 -> 3 -> NULL:
 *
 *   Initial:  prev=NULL, current=1, next=?
 *
 *   ITERATION 1 (at node 1):
 *       Step a: next = current->next        // next = 2 (save it!)
 *               NULL    1 -> 2 -> 3 -> NULL
 *               prev    curr next
 *
 *       Step b: current->next = prev        // 1 now points to NULL
 *               NULL <- 1    2 -> 3 -> NULL
 *               prev   curr  next
 *
 *       Step c: prev = current              // prev moves to 1
 *       Step d: current = next              // current moves to 2
 *               NULL <- 1    2 -> 3 -> NULL
 *                      prev curr
 *
 *   ITERATION 2 (at node 2):
 *       Step a: next = 3
 *       Step b: 2 now points to 1           // NULL <- 1 <- 2    3 -> NULL
 *       Step c: prev = 2
 *       Step d: current = 3
 *
 *   ITERATION 3 (at node 3):
 *       Step a: next = NULL
 *       Step b: 3 now points to 2           // NULL <- 1 <- 2 <- 3    NULL
 *       Step c: prev = 3
 *       Step d: current = NULL
 *
 *   current is NULL, exit loop.
 *   Return prev (which is 3, our new head!)
 *
 *   Result: 3 -> 2 -> 1 -> NULL  (REVERSED!)
 *
 * WHY IS THIS AN INTERVIEW FAVORITE?
 *   - Tests understanding of pointers
 *   - Must save next BEFORE changing it
 *   - O(1) space - no extra arrays or stacks
 *   - Common in embedded: reversing data streams, undo buffers
 * ============================================================================ */

Node* reverseList(Node* head) {
    Node* prev = NULL;      // Will become the new head's "next" (nothing before head)
    Node* current = head;   // Start at the beginning
    Node* next = NULL;      // Will save the next node before we change pointers

    // Walk through the entire list
    while (current) {
        // STEP 1: Save the next node (we're about to lose access to it!)
        next = current->next;

        // STEP 2: Reverse the arrow - point current node backward to prev
        current->next = prev;

        // STEP 3: Move prev forward (prev is now at current's position)
        prev = current;

        // STEP 4: Move current forward (to the node we saved in step 1)
        current = next;
    }

    // When loop ends, current is NULL and prev points to the last node
    // That last node is now our NEW head (first node of reversed list)
    return prev;
}

void demo_reverseList(void) {
    printf("\n=== 1. REVERSE LINKED LIST ===\n");
    int arr[] = {1, 2, 3, 4, 5};
    Node* head = createListFromArray(arr, 5);

    printf("Original: ");
    printList(head);

    head = reverseList(head);

    printf("Reversed: ");
    printList(head);

    freeList(head);
}

/* ============================================================================
 * 2. DETECT CYCLE (Floyd's Algorithm) AND FIND CYCLE START
 * Time: O(n), Space: O(1)
 * Important: No extra memory for hash table
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * A "cycle" is when a node points back to an earlier node, creating a loop.
 * Normal list:    1 -> 2 -> 3 -> 4 -> 5 -> NULL  (ends normally)
 * List with cycle: 1 -> 2 -> 3 -> 4 -> 5 -+
 *                            ^             |
 *                            +-------------+  (5 points back to 3, never ends!)
 *
 * WHY IS THIS A PROBLEM?
 *   - If you try to traverse or print a cyclic list, you loop forever!
 *   - Your program hangs or crashes (infinite loop)
 *   - In embedded systems, this can freeze your device
 *
 * THE "TORTOISE AND HARE" ALGORITHM (Floyd's Algorithm):
 *   Imagine two runners on a circular track:
 *   - Tortoise (slow): moves 1 step at a time
 *   - Hare (fast): moves 2 steps at a time
 *
 *   If the track is straight (no cycle), the hare reaches the end first.
 *   If the track is circular (has cycle), the hare will eventually LAP the tortoise!
 *   They MUST meet if there's a cycle (mathematical proof exists).
 *
 * VISUAL WALKTHROUGH for: 1 -> 2 -> 3 -> 4 -> 5 -> 3 (cycle back to 3)
 *
 *       1 -> 2 -> 3 -> 4 -> 5
 *                 ^         |
 *                 +---------+
 *
 *   Start:  slow=1, fast=1
 *   Step 1: slow=2, fast=3
 *   Step 2: slow=3, fast=5
 *   Step 3: slow=4, fast=4  <- They meet at node 4! CYCLE DETECTED!
 *
 * WHY CHECK "fast && fast->next"?
 *   - If no cycle, fast will reach NULL (end of list)
 *   - We check fast->next because fast moves 2 steps
 *   - If fast is at last node, fast->next->next would crash!
 *
 * INTERVIEW FAVORITE BECAUSE:
 *   - Tests understanding of two-pointer technique
 *   - O(1) space - no hash table or array needed
 *   - Critical for memory management in embedded systems
 * ============================================================================ */

bool hasCycle(Node* head) {
    Node* slow = head;  // Tortoise - moves 1 step
    Node* fast = head;  // Hare - moves 2 steps

    // Keep going while fast can take 2 steps
    while (fast && fast->next) {
        slow = slow->next;           // Tortoise: 1 step
        fast = fast->next->next;     // Hare: 2 steps

        // If they meet, there's a cycle!
        if (slow == fast) {
            return true;
        }
    }

    // If we get here, fast reached the end - no cycle
    return false;
}

/*
 * findCycleStart - Find WHERE the cycle begins
 *
 * BEGINNER EXPLANATION:
 * Once we know there's a cycle, we often need to know WHERE it starts.
 * This is useful for debugging or breaking the cycle.
 *
 * THE MATHEMATICAL TRICK:
 * After slow and fast meet inside the cycle, there's a beautiful property:
 *   - Reset slow to head
 *   - Move BOTH pointers 1 step at a time
 *   - They will meet at the cycle's starting node!
 *
 * WHY DOES THIS WORK? (simplified)
 *   Let's say:
 *   - Distance from head to cycle start = A
 *   - Distance from cycle start to meeting point = B
 *   - Cycle length = C
 *
 *   When they meet:
 *   - Slow traveled: A + B
 *   - Fast traveled: A + B + (some number of complete cycles)
 *   - Fast travels 2x slow's distance, so: 2(A+B) = A + B + k*C
 *   - Simplifying: A = k*C - B
 *
 *   This means: walking A steps from head = walking (k*C - B) steps from meeting point
 *   Both paths end up at the cycle start!
 *
 * VISUAL:
 *   Head                  Cycle Start     Meeting Point
 *    |                        |               |
 *    v                        v               v
 *    1 ----> 2 ----> 3 ----> 4 ----> 5 ----> 6
 *                            ^                |
 *                            +----------------+
 *    |<-------- A -------->|<----- B ----->|
 *
 *   After meeting: Reset slow to head, move both at same speed
 *   They meet at node 4 (cycle start)!
 */
Node* findCycleStart(Node* head) {
    Node* slow = head;
    Node* fast = head;

    // Phase 1: Detect cycle
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;

        if (slow == fast) {
            // Phase 2: Find cycle start
            // Reset slow to head, move both at same speed
            slow = head;
            while (slow != fast) {
                slow = slow->next;
                fast = fast->next;
            }
            return slow;  // Cycle start node
        }
    }
    return NULL;  // No cycle
}

void demo_detectCycle(void) {
    printf("\n=== 2. DETECT CYCLE & FIND CYCLE START ===\n");

    // Create list with cycle: 1->2->3->4->5->3 (cycle back to 3)
    Node* head = createNode(1);
    head->next = createNode(2);
    head->next->next = createNode(3);
    head->next->next->next = createNode(4);
    head->next->next->next->next = createNode(5);
    head->next->next->next->next->next = head->next->next;  // Create cycle to node 3

    printf("List with cycle (1->2->3->4->5->3...)\n");
    printf("Has cycle: %s\n", hasCycle(head) ? "YES" : "NO");

    Node* cycleStart = findCycleStart(head);
    if (cycleStart) {
        printf("Cycle starts at node with value: %d\n", cycleStart->data);
    }

    // Break cycle before freeing
    head->next->next->next->next->next = NULL;
    freeList(head);

    // Test without cycle
    int arr[] = {1, 2, 3, 4, 5};
    Node* noCycleList = createListFromArray(arr, 5);
    printf("\nList without cycle: ");
    printList(noCycleList);
    printf("Has cycle: %s\n", hasCycle(noCycleList) ? "YES" : "NO");
    freeList(noCycleList);
}

/* ============================================================================
 * 3. MERGE TWO SORTED LISTS
 * Time: O(n+m), Space: O(1)
 * Iterative approach - preferred for embedded
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * Given two lists that are ALREADY SORTED, combine them into ONE sorted list.
 *
 * L1: 1 -> 3 -> 5 -> NULL
 * L2: 2 -> 4 -> 6 -> NULL
 * Result: 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> NULL
 *
 * THE APPROACH:
 * Like merging two sorted stacks of playing cards:
 *   1. Look at the top card of each stack
 *   2. Take the smaller one and put it in your result pile
 *   3. Repeat until one stack is empty
 *   4. Put remaining cards from non-empty stack onto result
 *
 * THE "DUMMY NODE" TRICK:
 * A common pattern to simplify code. Instead of handling "first node" specially:
 *
 *   Without dummy (messy):
 *     if (head == NULL) head = newNode;  // Special case for first node
 *     else tail->next = newNode;          // Normal case for others
 *
 *   With dummy (clean):
 *     dummy.next = NULL;  // Dummy is a fake "node before head"
 *     tail = &dummy;       // tail always points to last node
 *     tail->next = newNode; // Same code works for ALL nodes
 *     return dummy.next;   // Skip dummy, return actual head
 *
 * VISUAL WALKTHROUGH:
 *   L1: 1 -> 3 -> 5        L2: 2 -> 4
 *
 *   Initial: dummy -> NULL, tail points to dummy
 *
 *   Step 1: L1(1) < L2(2), take 1 from L1
 *           dummy -> 1, L1 moves to 3, tail moves to 1
 *
 *   Step 2: L1(3) > L2(2), take 2 from L2
 *           dummy -> 1 -> 2, L2 moves to 4, tail moves to 2
 *
 *   Step 3: L1(3) < L2(4), take 3 from L1
 *           dummy -> 1 -> 2 -> 3, L1 moves to 5
 *
 *   Step 4: L1(5) > L2(4), take 4 from L2
 *           dummy -> 1 -> 2 -> 3 -> 4, L2 becomes NULL
 *
 *   L2 is NULL, attach remaining L1:
 *           dummy -> 1 -> 2 -> 3 -> 4 -> 5
 *
 *   Return dummy.next = node with 1 (the real head)
 *
 * EMBEDDED BONUS: We allocate dummy on STACK (not heap), so no malloc needed!
 * ============================================================================ */

Node* mergeTwoSortedLists(Node* l1, Node* l2) {
    Node dummy;              // Stack-allocated dummy node (no malloc!)
    Node* tail = &dummy;     // tail tracks end of merged list
    dummy.next = NULL;

    // While both lists have nodes to compare
    while (l1 && l2) {
        if (l1->data <= l2->data) {
            // L1's node is smaller (or equal), take it
            tail->next = l1;
            l1 = l1->next;    // Move L1 pointer forward
        } else {
            // L2's node is smaller, take it
            tail->next = l2;
            l2 = l2->next;    // Move L2 pointer forward
        }
        tail = tail->next;    // Move tail to the node we just added
    }

    // Attach remaining nodes (one list might still have nodes)
    // The ternary "l1 ? l1 : l2" means: if l1 is not NULL, use l1, else use l2
    tail->next = l1 ? l1 : l2;

    return dummy.next;  // Skip dummy, return the actual head
}

void demo_mergeSortedLists(void) {
    printf("\n=== 3. MERGE TWO SORTED LISTS ===\n");
    int arr1[] = {1, 3, 5, 7};
    int arr2[] = {2, 4, 6, 8, 10};

    Node* l1 = createListFromArray(arr1, 4);
    Node* l2 = createListFromArray(arr2, 5);

    printf("List 1: ");
    printList(l1);
    printf("List 2: ");
    printList(l2);

    Node* merged = mergeTwoSortedLists(l1, l2);

    printf("Merged: ");
    printList(merged);

    freeList(merged);
}

/* ============================================================================
 * 4. REMOVE NTH NODE FROM END
 * Time: O(n), Space: O(1)
 * Two-pointer technique - single pass
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * Remove the Nth node counting from the END of the list.
 *
 * Example: Remove 2nd from end in 1->2->3->4->5
 *   Counting from end: 5 is 1st, 4 is 2nd, 3 is 3rd...
 *   So remove node 4: Result is 1->2->3->5
 *
 * THE CHALLENGE:
 * In a singly linked list, we can only go FORWARD. We can't count backwards!
 * Naive approach: Walk to end to count length L, then walk to node (L - n).
 *   That requires TWO passes through the list.
 *
 * THE CLEVER TRICK - Two Pointers with Gap:
 *   1. Create a "gap" of n nodes between two pointers
 *   2. Move both pointers together
 *   3. When fast hits the end, slow is at the right spot!
 *
 * VISUAL for removing 2nd from end in 1->2->3->4->5:
 *
 *   Step 1: Move fast pointer n+1 steps ahead (n+1 = 3 steps)
 *           dummy -> 1 -> 2 -> 3 -> 4 -> 5 -> NULL
 *           slow          fast
 *           (slow at dummy, fast at 3)
 *
 *   Step 2: Move both until fast is NULL
 *           dummy -> 1 -> 2 -> 3 -> 4 -> 5 -> NULL
 *                              slow          fast
 *           (slow at 3, fast at NULL)
 *
 *   Step 3: Delete slow->next (which is 4)
 *           slow->next = slow->next->next  (3 now points to 5)
 *           Result: 1 -> 2 -> 3 -> 5 -> NULL
 *
 * WHY n+1 STEPS?
 *   We want slow to stop at the node BEFORE the one to delete.
 *   Because we need to change its "next" pointer.
 *   slow->next is the node to remove.
 *
 * WHY USE DUMMY NODE?
 *   Edge case: What if we need to remove the HEAD (1st node from end)?
 *   Without dummy, there's no "node before head" to update.
 *   With dummy: dummy->next = head, and we can remove head by
 *   changing dummy->next. Clean and consistent!
 * ============================================================================ */

Node* removeNthFromEnd(Node* head, int n) {
    Node dummy;             // Dummy node to handle edge case of removing head
    dummy.next = head;
    Node* fast = &dummy;    // Fast pointer starts at dummy
    Node* slow = &dummy;    // Slow pointer starts at dummy

    // Move fast n+1 steps ahead to create the gap
    for (int i = 0; i <= n; i++) {
        if (!fast) return head;  // n is larger than list size (safety check)
        fast = fast->next;
    }

    // Move both pointers until fast reaches the end (NULL)
    while (fast) {
        fast = fast->next;
        slow = slow->next;
    }

    // Now slow is at the node BEFORE the one to delete
    Node* toDelete = slow->next;         // The node to remove
    slow->next = slow->next->next;       // Skip over the removed node
    free(toDelete);                       // Free the memory!

    return dummy.next;  // Return head (might have changed if we removed original head)
}

void demo_removeNthFromEnd(void) {
    printf("\n=== 4. REMOVE NTH NODE FROM END ===\n");
    int arr[] = {1, 2, 3, 4, 5};
    Node* head = createListFromArray(arr, 5);

    printf("Original: ");
    printList(head);

    int n = 2;
    printf("Removing %dth node from end...\n", n);
    head = removeNthFromEnd(head, n);

    printf("Result:   ");
    printList(head);

    freeList(head);
}

/* ============================================================================
 * 5. MIDDLE OF LINKED LIST
 * Time: O(n), Space: O(1)
 * Slow/Fast pointer technique
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * Find the middle node of a linked list in ONE pass.
 *
 * Odd length:  1 -> 2 -> 3 -> 4 -> 5  -> NULL (5 nodes)
 *                        ^
 *                      middle (node 3)
 *
 * Even length: 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> NULL (6 nodes)
 *                        ^    ^
 *              first middle  second middle
 *              (this function returns second middle = 4)
 *
 * THE CHALLENGE:
 * Unlike arrays where we can instantly access middle[length/2],
 * linked lists don't know their length without traversing.
 *
 * NAIVE APPROACH:
 *   Pass 1: Count all nodes to get length
 *   Pass 2: Walk length/2 steps to reach middle
 *   This requires TWO passes.
 *
 * THE CLEVER TRICK - Tortoise and Hare (again!):
 *   - Slow pointer moves 1 step at a time
 *   - Fast pointer moves 2 steps at a time
 *   - When fast reaches the end, slow is at the middle!
 *
 * WHY DOES IT WORK?
 *   Think about it: fast travels twice as far as slow.
 *   When fast has traveled the whole list (length L),
 *   slow has traveled half the list (L/2) = middle!
 *
 * VISUAL WALKTHROUGH for 1->2->3->4->5->NULL:
 *
 *   Start:  slow=1, fast=1
 *
 *   Step 1: slow moves to 2, fast moves to 3
 *           1 -> 2 -> 3 -> 4 -> 5 -> NULL
 *                s         f
 *
 *   Step 2: slow moves to 3, fast moves to 5
 *           1 -> 2 -> 3 -> 4 -> 5 -> NULL
 *                     s              f
 *
 *   Check: fast(5)->next exists? Yes (NULL). fast->next->next? No!
 *          Loop condition "fast && fast->next" fails.
 *          slow is at 3, which is the middle!
 *
 * THIS PATTERN IS EVERYWHERE:
 *   - Finding middle (this function)
 *   - Detecting cycles (Floyd's algorithm)
 *   - Finding start of cycle
 *   - Checking palindrome (need to find middle to split list)
 * ============================================================================ */

Node* findMiddle(Node* head) {
    Node* slow = head;  // Tortoise - moves 1 step
    Node* fast = head;  // Hare - moves 2 steps

    // Move until fast can't take 2 more steps
    while (fast && fast->next) {
        slow = slow->next;           // Slow moves 1 step
        fast = fast->next->next;     // Fast moves 2 steps
    }

    // When fast reaches end, slow is at middle
    return slow;  // Note: For even length, returns second middle node
}

void demo_findMiddle(void) {
    printf("\n=== 5. MIDDLE OF LINKED LIST ===\n");

    // Odd length
    int arr1[] = {1, 2, 3, 4, 5};
    Node* head1 = createListFromArray(arr1, 5);
    printf("List (odd):  ");
    printList(head1);
    Node* mid1 = findMiddle(head1);
    printf("Middle node value: %d\n", mid1->data);
    freeList(head1);

    // Even length
    int arr2[] = {1, 2, 3, 4, 5, 6};
    Node* head2 = createListFromArray(arr2, 6);
    printf("List (even): ");
    printList(head2);
    Node* mid2 = findMiddle(head2);
    printf("Middle node value: %d (second middle)\n", mid2->data);
    freeList(head2);
}

/* ============================================================================
 * 6. PALINDROME LINKED LIST
 * Time: O(n), Space: O(1)
 * In-place check: find middle, reverse second half, compare
 * ============================================================================ */

bool isPalindrome(Node* head) {
    if (!head || !head->next) return true;

    // Find middle
    Node* slow = head;
    Node* fast = head;
    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // Reverse second half
    Node* secondHalf = reverseList(slow->next);
    Node* secondHalfHead = secondHalf;  // Save for restoration

    // Compare first and second half
    Node* firstHalf = head;
    bool result = true;
    while (secondHalf) {
        if (firstHalf->data != secondHalf->data) {
            result = false;
            break;
        }
        firstHalf = firstHalf->next;
        secondHalf = secondHalf->next;
    }

    // Restore the list (optional but good practice)
    slow->next = reverseList(secondHalfHead);

    return result;
}

void demo_isPalindrome(void) {
    printf("\n=== 6. PALINDROME LINKED LIST ===\n");

    int arr1[] = {1, 2, 3, 2, 1};
    Node* head1 = createListFromArray(arr1, 5);
    printf("List: ");
    printList(head1);
    printf("Is palindrome: %s\n", isPalindrome(head1) ? "YES" : "NO");
    freeList(head1);

    int arr2[] = {1, 2, 3, 4, 5};
    Node* head2 = createListFromArray(arr2, 5);
    printf("List: ");
    printList(head2);
    printf("Is palindrome: %s\n", isPalindrome(head2) ? "YES" : "NO");
    freeList(head2);
}

/* ============================================================================
 * 7. INTERSECTION OF TWO LINKED LISTS
 * Time: O(n+m), Space: O(1)
 * Two-pointer technique with length equalization
 * ============================================================================ */

int getLength(Node* head) {
    int len = 0;
    while (head) {
        len++;
        head = head->next;
    }
    return len;
}

Node* getIntersectionNode(Node* headA, Node* headB) {
    if (!headA || !headB) return NULL;

    Node* pA = headA;
    Node* pB = headB;

    // When one pointer reaches end, redirect to other list's head
    // Both will meet at intersection or NULL after at most 2 passes
    while (pA != pB) {
        pA = pA ? pA->next : headB;
        pB = pB ? pB->next : headA;
    }

    return pA;
}

void demo_intersection(void) {
    printf("\n=== 7. INTERSECTION OF TWO LINKED LISTS ===\n");

    // Create intersecting lists:
    // List A: 1 -> 2 -----+
    //                     +--> 6 -> 7 -> 8
    // List B: 3 -> 4 -> 5-+

    Node* common = createNode(6);
    common->next = createNode(7);
    common->next->next = createNode(8);

    Node* headA = createNode(1);
    headA->next = createNode(2);
    headA->next->next = common;

    Node* headB = createNode(3);
    headB->next = createNode(4);
    headB->next->next = createNode(5);
    headB->next->next->next = common;

    printf("List A: 1 -> 2 -> 6 -> 7 -> 8\n");
    printf("List B: 3 -> 4 -> 5 -> 6 -> 7 -> 8\n");
    printf("(Lists intersect at node 6)\n");

    Node* intersection = getIntersectionNode(headA, headB);
    if (intersection) {
        printf("Intersection at node with value: %d\n", intersection->data);
    } else {
        printf("No intersection\n");
    }

    // Careful freeing - don't double free common part
    free(headA->next);
    free(headA);
    free(headB->next->next);
    free(headB->next);
    free(headB);
    freeList(common);
}

/* ============================================================================
 * 8. REVERSE NODES IN K-GROUP
 * Time: O(n), Space: O(1)
 * Advanced problem - tests pointer manipulation skills
 * ============================================================================ */

Node* reverseKGroup(Node* head, int k) {
    if (!head || k == 1) return head;

    Node dummy;
    dummy.next = head;
    Node* prevGroupEnd = &dummy;

    while (true) {
        // Check if k nodes exist
        Node* kthNode = prevGroupEnd;
        for (int i = 0; i < k; i++) {
            kthNode = kthNode->next;
            if (!kthNode) {
                return dummy.next;  // Less than k nodes remaining
            }
        }

        // Reverse k nodes
        Node* groupStart = prevGroupEnd->next;
        Node* nextGroupStart = kthNode->next;

        // Reverse the group
        Node* prev = nextGroupStart;
        Node* curr = groupStart;
        while (curr != nextGroupStart) {
            Node* next = curr->next;
            curr->next = prev;
            prev = curr;
            curr = next;
        }

        // Connect with previous part
        prevGroupEnd->next = kthNode;
        prevGroupEnd = groupStart;
    }

    return dummy.next;
}

void demo_reverseKGroup(void) {
    printf("\n=== 8. REVERSE NODES IN K-GROUP ===\n");
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8};
    Node* head = createListFromArray(arr, 8);

    printf("Original: ");
    printList(head);

    int k = 3;
    printf("Reversing in groups of %d...\n", k);
    head = reverseKGroup(head, k);

    printf("Result:   ");
    printList(head);

    freeList(head);
}

/* ============================================================================
 * 9. SORT LIST (MERGE SORT)
 * Time: O(n log n), Space: O(log n) for recursion
 * Preferred for linked lists - no random access needed
 * ============================================================================ */

Node* getMiddleForSort(Node* head) {
    Node* slow = head;
    Node* fast = head->next;  // Start fast one ahead for proper split

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

Node* sortList(Node* head) {
    // Base case
    if (!head || !head->next) return head;

    // Split list in half
    Node* mid = getMiddleForSort(head);
    Node* rightHead = mid->next;
    mid->next = NULL;  // Split the list

    // Recursively sort both halves
    Node* left = sortList(head);
    Node* right = sortList(rightHead);

    // Merge sorted halves
    return mergeTwoSortedLists(left, right);
}

void demo_sortList(void) {
    printf("\n=== 9. SORT LIST (MERGE SORT) ===\n");
    int arr[] = {4, 2, 1, 3, 8, 5, 7, 6};
    Node* head = createListFromArray(arr, 8);

    printf("Unsorted: ");
    printList(head);

    head = sortList(head);

    printf("Sorted:   ");
    printList(head);

    freeList(head);
}

/* ============================================================================
 * 10. COPY LIST WITH RANDOM POINTER
 * Time: O(n), Space: O(1) - clever interleaving technique
 * Important for embedded: No hash map needed
 * ============================================================================ */

Node* copyRandomList(Node* head) {
    if (!head) return NULL;

    // Step 1: Create interleaved list (original -> copy -> original -> copy)
    Node* curr = head;
    while (curr) {
        Node* copy = createNode(curr->data);
        copy->next = curr->next;
        curr->next = copy;
        curr = copy->next;
    }

    // Step 2: Set random pointers for copies
    curr = head;
    while (curr) {
        if (curr->random) {
            curr->next->random = curr->random->next;
        }
        curr = curr->next->next;
    }

    // Step 3: Separate the two lists
    Node* newHead = head->next;
    curr = head;
    Node* copyCurr = newHead;

    while (curr) {
        curr->next = curr->next->next;
        if (copyCurr->next) {
            copyCurr->next = copyCurr->next->next;
        }
        curr = curr->next;
        copyCurr = copyCurr->next;
    }

    return newHead;
}

void printListWithRandom(Node* head) {
    printf("List with random pointers:\n");
    Node* curr = head;
    int index = 0;
    while (curr) {
        printf("  Node[%d] data=%d, random->", index, curr->data);
        if (curr->random) {
            printf("%d\n", curr->random->data);
        } else {
            printf("NULL\n");
        }
        curr = curr->next;
        index++;
    }
}

void demo_copyRandomList(void) {
    printf("\n=== 10. COPY LIST WITH RANDOM POINTER ===\n");

    // Create list: 1 -> 2 -> 3 -> 4 -> 5
    Node* head = createNode(1);
    head->next = createNode(2);
    head->next->next = createNode(3);
    head->next->next->next = createNode(4);
    head->next->next->next->next = createNode(5);

    // Set random pointers
    head->random = head->next->next;           // 1 -> 3
    head->next->random = head;                  // 2 -> 1
    head->next->next->random = head->next->next->next->next;  // 3 -> 5
    head->next->next->next->random = head->next;  // 4 -> 2
    head->next->next->next->next->random = NULL;  // 5 -> NULL

    printf("Original ");
    printListWithRandom(head);

    Node* copied = copyRandomList(head);

    printf("\nCopied ");
    printListWithRandom(copied);

    printf("\nVerifying deep copy (different memory addresses):\n");
    printf("Original head: %p, Copied head: %p\n", (void*)head, (void*)copied);

    freeList(head);
    freeList(copied);
}

/* ============================================================================
 * ============================================================================
 *
 *                    ADVANCED EMBEDDED SYSTEMS TOPICS
 *                    (With Working Implementations)
 *
 * ============================================================================
 * ============================================================================ */

/* ============================================================================
 * 11. STATIC MEMORY POOL (NO MALLOC - CRITICAL FOR EMBEDDED)
 *
 * WHY THIS MATTERS:
 * - malloc() is non-deterministic (variable execution time)
 * - malloc() can fragment the heap over time
 * - malloc() can fail unpredictably
 * - Many safety-critical systems (automotive, medical) FORBID dynamic allocation
 *
 * INTERVIEW Q: "How would you implement a linked list without using malloc?"
 * ============================================================================ */

#define POOL_SIZE 16

typedef struct PoolNode {
    int data;
    struct PoolNode* next;
    bool in_use;  // Track allocation status (for debugging)
} PoolNode;

// Static pool - allocated at compile time in .bss section
static PoolNode g_node_pool[POOL_SIZE];
static PoolNode* g_free_list = NULL;
static int g_pool_allocated = 0;
static int g_pool_high_watermark = 0;  // Track peak usage

void pool_init(void) {
    // Chain all nodes into free list
    for (int i = 0; i < POOL_SIZE - 1; i++) {
        g_node_pool[i].next = &g_node_pool[i + 1];
        g_node_pool[i].in_use = false;
        g_node_pool[i].data = 0;
    }
    g_node_pool[POOL_SIZE - 1].next = NULL;
    g_node_pool[POOL_SIZE - 1].in_use = false;
    g_free_list = &g_node_pool[0];
    g_pool_allocated = 0;
    g_pool_high_watermark = 0;
}

PoolNode* pool_alloc(void) {
    if (!g_free_list) {
        return NULL;  // Pool exhausted - handle gracefully!
    }
    PoolNode* node = g_free_list;
    g_free_list = g_free_list->next;
    node->next = NULL;
    node->in_use = true;
    g_pool_allocated++;
    if (g_pool_allocated > g_pool_high_watermark) {
        g_pool_high_watermark = g_pool_allocated;
    }
    return node;
}

void pool_free(PoolNode* node) {
    if (!node) return;

    // DEFENSIVE: Check if node is actually from our pool
    if (node < &g_node_pool[0] || node > &g_node_pool[POOL_SIZE - 1]) {
        printf("  [ERROR] Attempted to free node not from pool!\n");
        return;
    }

    // DEFENSIVE: Check for double-free
    if (!node->in_use) {
        printf("  [ERROR] Double-free detected!\n");
        return;
    }

    node->in_use = false;
    node->data = 0xDEAD;  // Poison value for debugging
    node->next = g_free_list;
    g_free_list = node;
    g_pool_allocated--;
}

int pool_available(void) {
    return POOL_SIZE - g_pool_allocated;
}

void demo_staticMemoryPool(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|        ADVANCED EMBEDDED TOPIC: STATIC MEMORY POOL                |\n");
    printf("+====================================================================+\n");

    printf("\n[WHY USE STATIC POOLS IN EMBEDDED?]\n");
    printf("  - Deterministic O(1) allocation time (real-time safe)\n");
    printf("  - No heap fragmentation over long runtime\n");
    printf("  - Memory usage known at compile time\n");
    printf("  - Required by safety standards (MISRA, DO-178C, ISO 26262)\n");
    printf("  - Works without OS/stdlib\n");

    printf("\n[IMPLEMENTATION DETAILS]\n");
    printf("  Pool size: %d nodes\n", POOL_SIZE);
    printf("  Node size: %zu bytes\n", sizeof(PoolNode));
    printf("  Total static memory: %zu bytes\n", sizeof(g_node_pool));

    pool_init();
    printf("\n[DEMO] Initializing pool...\n");
    printf("  Available nodes: %d\n", pool_available());

    // Allocate some nodes
    printf("\n[DEMO] Allocating 5 nodes...\n");
    PoolNode* head = NULL;
    for (int i = 0; i < 5; i++) {
        PoolNode* node = pool_alloc();
        if (node) {
            node->data = (i + 1) * 10;
            node->next = head;
            head = node;
        }
    }
    printf("  Allocated: %d, Available: %d, High watermark: %d\n",
           g_pool_allocated, pool_available(), g_pool_high_watermark);

    // Print the list
    printf("  List: ");
    PoolNode* curr = head;
    while (curr) {
        printf("%d -> ", curr->data);
        curr = curr->next;
    }
    printf("NULL\n");

    // Demonstrate pool exhaustion handling
    printf("\n[DEMO] Attempting to exhaust pool...\n");
    int exhaustion_count = 0;
    while (pool_alloc() != NULL) {
        exhaustion_count++;
    }
    printf("  Allocated %d more nodes before exhaustion\n", exhaustion_count);
    printf("  Pool exhausted! pool_alloc() returned NULL (safe failure)\n");

    printf("\n[INTERVIEW TIP]\n");
    printf("  Always check pool_alloc() return value!\n");
    printf("  Have a strategy: drop message, use backup, signal error\n");

    // Free all and reinit for next demo
    pool_init();
}

/* ============================================================================
 * 12. INTRUSIVE LINKED LIST (LINUX KERNEL STYLE)
 *
 * WHY THIS MATTERS:
 * - Used in Linux kernel, FreeRTOS, Zephyr, most professional RTOS
 * - More memory efficient (one allocation per object)
 * - Object can be on multiple lists simultaneously
 * - No separate "node" allocation
 *
 * INTERVIEW Q: "Explain container_of macro and why it's used"
 * ============================================================================ */

#include <stddef.h>  // For offsetof

// The magic macro - MUST understand this for embedded interviews!
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

// Generic list head - just pointers, no data
typedef struct list_head {
    struct list_head* next;
    struct list_head* prev;
} list_head;

// Example: A "Task" structure with embedded list node
typedef struct {
    int task_id;
    int priority;
    const char* name;
    list_head list;  // Embedded list node - NOT a pointer!
} Task;

// Initialize list head
void INIT_LIST_HEAD(list_head* head) {
    head->next = head;
    head->prev = head;
}

// Add node after head
void list_add(list_head* new_node, list_head* head) {
    new_node->next = head->next;
    new_node->prev = head;
    head->next->prev = new_node;
    head->next = new_node;
}

// Add node before head (at tail)
void list_add_tail(list_head* new_node, list_head* head) {
    new_node->next = head;
    new_node->prev = head->prev;
    head->prev->next = new_node;
    head->prev = new_node;
}

// Remove node from list
void list_del(list_head* entry) {
    entry->prev->next = entry->next;
    entry->next->prev = entry->prev;
    entry->next = NULL;  // Poison
    entry->prev = NULL;
}

// Check if list is empty
bool list_empty(list_head* head) {
    return head->next == head;
}

// Iterate over list entries
#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

// Get the struct containing this list entry
#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

void demo_intrusiveList(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|        ADVANCED EMBEDDED TOPIC: INTRUSIVE LINKED LIST             |\n");
    printf("+====================================================================+\n");

    printf("\n[WHY INTRUSIVE LISTS?]\n");
    printf("  - Standard approach: Node { data, next } -> separate allocation\n");
    printf("  - Intrusive approach: Data struct contains list_head member\n");
    printf("  - Saves memory: 1 allocation vs 2 (node + data)\n");
    printf("  - Object can be on MULTIPLE lists simultaneously!\n");
    printf("  - Used in: Linux kernel, FreeRTOS, Zephyr, ESP-IDF\n");

    printf("\n[THE container_of MACRO - MUST KNOW FOR INTERVIEWS]\n");
    printf("  Given: pointer to 'list' member inside Task struct\n");
    printf("  Returns: pointer to the containing Task struct\n");
    printf("  Formula: (Task*)((char*)list_ptr - offsetof(Task, list))\n");

    // Create a list head
    list_head task_list;
    INIT_LIST_HEAD(&task_list);

    // Create tasks (could be static or from a pool in real embedded)
    Task tasks[4] = {
        {.task_id = 1, .priority = 3, .name = "Sensor_Read"},
        {.task_id = 2, .priority = 1, .name = "Motor_Control"},
        {.task_id = 3, .priority = 2, .name = "Communication"},
        {.task_id = 4, .priority = 1, .name = "Logging"}
    };

    // Add tasks to list (by priority - lower number = higher priority)
    printf("\n[DEMO] Adding tasks to intrusive list...\n");
    for (int i = 0; i < 4; i++) {
        list_add_tail(&tasks[i].list, &task_list);
        printf("  Added: Task %d '%s' (priority %d)\n",
               tasks[i].task_id, tasks[i].name, tasks[i].priority);
    }

    // Iterate and print using container_of
    printf("\n[DEMO] Iterating with container_of macro...\n");
    list_head* pos;
    list_for_each(pos, &task_list) {
        Task* task = list_entry(pos, Task, list);
        printf("  -> Task %d: %s (pri=%d) at addr %p\n",
               task->task_id, task->name, task->priority, (void*)task);
    }

    // Demonstrate offset calculation
    printf("\n[MEMORY LAYOUT]\n");
    printf("  sizeof(Task) = %zu bytes\n", sizeof(Task));
    printf("  offsetof(Task, task_id) = %zu\n", offsetof(Task, task_id));
    printf("  offsetof(Task, priority) = %zu\n", offsetof(Task, priority));
    printf("  offsetof(Task, name) = %zu\n", offsetof(Task, name));
    printf("  offsetof(Task, list) = %zu\n", offsetof(Task, list));

    printf("\n[INTERVIEW TIP]\n");
    printf("  Q: 'Why not just store Task* in the node?'\n");
    printf("  A: Intrusive lists let one object be on multiple lists!\n");
    printf("     Example: Task on ready_list AND timer_list simultaneously\n");

    // Remove a task
    printf("\n[DEMO] Removing 'Communication' task from list...\n");
    list_del(&tasks[2].list);

    printf("  Remaining tasks: ");
    list_for_each(pos, &task_list) {
        Task* task = list_entry(pos, Task, list);
        printf("%s -> ", task->name);
    }
    printf("(end)\n");
}

/* ============================================================================
 * 13. CIRCULAR LINKED LIST (ROUND-ROBIN SCHEDULER)
 *
 * WHY THIS MATTERS:
 * - Round-robin task scheduling in RTOS
 * - Ring buffers with variable-size entries
 * - Continuous polling of peripherals
 * - Token ring protocols
 *
 * INTERVIEW Q: "How would you implement a round-robin scheduler?"
 * ============================================================================ */

typedef struct RRNode {
    int id;
    const char* name;
    int time_slice_ms;
    struct RRNode* next;
} RRNode;

void demo_circularList(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|        ADVANCED EMBEDDED TOPIC: CIRCULAR LINKED LIST              |\n");
    printf("+====================================================================+\n");

    printf("\n[USE CASES IN EMBEDDED SYSTEMS]\n");
    printf("  - Round-robin task scheduling\n");
    printf("  - Polling multiple sensors in sequence\n");
    printf("  - LED pattern cycling\n");
    printf("  - Audio/video buffer management\n");
    printf("  - Token passing in communication protocols\n");

    // Create circular list of "tasks" for round-robin
    RRNode tasks[4] = {
        {.id = 1, .name = "ADC_Sample", .time_slice_ms = 10},
        {.id = 2, .name = "UART_Process", .time_slice_ms = 5},
        {.id = 3, .name = "GPIO_Update", .time_slice_ms = 2},
        {.id = 4, .name = "Watchdog_Pet", .time_slice_ms = 1}
    };

    // Link into circular list
    for (int i = 0; i < 3; i++) {
        tasks[i].next = &tasks[i + 1];
    }
    tasks[3].next = &tasks[0];  // Complete the circle!

    printf("\n[DEMO] Circular task list created:\n");
    RRNode* current = &tasks[0];
    for (int i = 0; i < 4; i++) {
        printf("  [%d] %s (%dms) -> ", current->id, current->name, current->time_slice_ms);
        current = current->next;
    }
    printf("(back to start)\n");

    // Simulate round-robin scheduling
    printf("\n[DEMO] Simulating round-robin scheduler (2 full cycles):\n");
    current = &tasks[0];
    for (int tick = 0; tick < 8; tick++) {
        printf("  Tick %d: Running '%s' for %dms\n",
               tick, current->name, current->time_slice_ms);
        current = current->next;  // Move to next task
    }

    printf("\n[DETECTING CIRCULAR LIST - Floyd's Algorithm]\n");
    printf("  Same algorithm as cycle detection!\n");
    printf("  If slow and fast pointers meet -> circular\n");

    // Demonstrate detection
    RRNode* slow = &tasks[0];
    RRNode* fast = &tasks[0];
    bool is_circular = false;

    for (int i = 0; i < 10; i++) {  // Limit iterations
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) {
            is_circular = true;
            break;
        }
    }
    printf("  Is circular? %s\n", is_circular ? "YES" : "NO");

    printf("\n[INTERVIEW TIP]\n");
    printf("  Watch out for: infinite loops when traversing!\n");
    printf("  Always track starting point or use counter limit\n");
}

/* ============================================================================
 * 14. ISR-SAFE OPERATIONS (CRITICAL SECTIONS)
 *
 * WHY THIS MATTERS:
 * - Lists modified by both ISR and main code can corrupt
 * - Race conditions are the #1 embedded bug
 * - Must understand volatile, memory barriers, atomics
 *
 * INTERVIEW Q: "How do you safely share a linked list between ISR and main?"
 * ============================================================================ */

// Simulated interrupt enable/disable (platform-specific in real code)
static volatile bool g_interrupts_enabled = true;
static volatile int g_isr_count = 0;

#define ENTER_CRITICAL() do { g_interrupts_enabled = false; } while(0)
#define EXIT_CRITICAL()  do { g_interrupts_enabled = true; } while(0)

// ISR-safe queue using static pool
typedef struct {
    PoolNode* head;
    PoolNode* tail;
    volatile int count;  // volatile - accessed from ISR!
} ISRSafeQueue;

void queue_init(ISRSafeQueue* q) {
    q->head = NULL;
    q->tail = NULL;
    q->count = 0;
}

// Called from ISR - must be fast!
bool queue_enqueue_isr(ISRSafeQueue* q, int data) {
    // In real code: already in ISR, interrupts disabled
    PoolNode* node = pool_alloc();
    if (!node) return false;  // Pool exhausted

    node->data = data;
    node->next = NULL;

    if (q->tail) {
        q->tail->next = node;
    } else {
        q->head = node;
    }
    q->tail = node;
    q->count++;
    g_isr_count++;

    return true;
}

// Called from main loop - must protect with critical section
bool queue_dequeue_main(ISRSafeQueue* q, int* data) {
    ENTER_CRITICAL();  // Disable interrupts

    if (!q->head) {
        EXIT_CRITICAL();
        return false;
    }

    PoolNode* node = q->head;
    *data = node->data;
    q->head = node->next;
    if (!q->head) {
        q->tail = NULL;
    }
    q->count--;

    EXIT_CRITICAL();  // Re-enable interrupts

    pool_free(node);  // Free outside critical section (takes time)
    return true;
}

void demo_isrSafeOperations(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|        ADVANCED EMBEDDED TOPIC: ISR-SAFE LIST OPERATIONS          |\n");
    printf("+====================================================================+\n");

    printf("\n[THE PROBLEM - RACE CONDITIONS]\n");
    printf("  Main loop: traversing list to find node\n");
    printf("  ISR fires: modifies the same list\n");
    printf("  Result: corrupted pointers, crashes, data loss!\n");

    printf("\n[SOLUTIONS]\n");
    printf("  1. Critical sections (disable interrupts) - simplest\n");
    printf("  2. Lock-free algorithms (CAS) - no blocking\n");
    printf("  3. Double buffering - ISR writes to buffer A, main reads B\n");
    printf("  4. Message queues (RTOS primitive)\n");

    printf("\n[CRITICAL SECTION RULES]\n");
    printf("  - Keep critical sections SHORT (<10us typical)\n");
    printf("  - Never call blocking functions inside\n");
    printf("  - Never call malloc/free inside (non-deterministic)\n");
    printf("  - Disable only necessary interrupt levels\n");

    // Initialize pool and queue
    pool_init();
    ISRSafeQueue msg_queue;
    queue_init(&msg_queue);

    printf("\n[DEMO] Simulating ISR -> Main queue communication\n");

    // Simulate ISR adding messages
    printf("\n  [Simulated ISR] Enqueueing sensor readings...\n");
    for (int i = 0; i < 5; i++) {
        int sensor_value = 100 + i * 10;
        if (queue_enqueue_isr(&msg_queue, sensor_value)) {
            printf("    ISR enqueued: %d (queue count: %d)\n",
                   sensor_value, msg_queue.count);
        }
    }

    // Main loop processing
    printf("\n  [Main Loop] Processing queued messages...\n");
    int value;
    while (queue_dequeue_main(&msg_queue, &value)) {
        printf("    Main processed: %d (queue count: %d)\n",
               value, msg_queue.count);
    }

    printf("\n[VOLATILE KEYWORD - CRITICAL]\n");
    printf("  volatile int count;  // Tells compiler:\n");
    printf("    - Don't optimize away reads\n");
    printf("    - Don't cache in register\n");
    printf("    - Re-read from memory each time\n");
    printf("  Without volatile: compiler may optimize incorrectly!\n");

    printf("\n[INTERVIEW TIP]\n");
    printf("  Q: 'What's wrong with this ISR code?'\n");
    printf("     void ISR() { list->count++; }  // NOT atomic!\n");
    printf("  A: count++ is read-modify-write (3 operations)\n");
    printf("     Main could interrupt between read and write\n");
    printf("     Use atomic operations or critical section\n");

    pool_init();  // Reset pool
}

/* ============================================================================
 * 15. XOR LINKED LIST (MEMORY OPTIMIZATION)
 *
 * WHY THIS MATTERS:
 * - Saves one pointer per node (4-8 bytes)
 * - Used when memory is extremely constrained
 * - Classic interview question
 *
 * INTERVIEW Q: "How can you implement a doubly-linked list with one pointer?"
 * ============================================================================ */

#include <stdint.h>

typedef struct XORNode {
    int data;
    struct XORNode* npx;  // XOR of next and prev addresses
} XORNode;

// XOR two pointers
XORNode* XOR(XORNode* a, XORNode* b) {
    return (XORNode*)((uintptr_t)a ^ (uintptr_t)b);
}

void demo_xorLinkedList(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|        ADVANCED EMBEDDED TOPIC: XOR LINKED LIST                   |\n");
    printf("+====================================================================+\n");

    printf("\n[THE CONCEPT]\n");
    printf("  Normal doubly-linked: each node has prev AND next (2 pointers)\n");
    printf("  XOR linked list: store XOR(prev, next) in ONE pointer!\n");
    printf("\n");
    printf("  Memory savings per node: 4 bytes (32-bit) or 8 bytes (64-bit)\n");

    printf("\n[HOW IT WORKS]\n");
    printf("  node->npx = prev_addr XOR next_addr\n");
    printf("\n");
    printf("  To traverse forward (knowing prev):\n");
    printf("    next = prev XOR node->npx\n");
    printf("\n");
    printf("  To traverse backward (knowing next):\n");
    printf("    prev = next XOR node->npx\n");

    // Create XOR linked list: A <-> B <-> C <-> D
    XORNode nodes[4];
    nodes[0].data = 10;  // A
    nodes[1].data = 20;  // B
    nodes[2].data = 30;  // C
    nodes[3].data = 40;  // D

    // Set up XOR pointers
    // A: prev=NULL, next=B -> npx = NULL XOR B = B
    nodes[0].npx = XOR(NULL, &nodes[1]);
    // B: prev=A, next=C -> npx = A XOR C
    nodes[1].npx = XOR(&nodes[0], &nodes[2]);
    // C: prev=B, next=D -> npx = B XOR D
    nodes[2].npx = XOR(&nodes[1], &nodes[3]);
    // D: prev=C, next=NULL -> npx = C XOR NULL = C
    nodes[3].npx = XOR(&nodes[2], NULL);

    printf("\n[DEMO] Traversing XOR list forward:\n");
    XORNode* prev = NULL;
    XORNode* curr = &nodes[0];

    printf("  ");
    while (curr) {
        printf("%d ", curr->data);
        XORNode* next = XOR(prev, curr->npx);
        prev = curr;
        curr = next;
    }
    printf("\n");

    printf("\n[DEMO] Traversing XOR list backward:\n");
    XORNode* next_node = NULL;
    curr = &nodes[3];  // Start from end

    printf("  ");
    while (curr) {
        printf("%d ", curr->data);
        XORNode* prev_node = XOR(next_node, curr->npx);
        next_node = curr;
        curr = prev_node;
    }
    printf("\n");

    printf("\n[TRADE-OFFS]\n");
    printf("  PROS:\n");
    printf("    - Saves 4-8 bytes per node\n");
    printf("    - Bidirectional traversal with single pointer\n");
    printf("  CONS:\n");
    printf("    - Cannot traverse from arbitrary node (need adjacent)\n");
    printf("    - Harder to debug (pointer values look like garbage)\n");
    printf("    - Some garbage collectors can't handle XOR'd pointers\n");
    printf("    - Code complexity increases significantly\n");

    printf("\n[INTERVIEW TIP]\n");
    printf("  Know the concept, but admit it's rarely used in practice.\n");
    printf("  Good answer: 'I'd use it only if memory is critical AND\n");
    printf("  debugging tools can handle it. Otherwise, standard doubly-linked.'\n");
}

/* ============================================================================
 * 16. COMMON BUGS DEMONSTRATION
 *
 * WHY THIS MATTERS:
 * - Interviewers love asking "what's wrong with this code?"
 * - Memory bugs are the #1 cause of embedded system failures
 *
 * INTERVIEW Q: "What bugs do you see in this linked list code?"
 * ============================================================================ */

void demo_commonBugs(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|        ADVANCED EMBEDDED TOPIC: COMMON BUGS & PITFALLS            |\n");
    printf("+====================================================================+\n");

    printf("\n[BUG #1: LOST NODE (Memory Leak)]\n");
    printf("  WRONG:\n");
    printf("    head = head->next;  // Old head is lost forever!\n");
    printf("  CORRECT:\n");
    printf("    Node* temp = head;\n");
    printf("    head = head->next;\n");
    printf("    free(temp);\n");

    printf("\n[BUG #2: USE-AFTER-FREE]\n");
    printf("  WRONG:\n");
    printf("    free(node);\n");
    printf("    printf(\"%%d\", node->data);  // UNDEFINED BEHAVIOR!\n");
    printf("  CORRECT:\n");
    printf("    int data = node->data;  // Copy first\n");
    printf("    free(node);\n");
    printf("    printf(\"%%d\", data);\n");

    printf("\n[BUG #3: NULL POINTER DEREFERENCE]\n");
    printf("  WRONG:\n");
    printf("    while (node->next != NULL)  // Crashes if node is NULL!\n");
    printf("  CORRECT:\n");
    printf("    while (node && node->next != NULL)\n");

    printf("\n[BUG #4: UPDATING POINTER ORDER]\n");
    printf("  WRONG (loses rest of list):\n");
    printf("    prev->next = new_node;\n");
    printf("    new_node->next = prev->next;  // Points to itself!\n");
    printf("  CORRECT:\n");
    printf("    new_node->next = prev->next;  // Save old next first\n");
    printf("    prev->next = new_node;\n");

    printf("\n[BUG #5: DOUBLE FREE]\n");
    printf("  Freeing same node twice -> heap corruption\n");
    printf("  DEFENSE: Set pointer to NULL after free\n");
    printf("    free(node);\n");
    printf("    node = NULL;  // Prevents double-free\n");

    printf("\n[BUG #6: DANGLING POINTER]\n");
    printf("  Multiple pointers to same node, one frees it\n");
    printf("  Other pointers become invalid (dangling)\n");
    printf("  DEFENSE: Clear ownership rules, reference counting\n");

    printf("\n[BUG #7: STACK OVERFLOW (Recursion)]\n");
    printf("  WRONG (for long lists):\n");
    printf("    void print(Node* n) {\n");
    printf("        if(n) { printf(\"%%d\",n->data); print(n->next); }\n");
    printf("    }\n");
    printf("  On 10000-node list with 1KB stack = CRASH\n");
    printf("  CORRECT: Use iterative approach\n");

    printf("\n[BUG #8: RACE CONDITION (ISR)]\n");
    printf("  Main loop reading list while ISR modifies it\n");
    printf("  DEFENSE: Critical sections, lock-free, copy-on-write\n");

    printf("\n[DEFENSIVE CODING PATTERN]\n");
    printf("  void safe_delete(Node** head_ref, Node* target) {\n");
    printf("      if (!head_ref || !*head_ref || !target) return;\n");
    printf("      // ... deletion logic ...\n");
    printf("      target->next = NULL;    // Poison\n");
    printf("      target->data = 0xDEAD;  // Debug marker\n");
    printf("      free(target);\n");
    printf("  }\n");

    printf("\n[INTERVIEW TIP]\n");
    printf("  When asked to review code, check for:\n");
    printf("  1. NULL checks before dereference\n");
    printf("  2. Correct pointer update order\n");
    printf("  3. Memory leaks (every malloc has matching free)\n");
    printf("  4. Thread/ISR safety\n");
    printf("  5. Boundary conditions (empty list, single node)\n");
}

/* ============================================================================
 * MAIN - Run all demonstrations
 * ============================================================================ */

int main(void) {
    printf("+====================================================================+\n");
    printf("|   SINGLY LINKED LIST - EMBEDDED SYSTEMS INTERVIEW PROBLEMS       |\n");
    printf("+====================================================================+\n");

    // ==========================================
    // PART 1: Classic Algorithm Problems (1-10)
    // ==========================================
    demo_reverseList();
    demo_detectCycle();
    demo_mergeSortedLists();
    demo_removeNthFromEnd();
    demo_findMiddle();
    demo_isPalindrome();
    demo_intersection();
    demo_reverseKGroup();
    demo_sortList();
    demo_copyRandomList();

    // ==========================================
    // PART 2: Advanced Embedded Topics (11-16)
    // ==========================================
    demo_staticMemoryPool();
    demo_intrusiveList();
    demo_circularList();
    demo_isrSafeOperations();
    demo_xorLinkedList();
    demo_commonBugs();

    // ==========================================
    // Final Summary
    // ==========================================
    printf("\n");
    printf("+====================================================================+\n");
    printf("|                    INTERVIEW PREPARATION SUMMARY                  |\n");
    printf("+====================================================================+\n");

    printf("\n[ALGORITHM COMPLEXITY - QUICK REFERENCE]\n");
    printf("  Reverse list:      O(n) time, O(1) space\n");
    printf("  Cycle detection:   O(n) time, O(1) space - Floyd's\n");
    printf("  Merge sorted:      O(n+m) time, O(1) space\n");
    printf("  Find middle:       O(n) time, O(1) space - slow/fast\n");
    printf("  Sort list:         O(n log n) time, O(log n) space\n");

    printf("\n[EMBEDDED-SPECIFIC KNOWLEDGE]\n");
    printf("  1. Static memory pools - avoid malloc in production\n");
    printf("  2. Intrusive lists - Linux kernel style, container_of\n");
    printf("  3. ISR safety - critical sections, volatile, atomics\n");
    printf("  4. Memory alignment - DMA requires aligned buffers\n");
    printf("  5. Circular lists - schedulers, ring buffers\n");

    printf("\n[COMMON INTERVIEW QUESTIONS]\n");
    printf("  - Reverse a linked list (iterative, O(1) space)\n");
    printf("  - Detect cycle and find start (Floyd's algorithm)\n");
    printf("  - Implement without malloc (static pool)\n");
    printf("  - Make it ISR-safe (critical sections)\n");
    printf("  - Explain container_of macro\n");
    printf("  - What's wrong with this code? (bug hunting)\n");

    printf("\n[RED FLAGS TO AVOID]\n");
    printf("  - Using recursion on long lists (stack overflow)\n");
    printf("  - Forgetting NULL checks\n");
    printf("  - Memory leaks (every alloc needs free)\n");
    printf("  - Race conditions with ISR\n");
    printf("  - Wrong pointer update order\n");

    printf("\n+====================================================================+\n");

    return 0;
}
