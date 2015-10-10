/* assignment 4 */
/* pipeline simulator for LC-S4 */ 

#include <stdio.h> 
#include <string.h> 

#define NUMMEMORY 65536 /* maximum number of words in memory */ 
#define NUMREGS 8 /* number of machine registers */ 
#define MAXLINELENGTH 1000 

#define ADD 1
#define AND 2
#define NOR 3
#define LW 17
#define SW 18
#define BNE 19
#define HALT 49
#define NOOP 50

#define NOOPINSTRUCTION 0x32000000

#define DEBUG = 1;

typedef struct IFIDStruct {
  int instr;
  int pcPlus1;
} IFIDType;

typedef struct IDEXStruct {
  int instr;
  int pcPlus1;
  int readRegA;
  int readRegB;
  int offset;
} IDEXType;

typedef struct EXMEMStruct {
  int instr;
  int branchTarget;
  int aluResult;
  int readRegB;
} EXMEMType;

typedef struct MEMWBStruct {
  int instr;
  int writeData;
} MEMWBType;

typedef struct WBENDStruct {
  int instr;
  int writeData;
} WBENDType;

typedef struct stateStruct {
  int pc;
  int instrMem[NUMMEMORY];
  int dataMem[NUMMEMORY];
  int reg[NUMREGS];
  int numMemory;
  IFIDType IFID;
  IDEXType IDEX;
  EXMEMType EXMEM;
  MEMWBType MEMWB;
  WBENDType WBEND;
  int cycles; /* number of cycles run so far */
} stateType;

void printState(stateType *); 

int 
main(int argc, char *argv[]) 
{ 
  char line[MAXLINELENGTH]; 
  stateType state;

  FILE *filePtr; 

  if (argc != 2) { 
    printf("error: usage: %s <machine-code file>\n", argv[0]); 
    exit(1); 
  } 

  filePtr = fopen(argv[1], "r"); 
  if (filePtr == NULL) { 
    printf("error: can't open file %s", argv[1]); 
    perror("fopen"); 
    exit(1); 
  } 

  /* read in the entire machine-code file into memory */ 
  for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL; state.numMemory++) 
  { 
    if (sscanf(line, "%d", state.instrMem+state.numMemory) != 1) 
    { 
      printf("error in reading address %d\n", state.numMemory); 
      exit(1); 
    } 
 
    if (sscanf(line, "%d", state.dataMem+state.numMemory) != 1) 
    { 
      printf("error in reading address %d\n", state.numMemory); 
      exit(1); 
    } 
    //printf("dataMem[%d]=%d\n", state.numMemory, state.dataMem[state.numMemory]); 
  } 

  int x;

  //sets all registers to 0 and instructs to noop
  for (x=0; x<NUMREGS; x++) 
    state.reg[x] =0;

    state.cycles = 0;
    state.IFID.instr = NOOPINSTRUCTION;
    state.IDEX.instr = NOOPINSTRUCTION;
    state.EXMEM.instr = NOOPINSTRUCTION;
    state.MEMWB.instr = NOOPINSTRUCTION;
    state.WBEND.instr = NOOPINSTRUCTION;

  //prints initial memory states to screen
  for(x = 0; x < state.numMemory; x++)
  {
     printf("memory[%d]=%d\n", x, state.instrMem[x]); 
  }
  
  printf("%d memory words\n", state.numMemory);
  printf("\tinstruction memory:\n");

  //prints instructions to screen
  for(x = 0; x < state.numMemory; x++)
  {
    printf("\t\tdataMem[ %d ] ", x);
    printInstruction(state.instrMem[x]);
  }

  state.pc = 0;

  run(state); //runs pipeline simulator
  return(0); 
}//main()

run(stateType state)
{
  stateType newState;

  int regA, regB, regC, offset, destReg, destMem, address, instructCount;
  address = -1;

  while(1)
  {
    printState(&state);

    //int pause;
    //scanf("%d", &pause);

    //debug for infinite loops
    
    if(state.cycles>31)
    {
      printf("Probably infinite loop.  Exitting\n");
      exit(1);
    }
    
    /* check for halt */
    if (opcode(state.MEMWB.instr) == HALT) 
    {
      printf("machine halted\n");
      printf("total of %d cycles executed\n", state.cycles);
      exit(0);
    }

    newState = state;
    newState.cycles++;
    
    /* --------------------- IF stage --------------------- */
    //gets instruction from PC and increments PC for next instr
    newState.IFID.instr = state.instrMem[state.pc];
    newState.pc = newState.IFID.pcPlus1 = state.pc + 1;
    //state.IFID.pcPlus1 = state.pc + 1;
    /* --------------------- ID stage --------------------- */
    //gets registers and offsets, and passes incremented PC
    newState.IDEX.instr = state.IFID.instr;
    newState.IDEX.readRegA = state.reg[field0(state.IFID.instr)];
    newState.IDEX.readRegB = state.reg[field1(state.IFID.instr)];
    newState.IDEX.offset = convertNum(field2(state.IFID.instr));
    newState.IDEX.pcPlus1 = state.IFID.pcPlus1;

    //check for forwarding?
    if(opcode(state.IDEX.instr) == LW)
    {
      if(field1(state.IDEX.instr) == field1(state.IFID.instr) ||
         field1(state.IDEX.instr) == field0(state.IFID.instr)) 
        {
          newState.IDEX.instr = NOOPINSTRUCTION;
          newState.IFID.instr = state.IFID.instr;
          newState.pc--;
          newState.IFID.pcPlus1--;
        }
    }     
    
    /* --------------------- EX stage --------------------- */
    //gets regA
    if(field0(state.IDEX.instr)==field2(state.EXMEM.instr)&&opcode(state.EXMEM.instr)<=3)
      regA = state.EXMEM.aluResult;
    else if((field0(state.IDEX.instr)==field1(state.MEMWB.instr)&&opcode(state.MEMWB.instr)==LW) 
         || (field0(state.IDEX.instr)==field2(state.MEMWB.instr)&&opcode(state.MEMWB.instr)<=3))
      regA = state.MEMWB.writeData;
    else if((field0(state.IDEX.instr)==field1(state.WBEND.instr)&&opcode(state.WBEND.instr)==LW)
         || (field0(state.IDEX.instr)==field2(state.WBEND.instr)&&opcode(state.WBEND.instr)<=3))
      regA = state.WBEND.writeData;
    else //no forwarding
      regA = state.IDEX.readRegA;

    //gets regB
    if(field1(state.IDEX.instr)==field2(state.EXMEM.instr)&&opcode(state.EXMEM.instr)<=3)
      regB = state.EXMEM.aluResult;
    else if((field1(state.IDEX.instr)==field1(state.MEMWB.instr)&&opcode(state.MEMWB.instr)==LW) 
         || (field1(state.IDEX.instr)==field2(state.MEMWB.instr)&&opcode(state.MEMWB.instr)<=3))
      regB = state.MEMWB.writeData;
    else if((field1(state.IDEX.instr)==field1(state.WBEND.instr)&&opcode(state.WBEND.instr)==LW)
         || (field1(state.IDEX.instr)==field2(state.WBEND.instr)&&opcode(state.WBEND.instr)<=3))
      regB = state.WBEND.writeData;
    else //no forwarding
      regB = state.IDEX.readRegB;

    if (opcode(state.IDEX.instr) == ADD)
    {
      newState.EXMEM.aluResult = regA + regB;
    }//ADD

    if (opcode(state.IDEX.instr) == AND)
    {
      newState.EXMEM.aluResult = regA & regB;
    }//AND

    if (opcode(state.IDEX.instr) == NOR)
    {
       newState.EXMEM.aluResult = ~(regA | regB);
    }//NOR

    if (opcode(state.IDEX.instr) == LW)
    {
      newState.EXMEM.readRegB = 0;
      newState.EXMEM.branchTarget = state.IDEX.pcPlus1 + state.IDEX.offset;
      newState.EXMEM.aluResult = regA + state.IDEX.offset;
    }

    if (opcode(state.IDEX.instr) == SW)
    {
      newState.EXMEM.readRegB = state.IDEX.readRegB;
      newState.EXMEM.aluResult = regA + state.IDEX.offset;
    }//SW
      
    if (opcode(state.IDEX.instr) == BNE)
    {
      
      newState.EXMEM.aluResult = regA - regB;
    }//BNE

    ///rearrange this
    newState.EXMEM.instr = state.IDEX.instr;
    newState.EXMEM.readRegB = regB;
    newState.EXMEM.branchTarget = (state.IDEX.pcPlus1 + state.IDEX.offset);

    /* --------------------- MEM stage --------------------- */
    newState.MEMWB.instr = state.EXMEM.instr;
    newState.MEMWB.writeData = state.EXMEM.aluResult;

    if (opcode(state.EXMEM.instr) == ADD || opcode(state.EXMEM.instr) == AND)
      newState.MEMWB.writeData = state.EXMEM.aluResult;

    if (opcode(state.EXMEM.instr) == LW)
      newState.MEMWB.writeData = state.dataMem[state.EXMEM.aluResult];
   
    if (opcode(state.EXMEM.instr) == SW)
      newState.dataMem[state.EXMEM.aluResult] = state.EXMEM.readRegB;

    if (opcode(state.EXMEM.instr) == BNE && state.EXMEM.aluResult != 0) 
    {
      newState.pc = state.EXMEM.branchTarget;
      newState.IFID.instr = NOOPINSTRUCTION;
      newState.IDEX.instr = NOOPINSTRUCTION;
      newState.EXMEM.instr = NOOPINSTRUCTION;
    }

    /* --------------------- WB stage --------------------- */
    newState.WBEND.instr = state.MEMWB.instr;
    newState.WBEND.writeData = state.MEMWB.writeData;
    
    if (opcode(state.MEMWB.instr) == ADD || opcode(state.MEMWB.instr) == AND) 
    {
      newState.WBEND.writeData = state.MEMWB.writeData;
      newState.reg[field2(state.MEMWB.instr)] = state.MEMWB.writeData;
    } 
    
    if (opcode(state.MEMWB.instr) == LW) 
    {
      newState.WBEND.writeData = state.MEMWB.writeData;
      newState.reg[field1(state.MEMWB.instr)] = state.MEMWB.writeData;
    }

    state = newState;       /* this is the last statement before end of the loop.
			    It marks the end of the cycle and updates the
			    current state with the values calculated in this
			    cycle */
  }// end while(1)
}//end run()


int
field0(int instruction)
{
  return( (instruction>>4) & 0xF);
}


int
field1(int instruction)
{
  return( (instruction>>0) & 0xF);
}

int
field2(int instruction)
{
  return( (instruction>>8) & 0xFFFF);
}

int
opcode(int instruction)
{
  return( (instruction>>24) & 0x3F);
}

void
printInstruction(int instr)
{
  char opcodeString[10];
  if (opcode(instr) == ADD) {
    strcpy(opcodeString, "add");
  } else if (opcode(instr) == AND) {
    strcpy(opcodeString, "and");
  } else if (opcode(instr) == NOR) {
    strcpy(opcodeString, "nor");
  } else if (opcode(instr) == LW) {
    strcpy(opcodeString, "lw");
  } else if (opcode(instr) == SW) {
    strcpy(opcodeString, "sw");
  } else if (opcode(instr) == BNE) {
    strcpy(opcodeString, "bne");
  } else if (opcode(instr) == HALT) {
    strcpy(opcodeString, "halt");
  } else if (opcode(instr) == NOOP) {
    strcpy(opcodeString, "noop");
  } else {
    strcpy(opcodeString, "data");
  }
  
  printf("%s %d %d %d\n", opcodeString, field0(instr), field1(instr),
	 field2(instr));
}  /* end printInstruction() */

int convertNum(int num)
{
  //convert a 16-bit numeral into a 32-bit integer
  if(num & (1 << 15))
  {
    num -= (1 << 16);
  }

  return(num);
}

void
printState(stateType *statePtr)
{
  int i;
  printf("\n@@@\nstate before cycle %d starts\n", statePtr->cycles);
  printf("\tpc %d\n", statePtr->pc);
  
  printf("\tdata memory:\n");
  for (i=0; i<statePtr->numMemory; i++) {
    printf("\t\tdataMem[ %d ] %d\n", i, statePtr->dataMem[i]);
  }
  printf("\tregisters:\n");
  for (i=0; i<NUMREGS; i++) {
    printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
  }
  printf("\tIFID:\n");
  printf("\t\tinstruction ");
  printInstruction(statePtr->IFID.instr);
  printf("\t\tpcPlus1 %d\n", statePtr->IFID.pcPlus1);
  printf("\tIDEX:\n");
  printf("\t\tinstruction ");
  printInstruction(statePtr->IDEX.instr);
  printf("\t\tpcPlus1 %d\n", statePtr->IDEX.pcPlus1);
  printf("\t\treadRegA %d\n", statePtr->IDEX.readRegA);
  printf("\t\treadRegB %d\n", statePtr->IDEX.readRegB);
  printf("\t\toffset %d\n", statePtr->IDEX.offset);
  printf("\tEXMEM:\n");
  printf("\t\tinstruction ");
  printInstruction(statePtr->EXMEM.instr);
  printf("\t\tbranchTarget %d\n", statePtr->EXMEM.branchTarget);
  printf("\t\taluResult %d\n", statePtr->EXMEM.aluResult);
  printf("\t\treadRegB %d\n", statePtr->EXMEM.readRegB);
  printf("\tMEMWB:\n");
  printf("\t\tinstruction ");
  printInstruction(statePtr->MEMWB.instr);
  printf("\t\twriteData %d\n", statePtr->MEMWB.writeData);
  printf("\tWBEND:\n");
  printf("\t\tinstruction ");
  printInstruction(statePtr->WBEND.instr);
  printf("\t\twriteData %d\n", statePtr->WBEND.writeData);
} 

