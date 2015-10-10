#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int x;
	for(x=0;x<100;x++)
	{
		printf("	insert(root,%d,NULL);\n",rand()%99999);
	}
}