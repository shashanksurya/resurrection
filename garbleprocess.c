#include <stdio.h>
#include <string.h>
#include <sys/types.h>

int main(void)
{
	FILE *fp;
	int i,j;
	pid_t p1;
	fp = fopen("tempfile.txt","w");
	p1 = fork();
	char message1[] = "Message1 is good";
	char message2[] = "Message2 is bad";
	if(p1> 0)//parent
	{
		for(i=0;i<16;i++)
			{
			fprintf(fp, "%c",message1[i]);
			if(i==15)
				i=-1;
			}

	}
	else if(p1==0)//child
	{
		for(j=0;j<15;j++)
		{
				fprintf(fp, "%c",message2[j]);	
				if(j==14)
				j=-1;
		}
	}
	fclose(fp);
	return 0;
}