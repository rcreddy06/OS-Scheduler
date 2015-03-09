#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
long threadIDArray[100][3];
int counter = 0;

sigset_t set;

typedef struct resource_block { 
	struct resource_block *previous, *next;   			 
	unsigned int rid;
	int rstatus;
	int rpid;
	struct resource_wait_queue *resource_wait_queue_start, *resource_wait_queue_end,*resource_wait_queue_temp;
}resource_block_start,resource_block_end,resource_block_current,resource_block_temp;


struct task_block{
	struct task_block *previous, *next;
	int tid;
	int taskIDC;
	int tstatus;
	int tstart;
	int texecution;
	int tend;
	long threadID;
	struct resource_allocation *task_resource_assignment_start,*task_resource_assignment_end, *task_resource_assignment_temp;

}*task_block_start,*task_block_end,*task_block_current,*task_block_temp , *task_block_sort,task_block,*task_block_start2,*task_block_end2,*task_block_current2,*task_block_temp2;

struct resource_allocation{
	struct resource_allocation *previous, *next;
	int id;
	int start_time;
	int hold_time;
	
};



struct task_queue{
	struct task_queue *previous;
	struct task_queue *next;
	int task_id;
}task_queue_start, task_queue_end, task_queue_current, task_queue_temp;

struct resource_wait_queue{
	
	int task_wait_id;
	struct resource_wait_queue *previous, *next;

};


void sig_func(int sig) {
	//counter++;
	//printf("Running Thread: %ld, Task ID: %ld\n",threadIDArray[counter][0],threadIDArray[counter][2]);
	signal(SIGUSR1,sig_func);
}


int create_resources(int r){

	int i;
	struct resource_block *resource_block_temp, *resource_block_start, *resource_block_end, *resource_block_current;
	resource_block_temp=resource_block_start=resource_block_end=resource_block_current=NULL;
	for(i=0;i<r;i++)
	{
		resource_block_temp=(struct resource_block*) malloc (sizeof(struct resource_block));
		if(resource_block_start==NULL)
		{
			resource_block_start=resource_block_end=resource_block_current=resource_block_temp;
			resource_block_start->rid = i;
			resource_block_start->rstatus=1;
			resource_block_start->rpid=0;
			resource_block_start->resource_wait_queue_start = NULL;
			resource_block_start->resource_wait_queue_end = NULL;
			resource_block_start->resource_wait_queue_temp = NULL;
			resource_block_start->next=NULL;
			resource_block_start->previous=NULL;		
		} else {
			//resource_block_temp->previous = resource_block_end;
			resource_block_end->next = resource_block_current->next = resource_block_temp;
			resource_block_current = resource_block_end = resource_block_temp;
			resource_block_end->rid = i;
			resource_block_end->rstatus=1;
			resource_block_end->rpid=0;
			resource_block_end->resource_wait_queue_start = NULL;
			resource_block_end->resource_wait_queue_end = NULL;
			resource_block_end->resource_wait_queue_temp = NULL;
			resource_block_end->next=NULL;
			resource_block_end->previous=NULL;
			
		}			
	
	}

	resource_block_temp = resource_block_start;
	//while(resource_block_temp != NULL) {
	for(i=0;i<r;i++){
		printf("Resource ID: %d, Resource Status: %d\n",resource_block_temp->rid,resource_block_temp->rstatus);
		resource_block_temp = resource_block_temp->next;
		
	}
	

}


void create_tasks(int n,int r)
{
	int i;
	int j;
	int resourceTime = 0;
	int changeFlag=0;
	task_block_temp=task_block_start=task_block_end=task_block_current=NULL;
	int resourceArray[3];
	int swap;
	for(i=1;i<=n;i++)
	{
		resourceTime = 0;  
		resourceArray[0] = rand()%r;
		resourceArray[1] = rand()%r;
		while(resourceArray[1] == resourceArray[0]) {
			resourceArray[1] = rand()%r;
		}
		resourceArray[2] = rand()%r;
		while(resourceArray[2] == resourceArray[0] || resourceArray[1] == resourceArray[2]) {
			resourceArray[2] = rand()%r;
		}

		task_block_temp=(struct task_block*) malloc (sizeof(struct task_block));
		if(task_block_start==NULL)
		{
			task_block_start=task_block_end=task_block_current=task_block_temp;
			task_block_start->tid = i;
			task_block_start->taskIDC =i;
			threadIDArray[i][2] = i; //printf("TASKIDC: %d\n",task_block_start->taskIDC);
			task_block_start->tstatus=1;
			task_block_start->tstart=0;
			
			

			task_block_start->task_resource_assignment_start = task_block_start->task_resource_assignment_end = task_block_start->task_resource_assignment_temp = NULL;
			for(j = 0; j<3;j++) {
				task_block_start->task_resource_assignment_temp = (struct resource_allocation*) malloc(sizeof(struct resource_allocation));
				if(task_block_start->task_resource_assignment_start == NULL) {
					task_block_start->task_resource_assignment_end = task_block_start->task_resource_assignment_start = task_block_start->task_resource_assignment_temp;
					task_block_start->task_resource_assignment_start->next = NULL;
					task_block_start->task_resource_assignment_start->id = resourceArray[j];
					  
					task_block_start->task_resource_assignment_start->start_time = rand() % 20;
					task_block_start->task_resource_assignment_start->hold_time = ((rand()%10) + 1) * 100;
					resourceTime += task_block_start->task_resource_assignment_start->hold_time;
				} else {
					task_block_start->task_resource_assignment_end->next = task_block_start->task_resource_assignment_temp; 
					task_block_start->task_resource_assignment_end = task_block_start->task_resource_assignment_temp;
					task_block_start->task_resource_assignment_end->next = NULL;
					task_block_start->task_resource_assignment_end->id = resourceArray[j];
					  
					task_block_start->task_resource_assignment_end->start_time = rand() % 20;
					task_block_start->task_resource_assignment_end->hold_time = ((rand()%10) + 1) * 100;
					resourceTime += task_block_start->task_resource_assignment_end->hold_time;
				}
			}
			task_block_start->texecution = resourceTime + (rand()%20);
			threadIDArray[i][1] = (long)task_block_start->texecution;
			task_block_start->tend = rand()%100;
			task_block_start->next=NULL;
			task_block_start->previous=NULL;		
		} else {
			task_block_temp->previous = task_block_end;
			task_block_end->next = task_block_current->next = task_block_temp;
			task_block_current = task_block_end = task_block_temp;
			task_block_end->tid = i;
			task_block_end->taskIDC = i;
			threadIDArray[i][2] = i; //printf("TASKIDC: %d\n",task_block_end->taskIDC);
			task_block_end->tstatus=1;
			task_block_end->tstart=0;
			

			task_block_end->task_resource_assignment_start = task_block_end->task_resource_assignment_end = task_block_end->task_resource_assignment_temp = NULL;
			for(j = 0; j<3;j++) {
				task_block_end->task_resource_assignment_temp = (struct resource_allocation*) malloc(sizeof(struct resource_allocation));
				if(task_block_end->task_resource_assignment_start == NULL) {
					task_block_end->task_resource_assignment_end = task_block_end->task_resource_assignment_start = task_block_end->task_resource_assignment_temp;
					task_block_end->task_resource_assignment_start->next = NULL;
					task_block_end->task_resource_assignment_start->id = resourceArray[j];
					  
					task_block_end->task_resource_assignment_start->start_time = rand() % 20;
					task_block_end->task_resource_assignment_start->hold_time = ((rand()%10) + 1) * 100;
					resourceTime += task_block_end->task_resource_assignment_start->hold_time;
				} else {
					task_block_end->task_resource_assignment_end->next = task_block_end->task_resource_assignment_temp; 
					task_block_end->task_resource_assignment_end = task_block_end->task_resource_assignment_temp;
					task_block_end->task_resource_assignment_end->next = NULL;
					task_block_end->task_resource_assignment_end->id = resourceArray[j];
					  
					task_block_end->task_resource_assignment_end->start_time = rand() % 20;
					task_block_end->task_resource_assignment_end->hold_time = ((rand()%10) + 1) * 100;
					resourceTime += task_block_end->task_resource_assignment_end->hold_time;
				}
				task_block_end->texecution = resourceTime + (rand()%20);
				threadIDArray[i][1] = (long)task_block_end->texecution;
				task_block_end->tend = rand()%100;
			}	
			
		}
			
	
	}


		

	task_block_temp = task_block_start;
	while(task_block_temp != NULL) {
		printf("task ID: %d, task Status: %d, execution time: %d, end time %d\n",task_block_temp->tid,task_block_temp->tstatus,task_block_temp->texecution,task_block_temp->tend);
		task_block_temp->task_resource_assignment_temp = task_block_temp->task_resource_assignment_start;
		while(task_block_temp->task_resource_assignment_temp != NULL) {
			printf("Resource ID: %d, Start Time: %d, Hold Time: %d\n",task_block_temp->task_resource_assignment_temp->id,task_block_temp->task_resource_assignment_temp->start_time,task_block_temp->task_resource_assignment_temp->hold_time);
			task_block_temp->task_resource_assignment_temp = task_block_temp->task_resource_assignment_temp->next;
		} 
		task_block_temp = task_block_temp->next;
		
	}



	printf("************************************************************************************\n");

	/*task_block_current = task_block_temp = task_block_start;

	while(task_block_current != NULL) {
		while(task_block_temp != NULL) {
			if(task_block_current->texecution > task_block_temp->texecution) {
				swap = task_block_current->tid;
				task_block_current->tid = task_block_temp->tid;
				task_block_temp->tid = swap;

				swap = task_block_current->tstatus;
				task_block_current->tstatus = task_block_temp->tstatus;
				task_block_temp->tstatus = swap;

				swap = task_block_current->tstart;
				task_block_current->tstart = task_block_temp->tstart;
				task_block_temp->tstart = swap;

				swap = task_block_current->texecution;
				task_block_current->texecution = task_block_temp->texecution;
				task_block_temp->texecution = swap;

				swap = task_block_current->tend;
				task_block_current->tend = task_block_temp->tend;
				task_block_temp->tend = swap;


			}

			
			task_block_temp = task_block_temp->next;

		}
		task_block_temp = task_block_start;
		task_block_current = task_block_current->next;
	
	}


	task_block_temp = task_block_start;
	while(task_block_temp != NULL) {
		printf("task ID: %d, task Status: %d, execution time: %d\n",task_block_temp->tid,task_block_temp->tstatus,task_block_temp->texecution);
		task_block_temp->task_resource_assignment_temp = task_block_temp->task_resource_assignment_start;
		while(task_block_temp->task_resource_assignment_temp != NULL) {
			printf("Resource ID: %d, Start Time: %d, Hold Time: %d\n",task_block_temp->task_resource_assignment_temp->id,task_block_temp->task_resource_assignment_temp->start_time,task_block_temp->task_resource_assignment_temp->hold_time);
			task_block_temp->task_resource_assignment_temp = task_block_temp->task_resource_assignment_temp->next;
		} 
		task_block_temp = task_block_temp->previous;
		
	}*/

}

static void *thread_start(void *arg) {
	int taskID = (int*)arg;
	int threadSearch;
	pthread_mutex_lock(&mut);
	
	threadIDArray[taskID][0] = pthread_self();
	signal(SIGUSR1,sig_func);
	printf("\nFrom Thread: Task ID: %d Thead ID: %ld\n",taskID,pthread_self());	
	pthread_mutex_unlock(&mut);
	sigfillset(&set);
	sigdelset(&set,SIGUSR1);
	sigsuspend(&set);
	//signal(SIGUSR1,sig_func);
	printf("Thread %d Running\n",taskID);
	while(threadIDArray[taskID][1] > 0) {
		
		
		threadIDArray[taskID][1]--;
		usleep(1);
		
	}
	/*while(task_block_thread != NULL) {
		if(task_block_thread->tid == taskID) {
			task_block_thread->threadID = pthread_self();
			printf("Thread: TaskID: %d, ThreadID: %ld",taskID,pthread_self());
			break;
		}
	}*/
	
}


void sjb_execution(int n) {
	int z,i;
	int *threadCreate;
	printf("INSIDE SJB.");
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	printf("\nThread ID Array:\n");
	

	for(i = 0; i<=n;i++){
		printf("Thread ID: %ld, Execution Time: %ld\n",threadIDArray[i][0],threadIDArray[i][1]);
	}
	task_block_temp2 = task_block_start;
	while(task_block_temp2 != NULL) {
		threadCreate = task_block_temp2->tid;		
		z = pthread_create(&threadCreate,&attr,&thread_start,(void*)task_block_temp2->tid);
		printf("SJB: %ld\n",threadCreate);
		//printf("SJB z:%d",z);
		if (z!=0) {
			exit(0);
		}
		task_block_temp2 = task_block_temp2->next;
	}	

}













int main(int argc ,  char **argv)
{

int i;
int n,r,s,q,z;
int j;
long swap;
long mainThreadId;
struct task_queue *task_queue_start,*task_queue_current,*task_queue_end,*task_queue_temp;
pthread_mutex_init(&mut,NULL);
task_queue_start=task_queue_current=task_queue_end=task_queue_temp=NULL;
mainThreadId = pthread_self();
printf("Main Thread ID: %ld\n",mainThreadId);
if (argc < 3 ) 
        {
		printf("\n -N: Number of Processes \n -r: Number of Resources \n -S: scheduling scheme (0: SJB   1: Round Robin)\n -Q : Quantum (Round Robin)\n ");
		exit(0);
        }
     else 
	{
	for (i=1; i < argc ; i+=2)
	{
                if (argv[i][0] == '-')
		{
			switch(argv[i][1])
			{
			case 'n':
	 			n = atoi(argv[i+1]);  // Number of Tasks
				break;
			case 'r':
	 			r = atoi(argv[i+1]); // Number of Resources
				break;
			case 's':                         
	 			s =  atoi(argv[i+1]); // Scheduling Scheme 
				break;
			case 'q':                         
	 			q =  atoi(argv[i+1]); //Quantum (Round Robin) 
				break;
			default: 
				return(0);		// Default Value
				break;
                        }

		}
        }
      }
printf("INSIDE SJB2.");
sigaddset(&set,SIGUSR1);
signal(SIGUSR1,sig_func);
//infinite_task=pthread_create(&task_block, &attr, );
switch (s)
{
case 0 :
	create_resources(r);
	create_tasks(n,r);	
	sjb_execution(n);
	break;

case 1 :
	create_resources(r);
	create_tasks(n,r);
	//assign_tasks_resources(n,r);
	//rr_execution(n);
	break;
default: 
	printf("\nPlease choose '0' for SJB or '1' for RR\n");
	exit(0);
	break;
}


printf("\n-------------------------------------------------------------------------------\nFinal Scheduler Summary\n-------------------------------------------------------------------------------\n");
if(s==0)
{
printf("\nScheduling Scheme = SJB");
printf("\nNumber of Processes = %d \nNumber of Resources =%d\n",n,r);
}
else
{
printf("\nScheduling Scheme = Round Robin");
printf("\nNumber of Processes = %d \nNumber of Resources =%d \nQuantum (Round Robin) = %d\n ",n,r,q);
}
printf("-------------------------------------------------------------------------------\n");
sleep(3);
task_block_temp = task_block_start;
	while(task_block_temp != NULL) {
		printf("MAIN: task execution: %d, threadID: %ld\n",task_block_temp->taskIDC,task_block_temp->threadID);
		task_block_temp = task_block_temp->next;
		
	}
	printf("\nThread ID Array:\n");
	

	for(i = 0; i<=n;i++){
		printf("Thread ID: %ld, Execution Time: %ld\n",threadIDArray[i][0],threadIDArray[i][1]);
	}

	for(i=1;i<=n;i++) {
		for (j=1;j<=n;j++) {
			if(threadIDArray[i][1] < threadIDArray[j][1]) {
				swap = threadIDArray[i][1];
				threadIDArray[i][1] = threadIDArray[j][1];
				threadIDArray[j][1] = swap;

				swap = threadIDArray[i][0];
				threadIDArray[i][0] = threadIDArray[j][0];
				threadIDArray[j][0] = swap;

				swap = threadIDArray[i][2];
				threadIDArray[i][2] = threadIDArray[j][2];
				threadIDArray[j][2] = swap;
			}
		}
	}

	printf("\nThread ID Array:\n");
	

	for(i = 0; i<=n;i++){
		printf("Thread ID: %ld, Execution Time: %ld, Task ID: %ld\n",threadIDArray[i][0],threadIDArray[i][1],threadIDArray[i][2]);
	}
	for(i = 1;i<=n;i++) {
		z = pthread_kill(threadIDArray[i][0],SIGUSR1);
		z = pthread_join(threadIDArray[i][0],NULL);
		printf("\nBACK IN SCHEDULER\n");
	}
pthread_mutex_destroy(&mut);
}
