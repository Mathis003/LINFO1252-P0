#include <stdio.h>
#include <stdint.h>

uint8_t MY_HEAP[64000];

/*
* TODO: Faire en sorte que les blocs disponibles soit rassemblés en un gros bloc de taille 64000.
*/
void init()
{
    for (uint16_t i = 2; i < 64000; i++)
    {
        MY_HEAP[i] = 0;
    }
    
    MY_HEAP[0] = 0xFE;
    MY_HEAP[1] = 0xF9;
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
    printf("malloc, size: %ld\n", size);
    // Check the validity of the arguments
    if ((size > 64000) || (size <= 0)) return NULL;

    // If uneven add 1
    if(size & 1){
        size++;
    }

    // Get the first available location
    uint16_t location = 0;
    uint16_t available_size;
    while(location < 64000)
    {
        if (MY_HEAP[location] & 0x1)
        {
            // Move to the next block
            location += (MY_HEAP[location] + (MY_HEAP[location+1] << 8)) + 1;
        } else
        {
            // get available size
            available_size = MY_HEAP[location] + (MY_HEAP[location+1] << 8);
            
            // check if next block is free
            while(!(MY_HEAP[location+available_size+2] & 1) && (location+available_size+2 < 64000)){
                available_size += MY_HEAP[location+available_size+2] + (MY_HEAP[location+available_size+3] << 8)+2;
            }

            printf("available_size: %d\n", available_size);
            

            // Move outside the while loop if we have enough bytes availables
            if(size <= available_size || MY_HEAP[location]==0){
                // if(size+2 >= available_size){
                //     size = available_size;
                // }
                break;
            }
            location += available_size+2;
        }
    }
       // If no block available
    if (location+size+2 >= 64000) return NULL;
    if (size < available_size){
        uint16_t leftovers = available_size-size-2;
        printf("leftovers %d\n",leftovers);

        if(leftovers & 1){
            leftovers++;
        }

        printf("leftovers %d\n",leftovers);

        // printf("location+size+2 %d\n",location+size+2);
        MY_HEAP[location+size+2] = leftovers;
        MY_HEAP[location+size+3] = leftovers >> 8;;
        
        
    }
    // Write the header
    MY_HEAP[location] = size+1;
    MY_HEAP[location+1] = size >> 8;
    // printf("header: %x\n", MY_HEAP[location]);
    // Get the adress to the first byte allocated
    char *adress = ((char *) MY_HEAP + location + 2);


    uint16_t b = (MY_HEAP[0]&0xFE) + (MY_HEAP[1] << 8);
  
    // printf("Hello, World! %d %ld\n",b, size);
    
    return (void *) adress;

}


void my_free(void *pointer)
{
    // Check the validity of the argument
    if (pointer == NULL) return;
    // Get the header associated to the block allocated
    uint8_t *new_ptr = (uint8_t *) pointer;
    new_ptr-=2;
    if (new_ptr == NULL) return;
    // Put the weak byte to 0 (= free block)
    if (*new_ptr & 0x1) *new_ptr = *new_ptr & 0xFE;
}


void print_HEAP(){
    
    printf("==== PRINT THE HEAP ====\n");

    uint16_t location = 0;
    size_t size = 0;

    while (location <= 64000)
    {
        
        size = (MY_HEAP[location] & 0xFE) + (MY_HEAP[location+1] << 8);

        if(!(MY_HEAP[location] & 0x1)) printf("free    \t");
        else printf("allocated\t");

        printf("adress: %d\t", location);
        printf("size: %ld \t", (size));

        if(!(MY_HEAP[location] & 0x1)) printf("value: [ ... ");
        else
        {
            uint16_t zero_counter = 0;
            printf("value: [ ");
            for (size_t i = location + 1; i < location + size + 1; i++) {
                if(MY_HEAP[i] == 0){
                    zero_counter++;                
                }else
                {
                    if(zero_counter > 0){
                        printf("0*%d ", zero_counter);
                    }
                    zero_counter = 0;
                    printf("%x ", MY_HEAP[i]);
                }
            }
            if(zero_counter > 0){
                printf("0*%d ", zero_counter);
            }
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
    print_HEAP();

    uint8_t *second = (uint8_t *) my_malloc(11);
    *second = 1;

    uint8_t *third = (uint8_t *) my_malloc(42);
    *third = 243;
    uint8_t *a = my_malloc(130);
    uint8_t *b = my_malloc(1540);
    uint8_t *c = my_malloc(22);
    uint8_t *d = my_malloc(30);
    uint8_t *e = my_malloc(30);
    my_malloc(18);

    print_HEAP();

    my_free(first);
    print_HEAP();

    my_free(third);
    print_HEAP();

    my_free(second);

    print_HEAP();
    
    uint8_t *fourth = (uint8_t *) my_malloc(14);
    *fourth = 1;

    print_HEAP();

    my_free(c);
    my_free(d);

    print_HEAP();

    uint8_t *f = my_malloc(52);

    print_HEAP();

    my_free(e);

    print_HEAP();

    uint8_t *g = my_malloc(40);
    uint8_t *h = my_malloc(32);
    
    print_HEAP();
    // uint8_t *fitht = (uint8_t *) my_malloc(30);
    // *fitht = 1;

    // print_HEAP();
}