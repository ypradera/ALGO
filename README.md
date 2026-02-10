# Algorithm & Data Structures for Embedded Systems

A comprehensive collection of algorithms and data structures implementations tailored for embedded systems engineering interviews.

## Contents

| Folder | Description |
|--------|-------------|
| `Singly_LinkedList/` | 16 linked list problems with embedded focus (static pools, ISR-safe, intrusive lists) |
| `Doubly_LinkedList/` | LRU cache, two-pointer techniques, bidirectional traversal |
| `Arrays/` | Two Sum, Kadane's Algorithm, Circular Buffers, Binary Search, Lookup Tables |
| `Trees/` | BST operations, traversals, heap/priority queue for task scheduling |
| `Graphs/` | BFS, DFS, Dijkstra, Topological Sort, State Machines |
| `RTOS_Primitives/` | Semaphores, Mutexes, Message Queues, Event Flags, Deadlock Prevention |
| `PATTERNS/` | Common algorithm patterns in C, C++, and Python |
| `DSA/` | Embedded-specific implementations (ring buffers, memory pools, PID controllers) |

## Key Features

- **No dynamic allocation**: Static memory pools suitable for embedded systems
- **ISR-safe operations**: Volatile keywords and atomic considerations
- **Big-O complexity**: Each function documented with time/space complexity
- **Beginner-friendly**: Detailed comments explaining concepts step-by-step
- **Interview-focused**: Common questions and edge cases covered

## Building

All C files can be compiled with:

```bash
gcc -Wall -o output filename.c
```

## Topics Covered

### Linked Lists
- Reversal, cycle detection (Floyd's algorithm)
- Merge sorted lists, find middle node
- Static memory pools, intrusive lists (Linux kernel style)

### Arrays
- Sliding window, two pointers
- Circular buffers for sensor data
- Lookup tables for fast computation

### Trees
- BST insert/delete/search
- Level-order traversal for hierarchical configs
- Min-heap for priority scheduling

### Graphs
- Adjacency list representation
- Shortest path algorithms
- State machine patterns

### RTOS
- Binary/counting semaphores
- Mutex with priority inheritance
- Producer-consumer queues
- Deadlock prevention strategies

## License

MIT
