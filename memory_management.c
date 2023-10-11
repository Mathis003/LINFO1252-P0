#include <stdio.h>
#include <stdint.h>

uint8_t MY_HEAP[64000];

void init()
{

}

void *my_malloc(size_t size)
{
    return NULL;
}

void my_free(void *ptr)
{

}

int main(int argc, char **argv)
{
    printf("Main\n");
    printf("Test print : %d\n", 5);
}