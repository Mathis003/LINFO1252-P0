#include <stdio.h>
#include <stdint.h>

const uint16_t HEAP_SIZE = 64000;
const uint8_t SIZE_HEADER = 2;
uint16_t nextfit_offset;

uint8_t MY_HEAP[64000];


void init()
{
    uint16_t *my_heap = (uint16_t *) MY_HEAP;

    nextfit_offset = 0;                    // nextfit offset
    my_heap[0] = HEAP_SIZE - SIZE_HEADER;  // size of the first free block
}


/*
* Merge the current free block with next blocks if they are availables in the heap and free.
* @param my_heap:          pointer to the heap (uint16_t array)
* @param loc:              location of the current free block (index in the heap)
* @param available_size:   pointer to the size of the current free block
* @return:                 the size of the new merged free block
*/
uint16_t merge_next_free_blocks(uint16_t *my_heap, uint16_t loc)
{
    uint16_t available_size = my_heap[loc];
    uint16_t next_loc = loc + (available_size + SIZE_HEADER) / 2;
    uint16_t new_size_block;

    while ((next_loc <= (HEAP_SIZE - 2 * SIZE_HEADER) / 2) &&  // The next block is available in the heap
            !(my_heap[next_loc] & 0x1))                        // The next block is free
    {
        // Add the size of the next block to the current block
        new_size_block = my_heap[next_loc];
        next_loc +=  (new_size_block + SIZE_HEADER) / 2;
        available_size += new_size_block + SIZE_HEADER;
    }

    // Update the new size of the new merged block
    uint16_t merged_size = available_size;
    my_heap[loc] = merged_size;
    return merged_size;
}


/*
* Initialize an allocated block.
* @param my_heap:          pointer to the heap (uint16_t array)
* @param loc:              location of the free block (index in the heap)
* @param available_size:   size of the free block
* @param size:             size requested by the user for the allocatation
* @return:                 1 if the block has been allocated, 0 otherwise
*/
uint8_t initialize_allocated_block(uint16_t *my_heap, uint16_t loc, uint16_t available_size, uint16_t size)
{
    if (size > available_size) return 0; // Return 0 to indicate EXIT_FAILURE

    uint16_t next_loc = loc + (size + SIZE_HEADER) / 2;

    // Special case: If the block is the last one (with no header after it) (special case)
    if (next_loc == HEAP_SIZE / 2)
    {
        my_heap[loc] = size;
        nextfit_offset = 0;
    }

    // Special case: If the block is not available in the heap (special case)
    else if (next_loc > HEAP_SIZE / 2) return 0; // Return 0 to indicate EXIT_FAILURE

    // General case: If the block is not the last one and are available in the heap
    else
    {
        // If the block is big enough to allocate the requested size + 2 bytes for the header + the remaining size (minimal 2 bytes)
        if (size <= available_size - 2 * SIZE_HEADER)
        {
            uint16_t leftovers = available_size - (size + SIZE_HEADER);
            my_heap[loc] = size;
            my_heap[next_loc] = leftovers;
            nextfit_offset = next_loc;
        }
        
        // Allocate the whole block
        else 
        {
            my_heap[loc] = available_size;
            nextfit_offset = loc + (available_size + SIZE_HEADER) / 2;
        }
    }

    my_heap[loc] += 0x1;  // Mark the block as allocated
    return 1;             // Return 1 to indicate EXIT_SUCCESS
}


/*
* If we reached the end of the heap, start from the beginning.
* @param loc:           pointer to the location of the current block (index in the heap)
* @param has_looped:    pointer to a boolean indicating if we have already looped
* @return:              void
*/
void looped(uint16_t *loc, uint8_t *has_looped)
{
    if (*loc >= HEAP_SIZE / 2 && !(*has_looped))
    {
        *has_looped = 1;
        *loc = 0;
    }
}


void *my_malloc(size_t size)
{
    // Check the validity of the arguments
    if ((size > HEAP_SIZE - SIZE_HEADER) || (size <= 0)) return NULL;

    uint16_t *my_heap = (uint16_t *) MY_HEAP;

    // If size is uneven, add 1
    if (size & 0x1) size++;

    uint16_t initial_loc = nextfit_offset;
    uint16_t loc = initial_loc;

    uint16_t available_size;
    uint8_t success_allocation;
    uint8_t has_looped = 0;

    while (((loc < HEAP_SIZE / 2) && !has_looped) ||  // If we haven't reached the end of the heap and we haven't looped
            (has_looped && (loc <= initial_loc)))     // If we have looped and we haven't reached the initial loc
    {
        // If the block is allocated, go to the next one
        if (my_heap[loc] & 0x1) loc += (my_heap[loc] - 1 + SIZE_HEADER) / 2;

        // If the block is free, check if we can merge it with the next ones and then check if we can allocate it
        else
        {
            available_size = merge_next_free_blocks(my_heap, loc);
            success_allocation = initialize_allocated_block(my_heap, loc, available_size, size);

            if (success_allocation) return (void *) (MY_HEAP + 2 * loc + SIZE_HEADER);
            else loc += (available_size + SIZE_HEADER) / 2;
        }

        // If we reached the end of the heap, start from the beginning
        looped(&loc, &has_looped);
    }

    // If we haven't found any free block, return NULL
    return NULL;
}


void my_free(void *pointer)
{
    // Check the validity of the argument
    if (pointer == NULL) return;
    
    uint16_t *header_block = (uint16_t *) pointer - SIZE_HEADER / 2;

    if (header_block == NULL) return;
    
    uint16_t *my_heap = (uint16_t *) MY_HEAP;

    *header_block &= 0xFFFE; // Mark the block as free

    uint16_t loc = header_block - my_heap;
    merge_next_free_blocks(my_heap, loc);

    // If the nextfit offset is in the middle of the merged blocks, update it at the beginning of the merged blocks
    if ((loc < nextfit_offset) && nextfit_offset <= loc + my_heap[loc] / 2) nextfit_offset = loc;
}



void print_HEAP()
{
    printf("==== PRINT THE HEAP ====\n");

    uint16_t loc = 0;
    uint16_t size;

    while (loc < HEAP_SIZE)
    {
        size = (MY_HEAP[loc] & 0xFE) + (MY_HEAP[loc + 1] << 8);

        if (!(MY_HEAP[loc] & 0x1)) printf("free    \t");
        else printf("allocated\t");

        printf("adress: %d\t", loc);
        printf("size: %d \t", size);
        printf("value: [ ");

        if (!(MY_HEAP[loc] & 0x1)) printf("... ");
        else
        {
            uint16_t zero_counter = 0;
            for (uint16_t i = loc + SIZE_HEADER; i < loc + size + SIZE_HEADER; i++)
            {
                if (MY_HEAP[i] == 0) zero_counter++;                
                else
                {
                    if (zero_counter > 0) printf("0*%d ", zero_counter);
                    zero_counter = 0;
                    printf("%x ", MY_HEAP[i]);
                }
            }
            if (zero_counter > 0) printf("0*%d ", zero_counter);
        }
        printf("]\n");
        loc += size + SIZE_HEADER;
    }
    printf("\n");
}


int main(int argc, char *argv[])
{
    init();
    print_HEAP();

    uint8_t *a = (uint8_t *) my_malloc(2);
    print_HEAP();

    uint8_t *b = (uint8_t *) my_malloc(11);
    print_HEAP();

    uint8_t *third = (uint8_t *) my_malloc(42);
    uint8_t *dsqddd = my_malloc(30);
    uint8_t *sq = my_malloc(30);
    print_HEAP();

    my_free(sq);
    print_HEAP();

    uint8_t *fff = my_malloc(45);
    print_HEAP();

    uint8_t *d = my_malloc(63806);
    uint8_t *e = my_malloc(19);
    print_HEAP();

    uint8_t *f = my_malloc(25);
    print_HEAP();

    uint8_t *ssss = my_malloc(1);
    print_HEAP();

    my_free(dsqddd);
    print_HEAP();

    my_free(third);
    print_HEAP();

    my_free(fff);
    print_HEAP();

    uint8_t *c = my_malloc(123);
    print_HEAP();

    uint8_t *ssfz = my_malloc(122);
    print_HEAP();

    uint8_t *vv = my_malloc(0);
    print_HEAP();

    a[0] = 32;
    a[1] = 3;

    *(b + 0) = 1;
    *(b + 1) = 43;
    *(b + 2) = 44;
    *(b + 3) = 61092;

    *(d + 262143) = 3;

    print_HEAP();
}