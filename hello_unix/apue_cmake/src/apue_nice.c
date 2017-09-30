#include "apue.h"
#include <errno.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/times.h>
#include <pthread.h>
#include <limits.h>

unsigned long long count = 0;
struct timeval end;
char * myname;


void * thr_counting(void * pv)
{
	struct timeval tv;
	int cnt = 0;
	for(;;)
	{
		if(++(cnt) == 0){
			fprintf(stderr, "%s count wrap", myname);
			exit(-1);
		}

		gettimeofday(&tv, NULL);

		if(tv.tv_sec > end.tv_sec && tv.tv_usec > end.tv_usec)
			return ((void*)cnt);
	}
}

int main(int argc, char * argv[])
{
	pid_t pid;
	struct tms tbuf;

	int adj = 0;
	int thread_cnt = 1;
	int time_out = 5;

	char * endptr;
	int status;

	int opt;
	while((opt=getopt(argc, argv, "t:n:T:")) != -1)
	{
		switch(opt)
		{
		case 'T':
			time_out = strtol(optarg, &endptr, 10);
			if(*endptr)
				fprintf(stderr, "-T with invalid optarg: %s (endptr returned by strtol:%s)\n",
						optarg, endptr), exit(-1);
			break;
		case 't':
			thread_cnt = strtol(optarg, &endptr, 10);
			if(*endptr)
				fprintf(stderr, "-t with invalid optarg: %s (endptr returned by strtol:%s)\n",
						optarg, endptr), exit(-1);
			break;
		case 'n':
			adj = strtol(optarg, &endptr, 10);
			if(*endptr)
				fprintf(stderr, "-n with invalid optarg: %s\n", optarg), exit(-1);
			break;
		default:
			fprintf(stderr,"Usage: %s [-t thread_cnt] [-n nice_adj]\n (opt=%c)\n",
					argv[0], opt);
			exit(-1);
		}
	}

	long clkticks_l = sysconf(_SC_CLK_TCK);
	float clkticks = clkticks_l;

	clock_t walltime0 = times(&tbuf);

	printf("%s -t %d -n %d -T %d (clk_tck=%d)\n", argv[0], thread_cnt, adj, time_out, clkticks_l);

	gettimeofday(&end, NULL);
	end.tv_sec += time_out;

	if((pid = fork()) < 0){
		perror("fork error");
		exit(-1);
	} else if (pid == 0){ /* child */
		myname = " child";
		errno = 0;
		nice(adj);
		if(errno)
			perror("nice error"), exit(-1);
	} else { /* parent */
		myname = "parent";
	}

	printf("%s(PID=%ld): nice value is %d \n", myname, (long)getpid(), nice(0));

	int i;
	int err;
	int cnt;
	pthread_t ntid[32];
	void * nret[32];

	for(i=0;i<thread_cnt;i++){
		if((err = pthread_create(&ntid[i], NULL, thr_counting, NULL)) != 0)
			fprintf(stderr, "can't create thread(%d), err=%d\n", i, err), exit(-1);
	}

	cnt = 0;
	for(i=0;i<thread_cnt;i++){
		if((err = pthread_join(ntid[i], &nret[i])) != 0)
			fprintf(stderr, "can't join thread(%d), err=%d\n", i, err), exit(-1);

		cnt += (int)(nret[i]);
	}

	printf("%s (%d threads, %d niceness) total cnt=%d\n", myname,
			thread_cnt, nice(0),
			cnt);

	if(pid){
		// wait for child process's state change,
		//so times will report correct value on children

		if(waitpid(-1, &status, 0) == -1)
			perror("waitpid error"), exit(-1);

		if(WIFEXITED(status))
			printf("child terminated normally with exit status:%d\n", WEXITSTATUS(status));
		else if (WIFSIGNALED(status))
			printf("child terminated by signal %d\n", WTERMSIG(status));
		else if (WIFSTOPPED(status))
			printf("child stopped by a delievery of signal %d\n", WSTOPSIG(status));
		else
			printf("Unknow exit status\n");

		clock_t walltime = times(&tbuf) - walltime0;
		printf("%s's times: w%6.2f, user/sys(%6.2f,%6.2f), children's(%6.2f,%6.2f)\n",
				myname, walltime/clkticks,
				tbuf.tms_utime/clkticks, tbuf.tms_stime/clkticks,
				tbuf.tms_cutime/clkticks, tbuf.tms_cstime/clkticks);

	}else{

		clock_t walltime = times(&tbuf) - walltime0;
		printf("%s's times: w%6.2f, user/sys(%6.2f,%6.2f), children's(%6.2f,%6.2f)\n",
				myname, walltime/clkticks,
				tbuf.tms_utime/clkticks, tbuf.tms_stime/clkticks,
				tbuf.tms_cutime/clkticks, tbuf.tms_cstime/clkticks);

		status /= 0;
		exit(-1);
	}
}
