/* BOUNDING BUFFER USING COUNTING SEMAPHORE- @author: Alekhya Gade */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>
#include "buffer.h"

int kill_flag = 0;
int n;
pthread_mutex_t mutex;
sem_t empty ;
sem_t full;

/*2. Initialize buffer */
buffer_item buffer[BUFFER_SIZE];


void init()
{
	 n=0;
	 sem_init(&empty, 0, BUFFER_SIZE);
	 sem_init(&full, 0 , 0);
	 pthread_mutex_init(&mutex,NULL);

}


void sig_handler(int signo)
{
	if(signo == SIGINT)
	{
		printf("\n Received Ctrl + C ");
		printf("\n Asking producer and consumer to kill themselves");
		kill_flag = 1;
		/*6. Delaying exit for produce and consumer to kill */
		sleep(2);
		//exit(-1);
	}
}

int insert_item(buffer_item item) {
	/* insert item into buffer
	return 0 if successful, otherwise
	return -1 indicating an error condition */
	if(n < BUFFER_SIZE)
	{
		buffer[n] = item;
		n+=1;
		return 0;
	}
	else
		return -1;


}
int remove_item(buffer_item *item)
{
/* remove an object from
placing it in item
return 0 if successful,
return -1 indicating an error condition*/
if(n>0)
	{
		n-=1;
		*item  = buffer[(n)];
		return 0;
	}
	else
		return -1;
}

//PRODUCER THREAD
void *producer()
{
	buffer_item item;

	do {
		/* produce an item in next produced */
		sleep(rand() % 10);
		item = rand();
		sem_wait(&empty);
		pthread_mutex_lock(&mutex);
		
		if(insert_item(item))
			printf("\nOops! Producer has a problem!");
		else
			printf("\nproducer produced %d",item);
		/* add next produced to the buffer */
		pthread_mutex_unlock(&mutex);
		sem_post(&full);
		if(kill_flag==1)
			break;
		} while (1);

}

//CONSUMER THREAD
void *consumer()
{
	buffer_item item;
	do {
		sleep(rand() % 10);
		sem_wait(&full);
		pthread_mutex_lock(&mutex);
		/* remove an item from buffer to next consumed */
		if(remove_item(&item))
			printf("\n Oops! Consumer has a problem");
		else
			printf("\nconsumer consumed %d",item);
		//signal(mutex);
		pthread_mutex_unlock(&mutex);
		sem_post(&empty);
		/* consume the item in next consumed */
		if(kill_flag==1)
			break;

		} while (1);
}

int main(int argc, char **argv) {

	/*1. Get command line arguments argv[1],argv[2],argv[3] */

	int ii,sleep_time,producer_threads,consumer_threads;
	pthread_t thread_id;
	// -ve values are implicitly considered as +ve values
	if(argc==4)
		{
			sleep_time = abs(atoi(argv[1]));
			producer_threads = abs(atoi(argv[2]));
			consumer_threads = abs(atoi(argv[3]));
		}
	
	printf("\n ----------------Configuration -----------------------");
	printf("\n SLEEP TIME = %d", sleep_time);
	printf("\n NO. OF PRODUCERS = %d", producer_threads);
	printf("\n NO. OF CONSUMERS = %d", consumer_threads);

	
	init();

	/*3. Create producer thread(s) */	
	for(ii=0;ii<producer_threads;ii++)
		pthread_create(&thread_id,NULL,producer,NULL);
	
	/*4. Create consumer thread(s) */
	for(ii=0;ii<consumer_threads;ii++)
		pthread_create(&thread_id,NULL,consumer,NULL);

	//followed the tutorial mentioned on the website
	if(signal(SIGINT,sig_handler)==SIG_ERR)
		printf("\n Cant catch");

	/*5. Sleep */
	sleep(sleep_time);

	printf("\n");
	//normal exit
	exit(0);
	
}