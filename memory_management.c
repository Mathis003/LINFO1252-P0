#include <stdio.h>
#include <stdint.h>

uint8_t MY_HEAP[64000];

/*
* TODO: Faire en sorte que les blocs disponibles soit rassemblés en un gros bloc de taille 64000.
*/
void init()
{
    for (size_t i = 0; i < 64000; i++)
    {
        MY_HEAP[i] = 0;
    }
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
    // Check the validity of the arguments
    if ((size > 64000) || (size <= 0)) return NULL;

    // Get the first available location
    uint16_t location = 0;
    uint16_t available_size;
    while(MY_HEAP[location] !=0 || location <= 64000)
    {
        if (MY_HEAP[location] & 0x1)
        {
            // Move to the next block
            location += (MY_HEAP[location] >> 1) + 1;
        } else
        {

            // Check if we have enough bytes availables
            available_size = MY_HEAP[location] >> 1;
            break; // Move outside the while loop
        }
    }

    // If no block available
    if (location > 64000) return NULL;
    if (size < available_size){
        printf("leftovers %d\n",location);
        uint16_t leftovers = available_size-size-1;
        uint8_t freed_header = (leftovers << 1);
        MY_HEAP[location+1+size] = freed_header;
    }
    // Write the header
    uint8_t header = (size << 1) + 0x1;
    MY_HEAP[location] = header;
    printf("header: %x\n", MY_HEAP[location]);

    // Get the adress to the first byte allocated
    char *adress = ((char *) MY_HEAP + location + 1);

    return (void *) adress;
}


void my_free(void *ptr)
{
    // Check the validity of the argument
    if (ptr == NULL) return;

    // Get the header associated to the block allocated
    uint8_t *new_ptr = (uint8_t *) ptr;
    new_ptr--;

    if (new_ptr == NULL) return;

    // Put the weak byte to 0 (= free block)
    if (*new_ptr & 0x1) *new_ptr = *new_ptr & 0xFE;
}


void print_HEAP(){

    uint16_t location = 0;
    
    printf("==== PRINT THE HEAP ====\n");
    size_t size = 0;
    while (1)
    {
        // print location
        if(MY_HEAP[location] == ((uint8_t)0)){
            return;
        }
        
        size = MY_HEAP[location]>>1;
        if(!(MY_HEAP[location] & 0x1)){
            printf("free    \t");
        }else
        {
            printf("allocated\t");
        }
        printf("adress: %d\t", location);
        printf("size: %d \t", (size));
        printf("value: [ ");
        for (size_t i = location+1; i < location+1+size; i++)
        {
            printf("%x ",MY_HEAP[i]);
        }
        printf("]\n");
        
        // printf("used: %d\n", location+1);
        location+=size+1;
    }

}

int main(int argc, char **argv)
{
    init();
    uint16_t *a = (uint16_t *) my_malloc(4);
    uint8_t *b = (uint8_t *) my_malloc(10);
    uint8_t *c = (uint8_t *) my_malloc(1);
    *b = (uint8_t)200;
    *a = (uint16_t)2000;

    print_HEAP();


    my_free(b);
    print_HEAP();

    uint8_t *d = (uint8_t *) my_malloc(8);
    print_HEAP();
}