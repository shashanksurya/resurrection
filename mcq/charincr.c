#include <stdio.h>

int main(void)
{
	char c = 'a';
	while(c++<='z')
		putchar(c-1);
	return 1;
}