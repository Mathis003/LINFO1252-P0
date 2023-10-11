#include <stdio.h>
#include <stdint.h>

uint8_t MY_HEAP[64000];

void init()
{
    for (size_t i = 0; i < 64000; i++)
    {
        MY_HEAP[i] = 0;
    }
}

void *my_malloc(size_t size)
{
    uint16_t location = 0;
    while(MY_HEAP[location] & 0x1){
        location+=(MY_HEAP[location]>>1)+1;
    }
    
    MY_HEAP[location] = (size<<1)+0x1;
    printf("header: %x\n",MY_HEAP[location]);

    // printf("location: %d\n",location);

    return (void *)((char *)MY_HEAP + location + 1);
}

void my_free(void *ptr)
{
    uint8_t *new_ptr = (uint8_t *) ptr;
    new_ptr--;
    if (*new_ptr & 0x1)
    {
        printf("ok\n");
        *new_ptr = *new_ptr & 0xFE;
    }
}

// TODO: remove when posting to inginious
void print_HEAP(){
    uint16_t location = 0;
    
    printf("print heap:\n");
    while (1)
    {
        // print location
        if(!(MY_HEAP[location] & 0x1)){
            return;
        }

        size_t size = MY_HEAP[location]>>1;
        printf("adress: %d, ", location);
        printf("value: [ ");
        for (size_t i = location+1; i < location+1+size; i++)
        {
            printf("%x ",MY_HEAP[i]);
        }
        printf("], ");
        
        printf("size: %d\n", (size));

        // printf("used: %d\n", location+1);
        location+=size+1;
    }
}

int main(int argc, char **argv)
{
    init();
    uint16_t *a = (uint16_t *) my_malloc(4);
    uint8_t *b = (uint8_t *) my_malloc(10);
    my_malloc(1);
    *b = (uint8_t)200;
    *a = (uint16_t)2000;

    print_HEAP();

    // my_free(b);
    // print_HEAP();




    printf("Test print : %d\n", 1<<5);
    // printf("Main\n");
}