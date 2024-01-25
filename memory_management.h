#ifndef MEMORY_MANAGEMENT_H
#define MEMORY_MANAGEMENT_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

const uint16_t HEAP_SIZE = 64000;
const uint8_t SIZE_HEADER = 2;

uint8_t MY_HEAP[64000];

/*
* Print the HEAP.
*
* @return: void
*/
void print_HEAP();

/*
* Merge the current free block with next blocks if they are availables in the heap and free.
*
* @param my_heap:          pointer to the heap (uint16_t array)
* @param loc:              location of the current free block (index in the heap)
* @param available_size:   pointer to the size of the current free block
* @return:                 the size of the new merged free block
*/
uint16_t merge_next_free_blocks(uint16_t *my_heap, uint16_t loc);

/*
* Initialize an allocated block.
*
* @param my_heap:          pointer to the heap (uint16_t array)
* @param loc:              location of the free block (index in the heap)
* @param available_size:   size of the free block
* @param size:             size requested by the user for the allocatation
* @return:                 1 if the block has been allocated, 0 otherwise
*/
void initialize_allocated_block(uint16_t *my_heap, uint16_t loc, uint16_t available_size, uint16_t size);


/*
* If we reached the end of the heap, start from the beginning.
*
* @param loc:           pointer to the location of the current block (index in the heap)
* @param has_looped:    pointer to a boolean indicating if we have already looped
* @return:              void
*/
void looped(uint16_t *loc, uint8_t *has_looped);

/*
* Initialize the heap with a two metadata :
* - The first one is the nextfit offset (index in the heap)
* - The second one is the size of the first free block
*
* @param: void
* @return: void
*/
void init();

/*
* Allocate a block of memory of the given size.
*
* @param size:     size of the block to allocate
* @return:         pointer to the allocated block
*/
void *my_malloc(size_t size);

/*
* Free a block of memory.
*
* @param pointer:  pointer to the block to free
* @return:         void
*/
void my_free(void *pointer);

/*
* Main function.
*
* @param argc:     number of arguments
* @param argv:     array of arguments
* @return:         0 if the program has been executed successfully, 1 otherwise
*/
int main(int argc, char *argv[]);

#endif /* MEMORY_MANAGEMENT_H */