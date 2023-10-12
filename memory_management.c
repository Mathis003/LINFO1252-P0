#include <stdio.h>
#include <stdint.h>

uint8_t MY_HEAP[64000];

/*
* TODO: Faire en sorte que les blocs disponibles soit rassemblés en un gros bloc de taille 64000.
*/
void init()
{
    for (uint16_t i = 0; i < 64000; i++)
    {
        MY_HEAP[i] = 0;
    }
    
    uint8_t header = 120 << 1;
    MY_HEAP[0] = header;
}


/*
* TODO: Trouver un moyen de savoir si le header prend un byte ou deux bytes.
* TODO: !! au types (uint8_t / uint16_t / size_t) => Pour l'instant, c'est un peu au pif.
        (tout en gardant le fait qu'on peut encoder 64000 sur 2 bytes).
* TODO: Trouver le bon alignement et l'implémenter.
* TODO: Réfléchir à la méthode pour savoir quel bloc choisir (first fit, best fit, next fit)
        => Pour l'instant, on a implémenté 'first fit'.
* TODO: Faire en sorte que les blocs disponibles soit rassemblés en gros bloc de grande taille
        => Pour l'instant, c'est tous des blocs individuels.
*/
void *my_malloc(size_t size)
{
    uint16_t HEAP_SIZE = 64000;

    if ((size > HEAP_SIZE) || (size <= 0)) return NULL;

    uint8_t current_location = 0;
    uint8_t available_size;

    while(current_location <= HEAP_SIZE)
    {
        if (MY_HEAP[current_location] & 0x1)
        {
            current_location += (MY_HEAP[current_location] >> 1) + 2;
        } else
        {
            available_size = MY_HEAP[current_location] >> 1;   

            if (size <= available_size)
            {
                uint8_t remaining_size = available_size - size;

                if (remaining_size > 0)
                {
                    uint8_t headers = (size << 1) + 0x1;
                    uint8_t new_headers = (remaining_size - size) << 1;

                    MY_HEAP[current_location] = headers;
                    MY_HEAP[current_location + size + 1] = headers;
                    MY_HEAP[current_location + size + 2] = new_headers;
                    MY_HEAP[current_location + available_size + 1] = new_headers;
                } else
                {
                    MY_HEAP[current_location] &= 0x1;
                    MY_HEAP[current_location + size + 1] &= 0x1;
                }

                char *adress = ((char *) MY_HEAP + current_location + 1);
                return (void *) adress;
            }

            current_location += available_size + 2;
        }
    }
    return NULL;
}


void my_free(void *pointer)
{
    if (pointer == NULL) return;

    uint8_t *ptr = (uint8_t *) pointer;
    ptr--;

    if ((ptr == NULL) | !(*ptr & 0x1)) return;

    uint8_t size_block = *ptr >> 1;
    uint8_t *next_header = ptr + (size_block + 2);
    ptr--;
    uint8_t size_previous_block = *ptr >> 1;
    ptr++;
    uint8_t *previous_header = ptr - (size_previous_block + 2);
    uint8_t size_total = size_block;
    uint8_t merged_header = *ptr - 0x1;

    if ((next_header != NULL) & (*next_header != 0x0) & !(*next_header & 0x1))
    {
        size_total += (*next_header >> 1) + 2;
        merged_header = size_total << 1;
        *(next_header + (*next_header >> 1) + 1) = NULL;
        *next_header = NULL;
    }
    
    if ((previous_header != NULL) & (*previous_header != 0x0) & !(*previous_header & 0x1))
    {
        size_total += (*previous_header >> 1) + 2;
        merged_header = size_total << 1;

        uint8_t *tmp_ptr = ptr;
        ptr = previous_header;
        previous_header = tmp_ptr;

        *(previous_header + (*previous_header >> 1) + 1) = NULL;
        *previous_header = NULL;
    }

    *ptr = merged_header;
    *(ptr + size_total) = merged_header;
}


void print_HEAP(){

    uint16_t location = 0;
    
    printf("==== PRINT THE HEAP ====\n");

    size_t size = 0;
    while (location <= 64000)
    {
        if (MY_HEAP[location] == 0)
        {
            printf("\n");
            return;
        }
        
        size = MY_HEAP[location] >> 1;

        if(!(MY_HEAP[location] & 0x1)) printf("free    \t");
        else printf("allocated\t");

        printf("adress: %d\t", location);
        printf("size: %d \t", (size));

        if(!(MY_HEAP[location] & 0x1)) printf("value: [ ... ");
        else
        {
            printf("value: [ ");
            for (size_t i = location + 1; i < location + size + 1; i++) printf("%x ", MY_HEAP[i]);
        }
        printf("]\n");
    
        location += size + 2;
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    init();
    print_HEAP();

    uint8_t *first = (uint8_t *) my_malloc(2);
    *first = 132;

    uint8_t *second = (uint8_t *) my_malloc(3);
    *second = 1;

    uint8_t *third = (uint8_t *) my_malloc(42);
    *third = 243;

    print_HEAP();

    my_free(first);
    print_HEAP();

    my_free(third);
    print_HEAP();

    my_free(second);
    print_HEAP();
}