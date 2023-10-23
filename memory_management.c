#include <stdio.h>
#include <stdint.h>

const uint16_t HEAP_SIZE = 64000;
const uint8_t SIZE_HEADER = 2;
uint16_t nextfit_offset;

uint8_t MY_HEAP[64000];



void init()
{
    uint16_t *my_heap = (uint16_t *) MY_HEAP;

    nextfit_offset = 0;                                 // nextfit offset
    my_heap[nextfit_offset] = HEAP_SIZE - SIZE_HEADER;  // size of the first free block
}


/*
* Merge the current free block with next blocks if they are availables in the heap and free.
* @param my_heap:          pointer to the heap (uint16_t array)
* @param loc:              location of the current free block
* @param available_size:   pointer to the size of the current free block
* @return:                 void
*/
void merge_next_free_blocks(uint16_t *my_heap, uint16_t loc, uint16_t *available_size)
{
    uint16_t next_loc = loc + (*available_size + SIZE_HEADER) / 2;
    uint16_t new_size_block;

    while ((next_loc <= (HEAP_SIZE - 2 * SIZE_HEADER) / 2) &&  // The next block is available in the heap
            !(my_heap[next_loc] & 0x1))                        // The next block is free
    {
        // Merge the next block with the current one
        new_size_block = my_heap[next_loc];
        next_loc +=  (new_size_block + SIZE_HEADER) / 2;
        *available_size += new_size_block + SIZE_HEADER;
    }

    // Update the size of the current block
    my_heap[loc] = *available_size;
}


/*
* Initialize an allocated block.
* @param my_heap:          pointer to the heap (uint16_t array)
* @param loc:              location of the free block
* @param available_size:   pointer to the size of the free block
* @param size:             size requested for the allocatation
* @return:                 1 if the block has been allocated, 0 otherwise
*/
uint8_t initialize_allocated_block(uint16_t *my_heap, uint16_t loc, uint16_t *available_size, uint16_t size)
{
    uint16_t next_loc = loc + (size + SIZE_HEADER) / 2;

    // If the block is the last one (with no header after it) (special case)
    if (next_loc == HEAP_SIZE / 2)
    {
        my_heap[loc] = size;
        nextfit_offset = 0;        // Start from the beginning of the heap
    }
    // If the block is not available in the heap (special case)
    else if (next_loc > HEAP_SIZE / 2) return 0; // Return 0 to indicate EXIT_FAILURE

    // If the block is not the last one and are available in the heap (general case)
    else
    {
        // If the block is big enough to allocate the requested size + 2 bytes for the header + remaining size
        if (size <= *available_size - 2 * SIZE_HEADER)
        {
            uint16_t leftovers = *available_size - size - SIZE_HEADER;
            my_heap[next_loc] = leftovers;
            my_heap[loc] = size;
        }
        // Allocate the whole block
        else  my_heap[loc] = *available_size;
    }

    my_heap[loc] += 0x1; // Mark the block as allocated
    return 1; // Return 1 to indicate EXIT_SUCCESS
}


void *my_malloc(size_t size, uint8_t verbose)
{
    // Check the validity of the arguments
    if ((size > HEAP_SIZE - SIZE_HEADER) || (size <= 0)) return NULL;

    uint16_t *my_heap = (uint16_t *) MY_HEAP;

    if (size & 0x1) size++; // If size is uneven, add 1

    // Get the initial and current loc in the heap (nextfit strategy)
    uint16_t initial_loc = nextfit_offset;
    uint16_t loc = initial_loc;

    /*
    * If we reached the end of the heap => has_looped = 1
    * => we can start from the beginning until initial_loc
    * to make sure we don't miss any free block
    */
    uint8_t has_looped = 0;
    uint16_t available_size;

    while (((loc < HEAP_SIZE / 2) && !has_looped) ||  // If we haven't reached the end of the heap and we haven't looped
            (has_looped && (loc <= initial_loc)))     // If we have looped and we haven't reached the initial loc
    {
        // If the block is allocated, go to the next one
        if (my_heap[loc] & 0x1) loc += (my_heap[loc] - 1 + SIZE_HEADER) / 2;

        // If the block is free
        else
        {
            available_size = my_heap[loc];

            // Merge the next free blocks, update the available size
            merge_next_free_blocks(my_heap, loc, &available_size);

            // If the block is big enough to allocate the requested size
            if (size <= available_size)
            {

                // Initialize the allocated block
                uint8_t result_value = initialize_allocated_block(my_heap, loc, &available_size, size);
                if (result_value == 0) return NULL; // If the block is not available in the heap, return NULL

                /*
                * Get the adress of the allocated block (for a uint8_t array!)
                * and point to the first allocated byte and return it
                */
                return (void *) (MY_HEAP + 2 * loc + SIZE_HEADER);
            }
            // If the block is not big enough, go to the next one
            else loc += (available_size + SIZE_HEADER) / 2;
        }

        // If we reached the end of the heap, start from the beginning
        if (loc >= HEAP_SIZE / 2 && has_looped == 0)
        {
            has_looped = 1;
            loc = 0;
        }
    }

    // If we haven't found any free block, return NULL
    return NULL;
}


void my_free(void *pointer)
{
    // Check the validity of the argument
    if (pointer == NULL) return;
    
    // Get the block header
    uint16_t *header = (uint16_t *) pointer - SIZE_HEADER / 2;

    // Check the validity of the block header
    if (header == NULL) return;

    uint16_t *my_heap = (uint16_t *) MY_HEAP;

    *header &= 0xFFFE; // Mark the block as free

    // Merge the next free blocks if they are availables in the heap and free
    merge_next_free_blocks(my_heap, header - my_heap, header);

    // If the nextfit offset is in the middle of the merged blocks, update it at the beginning of the merged blocks
    uint16_t loc = header - my_heap;
    if ((loc < nextfit_offset) && nextfit_offset <= loc + my_heap[loc]) nextfit_offset = loc; // Update the nextfit offset    
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

    uint8_t *first = (uint8_t *) my_malloc(2, 0);
    *first = 2;
    print_HEAP();

    uint8_t *second = (uint8_t *) my_malloc(11, 0);
    *second = 1;
    print_HEAP();

    uint8_t *third = (uint8_t *) my_malloc(42, 0);
    *third = 243;

    uint8_t *d = my_malloc(30, 0);
    uint8_t *e = my_malloc(30, 0);

    print_HEAP();

    my_free(e);

    print_HEAP();

    uint8_t *f = my_malloc(45, 0);
    print_HEAP();

    uint8_t *h = my_malloc(63806, 1);
    *f = 5;

    uint8_t *dd = my_malloc(19, 0);
    print_HEAP();

    uint8_t *ssdd = my_malloc(25, 0);
    print_HEAP();

    uint8_t *ssss = my_malloc(1, 0);
    print_HEAP();
    
    my_free(d);
    my_free(third);
    my_free(f);

    print_HEAP();

    uint8_t *ssssssss = my_malloc(123, 0);

    print_HEAP();
}