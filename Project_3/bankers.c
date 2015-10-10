/* BANKERS ALGORITHM USING PTHREAD AND MUTEX LOCKS- @author: Alekhya Gade */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3

int available[NUMBER_OF_RESOURCES];
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

//four mutexes for four shared data structures
pthread_mutex_t mutexavail;
pthread_mutex_t mutexalloc;
pthread_mutex_t mutexneed;


int request_resources(int customer_num, int request[]);
int release_resources(int customer_num, int request[]);
void print_matrix(int M[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES],char *name);

void generate_maximum()
{
	int ii,jj;
	//for choosing different seed everytime
	srand(time(NULL));
	for(ii=0;ii<NUMBER_OF_CUSTOMERS;ii++)
	{
		for(jj=0;jj<NUMBER_OF_RESOURCES;jj++)
		{
			maximum[ii][jj] =  rand() % available[jj];
			allocation[ii][jj] = 0;
		}
	}
}

int request_resources(int customer_num, int request[])
{
	int iter,j,ii;
	/*
	printf("\n %d customer request arrived ", customer_num);
	for(j=0;j<NUMBER_OF_RESOURCES;j++)
				printf("%d ", request[j]);
	printf("\n");
	*/
	for(iter=0;iter<NUMBER_OF_RESOURCES;iter++)
	{
		//request cannot be processed
		if(request[iter] > need[customer_num][iter])
			return -1;
	}

	//pretend to have allocated the requested resources to process Pi
	for(iter=0;iter<NUMBER_OF_RESOURCES;iter++)
	{
		pthread_mutex_lock(&mutexalloc);
		allocation[customer_num][iter] += request[iter];
		pthread_mutex_unlock(&mutexalloc);
		
		pthread_mutex_lock(&mutexavail);
		available[iter] -= request[iter];
		pthread_mutex_unlock(&mutexavail);

		pthread_mutex_lock(&mutexneed);
		need[customer_num][iter] -= request[iter];
		pthread_mutex_unlock(&mutexneed);		
	}

	/*
		printf("\n Available Matrix \n");
		for(ii=1;ii<=NUMBER_OF_RESOURCES;ii++)
		{
			printf("%d ",available[ii-1]);
		}
		printf("\n");

		print_matrix(allocation,"allocation");
		print_matrix(need,"need");
		print_matrix(maximum,"max");
	*/
	//revert to old state if its not safe
	if(check_safe()<0)
	{
		for(iter=0;iter<NUMBER_OF_RESOURCES;iter++)
		{
			pthread_mutex_lock(&mutexalloc);
			allocation[customer_num][iter] -= request[iter];
			pthread_mutex_unlock(&mutexalloc);
			
			pthread_mutex_lock(&mutexavail);
			available[iter] += request[iter];
			pthread_mutex_unlock(&mutexavail);

			pthread_mutex_lock(&mutexneed);
			need[customer_num][iter] += request[iter]; 
			pthread_mutex_unlock(&mutexneed);		
		}
		return -1;
	}

	return 0;
}

int release_resources(int customer_num, int request[])
{
	int iter;
	for(iter=0;iter<NUMBER_OF_RESOURCES;iter++)
		{
			pthread_mutex_lock(&mutexalloc);
			allocation[customer_num][iter] -= request[iter];
			pthread_mutex_unlock(&mutexalloc);
			
			pthread_mutex_lock(&mutexavail);
			available[iter] += request[iter];
			pthread_mutex_unlock(&mutexavail);

			pthread_mutex_lock(&mutexneed);
			need[customer_num][iter] = maximum[customer_num][iter] + allocation[customer_num][iter];
			pthread_mutex_unlock(&mutexneed);		
		}

	return 1;
}

int check_safe()
{
	int ii,jj, work[NUMBER_OF_RESOURCES],finish[NUMBER_OF_CUSTOMERS];
	int success_flag = 0;
	for(ii=0;ii<NUMBER_OF_RESOURCES;ii++)
	{
		work[ii] = available[ii];
	}

	for(ii=0;ii<NUMBER_OF_CUSTOMERS;ii++)
	{
		//0- False 1 - True
		finish[ii] = 0;
	}

	//bankers algorithm
	for(ii=0;ii<NUMBER_OF_CUSTOMERS;ii++)
	{
		if(finish[ii]==0)
		{
			for(jj=0;jj<NUMBER_OF_RESOURCES;jj++)
			{
				//unsafe to allocate
				if(need[ii][jj] > work[jj])
					return -1;
			}

			for(jj=0;jj<NUMBER_OF_RESOURCES;jj++)
				work[jj] += allocation[ii][jj];

			success_flag = 1;
		}
	}
	return success_flag;

}

void *thread_create(void *cno)
{
	int ii,j,request[NUMBER_OF_RESOURCES],request_flag=0;
	int cust_no = (int)cno;
	//Create a random request
	for(ii=0;ii<NUMBER_OF_RESOURCES;ii++)
	{
		//this will be always less than the max that can be requested
		request[ii] = rand() % available[ii];
	}

	//Resource allocated depending on safety and this customer waits on request
		if(request_resources(cust_no,request)<0)
		{
			printf("\n Customer %d ", cust_no);
			for(j=0;j<NUMBER_OF_RESOURCES;j++)
				printf("%d ", request[j]);
			printf(":Request DENIED\n");
		}
		else
		{
			request_flag = 1;
			printf("\n Customer %d ", cust_no);
			for(j=0;j<NUMBER_OF_RESOURCES;j++)
				printf("%d ", request[j]);
			printf(":Request ACCEPTED\n");
		}	

	//Release the resource if allocated
	if(request_flag==1)
	{
		//sleep for a random amount of time
		sleep(rand() % 10);
		release_resources(cust_no, request);
		printf("\n Customer %d released resouces", cust_no);
	}

}

void print_matrix(int M[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES],char *name)
{
	int i,j;
	printf("\n-----%s Matrix----\n",name);
	for(i=0;i<NUMBER_OF_CUSTOMERS;i++)
	{
		for(j=0;j<NUMBER_OF_RESOURCES;j++)
			printf("%d ",M[i][j]);
		printf("\n");
	}
}

int main(int argc, char **argv)
{
	int ii,jj,kk,run_count = 50;
	pthread_t thread_id;
	if(argc==(NUMBER_OF_RESOURCES+1))
	{
		printf("\n Available Matrix \n");
		//assign the input arguments to available array
		for(ii=1;ii<=NUMBER_OF_RESOURCES;ii++)
		{
			available[ii-1] = abs(atoi(argv[ii]));
			printf("%d ",available[ii-1]);
		}
		printf("\n");
	}
	//generate the maximum and allocation matrices
	generate_maximum();
	//calculate the current need matrix and print maximum matrix
	printf("\n Maximum matrix is: \n");
	for(ii=0; ii<NUMBER_OF_CUSTOMERS;ii++)
	{
		for(jj=0; jj<NUMBER_OF_RESOURCES; jj++)
		{
			need[ii][jj] = maximum[ii][jj] - allocation[ii][jj];	
			printf("%d ",maximum[ii][jj]);
		}
		printf("\n");
	}

	printf("\n Need Matrix\n");
	for(ii=0; ii< NUMBER_OF_CUSTOMERS;ii++)
	{
		for(jj=0;jj<NUMBER_OF_RESOURCES;jj++)
		{
			printf("%d ",need[ii][jj]);
		}
		printf("\n");
	}


	for(ii=0;ii<run_count;ii++)
	{
		
		for(jj=0;jj<NUMBER_OF_CUSTOMERS;jj++)
		{
			//printf("\n creating %d",jj);
			pthread_create(&thread_id,NULL,thread_create,(void *)jj);
			

		}
	}

	printf("\n All threads finished without any deadlocks! ");
	return 0;
}