#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	char *mystr;
	printf( "Please enter a long string: " );
	mystr = malloc(sizeof(char)*256);
    /* notice stdin being passed in */
    fgets ( mystr, 256, stdin );  
    printf("%s\n",mystr );
    printf("%d\n",(mystr[0]-'0'));

}