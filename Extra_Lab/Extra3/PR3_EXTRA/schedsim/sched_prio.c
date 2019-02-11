#include "sched.h"

#define MAX_PRIO 5

/* Per-CPU private structure to store an array of run queues */
struct prio_private_rq {
    slist_t tasks_prio[MAX_PRIO];
};

/* Initialize class-specific global data structures */
static int sched_init_prio(void)
{
    int cpu,prio;
    struct prio_private_rq* priv_rq;
    runqueue_t* cpu_rq;

    /*
     * Reserve an array of run queues for each CPU
     * and store the pointer of the private structure
     * in the "rq_cs_data" field of per-CPU run queues.
     * This makes it possible to retrieve the pointer to
     * the structure later from the sched_class operations.
     */
    for (cpu=0; cpu<nr_cpus; cpu++) {
        cpu_rq=get_runqueue_cpu(cpu);
        priv_rq=malloc(sizeof(struct prio_private_rq));

        if (priv_rq==NULL) {
            fprintf(stderr,"Cannot allocate memory for private run queues\n");
            return 1;
        }

        for (prio=0; prio<MAX_PRIO; prio++)
            init_slist(&priv_rq->tasks_prio[prio],offsetof(task_t,rq_links));

        /* Assign pointer */
        cpu_rq->rq_cs_data=priv_rq;
    }
    return 0;
}

/* Free up class-specific global data structures */
static void sched_destroy_prio(void)
{
    int cpu;
    runqueue_t* cpu_rq;

    for (cpu=0; cpu<nr_cpus; cpu++) {
        cpu_rq=get_runqueue_cpu(cpu);
        /* Free up data structure */
        free(cpu_rq->rq_cs_data);
        cpu_rq->rq_cs_data=NULL;
    }
}


static task_t* pick_next_task_prio(runqueue_t* rq)
{
    struct prio_private_rq* prq=rq->rq_cs_data;
    task_t *t=NULL;
    int i=0;
    int ok=0;
    while(i < MAX_PRIO && !ok){
    	if(!list_empty(&prq->tasks_prio[i].list)){
    		t= head_slist(&prq->tasks_prio[i]);
    		ok=1;
    	}
    	else
    		i++;
    }
    /* TODO ... */
	if (t)
		remove_slist(&prq->tasks_prio[i],t);
	return t;

}


static void enqueue_task_prio(task_t* t,runqueue_t* rq, int preempted)
{
    struct prio_private_rq* prq=rq->rq_cs_data;
    if (t->on_rq || is_idle_task(t))
    		return;

	if (t->flags & TF_INSERT_FRONT) {
		//Clear flag
		t->flags&=~TF_INSERT_FRONT;
		insert_slist(&prq->tasks_prio[t->prio],t);
	}
	else{
		insert_slist(&prq->tasks_prio[t->prio],t);
	}


	if (!preempted) {
		task_t* current=rq->cur_task;

		/* Trigger a preemption if this task has a shorter CPU burst than current */
		if (preemptive_scheduler && t->prio < current->prio) {
			rq->need_resched=TRUE;
			current->flags|=TF_INSERT_FRONT; /* To avoid unfair situations in the event
                                                another task with the same length wakes up as well*/
		}
	}

    /* TODO ... */
}

static task_t* steal_task_prio(runqueue_t* rq)
{

    struct prio_private_rq* prq=rq->rq_cs_data;
    task_t *t=NULL;
    int i=MAX_PRIO-1;
    int ok=0;
    while(i>0 && !ok){
    	if(!list_empty(&prq->tasks_prio[i].list)){
    		t= tail_slist(&prq->tasks_prio[i]);

    		ok=1;
    	}
    	else
    		i--;
    }
    /* TODO ... */
	if (t)
		remove_slist(&prq->tasks_prio[i],t);

	return t;


    /* TODO ... */
}

sched_class_t prio_sched= {
    .sched_init=sched_init_prio,
    .sched_destroy=sched_destroy_prio,
    .pick_next_task=pick_next_task_prio,
    .enqueue_task=enqueue_task_prio,
    .steal_task=steal_task_prio
};
