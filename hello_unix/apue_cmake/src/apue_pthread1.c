/*
 * apue_pthread1.c
 *
 *  Created on: Sep 7, 2017
 *      Author: tingqian
 */
#include "apue.h"
#include <pthread.h>



//-------------------------------------------------------------------
#define JOB_QUEUE_MAX_SIZE  (256)

enum job_type_t
{
	JOB_NORMAL=0,
	JOB_EXIT
};

struct job_t{
	struct job_t * next;
	struct job_t * prev;
	char * sender;
	int id;
	enum job_type_t job_type;
	int job_param;
	int job_result;
};
struct job_t * job_head = NULL;
struct job_t * job_tail = NULL;

int job_queue_size = 0;
int job_queue_size_max = 0;

void (*jobs_enqueue)(struct job_t *pj);
struct job_t * (*jobs_dequeue)(void);

void jobs_enqueue_v0(struct job_t *pj)
{
	pj->next = pj->prev = NULL;
	if(job_tail){
		job_tail->next = pj;
		pj->prev = job_tail;
		pj->next = NULL;
	}else{
		job_tail = job_head = pj;
	}
	job_tail = pj;

	job_queue_size ++;
}

struct job_t * jobs_dequeue_v0(void)
{
	struct job_t * pj = NULL;
	if(job_head){
		pj = job_head;

		job_head = job_head->next;
		if(job_head)
			job_head->prev = NULL;
		else
			job_tail = NULL;
	}

	if(job_queue_size_max < job_queue_size)
		job_queue_size_max = job_queue_size;

	if(pj) job_queue_size --;

	return pj;
}

//====================================================================
pthread_mutex_t job_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t job_qsize = PTHREAD_COND_INITIALIZER;
void jobs_enqueue_v1(struct job_t *pj)
{
	pthread_mutex_lock(&job_lock);
	while(job_queue_size >= JOB_QUEUE_MAX_SIZE)
		pthread_cond_wait(&job_qsize, &job_lock);

	jobs_enqueue_v0(pj);
	pthread_mutex_unlock(&job_lock);
}

struct job_t * jobs_dequeue_v1(void)
{
	struct job_t * pj = NULL;
	pthread_mutex_lock(&job_lock);
	pj = jobs_dequeue_v0();
	pthread_mutex_unlock(&job_lock);

	//inform allocator that queue has more free space
	if(pj)
		pthread_cond_signal(&job_qsize);

	return pj;
}
/* Basic mutex solution, worker thread is competing with allocator thread
 * when they are busy at checking for new jobs to do, wasted a lot of time
 * just for getting nothing(queue is empty most of the time)
 *
 * so we can observe a lot of job_null
 * */

//=====================================================================
pthread_cond_t job_ready = PTHREAD_COND_INITIALIZER;
void jobs_enqueue_v2(struct job_t *pj)
{
	//protect data structure
	pthread_mutex_lock(&job_lock);
	while(job_queue_size >= JOB_QUEUE_MAX_SIZE)
		pthread_cond_wait(&job_qsize, &job_lock);

	jobs_enqueue_v0(pj);
	pthread_mutex_unlock(&job_lock);

	/* inform worker thread
	 * Note that the condition signal doesn't do the counting
	 * multiple consecutive call to condition-signal may only
	 * wakes up one thread depending on scheduling of the pthread*/

	pthread_cond_signal(&job_ready);
}
struct job_t * jobs_dequeue_v2(void)
{
	struct job_t * pj = NULL;
	pthread_mutex_lock(&job_lock);

	/* Unlike semaphore, condition is binary event(no counting inside)
	 * so a true condition means a job is ready, but a false condition
	 * does not mean there is no job!
	 * so standard way to use condition is first do a check in competing-way
	 * if no result, we wait for the condition, and the mutex can ensure that only
	 * one pending thread will be woke up by the condition */
	while((pj = jobs_dequeue_v0()) == NULL)
		pthread_cond_wait(&job_ready, &job_lock);

	pthread_mutex_unlock(&job_lock);

	pthread_cond_signal(&job_qsize);
	return pj;
}
//==================================================================
struct sema_t{
	pthread_cond_t 		cond;
	pthread_mutex_t 	mutex;
	int cnt;
};

void sema_init(struct sema_t * p, int cnt)
{
	pthread_cond_init(&p->cond, NULL);
	pthread_mutex_init(&p->mutex, NULL);
	p->cnt = cnt;
}
void sema_wait(struct sema_t * p)
{
	pthread_mutex_lock(&p->mutex);
	while(p->cnt == 0)
		pthread_cond_wait(&p->cond, &p->mutex);
	p->cnt --;
	pthread_mutex_unlock(&p->mutex);
}
void sema_post(struct sema_t * p)
{
	pthread_mutex_lock(&p->mutex);
	p->cnt ++;
	//few comments:
	// 1.pthread_cond_signal() can be called here or outside,
	//   both are OK.
	// 2.no need to do cond_signal if cnt is bigger than zero
	//   because no one will need to wait that condition/signal
	//   only do it when cnt is zero(before we increase it)
	//	 but we should use broadcast instead of signal, because
	// 	 following corner-case
	//		 t1.	th1 & th2 blocking on cond
	//	     t2.	th0 post once  (with cond_signal)
	//		 t3.    th0 post twice (w/o  cond_signal)
	//       t4.    only th1 woke up by condition and proceeds
	//              th2 will never be woke-up although semaphore
	//				is not zero!
	if(p->cnt == 1)
		//pthread_cond_signal(&p->cond);
		pthread_cond_broadcast(&p->cond);

	pthread_mutex_unlock(&p->mutex);
	//pthread_cond_signal(&p->cond);
}

struct sema_t job_sema_free;
struct sema_t job_sema_full;

void jobs_enqueue_v3(struct job_t *pj)
{
	//protect data structure
	sema_wait(&job_sema_free);

	pthread_mutex_lock(&job_lock);
	jobs_enqueue_v0(pj);
	pthread_mutex_unlock(&job_lock);

	sema_post(&job_sema_full);
}

struct job_t * jobs_dequeue_v3(void)
{
	struct job_t * pj = NULL;
	sema_wait(&job_sema_full);

	pthread_mutex_lock(&job_lock);
	pj = jobs_dequeue_v0();
	pthread_mutex_unlock(&job_lock);

	sema_post(&job_sema_free);
	return pj;
}
/* pthread condition can be used to simulate semaphore,
 * and it seems to be more versatile when thread needs pending on something (the condition)*/
//==========================================================
struct worker_context_t{
	pthread_t tid;
	int local_id;
	int jobs_done;
	int jobs_null;
	int over;
};
static void * worker_thr(void * ctx);

int
main(int argc, char *argv[])
{
	int i, err;
	struct worker_context_t workers[4]={0};
	void * ret;
	int total_done, total_sent;
	int job_queue_ver = 1;
	int JOBS = 102400;

	if(argc >= 2)
		job_queue_ver = atoi(argv[1]);
	if(argc >=3 )
		JOBS = atoi(argv[2]);

	switch(job_queue_ver)
	{
	case 0:
		jobs_enqueue = jobs_enqueue_v0;
		jobs_dequeue = jobs_dequeue_v0;
		break;
	case 1:
		jobs_enqueue = jobs_enqueue_v1;
		jobs_dequeue = jobs_dequeue_v1;
		break;
	case 2:
		jobs_enqueue = jobs_enqueue_v2;
		jobs_dequeue = jobs_dequeue_v2;
		break;
	case 3:
		sema_init(&job_sema_free, JOB_QUEUE_MAX_SIZE);
		sema_init(&job_sema_full, 0);
		jobs_enqueue = jobs_enqueue_v3;
		jobs_dequeue = jobs_dequeue_v3;
		break;
	default:
		printf("Invalid implementation version:%d\n", job_queue_ver);
		exit(-1);
		break;
	}

	printf("implementation version %d is adopted, JOBS count=%d\n",
			job_queue_ver, JOBS);

	for(i=0;i<4;i++)
	{
		workers[i].local_id = i;
		if((err = pthread_create(&workers[i].tid, NULL, worker_thr, &workers[i])) != 0){
			fprintf(stderr, "Can't create thread %d, errcode=%d\n", i, err);
			exit(1);
		}
	}

	//as the front-end, allocate jobs
	total_done = total_sent = 0;
	for(i=0;i<JOBS;i++)
	{
		struct job_t * pj = (struct job_t *)malloc(sizeof(struct job_t));
		pj->sender = "main";
		pj->id = i;
		pj->job_type = i<(JOBS-4)?JOB_NORMAL:JOB_EXIT;
		pj->job_param = i & 0xFF;
		pj->job_result = 0;
		total_sent ++;
		jobs_enqueue(pj);
	}

	printf("all jobs enqueued!\n");

	sleep(2);
	{
		char thr_over[5]={0};
		total_done = 0;
		for(i=0;i<4;i++) {
			thr_over[i] = workers[i].over?'X':'.';
			total_done += workers[i].jobs_done;
		}

		printf("JOB STAT: %s\n"
				"\tsent %d\n"
				"\tdone %d\n"
				"\tmax_queue_size %d\n", thr_over,
				total_sent, total_done, job_queue_size_max);
		if(job_queue_ver == 3){
			printf("\tjob_sema_free=%d\n", job_sema_free.cnt);
			printf("\tjob_sema_full=%d\n", job_sema_full.cnt);
		}
	}

	total_done = 0;
	for(i=0;i<4;i++)
	{
		if((err = pthread_join(workers[i].tid, &ret)) != 0){
			fprintf(stderr, "Can't join thread %d, err=%d\n", i, err);
			exit(1);
		}
		total_done += workers[i].jobs_done;
		printf("thread-%d returns %lu, jobs_done=%d, jobs_null=%d\n",
				workers[i].local_id, (unsigned long)ret,
				workers[i].jobs_done, workers[i].jobs_null);
	}

	printf("JOB STAT:\n"
			"\tsent %d\n"
			"\tdone %d\n"
			"\tmax_queue_size %d\n",
			total_sent, total_done, job_queue_size_max);
}


static void * worker_thr(void * ctx)
{
	struct worker_context_t * this = (struct worker_context_t *)ctx;
	int exit_flag=0;
	while(exit_flag == 0)
	{
		struct job_t * pj = jobs_dequeue();
		if(pj){

			if(pj->job_type == JOB_NORMAL)
			{
				int i,j,k=0;
				for(i=0;i<900*pj->job_param;i++)
					//for(j=0;j<30;j++)
						k += i*10;
				pj->job_result = k;
			}

			this->jobs_done ++;
			if(pj->job_type == JOB_EXIT)
				exit_flag = 1;

			free(pj);
		}
		else
			this->jobs_null ++;
	}
	this->over = 1;
	if(this->local_id == 2)
		pthread_exit((void*)128);

	return (void *)this->local_id;
}
