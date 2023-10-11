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
    
    uint8_t header = UINT16_MAX << 1;
    MY_HEAP[0] = header;
}


/*
* TODO: Trouver un moyen de savoir si le header prend un byte ou deux bytes.
* TODO: !! au types (uint8_t / uint16_t / size_t) => Pour l'instant, c'est un peu au pif.
        (tout en gardant le fait qu'on peut encoder 64000 sur 2 bytes).
* TODO: Fragmenter les blocs de la taille voulu => Si bloc de 10 bytes dispo
        mais je veux que 4 bytes, alors je fragmente en 4 / 6 ou en 8 / 2 (dépend de l'alignement).
* TODO: Mettre un header à la fin du bloc aussi (sera utile pour my_free()).
* TODO: Réfléchir à la méthode pour savoir quel bloc choisir (first fit, best fit, next fit)
        => Pour l'instant, on a implémenté 'first fit'.
* TODO: Faire en sorte que les blocs disponibles soit rassemblés en gros bloc de grande taille
        => Pour l'instant, c'est tous des blocs individuels.
*/
void *my_malloc(size_t size)
{
    if ((size > 64000) || (size <= 0)) return NULL;

    uint16_t current_location = 0;
    uint16_t available_size;

    while(current_location <= 64000)
    {
        if (MY_HEAP[current_location] & 0x1)
        {
            current_location += (MY_HEAP[current_location] >> 1) + 1;
        } else
        {
            available_size = MY_HEAP[current_location] >> 1;            

            if (size <= available_size)
            {
                uint16_t remaining_size = available_size - size;

                if (remaining_size > 0)
                {
                    MY_HEAP[current_location] = (size << 1) + 0x1;
                    uint16_t new_header = ((remaining_size - 1) << 1);
                    MY_HEAP[current_location + size + 1] = new_header;
                } else
                {
                    MY_HEAP[current_location] &= 0x1;
                }

                char *adress = ((char *) MY_HEAP + current_location + 1);
                return (void *) adress;
            }

            current_location += available_size + 1;
        }
    }
    return NULL;
}


void my_free(void *pointer)
{
    if (pointer == NULL) return;

    uint8_t *new_ptr = (uint8_t *) pointer;
    new_ptr--;

    if ((new_ptr == NULL) | !(*new_ptr & 0x1)) return;

    uint16_t size_block = *new_ptr >> 1;
    uint8_t *next_header = new_ptr + size_block + 1;

    //TODO: Same pour le header d'avant

    if ((*next_header != 0) & !(*next_header & 0x1))
    {
        // Fusionner les deux blocs en un
        uint16_t size_total = size_block + (*next_header >> 1) + 1;
        uint8_t merged_header = size_total << 1;
        *next_header = 0;
        *new_ptr = merged_header;
    } else *new_ptr -= 0x1;
}


void print_HEAP(){

    uint16_t location = 0;
    
    printf("==== PRINT THE HEAP ====\n");

    size_t size = 0;
    while (location <= 100)
    {
        // if (MY_HEAP[location] == ((uint8_t) 0)) return;
        
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
    
        location += size + 1;
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    init();
    print_HEAP();

    uint16_t *a = (uint16_t *) my_malloc(4);
    uint8_t *b = (uint8_t *) my_malloc(10);
    uint8_t *c = (uint8_t *) my_malloc(10);
    uint8_t *d = (uint8_t *) my_malloc(11);
    *d = 122;
    *b = (uint8_t) 200;
    *a = (uint16_t) 2000;
    *c = 12;

    print_HEAP();

    my_free(c);

    print_HEAP();

    my_free(b);

    print_HEAP();


    print_HEAP();

    uint8_t *e = (uint8_t *) my_malloc(8);
    *e = 902;
    print_HEAP();
}