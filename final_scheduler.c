#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
long global_data[100][3];
int counter = 0;

sigset_t set;

typedef struct resource_header { 
	struct resource_header *previous, *next;   			 
	unsigned int resource_id;
	int resource_status;
	int resource_process_id;
	struct resource_wait_queue *resource_wait_queue_start, *resource_wait_queue_end,*resource_wait_queue_temp;
}resource_header_start,resource_header_end,resource_header_current,resource_header_temp;


struct process_header{
	struct process_header *previous, *next;
	int pid;
	int pid2;
	int pstatus;
	int pstart;
	int pexecution;
	int pend;
	long threadID;
	struct resource_allocation *task_resource_assignment_start,*task_resource_assignment_end, *task_resource_assignment_temp;

}*process_header_start,*process_header_end,*process_header_current,*process_header_temp , *process_header_sort,process_header,*process_header_start2,*process_header_end2,*process_header_current2,*process_header_temp2;

struct resource_allocation{
	struct resource_allocation *previous, *next;
	int id;
	int start_time;
	int hold_time;
	
};

struct process_queue{
	struct process_queue *previous;
	struct process_queue *next;
	int task_id;
}process_queue_start, process_queue_end, process_queue_current, process_queue_temp;

struct resource_wait_queue{
	
	int task_wait_id;
	struct resource_wait_queue *previous, *next;

};


void sig_func(int sig) {
		signal(SIGUSR1,sig_func);
}


int create_resources(int r){
	int i;
	struct resource_header *resource_header_temp, *resource_header_start, *resource_header_end, *resource_header_current;
	resource_header_temp=resource_header_start=resource_header_end=resource_header_current=NULL;
	for(i=0;i<r;i++)
	{
		resource_header_temp=(struct resource_header*) malloc (sizeof(struct resource_header));
		if(resource_header_start==NULL)
		{
			resource_header_start=resource_header_end=resource_header_current=resource_header_temp;
			resource_header_start->resource_id = i;
			resource_header_start->resource_status=0;
			resource_header_start->resource_process_id=0;
			resource_header_start->resource_wait_queue_start = NULL;
			resource_header_start->resource_wait_queue_end = NULL;
			resource_header_start->resource_wait_queue_temp = NULL;
			resource_header_start->next=NULL;
			resource_header_start->previous=NULL;		
		} else {
			resource_header_end->next = resource_header_current->next = resource_header_temp;
			resource_header_current = resource_header_end = resource_header_temp;
			resource_header_end->resource_id = i;
			resource_header_end->resource_status=0;
			resource_header_end->resource_process_id=0;
			resource_header_end->resource_wait_queue_start = NULL;
			resource_header_end->resource_wait_queue_end = NULL;
			resource_header_end->resource_wait_queue_temp = NULL;
			resource_header_end->next=NULL;
			resource_header_end->previous=NULL;
			
		}			
	
	}

	resource_header_temp = resource_header_start;
	for(i=0;i<r;i++){
		printf("Resource ID: %d \t Resource Status: %d\n",resource_header_temp->resource_id,resource_header_temp->resource_status);
		resource_header_temp = resource_header_temp->next;
		
	}
	

}


void create_processes(int n,int r)
{
	int i,j;
	int resource_time = 0;
	int changeFlag=0;
	process_header_temp=process_header_start=process_header_end=process_header_current=NULL;
	int resource_header_array[3];
	int swap;
	for(i=1;i<=n;i++)
	{
		resource_time = 0;  
		resource_header_array[0] = rand()%r;
		resource_header_array[1] = rand()%r;
		while(resource_header_array[1] == resource_header_array[0]) {
			resource_header_array[1] = rand()%r;
		}
		resource_header_array[2] = rand()%r;
		while(resource_header_array[2] == resource_header_array[0] || resource_header_array[1] == resource_header_array[2]) {
			resource_header_array[2] = rand()%r;
		}

		process_header_temp=(struct process_header*) malloc (sizeof(struct process_header));
		if(process_header_start==NULL)
		{
			process_header_start=process_header_end=process_header_current=process_header_temp;
			process_header_start->pid = i;
			process_header_start->pid2 =i;
			global_data[i][2] = i; 
			process_header_start->pstatus=1;
			process_header_start->pstart=0;
			
			

			process_header_start->task_resource_assignment_start = process_header_start->task_resource_assignment_end = process_header_start->task_resource_assignment_temp = NULL;
			for(j = 0; j<3;j++) {
				process_header_start->task_resource_assignment_temp = (struct resource_allocation*) malloc(sizeof(struct resource_allocation));
				if(process_header_start->task_resource_assignment_start == NULL) {
					process_header_start->task_resource_assignment_end = process_header_start->task_resource_assignment_start = process_header_start->task_resource_assignment_temp;
					process_header_start->task_resource_assignment_start->next = NULL;
					process_header_start->task_resource_assignment_start->id = resource_header_array[j];
					  
					process_header_start->task_resource_assignment_start->start_time = rand() % 20;
					process_header_start->task_resource_assignment_start->hold_time = ((rand()%10) + 1) * 100;
					resource_time += process_header_start->task_resource_assignment_start->hold_time;
				} else {
					process_header_start->task_resource_assignment_end->next = process_header_start->task_resource_assignment_temp; 
					process_header_start->task_resource_assignment_end = process_header_start->task_resource_assignment_temp;
					process_header_start->task_resource_assignment_end->next = NULL;
					process_header_start->task_resource_assignment_end->id = resource_header_array[j];
					  
					process_header_start->task_resource_assignment_end->start_time = rand() % 20;
					process_header_start->task_resource_assignment_end->hold_time = ((rand()%10) + 1) * 100;
					resource_time += process_header_start->task_resource_assignment_end->hold_time;
				}
			}
			process_header_start->pexecution = resource_time + (rand()%20);
			global_data[i][1] = (long)process_header_start->pexecution;
			process_header_start->pend = rand()%100;
			process_header_start->next=NULL;
			process_header_start->previous=NULL;		
		} else {
			process_header_temp->previous = process_header_end;
			process_header_end->next = process_header_current->next = process_header_temp;
			process_header_current = process_header_end = process_header_temp;
			process_header_end->pid = i;
			process_header_end->pid2 = i;
			global_data[i][2] = i; 
			process_header_end->pstatus=1;
			process_header_end->pstart=0;
			

			process_header_end->task_resource_assignment_start = process_header_end->task_resource_assignment_end = process_header_end->task_resource_assignment_temp = NULL;
			for(j = 0; j<3;j++) {
				process_header_end->task_resource_assignment_temp = (struct resource_allocation*) malloc(sizeof(struct resource_allocation));
				if(process_header_end->task_resource_assignment_start == NULL) {
					process_header_end->task_resource_assignment_end = process_header_end->task_resource_assignment_start = process_header_end->task_resource_assignment_temp;
					process_header_end->task_resource_assignment_start->next = NULL;
					process_header_end->task_resource_assignment_start->id = resource_header_array[j];
					  
					process_header_end->task_resource_assignment_start->start_time = rand() % 20;
					process_header_end->task_resource_assignment_start->hold_time = ((rand()%10) + 1) * 100;
					resource_time += process_header_end->task_resource_assignment_start->hold_time;
				} else {
					process_header_end->task_resource_assignment_end->next = process_header_end->task_resource_assignment_temp; 
					process_header_end->task_resource_assignment_end = process_header_end->task_resource_assignment_temp;
					process_header_end->task_resource_assignment_end->next = NULL;
					process_header_end->task_resource_assignment_end->id = resource_header_array[j];
					  
					process_header_end->task_resource_assignment_end->start_time = rand() % 20;
					process_header_end->task_resource_assignment_end->hold_time = ((rand()%10) + 1) * 100;
					resource_time += process_header_end->task_resource_assignment_end->hold_time;
				}
				process_header_end->pexecution = resource_time + (rand()%20);
				global_data[i][1] = (long)process_header_end->pexecution;
				process_header_end->pend = rand()%100;
			}	
			
		}
			
	
	}
	process_header_temp = process_header_start;
	while(process_header_temp != NULL) {
		printf("Process ID: %d,\t Process Status: %d, Process Start Time = 0\t Process Execution Time: %d,\t Process End time %d\n",process_header_temp->pid,process_header_temp->pstatus,process_header_temp->pexecution,process_header_temp->pend);
		process_header_temp->task_resource_assignment_temp = process_header_temp->task_resource_assignment_start;
		while(process_header_temp->task_resource_assignment_temp != NULL) {
			printf("Resource ID: %d,\t Start Time: %d, \tHold Time: %d\n",process_header_temp->task_resource_assignment_temp->id,process_header_temp->task_resource_assignment_temp->start_time,process_header_temp->task_resource_assignment_temp->hold_time);
			process_header_temp->task_resource_assignment_temp = process_header_temp->task_resource_assignment_temp->next;
		} 
		process_header_temp = process_header_temp->next;
		
	}
}

static void *thread_start(void *arg) {
	int process_ID_2 = (int*)arg;
	int threadSearch;
	pthread_mutex_lock(&m1);
	
	global_data[process_ID_2][0] = pthread_self();
	signal(SIGUSR1,sig_func);
	printf("\nFrom Thread: Task ID: %d Thead ID: %ld\n",process_ID_2,pthread_self());	
	pthread_mutex_unlock(&m1);
	sigfillset(&set);
	sigdelset(&set,SIGUSR1);
	sigsuspend(&set);
	//signal(SIGUSR1,sig_func);
	printf("Thread %d Running\n",process_ID_2);
	while(global_data[process_ID_2][1] > 0) {
		
		
		global_data[process_ID_2][1]--;
		usleep(1);
		
	}
}


void sjf_execution(int n) {
	int z,i;
	int *threadCreate;
	printf("INSIDE sjf.");
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	printf("\nThread ID Array:\n");
	

	for(i = 0; i<=n;i++){
		printf("Thread ID: %ld, Execution Time: %ld\n",global_data[i][0],global_data[i][1]);
	}
	process_header_temp2 = process_header_start;
	while(process_header_temp2 != NULL) {
		threadCreate = process_header_temp2->pid;		
		z = pthread_create(&threadCreate,&attr,&thread_start,(void*)process_header_temp2->pid);
		printf("sjf: %ld\n",threadCreate);
		//printf("sjf z:%d",z);
		if (z!=0) {
			exit(0);
		}
		process_header_temp2 = process_header_temp2->next;
	}	

}













int main(int argc ,  char **argv)
{

int i;
int n,r,s,q,z;
int j;
long swap;
long mainThreadId;
struct process_queue *process_queue_start,*process_queue_current,*process_queue_end,*process_queue_temp;
pthread_mutex_init(&m1,NULL);
process_queue_start=process_queue_current=process_queue_end=process_queue_temp=NULL;
mainThreadId = pthread_self();
printf("Main Thread ID: %ld\n",mainThreadId);
if (argc < 3 ) 
        {
		printf("\n -N: Number of Processes \n -r: Number of Resources \n -S: scheduling scheme (0: sjf   1: Round Robin)\n -Q : Quantum (Round Robin)\n ");
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
sigaddset(&set,SIGUSR1);
signal(SIGUSR1,sig_func);
//infinite_task=pthread_create(&process_header, &attr, );
switch (s)
{
case 0 :
	create_resources(r);
	create_processes(n,r);	
	sjf_execution(n);
	printf("\nScheduling Scheme = sjf");
    printf("\nNumber of Processes = %d \nNumber of Resources =%d\n",n,r);

	break;

case 1 :
	create_resources(r);
	create_processes(n,r);
	//assign_tasks_resources(n,r);
	//rr_execution(n);
	printf("\nScheduling Scheme = Round Robin");
	printf("\nNumber of Processes = %d \nNumber of Resources =%d \nQuantum (Round Robin) = %d\n ",n,r,q);
	break;
default: 
	printf("\nPlease choose '0' for sjf or '1' for RR\n");
	exit(0);
	break;
}

sleep(10);
process_header_temp = process_header_start;
	while(process_header_temp != NULL) {
		printf("MAIN: task execution: %d, threadID: %ld\n",process_header_temp->pid2,process_header_temp->threadID);
		process_header_temp = process_header_temp->next;
		
	}
	printf("\nThread ID Array:\n");
	

	for(i = 0; i<=n;i++){
		printf("Thread ID: %ld, Execution Time: %ld\n",global_data[i][0],global_data[i][1]);
	}

	for(i=1;i<=n;i++) {
		for (j=1;j<=n;j++) {
			if(global_data[i][1] < global_data[j][1]) {
				swap = global_data[i][1];
				global_data[i][1] = global_data[j][1];
				global_data[j][1] = swap;

				swap = global_data[i][0];
				global_data[i][0] = global_data[j][0];
				global_data[j][0] = swap;

				swap = global_data[i][2];
				global_data[i][2] = global_data[j][2];
				global_data[j][2] = swap;
			}
		}
	}

	printf("\nThread ID Array:\n");
	

	for(i = 0; i<=n;i++){
		printf("Thread ID: %ld, Execution Time: %ld, Task ID: %ld\n",global_data[i][0],global_data[i][1],global_data[i][2]);
	}
	for(i = 1;i<=n;i++) {
		z = pthread_kill(global_data[i][0],SIGUSR1);
		z = pthread_join(global_data[i][0],NULL);
		printf("\nBACK IN SCHEDULER\n");
	}
pthread_mutex_destroy(&m1);
}
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
long global_data[100][3];
int counter = 0;

sigset_t set;

typedef struct resource_header { 
	struct resource_header *previous, *next;   			 
	unsigned int resource_id;
	int resource_status;
	int resource_process_id;
	struct resource_wait_queue *resource_wait_queue_start, *resource_wait_queue_end,*resource_wait_queue_temp;
}resource_header_start,resource_header_end,resource_header_current,resource_header_temp;


struct process_header{
	struct process_header *previous, *next;
	int pid;
	int pid2;
	int pstatus;
	int pstart;
	int pexecution;
	int pend;
	long threadID;
	struct resource_allocation *task_resource_assignment_start,*task_resource_assignment_end, *task_resource_assignment_temp;

}*process_header_start,*process_header_end,*process_header_current,*process_header_temp , *process_header_sort,process_header,*process_header_start2,*process_header_end2,*process_header_current2,*process_header_temp2;

struct resource_allocation{
	struct resource_allocation *previous, *next;
	int id;
	int start_time;
	int hold_time;
	
};



struct process_queue{
	struct process_queue *previous;
	struct process_queue *next;
	int task_id;
}process_queue_start, process_queue_end, process_queue_current, process_queue_temp;

struct resource_wait_queue{
	
	int task_wait_id;
	struct resource_wait_queue *previous, *next;

};


void sig_func(int sig) {
	signal(SIGUSR1,sig_func);
}


int create_resources(int r){

	int i;
	struct resource_header *resource_header_temp, *resource_header_start, *resource_header_end, *resource_header_current;
	resource_header_temp=resource_header_start=resource_header_end=resource_header_current=NULL;
	for(i=0;i<r;i++)
	{
		resource_header_temp=(struct resource_header*) malloc (sizeof(struct resource_header));
		if(resource_header_start==NULL)
		{
			resource_header_start=resource_header_end=resource_header_current=resource_header_temp;
			resource_header_start->resource_id = i;
			resource_header_start->resource_status=1;
			resource_header_start->resource_process_id=0;
			resource_header_start->resource_wait_queue_start = NULL;
			resource_header_start->resource_wait_queue_end = NULL;
			resource_header_start->resource_wait_queue_temp = NULL;
			resource_header_start->next=NULL;
			resource_header_start->previous=NULL;		
		} else {
			//resource_header_temp->previous = resource_header_end;
			resource_header_end->next = resource_header_current->next = resource_header_temp;
			resource_header_current = resource_header_end = resource_header_temp;
			resource_header_end->resource_id = i;
			resource_header_end->resource_status=1;
			resource_header_end->resource_process_id=0;
			resource_header_end->resource_wait_queue_start = NULL;
			resource_header_end->resource_wait_queue_end = NULL;
			resource_header_end->resource_wait_queue_temp = NULL;
			resource_header_end->next=NULL;
			resource_header_end->previous=NULL;
			
		}			
	
	}

	resource_header_temp = resource_header_start;
	//while(resource_header_temp != NULL) {
	for(i=0;i<r;i++){
		printf("Resource ID: %d, Resource Status: %d\n",resource_header_temp->resource_id,resource_header_temp->resource_status);
		resource_header_temp = resource_header_temp->next;
		
	}
	

}


void create_processes(int n,int r)
{
	int i;
	int j;
	int resource_time = 0;
	int changeFlag=0;
	process_header_temp=process_header_start=process_header_end=process_header_current=NULL;
	int resource_header_array[3];
	int swap;
	for(i=1;i<=n;i++)
	{
		resource_time = 0;  
		resource_header_array[0] = rand()%r;
		resource_header_array[1] = rand()%r;
		while(resource_header_array[1] == resource_header_array[0]) {
			resource_header_array[1] = rand()%r;
		}
		resource_header_array[2] = rand()%r;
		while(resource_header_array[2] == resource_header_array[0] || resource_header_array[1] == resource_header_array[2]) {
			resource_header_array[2] = rand()%r;
		}

		process_header_temp=(struct process_header*) malloc (sizeof(struct process_header));
		if(process_header_start==NULL)
		{
			process_header_start=process_header_end=process_header_current=process_header_temp;
			process_header_start->pid = i;
			process_header_start->pid2 =i;
			global_data[i][2] = i; //printf("pid2: %d\n",process_header_start->pid2);
			process_header_start->pstatus=1;
			process_header_start->pstart=0;
			
			

			process_header_start->task_resource_assignment_start = process_header_start->task_resource_assignment_end = process_header_start->task_resource_assignment_temp = NULL;
			for(j = 0; j<3;j++) {
				process_header_start->task_resource_assignment_temp = (struct resource_allocation*) malloc(sizeof(struct resource_allocation));
				if(process_header_start->task_resource_assignment_start == NULL) {
					process_header_start->task_resource_assignment_end = process_header_start->task_resource_assignment_start = process_header_start->task_resource_assignment_temp;
					process_header_start->task_resource_assignment_start->next = NULL;
					process_header_start->task_resource_assignment_start->id = resource_header_array[j];
					  
					process_header_start->task_resource_assignment_start->start_time = rand() % 20;
					process_header_start->task_resource_assignment_start->hold_time = ((rand()%10) + 1) * 100;
					resource_time += process_header_start->task_resource_assignment_start->hold_time;
				} else {
					process_header_start->task_resource_assignment_end->next = process_header_start->task_resource_assignment_temp; 
					process_header_start->task_resource_assignment_end = process_header_start->task_resource_assignment_temp;
					process_header_start->task_resource_assignment_end->next = NULL;
					process_header_start->task_resource_assignment_end->id = resource_header_array[j];
					  
					process_header_start->task_resource_assignment_end->start_time = rand() % 20;
					process_header_start->task_resource_assignment_end->hold_time = ((rand()%10) + 1) * 100;
					resource_time += process_header_start->task_resource_assignment_end->hold_time;
				}
			}
			process_header_start->pexecution = resource_time + (rand()%20);
			global_data[i][1] = (long)process_header_start->pexecution;
			process_header_start->pend = rand()%100;
			process_header_start->next=NULL;
			process_header_start->previous=NULL;		
		} else {
			process_header_temp->previous = process_header_end;
			process_header_end->next = process_header_current->next = process_header_temp;
			process_header_current = process_header_end = process_header_temp;
			process_header_end->pid = i;
			process_header_end->pid2 = i;
			global_data[i][2] = i; //printf("pid2: %d\n",process_header_end->pid2);
			process_header_end->pstatus=1;
			process_header_end->pstart=0;
			

			process_header_end->task_resource_assignment_start = process_header_end->task_resource_assignment_end = process_header_end->task_resource_assignment_temp = NULL;
			for(j = 0; j<3;j++) {
				process_header_end->task_resource_assignment_temp = (struct resource_allocation*) malloc(sizeof(struct resource_allocation));
				if(process_header_end->task_resource_assignment_start == NULL) {
					process_header_end->task_resource_assignment_end = process_header_end->task_resource_assignment_start = process_header_end->task_resource_assignment_temp;
					process_header_end->task_resource_assignment_start->next = NULL;
					process_header_end->task_resource_assignment_start->id = resource_header_array[j];
					  
					process_header_end->task_resource_assignment_start->start_time = rand() % 20;
					process_header_end->task_resource_assignment_start->hold_time = ((rand()%10) + 1) * 100;
					resource_time += process_header_end->task_resource_assignment_start->hold_time;
				} else {
					process_header_end->task_resource_assignment_end->next = process_header_end->task_resource_assignment_temp; 
					process_header_end->task_resource_assignment_end = process_header_end->task_resource_assignment_temp;
					process_header_end->task_resource_assignment_end->next = NULL;
					process_header_end->task_resource_assignment_end->id = resource_header_array[j];
					  
					process_header_end->task_resource_assignment_end->start_time = rand() % 20;
					process_header_end->task_resource_assignment_end->hold_time = ((rand()%10) + 1) * 100;
					resource_time += process_header_end->task_resource_assignment_end->hold_time;
				}
				process_header_end->pexecution = resource_time + (rand()%20);
				global_data[i][1] = (long)process_header_end->pexecution;
				process_header_end->pend = rand()%100;
			}	
			
		}
			
	
	}


		

	process_header_temp = process_header_start;
	while(process_header_temp != NULL) {
		printf("task ID: %d, task Status: %d, execution time: %d, end time %d\n",process_header_temp->pid,process_header_temp->pstatus,process_header_temp->pexecution,process_header_temp->pend);
		process_header_temp->task_resource_assignment_temp = process_header_temp->task_resource_assignment_start;
		while(process_header_temp->task_resource_assignment_temp != NULL) {
			printf("Resource ID: %d, Start Time: %d, Hold Time: %d\n",process_header_temp->task_resource_assignment_temp->id,process_header_temp->task_resource_assignment_temp->start_time,process_header_temp->task_resource_assignment_temp->hold_time);
			process_header_temp->task_resource_assignment_temp = process_header_temp->task_resource_assignment_temp->next;
		} 
		process_header_temp = process_header_temp->next;
		
	}



}

static void *thread_start(void *arg) {
	int process_ID_2 = (int*)arg;
	int threadSearch;
	pthread_mutex_lock(&m1);
	
	global_data[process_ID_2][0] = pthread_self();
	signal(SIGUSR1,sig_func);
	printf("\nFrom Thread: Task ID: %d Thead ID: %ld\n",process_ID_2,pthread_self());	
	pthread_mutex_unlock(&m1);
	sigfillset(&set);
	sigdelset(&set,SIGUSR1);
	sigsuspend(&set);
	printf("Thread %d Running\n",process_ID_2);
	while(global_data[process_ID_2][1] > 0) {
		global_data[process_ID_2][1]--;
		usleep(1);
	}
}


void sjf_execution(int n) {
	int z,i;
	int *threadCreate;
	printf("INSIDE sjf.");
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	printf("\nThread ID Array:\n");
	for(i = 0; i<=n;i++){
		printf("Thread ID: %ld, Execution Time: %ld\n",global_data[i][0],global_data[i][1]);
	}
	process_header_temp2 = process_header_start;
	while(process_header_temp2 != NULL) {
		threadCreate = process_header_temp2->pid;		
		z = pthread_create(&threadCreate,&attr,&thread_start,(void*)process_header_temp2->pid);
		printf("sjf: %ld\n",threadCreate);
		//printf("sjf z:%d",z);
		if (z!=0) {
			exit(0);
		}
		process_header_temp2 = process_header_temp2->next;
	}	
}
int main(int argc ,  char **argv)
{
int i;
int n,r,s,q,z;
int j;
long swap;
long mainThreadId;
struct process_queue *process_queue_start,*process_queue_current,*process_queue_end,*process_queue_temp;
pthread_mutex_init(&m1,NULL);
process_queue_start=process_queue_current=process_queue_end=process_queue_temp=NULL;
mainThreadId = pthread_self();
printf("Main Thread ID: %ld\n",mainThreadId);
if (argc < 3 ) 
        {
		printf("\n -N: Number of Processes \n -r: Number of Resources \n -S: scheduling scheme (0: sjf   1: Round Robin)\n -Q : Quantum (Round Robin)\n ");
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
//infinite_task=pthread_create(&process_header, &attr, );
switch (s)
{
case 0 :
	create_resources(r);
	create_processes(n,r);	
	sjf_execution(n);
	break;

case 1 :
	create_resources(r);
	create_processes(n,r);
	rr_execution(n);
	break;
default: 
	printf("\nPlease choose '0' for sjf or '1' for RR\n");
	exit(0);
	break;
}


printf("\n-------------------------------------------------------------------------------\nFinal Scheduler Summary\n-------------------------------------------------------------------------------\n");
if(s==0)
{
printf("\nScheduling Scheme = sjf");
printf("\nNumber of Processes = %d \nNumber of Resources =%d\n",n,r);
}
else
{
printf("\nScheduling Scheme = Round Robin");
printf("\nNumber of Processes = %d \nNumber of Resources =%d \nQuantum (Round Robin) = %d\n ",n,r,q);
}
printf("-------------------------------------------------------------------------------\n");
sleep(3);
process_header_temp = process_header_start;
	while(process_header_temp != NULL) {
		printf("MAIN: task execution: %d, threadID: %ld\n",process_header_temp->pid2,process_header_temp->threadID);
		process_header_temp = process_header_temp->next;
		
	}
	printf("\nThread ID Array:\n");
	

	for(i = 0; i<=n;i++){
		printf("Thread ID: %ld, Execution Time: %ld\n",global_data[i][0],global_data[i][1]);
	}

	for(i=1;i<=n;i++) {
		for (j=1;j<=n;j++) {
			if(global_data[i][1] < global_data[j][1]) {
				swap = global_data[i][1];
				global_data[i][1] = global_data[j][1];
				global_data[j][1] = swap;

				swap = global_data[i][0];
				global_data[i][0] = global_data[j][0];
				global_data[j][0] = swap;

				swap = global_data[i][2];
				global_data[i][2] = global_data[j][2];
				global_data[j][2] = swap;
			}
		}
	}

	printf("\nThread ID Array:\n");
	

	for(i = 0; i<=n;i++){
		printf("Thread ID: %ld, Process Execution Time: %ld, Process ID: %ld\n",global_data[i][0],global_data[i][1],global_data[i][2]);
	}
	for(i = 1;i<=n;i++) {
		z = pthread_kill(global_data[i][0],SIGUSR1);
		z = pthread_join(global_data[i][0],NULL);
		printf("\n End \n");
	}
pthread_mutex_destroy(&m1);
}
