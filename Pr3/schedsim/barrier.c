#include "barrier.h"
#include <errno.h>


#ifdef POSIX_BARRIER

/* Wrapper functions to use pthread barriers */

int sys_barrier_init(sys_barrier_t* barrier, unsigned int nthreads)
{
	return pthread_barrier_init(barrier,NULL,nthreads);
}

int sys_barrier_destroy(sys_barrier_t* barrier)
{
	return pthread_barrier_destroy(barrier);
}

int sys_barrier_wait(sys_barrier_t *barrier)
{
	return pthread_barrier_wait(barrier);
}

#else


/* Barrier initialization function */
int sys_barrier_init(sys_barrier_t *barrier, unsigned int nr_threads)
{
	int fail=0;

	fail=pthread_mutex_init(&barrier->mutex,NULL);
	if(fail==0){
		fail=pthread_cond_init(&barrier->cond,NULL);
		if(fail==0){
			barrier->max_threads=nr_threads;
			barrier->nr_threads_arrived=0;
		}
		else{
			pthread_mutex_destroy(&barrier->mutex);
			return fail;
		}
	}
	return 0;
}

/* Destroy barrier resources */
int sys_barrier_destroy(sys_barrier_t *barrier)
{
	pthread_mutex_destroy(&barrier->mutex);
	pthread_cond_destroy(&barrier->cond);
	return 0;
}

/* Main synchronization operation */
int sys_barrier_wait(sys_barrier_t *barrier)
{
	pthread_mutex_lock(&barrier->mutex);
	barrier->nr_threads_arrived++;
	if(barrier->nr_threads_arrived < barrier->max_threads){
		pthread_cond_wait(&barrier->cond,&barrier->mutex);
	}
	else{
		barrier->nr_threads_arrived=0;
		pthread_cond_broadcast(&barrier->cond);
	}
	pthread_mutex_unlock(&barrier->mutex);
	return 0;
/* Barrier con flags*/
/* 
	local_sense = (local_sense == 0) ? 1 : 0;
	pthread_mutex_lock(&barrier->mutex);
	barrier->nr_threads_arrived++;

	if(barrier->nr_threads_arrived < barrier->max_threads){
		pthread_cond_wait(&barrier->cond, &barrier->mutex);
	}
	else{
		barrier->nr_threads_arrived=0;
		barrier->flag = local_sense;
		pthread_cond_broadcast(&barrier->cond);
	}
	pthread_mutex_unlock(&barrier->mutex);
	return 0;
*/
}

#endif /* POSIX_BARRIER */
