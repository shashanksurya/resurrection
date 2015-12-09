/*An Elevator Scheduler Simulator*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#define MAX_FLOORS 10 //A 10 floor elevator
#define MAXIMUS 8 //max weight unit and passenger unit
#define MOVING 2 //2 secs between floors
#define ULOAD 1 //1 sec while load/unload

/* Dev notes <--Remove before production-->
adult - 1 pass 1 weight
child - 1 pass 1/2 weight
bellhop-2 pass 2 weight
rose -  1 pass 2 weight

Random pass each floor.They know where to go.
2,3 - > 1 always
2 sec wait b/w floors
1 sec unloading/loading
1 - 10 floors
*/

struct E
{
	float curweight;
	int curload;
	int rq[MAXIMUS];//PASSENGER REQUESTS
	int pt[MAXIMUS]; //PASSENGER TYPES
	int curfloor;
	int nextfloor;
	int state;
	int prev;
	int total_served;
	/*
	-1- NOT ACTIVE
	0 - IDLE
	1 - UP
	2 - DOWN
	3 - LOADING
	4 - STOPPED
	*/
};

struct floorQ
{
	int type;
	int dest;
	struct floorQ* next;
};
struct E elevator;
struct floorQ fq[MAX_FLOORS];
struct floorQ* front[MAX_FLOORS];
struct floorQ* rear[MAX_FLOORS];

void print_status()
{

}


float findweight(int type)
{
	switch(type)
	{
		case 0:
		return 1;
		case 1:
		return 0.5;
		case 2:
		return 2;
		case 3:
		return 2;
	}
}

void unloadpassenger()
{
	int ii,count=0,weight=0;
	if(elevator.curload==0)
		return;
	elevator.state = 3;
	elevator.curfloor = elevator.nextfloor;
	for(ii=0;ii<MAXIMUS;ii++)
	{
		if(elevator.rq[ii]==elevator.curfloor)
		{
			count+=1;
			weight+=findweight(elevator.pt[ii]);
			elevator.rq[ii] = -1;
			elevator.pt[ii] = -1;
		}
	}
	elevator.curload-= count;
	elevator.curweight-=weight;
	elevator.total_served+=count;
}

int find_slot()
{
	int ii=0;
	for(ii=0;ii<MAXIMUS;ii++)
	{
		if(elevator.rq[ii]==-1)
			return ii;
	}
	return -1;
}

void movepassenger(struct floorQ* f)
{
	int slot = find_slot();
	//if -1, then something went terribly wrong
	if(slot!=-1)
	{
		elevator.rq[slot] = f->dest;
		elevator.pt[slot] = f->type;
	}
	elevator.curweight+=findweight(f->type);
	elevator.curload+=1;
}

void consumepassenger()
{
	int ii;
	int c = elevator.curfloor;
	int cload,nxtload = 0;
	int cursum = 0;
	struct floorQ* temp = front[c-1];
	if(front[c-1]==NULL)
		return;
	cload = elevator.curload;
	cursum = elevator.curweight;
	while(temp!=NULL)
	{
		cursum+=findweight(temp->type);
		cload+=1;
		if(cursum>MAXIMUS||cload>MAXIMUS)
			break;
		nxtload+=1;
		temp = temp->next;
	}
	if(nxtload>0)
	{
		for(ii=nxtload;ii>0;ii--)
		{
			movepassenger(front[c-1]);
			temp = front[c-1];
			if(front[c-1] == rear[c-1])
				front[c-1] = rear[c-1] = NULL;
			else
				front[c-1] = front[c-1]->next;
			free(temp);
		}
	}
}

void setdestination()
{
	if(elevator.prev==0)
	{
		elevator.state = 1;
		//elevator.prev = 1;
		elevator.nextfloor+=1;
	}
	else if(elevator.prev==1&&elevator.curfloor<10)
	{
		elevator.state = 1;
		//elevator.prev = 1;
		elevator.nextfloor+=1;
	}
	else if((elevator.prev==1)||(elevator.prev==2&&elevator.curfloor>0))
	{
		elevator.state = 2;
		//elevator.prev = 2;
		elevator.nextfloor-=1;	
	}
	else if(elevator.prev==2&&elevator.curfloor==0)
	{
		elevator.state = 1;
		//also stop lift if no one is there		
		elevator.nextfloor+=1;
	}
}

void move_elevator()
{
	elevator.prev = elevator.state;

}
void run_elevator()
{
	//once started this keeps looping accepting passengers
	int testc = 3;
	while(testc!=0)
	{	
		unloadpassenger();
		consumepassenger();
		sleep(1);
		print_status();
		setdestination();
		move_elevator();
		//for moving
		sleep(2);
		testc--;
	}
}

//Function that 
void init_elevator()
{
	int i;
	elevator.curweight = -1;
	elevator.curload = -1;
	elevator.curfloor = 0;
	elevator.state = -1;
	//No passengers in any floor
	for(i=0;i<MAX_FLOORS;i++)
	{
		front[i] = NULL;
		rear[i] = NULL;
	}

	for(i=0;i<MAXIMUS;i++)
	{
		elevator.rq[i] = elevator.pt[i] = -1;
	}
}


int start_elevator()
{
	int i;
	if(elevator.state > 0)
		return 1;
	else
	{
		elevator.curweight = 0;
		elevator.curload = 0;
		elevator.curfloor = 1;
		elevator.nextfloor = 1;
		elevator.state = 0;
		elevator.prev = 0;
		elevator.total_served = 0;
		run_elevator();
	}
	return 0;

}

int stop_elevator()
{
	return 1;
}

int addpassenger(struct floorQ* f,int s)
{
	if(s>MAX_FLOORS||s<=0)
		return 1;
	if(front[s-1] == NULL && rear[s-1] == NULL)
	{
		front[s-1] = rear[s-1] = f;
	}
	else
	{
		rear[s-1]->next = f;
		rear[s-1] = f;
	}
	return 0;
}

int issue_request(int ptype,int src,int dest)
{	
	struct floorQ* tempf;
	if(dest>MAX_FLOORS||dest<=0||ptype<0||ptype>3)
		return 1;
	tempf = (struct floorQ*)malloc(sizeof(struct floorQ));
	tempf->type = ptype;
	tempf->dest = dest;
	tempf->next = NULL;
	return addpassenger(tempf,src);
}

int main(void)
{
	init_elevator();
	issue_request(0,1,10);
	start_elevator();
	return 0;
}