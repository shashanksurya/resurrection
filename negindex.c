#include <stdio.h>

int main(void)
{
	int *a = (int *)malloc(1000*sizeof(int));
	int *p;
	a[1] = 1;
	p = a;
	p+=3;
	*p = 2;
	printf("%d\n",a[4]);
	return 1;
}