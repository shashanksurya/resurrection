#include <stdio.h>

int main(void)
{
	extern int i;	
	printf("%d\n",i);
	return 0;
}
int i ;