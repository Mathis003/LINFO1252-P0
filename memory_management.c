#include <stdio.h>
#include <stdint.h>

uint8_t MY_HEAP[64000];


/*
Examples of pseudo-code (come from syllabus) :

parcours d'une liste implicite:
    h = start;
    while (h < end &&           // fin de liste ?
        ((*h & 0x1) != 0 ||  // déjà alloué
            *h <= len)) {        // trou trop petit
        h = h + (*h & ~0x1);    // progresse vers le prochain bloc
    }

fonction de placement d'un nouveau bloc de taille len à l'adresse pointée par h et retournée précédemment:
    void place_block(ptr h, int len) {
        int newsize = len + 1;                // ajoute 1 mot pour le header
        int oldsize = *h & ~0x1;              // récupère taille actuelle sans bit de poids faible
        *h = newsize | 0x1;                   // nouvelle taille avec bit de poids faible à 1
        if (newsize < oldsize)                // s'il reste de la place ...
            *(h + newsize) = oldsize - newsize; // nouveau bloc vide avec la taille restante et bit de poids faible à 0
        }
    }  

fonction free_block, prenant en argument l'adresse du header à libérer h:
    void free_block(ptr h) {
        *h = *h & ~0x1;          // remise à 0 du drapeau
        next = h + *h;           // calcul de l'adresse du header du bloc suivant
        if ((*next & 0x1) == 0)  // si ce bloc est aussi libre ...
            *h = *h + *next;       // combiner les blocs
        }
    }
*/

void init()
{
    const uint16_t HEAP_SIZE = 64000;
    const uint16_t NBERS_BYTES = 2;

    // Clear memory from index 4 to 63999
    for (uint16_t i = 0; i < 64000; i++) MY_HEAP[i] = 0; 

    // Initialize the next-fit offset
    uint8_t *nextfit_ptr = (uint8_t *) MY_HEAP;
    *nextfit_ptr = NBERS_BYTES; // The next free block is the first one (index 2)

    // Initialize the first block
    uint16_t *first_block = (uint16_t *) (MY_HEAP + NBERS_BYTES);

    uint16_t first_block_size = HEAP_SIZE - (NBERS_BYTES*2); // 63996 bytes available
    uint16_t next_free_block_offset = 0; // No next free block

    *first_block = first_block_size;

    // TODO: To implement (linked list for free-blocks)
    *(first_block + 1) = next_free_block_offset;
}


void *my_malloc(size_t size)
{   
    const uint16_t HEAP_SIZE = 64000;
    const uint16_t NBERS_BYTES = 2;

    // TODO: To implement (linked list for free-blocks)
    // size += 2 * NBERS_BYTES; // Add 4 bytes for the twos headers

    // Check the validity of the arguments
    if ((size >= HEAP_SIZE - NBERS_BYTES) || (size <= 0)) return NULL;

    // TODO: To implement (linked list for free-blocks)
    // uint8_t min_size_block = 3 * NBERS_BYTES;
    // size = size < min_size_block ? min_size_block : size;

    // If uneven, add 1
    if (size & 1) size++;

    // TODO: To implement (linked list for free-blocks)
    // uint16_t *nextfit_ptr = (uint16_t *) MY_HEAP;
    // uint16_t *current_block = (uint16_t *) MY_HEAP + *nextfit_ptr;
    // uint16_t current_block_size = *current_block;

    // Get the current location in the heap
    uint16_t initial_location = MY_HEAP[0] + (MY_HEAP[1] << 8);
    uint16_t location = initial_location;
    uint16_t available_size;
    uint16_t next_header_location;
    uint8_t will_merge;
    uint8_t has_looped = 0;

    while ((location + 1 < 64000 && has_looped == 0) || (has_looped == 1 && location < initial_location))
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

        if(location >= 64000 && has_looped == 0){
            has_looped = 1;
            location = 2;
        }

    }

    uint16_t next_block_header = location + size + 2;

    if (location + size + 2 > 64000) return NULL;

    // If there are leftover bytes, update the header for the remaining free block
    if (size < available_size)
    {
        uint16_t leftovers = available_size - (size + 2);

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
            for (uint16_t i = location + 2; i < location + size + 2; i++)
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
    printf("one_allocation\n"); 

    init();
    uint8_t *first = (uint8_t *) my_malloc(7);
    *first = 132;
    print_HEAP();
    init();
}

void multi_big_allocation()
{
    printf("multi_big_allocation\n");

    init();
    uint8_t *premier = (uint8_t *) my_malloc(7001);
    *premier = 132;
    uint8_t *deuxieme = (uint8_t *) my_malloc(34562);
    *deuxieme = 1;
    uint8_t *troisieme = (uint8_t *) my_malloc(1);
    *troisieme = 243;
    uint8_t *quatre = (uint8_t *) my_malloc(3);
    *quatre = 1;
    uint8_t *cinq = (uint8_t *) my_malloc(3);
    *cinq = 1;
    uint8_t *six = (uint8_t *) my_malloc(22412);
    *six = 1;
    print_HEAP();
    init();
}

void one_free()
{
    printf("one_free\n");

    init();
    uint8_t *first = (uint8_t *) my_malloc(7);
    *first = 132;
    
    my_free(first);
    print_HEAP();
    init();
}

void multi_big_free()
{
    printf("multi_big_free\n");

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
    printf("merged_free\n");

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
    *sept = 1;
    print_HEAP();
    init();
}

void size_too_big()
{
    printf("size_too_big\n");

    init();
    uint8_t *all_memory = (uint8_t *) my_malloc(63997);

    if (all_memory == NULL) printf("=== Test Passed ===\n");
    else printf("=== Test Failed ===\n");
    print_HEAP();
    init();
}

void max_allocation()
{
    printf("max_allocation\n");

    init();
    uint8_t *all_memory = (uint8_t *) my_malloc(63990);

    print_HEAP();

    my_free(all_memory);

    
    uint8_t *some_memory_again = (uint8_t *) my_malloc(400);

    uint8_t *some_memory_again2 = (uint8_t *) my_malloc(200);

    print_HEAP();

    init();
}

void size_too_low()
{
    printf("size_too_low\n");
    
    init();
    uint8_t *no_memory1 = (uint8_t *) my_malloc(0);
    uint8_t *no_memory2 = (uint8_t *) my_malloc(-302);

    if ((no_memory1 == NULL) && (no_memory2 == NULL)) printf("=== Test Passed === \n");
    else printf("=== Test Failed === \n");
    
    init();
}


int main(int argc, char **argv)
{
    init();

    print_HEAP();


    size_too_big();
    size_too_low();
    one_allocation();
    one_free();
    multi_big_allocation();
    multi_big_free();
    merged_free();

    max_allocation();

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