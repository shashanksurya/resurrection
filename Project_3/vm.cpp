/* A VIRTUAL MEMORY MANAGER with LRU page replacement policy in C++ -
 @author: Alekhya Gade */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <algorithm>

using namespace std;

//some constants
#define TABLE_SIZE  256
const int mask = 0x00FF;
int main_pointer,query_pointer, page_faults,TLB_HIT,address_count;

//INITIAL DATA STRUCTURES
//Arrays
int pagetable[TABLE_SIZE];
int frameTable[TABLE_SIZE];

//Vectors
vector<int> frame_queue;
vector<int> TLB_STACK;


//Maps
map<int,int> TLB_COUNTS;
map<int,int> TLB;
unsigned char main_memory[TABLE_SIZE*TABLE_SIZE];

//File pointers
ifstream BACKING_STORE ("BACKING_STORE.bin", ios::binary);
FILE *disk_simulator;

float fault_count,TLB_RATE;

int init()
{
	// iNITIALIZE all the variables to make peace with memory
	main_pointer = 0;
	query_pointer =0;
	page_faults =0;
	TLB_HIT = 0;
	fault_count =0;
	address_count =0;
	//Initialize Page Table with invalid entries
	for(int ii=0;ii< (TABLE_SIZE-1);ii++)
		pagetable[ii] = -1;

	//Initialize Frame Table 
	for(int ii=0;ii<(TABLE_SIZE-1);ii++)
		frameTable[ii] = ii* TABLE_SIZE;


}
//LRU algorithm
void updateTLB(int temp_page)
{
	std::vector<int>::iterator it;
	it = find(TLB_STACK.begin(),TLB_STACK.end(),temp_page);
	if(it!=TLB_STACK.end())
    {
    	TLB_COUNTS[temp_page]+=1;
    }
	else
	{
		TLB_COUNTS[temp_page] = 0;	
	}
}

int getLRU()
{
	int least = 100000;
	int page_num;
	map<int,int>::iterator it;
	for(it = TLB_COUNTS.begin(); it!= TLB_COUNTS.end();it++)
	{
		if(it->second < least)
		{
			least = it->second;
			page_num = it->first;
		}
	}
	TLB_COUNTS.erase(page_num);
	return page_num;
}

int memory_driver(int address)
{
	int tempval=0,frame_number=0;
	char buffer[TABLE_SIZE];
	int temp_page = address>>8;
	int offset = address & mask;

	//printf("\n%d 	%d 		%d",offset,temp_page,address);

	if((TLB.count(temp_page)<=0) && pagetable[temp_page] == -1)
	{
		
		page_faults+=1;
		address_count+=1;
		int output_address;
		int seeker = temp_page * TABLE_SIZE;
		BACKING_STORE.seekg(seeker);
		BACKING_STORE.read(buffer,TABLE_SIZE);
		memcpy(&main_memory[main_pointer], &buffer, TABLE_SIZE);
		tempval = main_memory[main_pointer+offset];
		main_pointer+=TABLE_SIZE;
		pagetable[temp_page] = frameTable[query_pointer];
		frame_number = pagetable[temp_page];
		query_pointer+=1;
		if (TLB.size() <=16)
		{
			TLB[temp_page] = frame_number;
			updateTLB(temp_page);
		}
		else
		{
			TLB.erase(getLRU());
		}

		//to get physical address
		output_address = frame_number | offset;
		tempval = (tempval>127)?-(256-tempval):tempval;
		printf("\nPysical Address %d --- Virtual Address %d--- Value %d",output_address,address,tempval);
	}
	else
	{
		if(TLB.count(temp_page)>0)
		{
			updateTLB(temp_page);
			TLB_HIT+=1;
			address_count+=1;
			if(TLB[temp_page]>0)
				frame_number = TLB[temp_page];
			else
				frame_number = 0;
			int output_address = frame_number | offset;
			int TLB_entry = main_memory[frame_number+offset];
			TLB_entry = (TLB_entry>127)?-(256-TLB_entry):TLB_entry;
			printf("\nPysical Address %d --- Virtual Address %d--- Value %d",output_address,address,TLB_entry);
		}
		else
		{
			address_count+=1;
			frame_number = pagetable[temp_page];
			int output_address = frame_number | offset;
			int TLB_entry = main_memory[frame_number+offset];
			TLB_entry = (TLB_entry>127)?-(256-TLB_entry):TLB_entry;
			printf("\nPysical Address %d --- Virtual Address %d--- Value %d",output_address,address,TLB_entry);
		}
	}
	
}


int main(int argc,char **argv)
{
	//opening necessary files
	FILE *fp = fopen(argv[1],"r");
	init();

	char ch;
	if(fp==NULL)
	{
		printf("Error opening the input file! Aborting!");
		exit(0);
	}
	else
	{
		while(1)
		{
			static char dummy[5];
			if(fscanf(fp,"%s",dummy)!=EOF)
			{
				memory_driver(atoi(dummy));			
			}
			//Reached the end of the file
			else
				break;
		}
	}
	fault_count = (float)page_faults/address_count;
	TLB_RATE = (float)TLB_HIT/address_count;
	printf("\n Total Addresses = %d",address_count);
	printf("\n Page Faults = %d",page_faults);
	printf("\n TLB Hits = %d",TLB_HIT);
	printf("\n Page Faults Rate = %f",fault_count);
	printf("\n TLB HITS Rate = %f",TLB_RATE);
	fclose(fp);
	BACKING_STORE.close();
	printf("\n");
	return 0;
}