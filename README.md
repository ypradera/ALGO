# Algorithm & Data Structures for Embedded Systems

A comprehensive collection of algorithms and data structures implementations tailored for embedded systems engineering interviews. Implementations in C, C++, and Python.

## Repository Structure

### Core Data Structures (Embedded-Focused)

| Folder | Description | Languages |
|--------|-------------|-----------|
| `Singly_LinkedList/` | 16 linked list problems: reversal, cycle detection (Floyd's), merge, static pools, ISR-safe ops, intrusive lists | C |
| `Doubly_LinkedList/` | LRU cache, bidirectional traversal, insert/delete operations | C |
| `Arrays/` | Two Sum, Kadane's Algorithm, Circular Buffers, Binary Search, Lookup Tables | C |
| `Trees/` | BST operations, traversals (in/pre/post/level), heap/priority queue | C |
| `Graphs/` | BFS, DFS, Dijkstra's shortest path, Topological Sort, State Machines | C |
| `RTOS_Primitives/` | Semaphores, Mutexes, Message Queues, Event Flags, Deadlock Prevention | C |

### Embedded-Specific Implementations (DSA/)

| File | Description |
|------|-------------|
| `ring_buffer.c` | Lock-free circular buffer for ISR/task communication |
| `static_queue.c` | Fixed-size queue without dynamic allocation |
| `static_stack.c` | Fixed-size stack without dynamic allocation |
| `memory_pool.c` | Static memory pool allocator |
| `linked_list_static.c` | Linked list with pre-allocated node pool |
| `state_machine.c` | Embedded state machine pattern |
| `priority_queue.c` | Heap-based priority queue for task scheduling |
| `timer_wheel.c` | Hierarchical timer wheel implementation |
| `pid_controller.c` | PID control loop implementation |
| `moving_average.c` | Moving average filter for sensor data |
| `debounce.c` | Button/input debouncing algorithms |
| `double_buffer.c` | Double buffering for DMA/display |
| `lookup_table.c` | Precomputed lookup tables for fast math |
| `crc.c` | CRC calculation algorithms |
| `bit_array.c` | Compact bit array operations |
| `security.c` | Secure coding patterns for embedded |

### Algorithm Patterns (PATTERNS/)

15 common interview patterns implemented in C, C++, and Python:

- `sliding_window` - Fixed/variable window problems
- `two_pointers` - Array/string traversal
- `fast_slow_pointers` - Cycle detection, middle finding
- `binary_search` - Search variations
- `merge_intervals` - Interval scheduling
- `cyclic_sort` - In-place sorting for ranges
- `linked_list` - Common list operations
- `tree_traversal` - DFS/BFS tree operations
- `graph_bfs_dfs` - Graph traversal
- `heap` - Priority queue operations
- `stack_queue` - Stack/queue problems
- `hashmap` - Hash table usage
- `dynamic_programming` - DP patterns
- `backtracking` - Recursive exploration
- `bit_manipulation` - Bitwise operations
- `prefix_sum` - Range sum queries

### LeetCode & Interview Problems

| Folder | Problem | Languages |
|--------|---------|-----------|
| `Bit_Manipulation/` | UTF-8 Validation (LeetCode 393) | C, C++ |
| `Divide_two_nums/` | Divide Two Integers without *,/,% operators | C, C++ |
| `Check_Palindrome_filter_non_letter/` | Valid Palindrome (letters only, case-insensitive) | C, C++, Python |
| `Check_non_identical_string_rotation/` | String Rotation Detection | C, C++, Python |
| `Max_Num_Non_overlapping/` | Maximum Non-Overlapping Intervals (greedy) | C, C++, Python |
| `Merge and Sort Intervals/` | Merge Overlapping Intervals + no-heap version | C, C++, Python |
| `Limits/` | Integer Overflow/Underflow Detection & Safe Math | C |

## Key Features

- **No dynamic allocation**: Static memory pools suitable for embedded systems
- **ISR-safe operations**: Volatile keywords and atomic considerations
- **Big-O complexity**: Each function documented with time/space complexity
- **Beginner-friendly**: Detailed comments explaining concepts step-by-step
- **Interview-focused**: Common questions and edge cases covered
- **Multi-language**: Core patterns available in C, C++, and Python

## Building

C files:
```bash
gcc -Wall -o output filename.c
```

C++ files:
```bash
g++ -Wall -std=c++17 -o output filename.cpp
```

Python files:
```bash
python filename.py
```

## Topics Covered

### Linked Lists
- Reversal, cycle detection (Floyd's Tortoise & Hare)
- Merge sorted lists, find middle node, remove nth from end
- Static memory pools, intrusive lists (Linux kernel style)
- XOR linked lists for memory optimization

### Arrays & Strings
- Two pointers, sliding window
- Circular buffers for sensor data streams
- Lookup tables for fast computation
- Safe integer arithmetic (overflow detection)

### Trees & Heaps
- BST insert/delete/search/validate
- All traversal methods (recursive & iterative)
- Min-heap for priority scheduling

### Graphs
- Adjacency list representation (static allocation)
- Shortest path algorithms (Dijkstra)
- Topological sort for dependency resolution
- State machine patterns

### RTOS Concepts
- Binary/counting semaphores
- Mutex with priority inheritance
- Producer-consumer queues
- Event flags and deadlock prevention

## License

MIT
