/*
 * Doubly Linked List - Interview Problems for Embedded Systems
 *
 * ============================================================================
 *                    BEGINNER'S GUIDE: DOUBLY LINKED LIST
 * ============================================================================
 *
 * WHAT IS A DOUBLY LINKED LIST?
 * -----------------------------
 * Like a singly linked list, but each node has TWO pointers:
 *   - "next" points to the next node (forward)
 *   - "prev" points to the previous node (backward)
 *
 * VISUAL REPRESENTATION:
 *
 *   NULL <--- +------+------+------+ <---> +------+------+------+ <---> +------+------+------+ ---> NULL
 *             | prev | data | next |       | prev | data | next |       | prev | data | next |
 *             | NULL |  10  |   -------->  |   <-----20  |   -------->  |   <-----30  | NULL |
 *             +------+------+------+       +------+------+------+       +------+------+------+
 *                   Node 1                       Node 2                       Node 3
 *                   (HEAD)                                                    (TAIL)
 *
 * SINGLY VS DOUBLY LINKED LIST:
 * -----------------------------
 *   Singly:  10 -> 20 -> 30 -> NULL     (can only go FORWARD)
 *   Doubly:  NULL <- 10 <-> 20 <-> 30 -> NULL  (can go BOTH ways)
 *
 * ADVANTAGES OF DOUBLY LINKED:
 *   1. Can traverse BACKWARDS (singly can't)
 *   2. Delete node in O(1) if you have pointer to it (no need to find prev)
 *   3. Easy to implement LRU cache, text editors, browser history
 *
 * DISADVANTAGES:
 *   1. Extra memory: 2 pointers per node instead of 1
 *   2. More complex: must update 2 pointers instead of 1
 *   3. More bugs: easy to forget updating prev pointer
 *
 * WHEN TO USE IN EMBEDDED SYSTEMS:
 *   - LRU (Least Recently Used) caches
 *   - Undo/Redo functionality
 *   - Navigation (back/forward)
 *   - Deques (double-ended queues)
 *   - Music playlists (next/previous song)
 *   - Text editors (cursor movement)
 *
 * ============================================================================
 * BIG-O COMPLEXITY
 * ============================================================================
 *
 * Operation              | Doubly LL | Singly LL | Notes
 * -----------------------|-----------|-----------|-----------------------------
 * Access by index        | O(n)      | O(n)      | Can start from either end
 * Insert at head         | O(1)      | O(1)      | Same
 * Insert at tail         | O(1)*     | O(n)      | *If tail pointer maintained
 * Insert after node      | O(1)      | O(1)      | Same
 * Insert before node     | O(1)      | O(n)      | Doubly wins! (has prev)
 * Delete head            | O(1)      | O(1)      | Same
 * Delete tail            | O(1)*     | O(n)      | *Doubly wins! (has prev)
 * Delete specific node   | O(1)      | O(n)      | Doubly wins! (has prev)
 * Search                 | O(n)      | O(n)      | Same
 * Reverse                | O(n)      | O(n)      | Doubly: just swap pointers
 *
 * ============================================================================
 * COMMON INTERVIEW PROBLEMS
 * ============================================================================
 *
 * #  | Problem                          | Difficulty | Key Concept
 * ---|----------------------------------|------------|---------------------------
 * 1  | Insert at position               | Easy       | Update 4 pointers
 * 2  | Delete node                      | Easy       | O(1) with direct pointer
 * 3  | Reverse doubly linked list       | Easy       | Swap prev and next
 * 4  | LRU Cache                        | Medium     | HashMap + DLL
 * 5  | Flatten multilevel DLL           | Medium     | Recursion/stack
 * 6  | Convert BST to sorted DLL        | Medium     | In-order traversal
 * 7  | Merge sort on DLL                | Medium     | Divide and conquer
 * 8  | Find pairs with given sum        | Medium     | Two pointers from ends
 *
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* ============================================================================
 * NODE STRUCTURE
 * ============================================================================
 *
 * VISUAL OF A DOUBLY LINKED NODE:
 *
 *   +------+------+------+
 *   | prev | data | next |
 *   +------+------+------+
 *      |             |
 *      v             v
 *   previous       next
 *    node          node
 *
 * Memory layout (on 64-bit system):
 *   prev pointer: 8 bytes
 *   data (int):   4 bytes
 *   padding:      4 bytes (alignment)
 *   next pointer: 8 bytes
 *   Total: 24 bytes per node (vs 16 bytes for singly linked)
 * ============================================================================ */

typedef struct DNode {
    int data;               // The value stored in this node
    struct DNode* prev;     // Pointer to previous node (NULL if first)
    struct DNode* next;     // Pointer to next node (NULL if last)
} DNode;

/*
 * Doubly Linked List structure with head AND tail pointers
 * Having both allows O(1) insertion/deletion at both ends
 */
typedef struct {
    DNode* head;            // Pointer to first node
    DNode* tail;            // Pointer to last node
    int size;               // Number of nodes (useful for quick length check)
} DoublyLinkedList;

/* ============================================================================
 * UTILITY FUNCTIONS
 * ============================================================================ */

/*
 * createNode - Create a new doubly linked node
 *
 * VISUAL:
 *   Before: Nothing
 *   After:  NULL <- [data] -> NULL
 */
DNode* createDNode(int data) {
    DNode* newNode = (DNode*)malloc(sizeof(DNode));
    if (newNode) {
        newNode->data = data;
        newNode->prev = NULL;   // New node has no connections yet
        newNode->next = NULL;
    }
    return newNode;
}

/*
 * initList - Initialize an empty doubly linked list
 */
void initList(DoublyLinkedList* list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

/*
 * printListForward - Print list from head to tail
 */
void printListForward(DoublyLinkedList* list) {
    printf("Forward:  NULL <-> ");
    DNode* curr = list->head;
    while (curr) {
        printf("%d <-> ", curr->data);
        curr = curr->next;
    }
    printf("NULL\n");
}

/*
 * printListBackward - Print list from tail to head
 * This is impossible with singly linked list!
 */
void printListBackward(DoublyLinkedList* list) {
    printf("Backward: NULL <-> ");
    DNode* curr = list->tail;
    while (curr) {
        printf("%d <-> ", curr->data);
        curr = curr->prev;
    }
    printf("NULL\n");
}

/*
 * freeList - Free all nodes in the list
 */
void freeList(DoublyLinkedList* list) {
    DNode* curr = list->head;
    while (curr) {
        DNode* next = curr->next;
        free(curr);
        curr = next;
    }
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

/* ============================================================================
 * 1. INSERT AT HEAD
 * Time: O(1)
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * Add a new node at the beginning of the list.
 *
 * VISUAL for inserting 5 into list [10 <-> 20]:
 *
 *   Before:  NULL <-> [10] <-> [20] <-> NULL
 *                      head     tail
 *
 *   Step 1: Create new node [5]
 *            NULL <- [5] -> NULL
 *
 *   Step 2: Point new node's next to current head
 *            [5] -> [10] <-> [20]
 *
 *   Step 3: Point current head's prev to new node
 *            [5] <-> [10] <-> [20]
 *
 *   Step 4: Update head to new node
 *   After:  NULL <-> [5] <-> [10] <-> [20] <-> NULL
 *                    head              tail
 *
 * THE 4-POINTER UPDATE PATTERN:
 * When inserting into a doubly linked list, you typically update 4 pointers:
 *   1. new_node->next = ...
 *   2. new_node->prev = ...
 *   3. existing_node->prev = new_node (or existing_node->next)
 *   4. head or tail pointer
 * ============================================================================ */

void insertAtHead(DoublyLinkedList* list, int data) {
    DNode* newNode = createDNode(data);
    if (!newNode) return;  // malloc failed

    if (list->head == NULL) {
        // List is empty - new node is both head and tail
        list->head = newNode;
        list->tail = newNode;
    } else {
        // List has nodes
        newNode->next = list->head;     // New node points to old head
        list->head->prev = newNode;     // Old head points back to new node
        list->head = newNode;           // Update head to new node
    }
    list->size++;
}

/* ============================================================================
 * 2. INSERT AT TAIL
 * Time: O(1) - Thanks to tail pointer!
 * ============================================================================
 *
 * VISUAL for inserting 30 into list [10 <-> 20]:
 *
 *   Before:  NULL <-> [10] <-> [20] <-> NULL
 *                      head     tail
 *
 *   After:   NULL <-> [10] <-> [20] <-> [30] <-> NULL
 *                      head              tail
 *
 * Without tail pointer, this would be O(n) - must walk to end first!
 * ============================================================================ */

void insertAtTail(DoublyLinkedList* list, int data) {
    DNode* newNode = createDNode(data);
    if (!newNode) return;

    if (list->tail == NULL) {
        // List is empty
        list->head = newNode;
        list->tail = newNode;
    } else {
        // List has nodes
        newNode->prev = list->tail;     // New node points back to old tail
        list->tail->next = newNode;     // Old tail points forward to new node
        list->tail = newNode;           // Update tail to new node
    }
    list->size++;
}

/* ============================================================================
 * 3. INSERT AT POSITION
 * Time: O(n) - Need to traverse to position
 * ============================================================================
 *
 * VISUAL for inserting 15 at position 1 in list [10 <-> 20 <-> 30]:
 *
 *   Position: 0    1    2
 *             10 - 20 - 30
 *
 *   We want:  10 - 15 - 20 - 30
 *                  ^
 *                inserted
 *
 * THE TRICKY PART:
 * Must update 4 pointers:
 *   1. new_node->prev = node_at_position-1
 *   2. new_node->next = node_at_position
 *   3. node_at_position-1->next = new_node
 *   4. node_at_position->prev = new_node
 * ============================================================================ */

void insertAtPosition(DoublyLinkedList* list, int data, int position) {
    // Edge cases
    if (position < 0 || position > list->size) {
        printf("Invalid position!\n");
        return;
    }

    // Insert at head
    if (position == 0) {
        insertAtHead(list, data);
        return;
    }

    // Insert at tail
    if (position == list->size) {
        insertAtTail(list, data);
        return;
    }

    // Insert in middle - traverse to position
    DNode* newNode = createDNode(data);
    if (!newNode) return;

    DNode* curr = list->head;
    for (int i = 0; i < position; i++) {
        curr = curr->next;
    }

    // Now curr is at the position where new node should go
    // new node will be inserted BEFORE curr
    newNode->prev = curr->prev;     // New node's prev is curr's old prev
    newNode->next = curr;           // New node's next is curr
    curr->prev->next = newNode;     // Old prev now points to new node
    curr->prev = newNode;           // Curr's prev is now new node

    list->size++;
}

/* ============================================================================
 * 4. DELETE NODE (given pointer to node)
 * Time: O(1) - This is the BIG advantage of doubly linked!
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * In SINGLY linked list, to delete a node we need to find the PREVIOUS node
 * (to update its next pointer). This requires O(n) traversal.
 *
 * In DOUBLY linked list, we HAVE the previous node via prev pointer!
 * So deletion is O(1) if we have a pointer to the node.
 *
 * VISUAL for deleting node 20 from [10 <-> 20 <-> 30]:
 *
 *   Before: NULL <-> [10] <-> [20] <-> [30] <-> NULL
 *                              ^ delete this
 *
 *   Step 1: 10->next = 30  (skip over 20)
 *   Step 2: 30->prev = 10  (skip over 20)
 *
 *   After:  NULL <-> [10] <-> [30] <-> NULL
 *
 * EDGE CASES TO HANDLE:
 *   - Deleting head (prev is NULL)
 *   - Deleting tail (next is NULL)
 *   - Deleting only node (both head and tail)
 * ============================================================================ */

void deleteNode(DoublyLinkedList* list, DNode* node) {
    if (!list || !node) return;

    // If node is head
    if (node == list->head) {
        list->head = node->next;
        if (list->head) {
            list->head->prev = NULL;
        }
    }

    // If node is tail
    if (node == list->tail) {
        list->tail = node->prev;
        if (list->tail) {
            list->tail->next = NULL;
        }
    }

    // If node is in middle (has both prev and next)
    if (node->prev) {
        node->prev->next = node->next;
    }
    if (node->next) {
        node->next->prev = node->prev;
    }

    free(node);
    list->size--;
}

/* ============================================================================
 * 5. DELETE BY VALUE
 * Time: O(n) - Need to find the node first
 * ============================================================================ */

void deleteByValue(DoublyLinkedList* list, int value) {
    DNode* curr = list->head;

    while (curr) {
        if (curr->data == value) {
            deleteNode(list, curr);
            return;  // Delete first occurrence only
        }
        curr = curr->next;
    }
}

/* ============================================================================
 * 6. REVERSE DOUBLY LINKED LIST
 * Time: O(n), Space: O(1)
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * Reversing a doubly linked list is actually SIMPLER than singly linked!
 * We just swap prev and next pointers for each node.
 *
 * VISUAL:
 *   Before: NULL <-> [1] <-> [2] <-> [3] <-> NULL
 *                    head              tail
 *
 *   For each node, swap prev and next:
 *     Node 1: prev=NULL, next=2  -->  prev=2, next=NULL
 *     Node 2: prev=1, next=3     -->  prev=3, next=1
 *     Node 3: prev=2, next=NULL  -->  prev=NULL, next=2
 *
 *   After:  NULL <-> [3] <-> [2] <-> [1] <-> NULL
 *                    head              tail
 *
 * Finally, swap head and tail pointers.
 * ============================================================================ */

void reverseList(DoublyLinkedList* list) {
    if (!list->head || !list->head->next) return;  // Empty or single node

    DNode* curr = list->head;
    DNode* temp = NULL;

    // Swap prev and next for all nodes
    while (curr) {
        // Swap prev and next pointers
        temp = curr->prev;
        curr->prev = curr->next;
        curr->next = temp;

        // Move to next node (which is now in prev due to swap)
        curr = curr->prev;
    }

    // Swap head and tail
    temp = list->head;
    list->head = list->tail;
    list->tail = temp;
}

/* ============================================================================
 * 7. FIND MIDDLE NODE
 * Time: O(n), Space: O(1)
 * Same slow/fast pointer technique as singly linked list
 * ============================================================================ */

DNode* findMiddle(DoublyLinkedList* list) {
    if (!list->head) return NULL;

    DNode* slow = list->head;
    DNode* fast = list->head;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    return slow;
}

/* ============================================================================
 * 8. FIND PAIRS WITH GIVEN SUM
 * Time: O(n), Space: O(1)
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * Given a SORTED doubly linked list, find all pairs that sum to target.
 *
 * THE TRICK - Two Pointers from Both Ends:
 * In doubly linked list, we can have a pointer at both ends!
 *   - left starts at head
 *   - right starts at tail
 *
 * If sum < target: move left forward (need larger)
 * If sum > target: move right backward (need smaller)
 * If sum == target: found a pair!
 *
 * VISUAL for finding pairs that sum to 7 in [1 <-> 2 <-> 3 <-> 4 <-> 5 <-> 6]:
 *
 *   left=1, right=6: sum=7 ✓ FOUND! Move both
 *   left=2, right=5: sum=7 ✓ FOUND! Move both
 *   left=3, right=4: sum=7 ✓ FOUND! Move both
 *   left=4, right=3: left >= right, STOP
 *
 *   Pairs: (1,6), (2,5), (3,4)
 *
 * This is O(n) because each pointer moves at most n times total.
 * ============================================================================ */

void findPairsWithSum(DoublyLinkedList* list, int targetSum) {
    if (!list->head) return;

    DNode* left = list->head;
    DNode* right = list->tail;

    printf("Pairs with sum %d: ", targetSum);
    bool found = false;

    while (left != right && left->prev != right) {
        int sum = left->data + right->data;

        if (sum == targetSum) {
            printf("(%d, %d) ", left->data, right->data);
            found = true;
            left = left->next;
            right = right->prev;
        } else if (sum < targetSum) {
            left = left->next;
        } else {
            right = right->prev;
        }
    }

    if (!found) printf("None");
    printf("\n");
}

/* ============================================================================
 * 9. LRU CACHE (Least Recently Used)
 * ============================================================================
 *
 * BEGINNER EXPLANATION:
 * LRU Cache is a VERY common interview question and practical embedded use case.
 *
 * CONCEPT:
 * A cache with limited capacity. When full and need to add new item:
 *   - Remove the LEAST RECENTLY USED item
 *   - "Recently used" = accessed or added
 *
 * WHY DOUBLY LINKED LIST + HASH MAP?
 *   - DLL: O(1) remove from middle, O(1) add to front
 *   - HashMap: O(1) lookup by key
 *
 * OPERATIONS:
 *   get(key): Return value if exists, mark as recently used
 *   put(key, value): Add/update, mark as recently used, evict LRU if full
 *
 * VISUAL:
 *   Cache with capacity 3:
 *
 *   Most Recent                              Least Recent
 *       |                                         |
 *       v                                         v
 *     HEAD                                      TAIL
 *   [Key:C] <-> [Key:B] <-> [Key:A] <-> NULL
 *
 *   When we access Key:A, move it to head:
 *   [Key:A] <-> [Key:C] <-> [Key:B] <-> NULL
 *
 *   When cache is full and we add Key:D, remove tail (LRU):
 *   [Key:D] <-> [Key:A] <-> [Key:C] <-> NULL  (Key:B evicted)
 *
 * EMBEDDED USE CASES:
 *   - Caching sensor readings
 *   - DNS cache in network stack
 *   - Page cache in file systems
 *   - Instruction/data cache simulation
 * ============================================================================ */

#define LRU_CAPACITY 3

typedef struct LRUNode {
    int key;
    int value;
    struct LRUNode* prev;
    struct LRUNode* next;
} LRUNode;

typedef struct {
    LRUNode* head;      // Most recently used
    LRUNode* tail;      // Least recently used
    int size;
    int capacity;
    // In a real implementation, add a hash map here for O(1) lookup
} LRUCache;

void lru_init(LRUCache* cache, int capacity) {
    cache->head = NULL;
    cache->tail = NULL;
    cache->size = 0;
    cache->capacity = capacity;
}

// Move node to head (most recently used)
void lru_moveToHead(LRUCache* cache, LRUNode* node) {
    if (node == cache->head) return;  // Already at head

    // Remove from current position
    if (node->prev) node->prev->next = node->next;
    if (node->next) node->next->prev = node->prev;
    if (node == cache->tail) cache->tail = node->prev;

    // Insert at head
    node->prev = NULL;
    node->next = cache->head;
    if (cache->head) cache->head->prev = node;
    cache->head = node;
    if (!cache->tail) cache->tail = node;
}

// Remove tail (least recently used)
LRUNode* lru_removeTail(LRUCache* cache) {
    if (!cache->tail) return NULL;

    LRUNode* node = cache->tail;
    cache->tail = node->prev;
    if (cache->tail) {
        cache->tail->next = NULL;
    } else {
        cache->head = NULL;
    }
    cache->size--;
    return node;
}

// Simple linear search (real impl uses hash map)
LRUNode* lru_find(LRUCache* cache, int key) {
    LRUNode* curr = cache->head;
    while (curr) {
        if (curr->key == key) return curr;
        curr = curr->next;
    }
    return NULL;
}

int lru_get(LRUCache* cache, int key) {
    LRUNode* node = lru_find(cache, key);
    if (!node) return -1;  // Not found

    lru_moveToHead(cache, node);  // Mark as recently used
    return node->value;
}

void lru_put(LRUCache* cache, int key, int value) {
    LRUNode* node = lru_find(cache, key);

    if (node) {
        // Key exists, update value and move to head
        node->value = value;
        lru_moveToHead(cache, node);
    } else {
        // Key doesn't exist, create new node
        LRUNode* newNode = (LRUNode*)malloc(sizeof(LRUNode));
        newNode->key = key;
        newNode->value = value;
        newNode->prev = NULL;
        newNode->next = cache->head;

        if (cache->head) cache->head->prev = newNode;
        cache->head = newNode;
        if (!cache->tail) cache->tail = newNode;

        cache->size++;

        // If over capacity, remove LRU (tail)
        if (cache->size > cache->capacity) {
            LRUNode* lru = lru_removeTail(cache);
            free(lru);
        }
    }
}

void lru_print(LRUCache* cache) {
    printf("LRU Cache (MRU -> LRU): ");
    LRUNode* curr = cache->head;
    while (curr) {
        printf("[%d:%d] ", curr->key, curr->value);
        curr = curr->next;
    }
    printf("\n");
}

/* ============================================================================
 * DEMO FUNCTIONS
 * ============================================================================ */

void demo_basicOperations(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|            DOUBLY LINKED LIST - BASIC OPERATIONS                   |\n");
    printf("+====================================================================+\n");

    DoublyLinkedList list;
    initList(&list);

    printf("\n[DEMO] Inserting at head: 30, 20, 10\n");
    insertAtHead(&list, 30);
    insertAtHead(&list, 20);
    insertAtHead(&list, 10);
    printListForward(&list);
    printListBackward(&list);

    printf("\n[DEMO] Inserting at tail: 40, 50\n");
    insertAtTail(&list, 40);
    insertAtTail(&list, 50);
    printListForward(&list);

    printf("\n[DEMO] Inserting 25 at position 2\n");
    insertAtPosition(&list, 25, 2);
    printListForward(&list);
    printf("Size: %d\n", list.size);

    printf("\n[DEMO] Deleting value 25\n");
    deleteByValue(&list, 25);
    printListForward(&list);

    printf("\n[DEMO] Reversing the list\n");
    reverseList(&list);
    printListForward(&list);
    printListBackward(&list);

    printf("\n[DEMO] Finding middle node\n");
    DNode* mid = findMiddle(&list);
    if (mid) printf("Middle node value: %d\n", mid->data);

    freeList(&list);
}

void demo_findPairs(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|            FIND PAIRS WITH GIVEN SUM (Two Pointers)                |\n");
    printf("+====================================================================+\n");

    DoublyLinkedList list;
    initList(&list);

    // Create sorted list: 1, 2, 3, 4, 5, 6
    for (int i = 1; i <= 6; i++) {
        insertAtTail(&list, i);
    }

    printf("\nSorted list: ");
    printListForward(&list);

    findPairsWithSum(&list, 7);
    findPairsWithSum(&list, 5);
    findPairsWithSum(&list, 10);

    freeList(&list);
}

void demo_lruCache(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|            LRU CACHE IMPLEMENTATION                                |\n");
    printf("+====================================================================+\n");

    printf("\n[WHY LRU CACHE IN EMBEDDED?]\n");
    printf("  - Limited memory, need to decide what to keep\n");
    printf("  - Fast O(1) get and put operations\n");
    printf("  - Common in: DNS cache, page cache, sensor data\n");

    LRUCache cache;
    lru_init(&cache, 3);  // Capacity of 3

    printf("\n[DEMO] Adding items (capacity=3)\n");

    printf("  put(1, 100): ");
    lru_put(&cache, 1, 100);
    lru_print(&cache);

    printf("  put(2, 200): ");
    lru_put(&cache, 2, 200);
    lru_print(&cache);

    printf("  put(3, 300): ");
    lru_put(&cache, 3, 300);
    lru_print(&cache);

    printf("\n[DEMO] Accessing key 1 (moves to front)\n");
    printf("  get(1) = %d: ", lru_get(&cache, 1));
    lru_print(&cache);

    printf("\n[DEMO] Adding key 4 (evicts LRU = key 2)\n");
    printf("  put(4, 400): ");
    lru_put(&cache, 4, 400);
    lru_print(&cache);

    printf("\n[DEMO] Try to get evicted key\n");
    printf("  get(2) = %d (not found)\n", lru_get(&cache, 2));
}

void demo_embeddedConsiderations(void) {
    printf("\n");
    printf("+====================================================================+\n");
    printf("|            EMBEDDED SYSTEMS CONSIDERATIONS                         |\n");
    printf("+====================================================================+\n");

    printf("\n[MEMORY OVERHEAD]\n");
    printf("  Singly linked node: data + 1 pointer = %zu bytes\n",
           sizeof(int) + sizeof(void*));
    printf("  Doubly linked node: data + 2 pointers = %zu bytes\n",
           sizeof(int) + 2*sizeof(void*));
    printf("  Extra memory per node: %zu bytes (50%% more!)\n", sizeof(void*));

    printf("\n[WHEN TO USE DOUBLY LINKED IN EMBEDDED]\n");
    printf("  USE when you need:\n");
    printf("    - O(1) deletion with pointer to node\n");
    printf("    - Bidirectional traversal\n");
    printf("    - Deque (double-ended queue)\n");
    printf("    - LRU cache or similar\n");
    printf("  AVOID when:\n");
    printf("    - Memory is extremely tight\n");
    printf("    - Only forward traversal needed\n");
    printf("    - Simple FIFO queue suffices\n");

    printf("\n[COMMON INTERVIEW QUESTIONS]\n");
    printf("  Q: 'When would you choose doubly over singly linked?'\n");
    printf("  A: When I need O(1) deletion or bidirectional traversal.\n");
    printf("     Trade-off is extra memory per node.\n");
    printf("\n");
    printf("  Q: 'Implement an LRU cache'\n");
    printf("  A: Use doubly linked list + hash map.\n");
    printf("     DLL for O(1) reordering, hash for O(1) lookup.\n");

    printf("\n[COMMON BUGS]\n");
    printf("  1. Forgetting to update BOTH prev and next pointers\n");
    printf("  2. Not handling head/tail edge cases\n");
    printf("  3. Memory leaks when deleting nodes\n");
    printf("  4. Circular references causing infinite loops\n");
}

/* ============================================================================
 * MAIN
 * ============================================================================ */

int main(void) {
    printf("+====================================================================+\n");
    printf("|    DOUBLY LINKED LIST - EMBEDDED SYSTEMS INTERVIEW GUIDE          |\n");
    printf("+====================================================================+\n");

    demo_basicOperations();
    demo_findPairs();
    demo_lruCache();
    demo_embeddedConsiderations();

    printf("\n+====================================================================+\n");
    printf("|                    SUMMARY                                         |\n");
    printf("+====================================================================+\n");
    printf("| Key advantage: O(1) deletion and bidirectional traversal          |\n");
    printf("| Key cost: Extra pointer per node (50%% more memory)                |\n");
    printf("| Must-know: LRU Cache implementation                               |\n");
    printf("| Common pattern: Update 4 pointers when inserting/deleting         |\n");
    printf("+====================================================================+\n");

    return 0;
}
