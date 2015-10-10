#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define NUMMEMORY 65536
#define NUMREGS 8
#define ADD 0
#define NAND 1
#define LW 2
#define SW 3
#define BEQ 4
#define MULT 5
#define HALT 6
#define NOOP 7

#define TAKEN 0
#define W_TAKEN 1
#define W_NOTTAKEN 2
#define NOTTAKEN 3

#define BUFFERSIZE 17


typedef struct rename
{
	int ren_idn;
	int valid_bit;
} rename_row;


typedef struct basic_rs
{
	int idn_destn;
	int idn_src1;
	int idn_src2;
	int value_at_src1;
	int validity_src1;
	int value_at_src2;
	int validity_src2;
}Brs;

typedef struct rs_addtype
{
	Brs add_rs;
	int ROB_beq;
	int BEQPC;
	int opertn_typ;
	
}ADD_resv;

typedef struct record_rs
{
	Brs load_rs;
	int ROB_sw;
	int opertn_typ;
	int offset;
}LOAD_resv;

typedef struct rs_multype
{

	Brs mul_rs;
	int bit_fu;
}MULT_resv;

typedef struct REORDER_BUFFER
{
	int validity;
	int acc_regfile_idn;
	int reset;
	int ren_idn;
	int dest_value;
	int bit_beq;
	
}ROB_row;

typedef struct Reorder
{
	ROB_row item[16];
	int front;
	int size;
	int rear;
}ROB_que;

typedef struct FUNCTIONAL_UNIT
{
	int isfree_fu;
	int idn_destn;
	int rec;
	int idn;	
	int begin_exec;
	int end_exec;
}FU;

typedef struct PATTERNHISTORY_TABLE
{
int state[64];
}st_PHT;

typedef struct BRANCHTARGET_BUFFER
{
	int pc;
	int pc_target;
}BTR_row;

typedef struct BRANCHTARGETBUFFER_STRUCTURE
{
	int front;
	int rear;
	BTR_row elements[64];
	
}str_BTB;

	ADD_resv Rtype_RS[3];
	LOAD_resv Itype_RS[3];
	MULT_resv Mtype_RS[3];
	rename_row renametable[8];
	ROB_que robQ;
	ADD_resv new_Rtype_RS[3];
	LOAD_resv new_Itype_RS[3];
	MULT_resv new_Mtype_RS[3];
	rename_row new_renametable[8];
	ROB_que new_robQ;
	FU new_addFU;
	FU new_loadFU;
	FU new_multFU[3];
	
	int i;
	int j;
	int N;
	int pick;
	int temp;
	int BranchTarget;
	int instruction_memory[NUMMEMORY];
	int data_memory[NUMMEMORY];
	int registers[NUMREGS];
	int num_mem;
	int new_Rbit;
	int new_Mbit;
	int new_Ibit;
	int new_intr1_alloc;
	int new_intr2_alloc;
	int new_intr1_fetch;
	int new_intr2_fetch;
	int new_ren_idn;
	
	int new_pcplus1;
	int new_intr_noop;
	int new_cycles;
	int new_haltfetch;
	int new_fetch1_only;
	
	int FETCHED=0;
	int RETIRED=0;
	int BRANCHES=0;
	int MISPRED=0;

int opcode(int instruction)
{
    return(instruction>>22);
}
int
field0(int instruction)
{
    return( (instruction>>19) & 0x7);
}
int
field1(int instruction)
{
    return( (instruction>>16) & 0x7);
}
int
field2(int instruction)
{
    return(instruction & 0xFFFF);
}
	
void assign_newtoold()
{
	 robQ=new_robQ;

			 for(i=0;i<3;i++)
			{
				 Rtype_RS[i]=new_Rtype_RS[i];
				 Itype_RS[i] = new_Itype_RS[i];
				 Mtype_RS[i]= new_Mtype_RS[i];
			}
	
	
			for(i=0;i<8;i++)
			{
				renametable[i]= new_renametable[i];
			}
}

void assign_oldtonew()
{
	 new_robQ=robQ;
	 
	 for(i=0;i<8;i++)
		{
		new_renametable[i]= renametable[i];
		}
	 for(i=0;i<3;i++)
		{
		 new_Rtype_RS[i]=Rtype_RS[i];
		 new_Itype_RS[i] = Itype_RS[i];
		 new_Mtype_RS[i]= Mtype_RS[i];
		}
	
}

void initialize_ROB(ROB_que *ROB)
	{
	ROB->front=0;
	ROB->rear=0;
	ROB->size=0;
}

void addto_ROB(ROB_que *ROB, int idn_ARF,int idn_ren,int destination,int validbit,int beqb,int res)
{
	ROB_row list;
	
	list.acc_regfile_idn = idn_ARF;
	list.reset=res;
	list.validity=validbit;
	list.dest_value=destination;
	list.bit_beq=beqb;
	list.ren_idn = idn_ren;

	ROB->item[ROB->rear] = list;
	ROB->rear = (ROB->rear+1)%BUFFERSIZE;
}

void remove_ROB(ROB_que *ROB){
	if(ROB->front!=ROB->rear)
		ROB->front = (ROB->front + 1)%BUFFERSIZE;
	else
		return;
		}


int idn_ROB(int S)
{
	temp=new_robQ.front;
	while(new_robQ.item[temp].ren_idn!=S)
		temp=(temp+1)%BUFFERSIZE;
	return temp;
}		
		
void new_initializeRS()
{
	
	for(i=0;i<3;i++)
	{

		new_Rtype_RS[i].add_rs.idn_src1=new_Rtype_RS[i].add_rs.idn_src2=new_Rtype_RS[i].add_rs.idn_destn=new_Rtype_RS[i].add_rs.value_at_src1=new_Rtype_RS[i].add_rs.value_at_src2=new_Rtype_RS[i].add_rs.validity_src1=new_Rtype_RS[i].add_rs.validity_src2=new_Rtype_RS[i].opertn_typ=new_Rtype_RS[i].BEQPC=new_Rtype_RS[i].ROB_beq=0;

		new_Mtype_RS[i].mul_rs.idn_src1=new_Mtype_RS[i].mul_rs.idn_src2=new_Mtype_RS[i].mul_rs.idn_destn=new_Mtype_RS[i].mul_rs.value_at_src1=new_Mtype_RS[i].mul_rs.value_at_src2=new_Mtype_RS[i].mul_rs.validity_src1=new_Mtype_RS[i].mul_rs.validity_src2=new_Mtype_RS[i].bit_fu=0;
		
		new_Itype_RS[i].load_rs.idn_src1=new_Itype_RS[i].load_rs.idn_src2=new_Itype_RS[i].load_rs.idn_destn=new_Itype_RS[i].load_rs.value_at_src1=new_Itype_RS[i].load_rs.value_at_src2=new_Itype_RS[i].offset=new_Itype_RS[i].load_rs.validity_src1=new_Itype_RS[i].load_rs.validity_src2=new_Itype_RS[i].opertn_typ=new_Itype_RS[i].ROB_sw=0;
	
	}
	
}

void initializeRS()
{

	for(i=0;i<3;i++)
	{

		Rtype_RS[i].add_rs.idn_src1=Rtype_RS[i].add_rs.idn_src2=Rtype_RS[i].add_rs.idn_destn=Rtype_RS[i].add_rs.value_at_src1=Rtype_RS[i].add_rs.value_at_src2=Rtype_RS[i].add_rs.validity_src1=Rtype_RS[i].add_rs.validity_src2=Rtype_RS[i].opertn_typ=Rtype_RS[i].BEQPC=Rtype_RS[i].ROB_beq=0;

		Mtype_RS[i].mul_rs.idn_src1=Mtype_RS[i].mul_rs.idn_src2=Mtype_RS[i].mul_rs.idn_destn=Mtype_RS[i].mul_rs.value_at_src1=Mtype_RS[i].mul_rs.value_at_src2=Mtype_RS[i].mul_rs.validity_src1=Mtype_RS[i].mul_rs.validity_src2=Mtype_RS[i].bit_fu=0;
		
		Itype_RS[i].load_rs.idn_src1=Itype_RS[i].load_rs.idn_src2=Itype_RS[i].load_rs.idn_destn=Itype_RS[i].load_rs.value_at_src1=Itype_RS[i].load_rs.value_at_src2=Itype_RS[i].offset=Itype_RS[i].load_rs.validity_src1=Itype_RS[i].load_rs.validity_src2=Itype_RS[i].opertn_typ=Itype_RS[i].ROB_sw=0;
	
	}
}

void new_pattern(st_PHT *PH_table, int pc_BEQ, int Decisive_bit)
{
	int checkfor = (pc_BEQ & 6);
	if(Decisive_bit){
	
	if(PH_table->state[checkfor]==TAKEN ||PH_table->state[checkfor]==W_TAKEN)
	{
	PH_table->state[checkfor] = TAKEN;
	}
	else if(PH_table->state[checkfor]==W_NOTTAKEN)
            PH_table->state[checkfor]=W_TAKEN;
        else
            PH_table->state[checkfor]=W_NOTTAKEN;
	}else{
	
	if(PH_table->state[checkfor]==NOTTAKEN ||PH_table->state[checkfor]==W_NOTTAKEN)
            PH_table->state[checkfor]=NOTTAKEN;
        else if(PH_table->state[checkfor]==W_TAKEN)
           PH_table->state[checkfor]=W_NOTTAKEN;
        else
          PH_table->state[checkfor]=W_TAKEN;
		
	}
}


int checkfor_decision (st_PHT *PH_table, int pc_BEQ)
{
	int checkfor = (pc_BEQ & 6);
	if (PH_table->state[checkfor]== W_NOTTAKEN)
		return 0;
	else if (PH_table->state[checkfor] == NOTTAKEN)
		return 0;
	return 1;
}

int check_BTB(str_BTB* bb, int pc_BEQ){
	int x=0;
	for(x=bb->front; x != bb->rear; x=(x+1)%BUFFERSIZE)
	{
		if(bb->elements[x].pc == pc_BEQ)
		{
				return bb->elements[x].pc_target;
		}
	}
	return -1;
}
void insertinto_branchtargetbuffer(str_BTB *bb, int pc_BEQ, int target_address)
{
	int x;
	BTR_row b;
	b.pc = pc_BEQ;
	b.pc_target = target_address;
	for(x=bb->front; x != bb->rear; x=(x+1)%BUFFERSIZE){
		if(bb->elements[x].pc == pc_BEQ){
			bb->elements[x].pc_target = target_address;
			return;
		}
	}
	
	if(bb->front == (bb->rear+1)%BUFFERSIZE)
	{

	if(bb->front == bb->rear)
		return;
	else
		bb->front = (bb->front + 1)%BUFFERSIZE;
		 
	}
	bb->elements[bb->rear] = b;
	bb->rear = (bb->rear+1)%BUFFERSIZE;
}

str_BTB BTBstruct;
st_PHT PHTstruct;


int checkRS_isfree(int x)
{
	switch(opcode(x))		
	{
	case ADD 	: return new_Rbit; 
	case MULT 	: return new_Mbit;
	case NAND 	: return new_Rbit;
	case LW 	: return new_Ibit;
	case SW 	: return new_Ibit;
	case BEQ 	: return new_Rbit;
	case HALT	: return TAKEN;
	case NOOP 	: return TAKEN;	
	default 	: return TAKEN; 
	}	
}

char op_type(int x)
{
switch(opcode(x))
{
	case ADD 	: return 'R'; 
	case MULT 	: return 'M';
	case NAND 	: return 'R';
	case LW 	: return 'I';
	case SW 	: return 'I';
	case BEQ 	: return 'R';
	case HALT	: return 'H';
	case NOOP 	: return 'N';	 
}


}

int required_RS(int x)
{
	if(op_type(x)=='R')
		return 1;
	else if (op_type(x)=='M')
		return 2;
	else if (op_type(x)=='I')
		return 3;
	else if (op_type(x)=='H')
		return 4;
	else if (op_type(x)=='N')
		return 5;	
}

void broadcast(int sourceid,int sourcevalue)
{
i=0;
	while(i<3)
	{  
	
		if(new_Rtype_RS[i].add_rs.idn_src1==sourceid)
		{
			if(new_Rtype_RS[i].add_rs.validity_src1==0)
			{
			new_Rtype_RS[i].add_rs.value_at_src1=sourcevalue;
			new_Rtype_RS[i].add_rs.validity_src1=1;
			}
		}
		
		if(new_Mtype_RS[i].mul_rs.idn_src1==sourceid)
		{
			if(new_Mtype_RS[i].mul_rs.validity_src1==0)
			{
			new_Mtype_RS[i].mul_rs.value_at_src1=sourcevalue;
			new_Mtype_RS[i].mul_rs.validity_src1=1;
			}
		}
		
		if(new_Itype_RS[i].load_rs.idn_src1==sourceid) 
		{ 
			if( new_Itype_RS[i].load_rs.validity_src1==0)
			{
			new_Itype_RS[i].load_rs.value_at_src1=sourcevalue;
			new_Itype_RS[i].load_rs.validity_src1=1;
			}
		}
		
		
		
		
		if(new_Rtype_RS[i].add_rs.idn_src2==sourceid )
		{
			if( new_Rtype_RS[i].add_rs.validity_src2==0)
			{
			new_Rtype_RS[i].add_rs.value_at_src2=sourcevalue;
			new_Rtype_RS[i].add_rs.validity_src2=1;
			}
		}
		
		
		if(new_Mtype_RS[i].mul_rs.idn_src2==sourceid)
		{
			if( new_Mtype_RS[i].mul_rs.validity_src2==0)
			{
			new_Mtype_RS[i].mul_rs.value_at_src2=sourcevalue;
			new_Mtype_RS[i].mul_rs.validity_src2=1;
			}
		}
		
		
		if(new_Itype_RS[i].load_rs.idn_src2==sourceid )
		{	if( new_Itype_RS[i].load_rs.validity_src2==0)
			{
			new_Itype_RS[i].load_rs.value_at_src2=sourcevalue;
			new_Itype_RS[i].load_rs.validity_src2=1;
			}
		}
		i++;
	} 
	
}

void remove_addRS(int x)
{
	if(x==0)
	{
		new_Rtype_RS[0]=new_Rtype_RS[1];
		new_Rtype_RS[1]=new_Rtype_RS[2];
	}
	if(x==1)
	{
		new_Rtype_RS[1]=new_Rtype_RS[2];
	}
				new_Rtype_RS[2].add_rs.idn_src1=0;
				new_Rtype_RS[2].add_rs.idn_src2=0;
				new_Rtype_RS[2].add_rs.idn_destn=0;
				new_Rtype_RS[2].add_rs.value_at_src1=0;
				new_Rtype_RS[2].add_rs.value_at_src2=0;
				new_Rtype_RS[2].add_rs.validity_src1=0;
				new_Rtype_RS[2].add_rs.validity_src2=0;
				new_Rtype_RS[2].opertn_typ=0;
				new_Rtype_RS[2].BEQPC=0;
				new_Rtype_RS[2].ROB_beq=0;
}


void remove_multRS(int x)
{
	if(x==0)
	{
		new_Mtype_RS[0]=new_Mtype_RS[1];
		new_Mtype_RS[1]=new_Mtype_RS[2];
	}
	 if(x==1)
	{
		new_Mtype_RS[1]=new_Mtype_RS[2];
	}
			new_Mtype_RS[2].mul_rs.idn_src1=0;
			new_Mtype_RS[2].mul_rs.idn_src2=0;
			new_Mtype_RS[2].mul_rs.idn_destn=0;
			new_Mtype_RS[2].mul_rs.value_at_src1=0;
			new_Mtype_RS[2].mul_rs.value_at_src2=0;
			new_Mtype_RS[2].mul_rs.validity_src1=0;
			new_Mtype_RS[2].mul_rs.validity_src2=0;
			new_Mtype_RS[2].bit_fu=0;
			new_Mbit--;
			new_multFU[0]=new_multFU[1];
			new_multFU[1]=new_multFU[2];
			new_multFU[2].isfree_fu=1;
}	

void remove_loadRS(int x)
{
	if(x==0)
	{
		new_Itype_RS[0]=new_Itype_RS[1];
		new_Itype_RS[1]=new_Itype_RS[2];
	}
	if(x==1)
	{
		new_Itype_RS[1]=new_Itype_RS[2];
	}
			new_Itype_RS[2].load_rs.idn_src1=0;
			new_Itype_RS[2].load_rs.idn_src2=0;
			new_Itype_RS[2].load_rs.idn_destn=0;
			new_Itype_RS[2].load_rs.value_at_src1=0;
			new_Itype_RS[2].load_rs.value_at_src2=0;
			new_Itype_RS[2].offset=0;
			new_Itype_RS[2].load_rs.validity_src1=0;
			new_Itype_RS[2].load_rs.validity_src2=0;
			new_Itype_RS[2].opertn_typ=0;
			new_Itype_RS[2].ROB_sw=0;
}


int allocation(int w)
{	
	int x;
		pick=required_RS(instruction_memory[w]); 
		if(pick==4){
			addto_ROB(&new_robQ,0,0,0,-1,0,0);
			new_intr_noop=1;x=1;
		}	
		else if(pick==5){
			addto_ROB(&new_robQ,0,0,0,1,0,0);
			x=1;
		}
		else if(pick==1){
			if(new_Rbit<3){
		if(opcode(instruction_memory[w])!=BEQ){
			addto_ROB(&new_robQ,field2(instruction_memory[w]),new_renametable[field2(instruction_memory[w])].ren_idn,0,0,0,0);
			new_Rtype_RS[new_Rbit].add_rs.idn_destn=new_renametable[field2(instruction_memory[w])].ren_idn;										
		}
		else{
			N=new_robQ.rear;
			addto_ROB(&new_robQ,0,0,field2(instruction_memory[w]),0,w,0);
			new_Rtype_RS[new_Rbit].BEQPC=w;
		}
		if(renametable[field0(instruction_memory[w])].valid_bit==0){
			new_Rtype_RS[new_Rbit].add_rs.idn_src1=field0(instruction_memory[w]);
			new_Rtype_RS[new_Rbit].add_rs.value_at_src1=registers[field0(instruction_memory[w])];
			new_Rtype_RS[new_Rbit].add_rs.validity_src1=1;
		}
		else{
			new_Rtype_RS[new_Rbit].add_rs.idn_src1=renametable[field0(instruction_memory[w])].ren_idn;
			i=new_robQ.front;
			while(new_robQ.item[i].ren_idn!=new_Rtype_RS[new_Rbit].add_rs.idn_src1)
				i++;
			if(new_robQ.item[i].validity==1){
				new_Rtype_RS[new_Rbit].add_rs.value_at_src1=new_robQ.item[i].dest_value;
				new_Rtype_RS[new_Rbit].add_rs.validity_src1=1;
			}
			else
				new_Rtype_RS[new_Rbit].add_rs.validity_src1=0;
		}
		if(renametable[field1(instruction_memory[w])].valid_bit==0){
			new_Rtype_RS[new_Rbit].add_rs.idn_src2=field1(instruction_memory[w]);
			new_Rtype_RS[new_Rbit].add_rs.value_at_src2=registers[field1(instruction_memory[w])];
			new_Rtype_RS[new_Rbit].add_rs.validity_src2=1;
		}
		else{
			new_Rtype_RS[new_Rbit].add_rs.idn_src2=renametable[field1(instruction_memory[w])].ren_idn;
			i=new_robQ.front;
			while(new_robQ.item[i].ren_idn!=new_Rtype_RS[new_Rbit].add_rs.idn_src2)
				i++;
			if(new_robQ.item[i].validity==1){
				new_Rtype_RS[new_Rbit].add_rs.value_at_src2=new_robQ.item[i].dest_value;
				new_Rtype_RS[new_Rbit].add_rs.validity_src2=1;
			}
			else
				new_Rtype_RS[new_Rbit].add_rs.validity_src2=0;
		}
		new_Rtype_RS[new_Rbit].opertn_typ=opcode(instruction_memory[w]);

		if(opcode(instruction_memory[w])!=BEQ)
			new_Rtype_RS[new_Rbit].add_rs.idn_destn=new_renametable[field2(instruction_memory[w])].ren_idn;		
		if(opcode(instruction_memory[w])==BEQ)
			new_Rtype_RS[new_Rbit].ROB_beq=N;
		new_Rbit++;
		for (i=0; i<NUMREGS; i++) 
		{
	    renametable[i].ren_idn=new_renametable[i].ren_idn;
	    renametable[i].valid_bit=new_renametable[i].valid_bit;
	    }
		x= 1;		
	}
	else
		x=0;

		}
		else if(pick==2)
		{
			if(new_Mbit<3){
		if(new_renametable[field0(instruction_memory[w])].valid_bit==0){
			new_Mtype_RS[new_Mbit].mul_rs.idn_src1=field0(instruction_memory[w]);
			new_Mtype_RS[new_Mbit].mul_rs.value_at_src1=registers[field0(instruction_memory[w])];
			new_Mtype_RS[new_Mbit].mul_rs.validity_src1=1;
		}
		else{
			new_Mtype_RS[new_Mbit].mul_rs.idn_src1=renametable[field0(instruction_memory[w])].ren_idn;
			i=new_robQ.front;
			while(new_robQ.item[i].ren_idn!=new_Mtype_RS[new_Mbit].mul_rs.idn_src1)
				i++;
			if(new_robQ.item[i].validity==1){
				new_Mtype_RS[new_Mbit].mul_rs.value_at_src1=new_robQ.item[i].dest_value;
				new_Mtype_RS[new_Mbit].mul_rs.validity_src1=1;
			}
			else
				new_Mtype_RS[new_Mbit].mul_rs.validity_src1=0;
		}
		if(new_renametable[field1(instruction_memory[w])].valid_bit==0){
			new_Mtype_RS[new_Mbit].mul_rs.idn_src2=field1(instruction_memory[w]);
			new_Mtype_RS[new_Mbit].mul_rs.value_at_src2=registers[field1(instruction_memory[w])];
			new_Mtype_RS[new_Mbit].mul_rs.validity_src2=1;
		}
		else{
			new_Mtype_RS[new_Mbit].mul_rs.idn_src2=renametable[field1(instruction_memory[w])].ren_idn;
			i=new_robQ.front;
			while(new_robQ.item[i].ren_idn!=new_Mtype_RS[new_Mbit].mul_rs.idn_src2)
				i++;
			if(new_robQ.item[i].validity==1){
				new_Mtype_RS[new_Mbit].mul_rs.value_at_src2=new_robQ.item[i].dest_value;
				new_Mtype_RS[new_Mbit].mul_rs.validity_src2=1;
			}
			else
				new_Mtype_RS[new_Mbit].mul_rs.validity_src2=0;
		}
		new_Mtype_RS[new_Mbit].mul_rs.idn_destn=new_renametable[field2(instruction_memory[w])].ren_idn;
		addto_ROB(&new_robQ,field2(instruction_memory[w]),new_renametable[field2(instruction_memory[w])].ren_idn,0,0,0,0);
		new_Mbit++;
		for (i=0; i<NUMREGS; i++) 
		{
	    renametable[i].ren_idn=new_renametable[i].ren_idn;
	    renametable[i].valid_bit=new_renametable[i].valid_bit;
	    }
		x=1;
	}
	else{
	x=0;
}	
		}
		else if(pick==3){

		if(new_Ibit<3){
		if(new_renametable[field0(instruction_memory[w])].valid_bit==0){
			new_Itype_RS[new_Ibit].load_rs.idn_src1=field0(instruction_memory[w]);
			new_Itype_RS[new_Ibit].load_rs.value_at_src1=registers[field0(instruction_memory[w])];
			new_Itype_RS[new_Ibit].load_rs.validity_src1=1;
		}
		else{
			new_Itype_RS[new_Ibit].load_rs.idn_src1=renametable[field0(instruction_memory[w])].ren_idn;
			i=new_robQ.front;
			while(new_robQ.item[i].ren_idn!=new_Itype_RS[new_Ibit].load_rs.idn_src1)
				i++;
			if(new_robQ.item[i].validity==1){
				new_Itype_RS[new_Ibit].load_rs.value_at_src1=new_robQ.item[i].dest_value;
				new_Itype_RS[new_Ibit].load_rs.validity_src1=1;
			}
			else
				new_Itype_RS[new_Ibit].load_rs.validity_src1=0;
		}
		if(opcode(instruction_memory[w])==LW){
			new_Itype_RS[new_Ibit].opertn_typ=LW;
			new_Itype_RS[new_Ibit].load_rs.validity_src2=1;
			new_Itype_RS[new_Ibit].load_rs.idn_destn=new_renametable[field1(instruction_memory[w])].ren_idn;
			addto_ROB(&new_robQ,field1(instruction_memory[w]),new_renametable[field1(instruction_memory[w])].ren_idn,0,0,0,0);
		}
		else{
			if(new_renametable[field1(instruction_memory[w])].valid_bit==0){
				new_Itype_RS[new_Ibit].load_rs.idn_src2=field1(instruction_memory[w]);
				new_Itype_RS[new_Ibit].load_rs.value_at_src2=registers[field1(instruction_memory[w])];
				new_Itype_RS[new_Ibit].load_rs.validity_src2=1;
			}
			else{
				new_Itype_RS[new_Ibit].load_rs.idn_src2=renametable[field1(instruction_memory[w])].ren_idn;
				i=new_robQ.front;
				while(new_robQ.item[i].ren_idn!=new_Itype_RS[new_Ibit].load_rs.idn_src2)
					i++;
				if(new_robQ.item[i].validity==1)
				{
					new_Itype_RS[new_Ibit].load_rs.value_at_src2=new_robQ.item[i].dest_value;
					new_Itype_RS[new_Ibit].load_rs.validity_src2=1;
				}
				else
					new_Itype_RS[new_Ibit].load_rs.validity_src2=0;
			}
			new_Itype_RS[new_Ibit].opertn_typ=SW;
			N=new_robQ.rear;
			new_Itype_RS[new_Ibit].load_rs.idn_destn=N;
			addto_ROB(&new_robQ,0,0,0,0,0,0);
		}
		new_Itype_RS[new_Ibit].offset=field2(instruction_memory[w]);
		new_Ibit++;
		for (i=0; i<NUMREGS; i++) 
		{
	    renametable[i].ren_idn=new_renametable[i].ren_idn;
	    renametable[i].valid_bit=new_renametable[i].valid_bit;
	    }
	    x=1;		
	}
	else{
	x=0;
}
		}
		
		else
			x=1;
		if(x==1)
			return 1;

	return 0;
}

int main(int argc, char* argv[])
{
	int data;
	FILE* fp = fopen(argv[1], "r");		
	if(fp == NULL)
		{
		fprintf(stderr, "%s Unsupported ",argv[1]);
		exit(1);
		}
	while(fscanf(fp, "%d",&data)!= EOF)
	{
		if(num_mem>= NUMMEMORY){
			fprintf(stderr, "File exceeds maximum size");
			exit(1);
		}
		instruction_memory[num_mem] = data;
		data_memory[num_mem] = data;
		num_mem++;
	}
		
	int brnch_dcsn;
	 BTBstruct.front = BTBstruct.rear = 0;
	
	 
	for(i=0; i<63; i++)
	PHTstruct.state[i] = W_NOTTAKEN; 
	
		initialize_ROB(&robQ);
		initializeRS();
	
		for(i=0;i<8;i++)
		{
			new_renametable[i].valid_bit=0;
		}  
		new_Rbit=0;
		new_Mbit=0;
		new_Ibit=0;
		new_fetch1_only=0;
		new_haltfetch=0;
		new_ren_idn=1;
		new_intr1_fetch=1;
		new_intr2_fetch=1;
		new_intr_noop=0;
		new_cycles=0;
		new_pcplus1=0;
		new_addFU.isfree_fu=1;
		new_loadFU.isfree_fu=1;
		
		for(j=0;j<3;j++)
		{
			new_multFU[j].isfree_fu=1;
		}
		while(i<NUMREGS)
		{
			registers[i]=0;
			i++;
		}
	
	
	int N;   
	
	while(1)
	{
			assign_oldtonew();

			printf("PCPLS!---------- %d\n",new_pcplus1 );

			printf("\n\n****************ROB at the rear of Cycle : %d **************\n\n",new_cycles);
			
			if	(robQ.front==robQ.rear)
				printf("\n\nROB is EMPTY  \n\n");
			else{
				N=0;
				for(i=robQ.front;i<robQ.rear;i=(i+1)%BUFFERSIZE)
				{
					printf("ROB field: %d\n",N);
					printf("\tArch.Register:%d\n",robQ.item[i].acc_regfile_idn);
					printf("\tValue        :%d\n",robQ.item[i].dest_value);
					printf("\tValid        :%d\n",robQ.item[i].validity);
					printf("\tRestore      :%d\n\n",robQ.item[i].reset);
					N++;	
				}
			}

			
			
			
			
			
			
			if(new_intr_noop==0 && new_haltfetch==0)
			{
	    		if(new_fetch1_only==0){
	    			new_intr1_fetch=new_pcplus1;
					if (check_BTB(&BTBstruct,new_pcplus1) ==-1)
						new_pcplus1++;
					else{
		        		if(checkfor_decision (&PHTstruct,new_pcplus1) == 1)
					   		new_pcplus1 = check_BTB(&BTBstruct,new_pcplus1);	
						}
					FETCHED++;
				}
				new_intr2_fetch=new_pcplus1;
			
				if (check_BTB(&BTBstruct,new_pcplus1)==-1)
					new_pcplus1++;
				else
				{
					if(checkfor_decision (&PHTstruct,new_pcplus1) == 1)
				    	new_pcplus1 = check_BTB(&BTBstruct,new_pcplus1);	
					
				}
				 FETCHED++;						
			}
			
			

	        if(new_intr_noop==0)
	        {
			
	        	if((checkRS_isfree(instruction_memory[new_intr1_fetch])<=2) && (new_robQ.front==(new_robQ.rear+1)%BUFFERSIZE)!=1)
				{
					if(opcode(instruction_memory[new_intr1_fetch])==ADD || opcode(instruction_memory[new_intr1_fetch])==NAND || opcode(instruction_memory[new_intr1_fetch])==MULT)
					{
					new_renametable[field2(instruction_memory[new_intr1_fetch])].ren_idn=new_ren_idn;
					new_renametable[field2(instruction_memory[new_intr1_fetch])].valid_bit=1;
					new_ren_idn++;
					}
					if(opcode(instruction_memory[new_intr1_fetch])==LW){
						new_renametable[field1(instruction_memory[new_intr1_fetch])].ren_idn=new_ren_idn;
						new_renametable[field1(instruction_memory[new_intr1_fetch])].valid_bit=1;
						new_ren_idn++;
					}
				}
	        	new_intr1_alloc=allocation(new_intr1_fetch);
	        	if(new_intr1_alloc==1)
	        	 {
				
	        	 	if((checkRS_isfree(instruction_memory[new_intr2_fetch])<=2) && (new_robQ.front==(new_robQ.rear+1)%BUFFERSIZE)!=1)
					{
						if(opcode(instruction_memory[new_intr2_fetch])==ADD || opcode(instruction_memory[new_intr2_fetch])==NAND || opcode(instruction_memory[new_intr2_fetch])==MULT)
						{
						new_renametable[field2(instruction_memory[new_intr2_fetch])].ren_idn=new_ren_idn;
						new_renametable[field2(instruction_memory[new_intr2_fetch])].valid_bit=1;
						new_ren_idn++;
						}
						if(opcode(instruction_memory[new_intr2_fetch])==LW){
							new_renametable[field1(instruction_memory[new_intr2_fetch])].ren_idn=new_ren_idn;
							new_renametable[field1(instruction_memory[new_intr2_fetch])].valid_bit=1;
							new_ren_idn++;
						}
					}
	        		new_intr2_alloc=allocation(new_intr2_fetch);
	        	 }
	        }
	        if(new_intr2_alloc==0 && new_intr1_alloc==0)
				new_haltfetch=1;
			else if(new_intr2_alloc==0 && new_intr1_alloc==1)
			{
				new_intr1_fetch=new_intr2_fetch;
				new_haltfetch=0;
				new_fetch1_only=1;
			}
			else if(new_intr2_alloc==1 && new_intr1_alloc==0){
				new_haltfetch=0;
				new_fetch1_only=1;
			}
			else{
				new_haltfetch=0;
				new_fetch1_only=0;
			}

		if(new_addFU.isfree_fu==1){
			for(i=0;i<3;i++){
				if((Rtype_RS[i].add_rs.validity_src1 && Rtype_RS[i].add_rs.validity_src2)==1 && (Rtype_RS[i].opertn_typ==BEQ))
					{
						new_addFU.begin_exec=new_cycles;
						new_addFU.end_exec=new_addFU.begin_exec+1;
						new_addFU.idn_destn=Rtype_RS[i].add_rs.idn_destn;
						new_addFU.rec=0;
						new_addFU.isfree_fu=0;	
						new_addFU.idn=i;
						break;
					}
			}
		}
		if(new_addFU.isfree_fu==1){
			for(i=0;i<3;i++){
				if((Rtype_RS[i].add_rs.validity_src1 && Rtype_RS[i].add_rs.validity_src2)==1)
					{
						new_addFU.begin_exec=new_cycles;
						new_addFU.end_exec=new_addFU.begin_exec+1;
						new_addFU.idn_destn=Rtype_RS[i].add_rs.idn_destn;
						if(Rtype_RS[j].opertn_typ==ADD)
							new_addFU.rec=Rtype_RS[i].add_rs.value_at_src1+Rtype_RS[i].add_rs.value_at_src2;
						if(Rtype_RS[i].opertn_typ==NAND)
							new_addFU.rec=!(Rtype_RS[i].add_rs.value_at_src1 & Rtype_RS[i].add_rs.value_at_src2);

						new_addFU.isfree_fu=0;	
						new_addFU.idn=i;
						break;
					}
			}
		}
		
		
		if(new_loadFU.isfree_fu==1)
		{
			for(i=0;i<3;i++)
			{
				if(Itype_RS[i].load_rs.validity_src1 && Itype_RS[i].load_rs.validity_src2==1)
				{
				new_loadFU.begin_exec=new_cycles;
				new_loadFU.end_exec=new_cycles+3;
				new_loadFU.idn=i;
				new_loadFU.isfree_fu=0;
				new_loadFU.rec=Itype_RS[i].load_rs.value_at_src1+Itype_RS[i].offset;
				new_loadFU.idn_destn=Itype_RS[i].load_rs.idn_destn;
				
				
				break;
				}
				
			}
		}
		i=j=N=0;
		
		while(new_multFU[N].isfree_fu==0 && N<3)
			{
				N++;
			}
	if(N<3)
	{
		for(i=0;i<3;i++)
		{
			if((Mtype_RS[i].mul_rs.validity_src1 && Mtype_RS[i].mul_rs.validity_src2==1) && (Mtype_RS[i].bit_fu==0))
			{
				if(N==0)
					new_multFU[N].begin_exec=new_cycles;
				else
				{
					if(new_multFU[N-1].begin_exec+2>new_cycles)
						new_multFU[N].begin_exec=new_multFU[N-1].begin_exec+2;
					else
						new_multFU[N].begin_exec=new_cycles;
				}
				new_multFU[N].end_exec=new_multFU[N].begin_exec+6;
				new_multFU[N].idn_destn=Mtype_RS[i].mul_rs.idn_destn;
				new_Mtype_RS[i].bit_fu=1;
				new_multFU[N].isfree_fu=0;
				new_multFU[N].idn=i;
				new_multFU[N].rec=Mtype_RS[i].mul_rs.value_at_src1*Mtype_RS[i].mul_rs.value_at_src2;
			
				break;
			}
			}
		}
		if(new_addFU.isfree_fu==0 && new_addFU.end_exec==new_cycles)
		{
			if(Rtype_RS[new_addFU.idn].opertn_typ==ADD)
			{
				new_robQ.item[idn_ROB(new_addFU.idn_destn)].dest_value=new_addFU.rec;
				broadcast(new_addFU.idn_destn,new_addFU.rec);
				new_robQ.item[idn_ROB(new_addFU.idn_destn)].validity=1;
			}
			else if (Rtype_RS[new_addFU.idn].opertn_typ==NAND)
			{
				new_robQ.item[idn_ROB(new_addFU.idn_destn)].dest_value=new_addFU.rec;
				broadcast(new_addFU.idn_destn,new_addFU.rec);
				new_robQ.item[idn_ROB(new_addFU.idn_destn)].validity=1;
			}
			else
			{
				BranchTarget=new_Rtype_RS[new_addFU.idn].BEQPC+1+(signed short int)field2(instruction_memory[new_Rtype_RS[new_addFU.idn].BEQPC]);
				printf("\n ************ %d %d\n",new_Rtype_RS[new_addFU.idn].BEQPC,(signed short int)field2(instruction_memory[new_Rtype_RS[new_addFU.idn].BEQPC]) );
				
				if(Rtype_RS[new_addFU.idn].add_rs.value_at_src1==Rtype_RS[new_addFU.idn].add_rs.value_at_src2)
					brnch_dcsn=1;
				else
					brnch_dcsn=0;
				if(brnch_dcsn==1)
				{ 
					if(checkfor_decision (&PHTstruct,new_Rtype_RS[new_addFU.idn].BEQPC) == 1)
					{
						if(BranchTarget!= check_BTB(&BTBstruct,new_Rtype_RS[new_addFU.idn].BEQPC))
						{
							new_pcplus1=new_Rtype_RS[new_addFU.idn].BEQPC+1;
							new_robQ.item[new_Rtype_RS[new_addFU.idn].ROB_beq].reset=1;
						}
						insertinto_branchtargetbuffer(&BTBstruct,new_Rtype_RS[new_addFU.idn].BEQPC,BranchTarget);
					}
        	    
					else 
					{
        		    MISPRED++;
        		    new_pcplus1=BranchTarget;
        		    
        		    new_robQ.item[new_Rtype_RS[new_addFU.idn].ROB_beq].reset=1;
        		    insertinto_branchtargetbuffer(&BTBstruct,new_Rtype_RS[new_addFU.idn].BEQPC,BranchTarget);
					}
        		}
				else 
				{
					if(checkfor_decision (&PHTstruct,new_robQ.item[idn_ROB(new_addFU.idn_destn)].acc_regfile_idn) == 1)
					{
						MISPRED++;
						new_pcplus1=new_Rtype_RS[new_addFU.idn].BEQPC+1;
						new_robQ.item[new_Rtype_RS[new_addFU.idn].ROB_beq].reset=1;
					}
				}
		        new_pattern(&PHTstruct,new_Rtype_RS[new_addFU.idn].BEQPC, brnch_dcsn);
		        new_robQ.item[new_Rtype_RS[new_addFU.idn].ROB_beq].validity=1;
			}
			remove_addRS(new_addFU.idn);
				
				new_Rbit--;
			new_addFU.isfree_fu=1;
		}
		if(new_loadFU.isfree_fu==0 && new_loadFU.end_exec==new_cycles)
		{
			if(Itype_RS[new_loadFU.idn].opertn_typ==SW)
			{
				data_memory[new_loadFU.rec]=Itype_RS[new_loadFU.idn].load_rs.value_at_src2;	
				new_robQ.item[new_loadFU.idn_destn].validity=1;
			}
			else
			{
				new_robQ.item[idn_ROB(new_loadFU.idn_destn)].dest_value=data_memory[new_loadFU.rec];
				broadcast(new_loadFU.idn_destn,new_robQ.item[idn_ROB(new_loadFU.idn_destn)].dest_value);
				new_robQ.item[idn_ROB(new_loadFU.idn_destn)].validity=1;
			}
			remove_loadRS(new_loadFU.idn);	
			
			new_Ibit--;
			new_loadFU.isfree_fu=1;
		}
		if (new_multFU[0].isfree_fu==0 && new_multFU[0].end_exec==new_cycles){
			new_robQ.item[idn_ROB(new_multFU[0].idn_destn)].dest_value=new_multFU[0].rec;
			new_robQ.item[idn_ROB(new_multFU[0].idn_destn)].validity=1;
			broadcast(new_multFU[0].idn_destn,new_multFU[0].rec);
			remove_multRS(new_multFU[0].idn);	
			
		}
	int a;	
	for(a=0;a<2;a++)
	{
		if(robQ.item[robQ.front].validity==-1)
		{
			RETIRED++;
			printf("\nCycles: %d\nFETCHED: %d\nRETIRED: %d\nBRANCHES:%d\nMISPRED:%d\n",new_cycles,FETCHED,RETIRED,BRANCHES,MISPRED);
	
			exit(1);
			
		}
		if(robQ.item[robQ.front].validity==1)
		{
			if(robQ.item[robQ.front].reset==1)
				{

				new_initializeRS();
				for(i=0;i<8;i++)
				{
					new_renametable[i].valid_bit=0;
				}
				for(i=robQ.front;i<robQ.rear;i++)
				{
					remove_ROB(&robQ);
				}
				for(i=new_robQ.front;i<new_robQ.rear;i++)
				{
					remove_ROB(&new_robQ);
				}
				new_Rbit=new_Mbit=new_Ibit=0;
				new_intr2_alloc=1;
				new_intr1_alloc=1;
				new_intr_noop=0;
				new_addFU.isfree_fu=1;
				new_loadFU.isfree_fu=1;
				BRANCHES++;
				RETIRED++;
				new_intr1_fetch=new_pcplus1;
				}
				
			else if(robQ.item[robQ.front].bit_beq==0)
			{
				registers[robQ.item[robQ.front].acc_regfile_idn]=robQ.item[robQ.front].dest_value;
				if(new_renametable[robQ.item[robQ.front].acc_regfile_idn].ren_idn==robQ.item[robQ.front].ren_idn)
					new_renametable[robQ.item[robQ.front].acc_regfile_idn].valid_bit=0;
				remove_ROB(&robQ);
				remove_ROB(&new_robQ);
				RETIRED++;
			}
			else
			{
				remove_ROB(&robQ);
				remove_ROB(&new_robQ);
				RETIRED++;
				BRANCHES++;
			}
		}		
	}
		
		new_cycles++;
		assign_newtoold();
	}
	return 0;
}
	


    