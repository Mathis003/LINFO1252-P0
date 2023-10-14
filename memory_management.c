#include <stdio.h>
#include <stdint.h>

uint8_t MY_HEAP[64000];


void init()
{
    // Clear memory from index 4 to 63999
    for (uint16_t i = 4; i < 64000; i++) MY_HEAP[i] = 0; 

    // Initialize the index of the next block (update it when we allocate a new block)
    MY_HEAP[0] = 2;
    MY_HEAP[1] = 0;
    
    // Initialize header bytes
    MY_HEAP[2] = 0xFC;
    MY_HEAP[3] = 0xF9;
}


void *my_malloc(size_t size)
{   
    // Check the validity of the arguments
    if ((size > 63996) || (size <= 0)) return NULL;

    // If uneven, add 1
    if (size & 1) size++;

    // Get the current location in the heap
    uint16_t location = MY_HEAP[0] + (MY_HEAP[1] << 8);
    uint16_t available_size;
    uint16_t next_header_location;
    uint8_t will_merge;

    while (location + 1 < 64000)
    {
        if (MY_HEAP[location] & 0x1)
        {
            // Move to the next block
            location += (MY_HEAP[location] + (MY_HEAP[location + 1] << 8)) + 1;
        } else
        {
            // Get available size in the current block
            available_size = MY_HEAP[location] + (MY_HEAP[location + 1] << 8);
            will_merge = 0;
            
            // Check if the next block is free and can be merged
            next_header_location = location + available_size + 2;
            while ((next_header_location <= 63996) & !(MY_HEAP[next_header_location] & 0x1))
            {
                will_merge = 1;
                uint16_t new_size_block = MY_HEAP[next_header_location] + (MY_HEAP[next_header_location + 1] << 8);
                available_size += new_size_block + 2;
                next_header_location +=  new_size_block + 2;
            }

            // Move outside the while loop if we have enough bytes availables
            if (size <= available_size || MY_HEAP[location] == 0) break;
            else if (will_merge)
            {
                MY_HEAP[location] = available_size;
                MY_HEAP[location + 1] = available_size >> 8;
            }
            location += available_size + 2;
        }
    }

    uint16_t next_block_header = location + size + 2;

    if (location + size + 2 > 64000) return NULL;

    // If there are leftover bytes, update the header for the remaining free block
    if (size < available_size)
    {
        uint16_t leftovers = available_size - (size + 2);
        if (leftovers & 0x1) leftovers++;

        MY_HEAP[next_block_header] = leftovers;
        MY_HEAP[next_block_header + 1] = leftovers >> 8;;
    }
    
    // Write the header for the allocated block and the next one
    MY_HEAP[location] = size + 1;
    MY_HEAP[location+1] = size >> 8;
    if (next_block_header == 64000) next_block_header = 2;
    MY_HEAP[0] = next_block_header;
    MY_HEAP[1] = next_block_header >> 8;

    // Return the address of the allocated block
    char *adress = ((char *) MY_HEAP + location + 2);
    return (void *) adress;
}


void my_free(void *pointer)
{
    // Check the validity of the argument
    if (pointer == NULL) return;

    // Get the block header
    uint8_t *header = (uint8_t *) pointer - 2;

    // Check the validity of the block header
    if (header == NULL) return;

    // Mark the block as free
    *header &= 0xFE;
}


void print_HEAP()
{
    printf("==== PRINT THE HEAP ====\n");

    uint16_t location = 2;
    uint16_t size = 0;

    while (location < 64000)
    {
        size = (MY_HEAP[location] & 0xFE) + (MY_HEAP[location + 1] << 8);

        if (!(MY_HEAP[location] & 0x1)) printf("free    \t");
        else printf("allocated\t");

        printf("adress: %d\t", location);
        printf("size: %d \t", size);
        printf("value: [ ");

        if (!(MY_HEAP[location] & 0x1)) printf("... ");
        else
        {
            uint16_t zero_counter = 0;
            for (uint16_t i = location + 1; i < location + size + 1; i++)
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
        location += size + 2;
    }
    printf("\n");
}





void one_allocation()
{
    init();
    uint8_t *first = (uint8_t *) my_malloc(7);
    *first = 132;
    print_HEAP();
    init();
}

void multi_big_allocation()
{
    init();
    uint8_t *premier = (uint8_t *) my_malloc(7001);
    uint8_t *deuxieme = (uint8_t *) my_malloc(34562);
    uint8_t *troisieme = (uint8_t *) my_malloc(1);
    uint8_t *quatre = (uint8_t *) my_malloc(3);
    uint8_t *cinq = (uint8_t *) my_malloc(3);
    uint8_t *six = (uint8_t *) my_malloc(22412);
    print_HEAP();
    init();
}

void one_free()
{
    init();
    uint8_t *first = (uint8_t *) my_malloc(7);
    *first = 132;
    
    my_free(first);
    print_HEAP();
    init();
}

void multi_big_free()
{
    init();
    uint8_t *premier = (uint8_t *) my_malloc(7001);
    uint8_t *deuxieme = (uint8_t *) my_malloc(34562);
    uint8_t *troisieme = (uint8_t *) my_malloc(1);
    my_free(troisieme);
    uint8_t *quatre = (uint8_t *) my_malloc(3);
    uint8_t *cinq = (uint8_t *) my_malloc(3);
    my_free(quatre);
    uint8_t *six = (uint8_t *) my_malloc(22412);
    my_free(deuxieme);
    my_free(six);
    my_free(cinq);
    my_free(premier);
    print_HEAP();
    init();
}

void merged_free()
{
    init();
    uint8_t *premier = (uint8_t *) my_malloc(3);
    uint8_t *deuxieme = (uint8_t *) my_malloc(3);
    uint8_t *troisieme = (uint8_t *) my_malloc(1);
    my_free(troisieme);
    uint8_t *quatre = (uint8_t *) my_malloc(34562);
    uint8_t *cinq = (uint8_t *) my_malloc(7001);
    my_free(quatre);
    uint8_t *six = (uint8_t *) my_malloc(22412);
    my_free(deuxieme);
    my_free(six);
    my_free(cinq);
    my_free(premier);

    uint8_t *sept = (uint8_t *) my_malloc(6);
    print_HEAP();
    init();
}

void size_too_big()
{
    init();
    uint8_t *all_memory = (uint8_t *) my_malloc(64000);

    if (all_memory == NULL) printf("=== Test Passed ===\n");
    else printf("=== Test Failed ===\n");
    init();
}

void size_too_low()
{
    init();
    uint8_t *no_memory1 = (uint8_t *) my_malloc(0);
    uint8_t *no_memory2 = (uint8_t *) my_malloc(-302);

    if ((no_memory1 == NULL) && (no_memory2 == NULL)) printf("=== Test Passed === \n");
    else printf("=== Test Failed === \n");
    
    init();
}


int main(int argc, char **argv)
{
    // size_too_big();
    // size_too_low();
    // one_allocation();
    // one_free();
    // multi_big_allocation();
    // multi_big_free();
    merged_free();

    // init();
    // print_HEAP();

    // uint8_t *first = (uint8_t *) my_malloc(2);
    // *first = 132;
    // print_HEAP();

    // uint8_t *second = (uint8_t *) my_malloc(11);
    // *second = 1;

    // uint8_t *third = (uint8_t *) my_malloc(42);
    // *third = 243;

    
    // uint8_t *a = my_malloc(10);
    // uint8_t *b = my_malloc(10);
    // uint8_t *c = my_malloc(22);
    // uint8_t *d = my_malloc(30);
    // uint8_t *e = my_malloc(30);

    // print_HEAP();

    // my_free(a);
    // my_free(b);
    // my_free(d);
    // my_free(e);

    // print_HEAP();

    // uint8_t *f = my_malloc(30);

    // print_HEAP();

    // my_free(first);
    // print_HEAP();

    // my_free(third);
    // print_HEAP();

    // my_free(second);

    // print_HEAP();
    
    // uint8_t *fourth = (uint8_t *) my_malloc(14);
    // *fourth = 1;

    // print_HEAP();

    // my_free(c);
    // my_free(d);

    // print_HEAP();

    // uint8_t *i = my_malloc(1);

    // print_HEAP();

    // my_free(e);

    // print_HEAP();

    // uint8_t *g = my_malloc(40);
    // uint8_t *h = my_malloc(32);
    
    // print_HEAP();
    // uint8_t *fitht = (uint8_t *) my_malloc(30);
    // *fitht = 1;

    // print_HEAP();

    // uint8_t *dss = (uint8_t *) my_malloc(63660);

    // *dss = 32;

    // print_HEAP();
}