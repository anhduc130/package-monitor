#define lock (volatile char*) 0x00000010
#include <stdio.h>

int main()
{
  while(1)
	  *lock = 1;

  return 0;
}
