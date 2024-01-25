# Dynamic Memory Allocation in C

## Description

Welcome to the project repository for the LINFO1252 course at UCL, part of the Civil Engineering program in the field of computer science. This repository contains the source code for a simple memory management system implemented in C. The project focuses on dynamic memory allocation and deallocation using a custom heap structure. The memory manager includes functions for allocating and freeing memory, along with helper functions for managing the internal heap.

### Project Details
- **Grade:** 16/20 (Code: 10/10, Report: 6/10).
- **Language:** C.
- **Environment:** Linux.

## Feedback Summary

### Report Assessment
- **Structure and Presentation:** Good.
- **Content and Explanation:** Insufficient.
  
### Customized Comments
- **Functionality of `multiblocs`:** The purpose of the `multiblocs` function is unclear. Further clarification on its role and functionality is needed for a more comprehensive understanding.

- **Memory Release Strategy:** It is not evident whether the memory release strategy incorporates the merging of free blocks. Additional details on the approach taken for memory release, specifically regarding the merging of free blocks, would enhance the clarity of the implementation.

---
*This feedback is provided to facilitate improvements in the report and enhance the overall understanding of the project.*


## Memory Management

The memory management system consists of a custom heap (`MY_HEAP`) with a fixed size (`HEAP_SIZE`). The heap is initialized with two metadata blocks, representing the nextfit offset and the size of the first free block. The project provides functions for allocating memory (`my_malloc`), freeing memory (`my_free`), and printing the state of the heap (`print_HEAP`).

### Features

1. **Dynamic Memory Allocation**: The `my_malloc` function allocates a block of memory of the given size. It manages the heap to find a suitable free block and initializes the allocated block.

2. **Memory Deallocation**: The `my_free` function frees a previously allocated block of memory. It updates the heap and may merge adjacent free blocks.

3. **Printing Heap State**: The `print_HEAP` function prints the current state of the heap, displaying information about allocated and free blocks.

### Testing

The `main` function includes a series of test cases demonstrating the functionality of memory allocation and deallocation. It allocates and frees various-sized memory blocks and prints the heap state after each operation.

## Makefile Commands

The project includes a Makefile to simplify compilation and execution. Key commands include:

- **`make`**: Uses `make build` and `make run`.
- **`make build`**: Compiles the project and generates the executable `memory_management.out`.
- **`make run`**: Executes the compiled program (`memory_management.out`).
- **`make clean`**: Removes generated files and the executable.

## Further Information

For additional details on function parameters, return values, and specific implementation details, refer to the source code and header files.
This memory management implementation is intentionally kept simple to serve as a learning tool for understanding the fundamental principles and trade-offs in dynamic memory allocation systems. The primary goal was to explore the balance between locality, fragmentation, and allocation speed. Please note that this is a basic implementation designed for educational purposes, emphasizing the foundational aspects of memory management.

## Contributions

Contributions, feedback, and suggestions are welcome. Feel free to share your thoughts and improvements.

---
*This project is part of a course at UCL in the Civil Engineering program, focusing on computer science.*