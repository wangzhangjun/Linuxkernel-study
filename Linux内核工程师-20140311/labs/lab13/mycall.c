#include "mycall.h"

int main(int argc, char *argv[])
{
  printf("\nDiving to kernel level\n\n");
  printf("value = %d\n",mysyscall(120));
  printf("\nRising to user level\n\n");
     
   return 0;
}


