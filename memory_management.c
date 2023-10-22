#include <stdio.h>
#include <stdint.h>

const uint16_t HEAP_SIZE = 64000;
const uint8_t SIZE_HEADER = 2;
uint8_t MY_HEAP[64000];



void init()
{
    uint16_t *my_heap = (uint16_t *) MY_HEAP;

    // Clear memory
    for (uint16_t i = 0; i < HEAP_SIZE / 2; i++) my_heap[i] = 0; 

    my_heap[0] = SIZE_HEADER / 2;              // nextfit ptr
    my_heap[1] = HEAP_SIZE - 2 * SIZE_HEADER;  // size of the first free block
}


void merge_next_free_blocks(uint16_t *my_heap, uint16_t loc, uint16_t *available_size)
{
    uint8_t will_merge = 0;
    uint16_t next_loc = loc + (*available_size + SIZE_HEADER) / 2;

    while ((next_loc <= (HEAP_SIZE - 3 * SIZE_HEADER) / 2) & !(my_heap[next_loc] & 0x1))
    {
        will_merge = 1;
        uint16_t new_size_block = my_heap[next_loc];
        *available_size += new_size_block + SIZE_HEADER;
        next_loc +=  (new_size_block + SIZE_HEADER) / 2;
    }

    if (will_merge) my_heap[loc] = *available_size;
}


uint8_t initialize_allocated_block(uint16_t *my_heap, uint16_t loc, uint16_t *available_size, uint16_t size)
{
    uint16_t next_block_header = loc + (size + SIZE_HEADER) / 2;

    // If the block is the last one (with no header)
    if (next_block_header == HEAP_SIZE / 2)
    {
        my_heap[loc] = size + 0x1; // Mark the block as allocated
        my_heap[0] = 1;
        return 1;
    } else if (next_block_header >= HEAP_SIZE / 2) return 0;

    if (*available_size - 2 * SIZE_HEADER >= size)
    {
        uint16_t leftovers = *available_size - size - SIZE_HEADER;
        my_heap[next_block_header] = leftovers;
        my_heap[loc] = size + 0x1; // Mark the block as allocated
    } else
    {
        my_heap[loc] = *available_size + 0x1; // Mark the block as allocated
    }

    if (next_block_header >= HEAP_SIZE / 2) next_block_header = 2;
    my_heap[0] = next_block_header;
    return 1;
}


void *my_malloc(size_t size, uint8_t verbose)
{
    // Check the validity of the arguments
    if ((size > HEAP_SIZE - 3 * SIZE_HEADER) || (size <= 0)) return NULL;

    uint16_t *my_heap = (uint16_t *) MY_HEAP;

    // If uneven, add 1
    if (size & 0x1) size++;

    // Get the current loc in the heap
    uint16_t initial_loc = my_heap[0];
    uint16_t loc = initial_loc;
    uint16_t available_size;
    uint8_t has_looped = 0;

    while (((loc < HEAP_SIZE / 2) && !has_looped) || (has_looped && (loc <= initial_loc)))
    {
        if (my_heap[loc] & 0x1) // If the block is allocated
        {
            loc += (my_heap[loc] - 1 + SIZE_HEADER) / 2;
        } else // If the block is free
        {
            available_size = my_heap[loc];
            merge_next_free_blocks(my_heap, loc, &available_size);

            // If the block is big enough
            if (size <= available_size)
            {
                uint8_t result_value = initialize_allocated_block(my_heap, loc, &available_size, size);
                if (result_value == 0) return NULL;
                char *adress = ((char *) MY_HEAP + (2 * loc + SIZE_HEADER));
                return (void *) adress;
            }

            loc += (available_size + SIZE_HEADER) / 2;
            printf("loc: %d\n", loc);
        }

        if (loc >= HEAP_SIZE / 2 && has_looped == 0)
        {
            has_looped = 1;
            loc = 1;
        }
    }
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

    // Mark the block as free
    *header &= 0xFFFE;

    // Merge the next free blocks
    merge_next_free_blocks(my_heap, header - my_heap, header);
    
    if ((header - my_heap < my_heap[0]) && (my_heap[0] <= my_heap[header - my_heap])) my_heap[0] = header - my_heap;
}





void print_HEAP()
{
    printf("==== PRINT THE HEAP ====\n");

    uint16_t loc = 2;
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



int main(int argc, char **argv)
{
    init();

    uint8_t *first = (uint8_t *) my_malloc(63992, 0);

    print_HEAP();
    uint8_t *second = (uint8_t *) my_malloc(11, 0);
    print_HEAP();
}