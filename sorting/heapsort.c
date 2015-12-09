/*Max Heap */
#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 100 //May be more if we need



struct PQ
{
	int q[MAX_SIZE];
	int qsize;
};


void print_heap(struct PQ* pq)
{
	int i=0;
	printf("Elements of the heap are\n");
	for(i=1;i<=pq->qsize;i++)
	{
		printf("\t%d\n",pq->q[i]);
	}
	printf("\n");
}


//Initialize the queue
void init_queue(struct PQ* pq)
{
	pq->qsize = 0;
}

//Get the parent of an element in a queue
int get_parent(int n)
{
	return (n==1)?-1:(n/2);
}

//Get the youngest child of an element in a queue
int get_child(int x)
{
	return (2*x);
}

//Swap two elements of the heap based on indexes
void swap(struct PQ* pq,int x,int y)
{
	int c = pq->q[x];
	pq->q[x] = pq->q[y];
	pq->q[y] = c;
}

//Re'curs'ive 
void bubble_up(struct PQ* pq,int z)
{
	if(get_parent(z) == -1) return; //Root of the heap
	//Max heap(parent dominates children)
	if(pq->q[z]>pq->q[get_parent(z)])
	{
		swap(pq,z,get_parent(z));
		bubble_up(pq,get_parent(z));
	}
}

//Insert into Heap
void insert(struct PQ* pq,int element)
{
	if(pq->qsize>=MAX_SIZE)
	{
		printf("%s\n","Queue overflow!!!");
	}
	else
	{
		pq->qsize+=1;
		pq->q[pq->qsize] = element;
		bubble_up(pq,pq->qsize);
	}
}


void make_heap(struct PQ* pq,int* s,int length)
{
	int i=0;
	init_queue(pq);
	for(i=0;i<length;i++)
	{
		insert(pq,s[i]);
		print_heap(pq);
	}
}

void bubble_down(struct PQ* pq,int z)
{
	int chindex;
	int i;
	int max;

	chindex = get_child(z);
	max = z;
	for(i=0;i<=1;i++)
	{
		if((pq->q[z]<pq->q[chindex+i]) && ((chindex+i)<=pq->qsize))
			{
				if(i==1)
				{
					if(pq->q[chindex]<pq->q[chindex+i])
						max = chindex+i;		
				}
				else
					max = chindex+i;
			}
	}
	if(max!=z)
	{
		swap(pq,z,max);
		bubble_down(pq,max);
	}
}


int extract_min(struct PQ* pq)
{
	int max = -1;
	if(pq->qsize<=0)
		printf("Queue Underflow!!!\n");
	else
	{
		max = pq->q[1];
		pq->q[1] = pq->q[pq->qsize];
		pq->qsize-=1;
		bubble_down(pq,1);
	}
	return max;
}




void print_sorted_heap(struct PQ* pq)
{
	int x;
	printf("Sorted Heap\n");
	while((x=extract_min(pq))!=-1)
		printf("\t%d\n",x);
}



int main(void)
{
	struct PQ* p = (struct PQ*)malloc(sizeof(struct PQ));
	int s[] = {8675,989,9999,99,3,2,22,777,342,222};
	int length = sizeof(s)/sizeof(s[0]);
	make_heap(p,s,length);
	//print_heap(p);
	print_sorted_heap(p);
	//free when done
	free(p);
	return 1;
}