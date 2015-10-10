#include <stdio.h>
#include <string.h>
#define BUFFER_SIZE 256


char* parse_arguments(char *input_str)
{

}

int main(void)
{
	char str[BUFFER_SIZE];
	while(1)
	{
		if(fgets(str,BUFFER_SIZE,stdin)!=NULL)
		{
			printf("%s\n",str);
			fflush(stdin);
		}
	}
	return 0;
}