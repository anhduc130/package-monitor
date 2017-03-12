#define lock (volatile char*) 0x00000010
#include <stdio.h>

int main()
{
  while(1)
		  *lock = 0;

  return 0;
}
