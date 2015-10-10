
#include <unistd.h>

void setup()
{
	
}

void print_prompt()
{
	char hostname[256];
	char username[256];
	char cwd[256];
	gethostname(hostname,256);
	getlogin_r(username,256);
	getcwd(cwd,256);
	printf("%s@%s:%s=>\n",username,hostname,cwd);
}

int main(void)
{
	print_prompt();
}