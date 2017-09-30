#include "apue.h"
#include <stdio.h>
#include <signal.h>

static void my_exit1(void);
static void my_exit2(void);
static void my_stack_prob(void);

extern char **environ;

static char * pfirst = NULL;
static unsigned int addr0 = 0;
static void show_addr(const void * pv, const char * ptail)
{
	unsigned int addr1 = (unsigned int) pv;
	char sign = addr1 > addr0?'+':'-';
	unsigned int diff = addr1>addr0?(addr1 - addr0):(addr0-addr1);

	printf("0x%08X (%c%08u) %s", addr1, sign, diff, ptail);

	addr0 = addr1;
}

void sig_int(int signo)
{
	int i=0;
	printf("SIGINT received: 0x%08X stack (address of local variable) \n", &i);
}
void sig_usr(int signo)
{
	int i=0;
	printf("SIGUSR(%d) received: 0x%08X stack (address of local variable) \n",signo, &i);
}
void call_pause(void)
{
	int i=0;
	printf("calling pause: 0x%08X stack \n", &i);
	pause();
}

int main(int argc, char * argv[])
{
	char ** env = environ;
	int i;
	char * ptr = malloc(1024);
	printf("hello,world\n");

	signal(SIGINT, sig_int);
	signal(SIGUSR1, sig_usr);
	signal(SIGUSR2, sig_usr);

	show_addr(&main, 	" code     (address of main function)\n");
	show_addr(&pfirst, 	" data1    (address of a global variable pfirst)\n");
	show_addr(&environ, " data2    (address of a global variable environ)\n");
	show_addr(ptr,    	" heap     (first pointer returned by malloc)\n");
	show_addr(sbrk(0),  " program break (returned by sbrk(0))\n");
	show_addr(&i,    	" stack    (address of a fixed local variable of main)\n");
	show_addr(environ,  " environ\n");

	int oc;
	while((oc=getopt(argc,argv,"sep")) != -1)
	{
		switch(oc)
		{
		case 's':
			my_stack_prob();
			break;
		case 'e':
			i = 0;
			while(env[i] != NULL){
				printf("\t0x%x (%+08lld) environ[%d] %s\n",
						env[i], (long long)(env[i]) - (long long)(environ), i, env[i]);
				i++;
			}
			break;
		case 'p':
			while(1)
			{
				call_pause();
				printf("%s:pause() returned\n", argv[0]);
			}
			break;
		default:
			fprintf(stderr,"Unkown arg: %c", oc);
			break;
		}
	}

	show_addr(argv,    " argument (argv)\n");
	for(i=0;i<argc; i++)
		printf("\t0x%x argv[%d]: %s\n", argv[i], i, argv[i]);


	if(atexit(my_exit2) != 0)
		perror("can't register my_exit2");

	if(atexit(my_exit1) != 0)
		perror("can't register my_exit1");
	if(atexit(my_exit1) != 0)
		perror("can't register my_exit1");

	printf("main is done\n");
	free(ptr);
	return 0;
}

static void my_exit1(void)
{
	printf("first exit handler\n");
}
static void my_exit2(void)
{
	printf("second exit handler\n");
}
static void my_stack_prob(void)
{

	char var[1024];
	if(pfirst == NULL) pfirst = var;
	sprintf(var, "stack probe on 0x%08X ok, offset 0x%08X(%.2fMB)      ",
			var, pfirst - var,
			(float)(pfirst - var)/(1024*1024.0f));
	printf("\r%s", var); fflush(stdout);
	my_stack_prob();
}
