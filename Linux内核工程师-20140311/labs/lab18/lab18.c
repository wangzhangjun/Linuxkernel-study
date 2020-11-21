#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    printf("Using exit...\n");
    printf("This is the content in buffer");
    //exit(0); // Do flush
    _exit(0);  // Not flush
}
