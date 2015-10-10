#include <stdio.h>

#define MAXMEMORY 32768	/* maximum number of data words in memory */
#define MAXBLOCKS 64	/* maximum number of data words in a cache */
#define MAXPREDTABLE 16	/* maximum number of entries in a prediction table */
#define NUMREGS 8	/* number of machine registers */
#define NUMPIPES 2	/* number of independent pipelines */
#define IBUFSIZE 4	/* entries in the instruction window */
#define MAXLINELENGTH 1000

#define HI { printf("line %d\n",__LINE__); fflush(stdout); }

#define CACHE_MISS_PENALTY 50

#define OP_SHIFT 13
#define A_SHIFT  10
#define B_SHIFT  7
#define C_SHIFT  0

#define ADD 0
#define ADDI 1
#define NAND 2
#define LUI 3
#define LW 4
#define SW 5
#define BEQ 6
#define JALR 7

#define NOPINSTRUCTION 0x0000
#define HALTINSTRUCTION (short)0xC07F

typedef struct IFIDStruct {
    short instr;
    short imiss;
    short predTaken;
    short branchTaken;
    short predBranchTarget;
    short branchHistoryIndex;
    short patternHistoryIndex;
    short branchTargetIndex;
    short pcPlus1;
    short offset;
    short valid;
} IFID_t;

typedef struct IDEXStruct {
    short instr;
    short imiss;
    short pcPlus1;
    short offset;
    short readRegA;
    short readRegB;
    short branchTaken;
    short branchHistoryIndex;
    short patternHistoryIndex;
    short branchTargetIndex;
    short predBranchTarget;
    short realBranchTarget;
} IDEX_t;

typedef struct EXMEMStruct {
    short instr;
    short imiss;
    short aluResult;
    short readRegB;
} EXMEM_t;

typedef struct MEMWBStruct {
    short instr;
    short imiss;
    short writeData;
} MEMWB_t;

typedef struct WBENDStruct {
    short instr;
    short imiss;
    short writeData;
} WBEND_t;

typedef struct {
    short valid;
    short tag;
    short *datap;
} block_t;

typedef struct {
    short mru;
    block_t *blockp;
} set_t;

typedef struct {
    set_t *setp;
    short numsets;
    short blocksize;
    short assoc;
    short word_mask;		/* this ANDed with address gives word w/in block */
    short log_blocksize;	/* log2 of the blocksize */
} cache_t;

enum bpredtypes {
    NONE,
    ONE_LEVEL,
    TWO_LEVEL,
    STATIC,
    MAX_BPRED_TYPES
};

typedef struct {
    short type;
    short branchHistory[MAXPREDTABLE];
    short bhtSize;
    short patternHistory[MAXPREDTABLE];
    short phtSize;
    short branchTarget[MAXPREDTABLE];
    short btbSize;
} bpredictor_t;

typedef struct stateStruct {
    short pc;
    cache_t iCache;
    cache_t dCache;
    bpredictor_t bPredictor;
    short mainMemory[MAXMEMORY];
    short reg[NUMREGS];
    short numMemory;
    IFID_t IFID		[IBUFSIZE];
    IDEX_t IDEX		[NUMPIPES];
    EXMEM_t EXMEM	[NUMPIPES];
    MEMWB_t MEMWB	[NUMPIPES];
    WBEND_t WBEND	[NUMPIPES];
} state_t;


typedef struct {
    long cycles;
    long instructions;
    long icache_access;
    long icache_miss;
    long dcache_access;
    long dcache_miss;
    long branches;
    long branch_dir_mispredicts;
    long branch_tgt_mispredicts;
    long load_use_interlocks;
    long zero_instructions_issued;
    long one_instructions_issued;
    long two_instructions_issued;
    long zero_instructions_retired;
    long one_instructions_retired;
    long two_instructions_retired;
} stats_t;

state_t state;
state_t new;
stats_t stats;


#define IC_SIZE  (short)256
#define IC_ASSOC (short)2
#define IC_BLOCK (short)8
#define DC_SIZE  (short)256
#define DC_ASSOC (short)2
#define DC_BLOCK (short)8
#define BP_TYPE  (short)TWO_LEVEL
#define BP_SIZE  (short)4
#define BP_TARG  (short)4

/* function declarations */

void init_pipeline();
void cache_fill(cache_t *cache, short addr, short *data, short *mainMemory);
int cache_write(cache_t *cache, short addr, short *data);
int cache_read(cache_t *cache, short addr, short *data);
void print_cache(cache_t *cache);
void init_cache(cache_t *cache, short size, short assoc, short block, char *name);
void init_bpredictor(bpredictor_t *pred, short type, short size, short target);
void print_bpredictor(bpredictor_t *pred);
short predict_btarget(bpredictor_t *pred, short pc, short *btb_idx);
short predict_bdirection(bpredictor_t *pred, short pc, short instr, short *bht_idx, short *pht_idx);
void update_bpredictor(bpredictor_t *pred, short bht_idx, short pht_idx, short btb_idx, short direction, short target);
void printState(state_t *state);
void printStats(stats_t *stats);
short opcode(short instruction);
short field_ra(short instruction);
short field_rb(short instruction);
short field_rc(short instruction);
short field_imm(short instruction);
short offset(short instruction);
short target(short instr);
short source1(short instr);
short source2(short instr);
char *printInstruction(short instr);
void print_rtype(short instr, char *name);
void print_itype(short instr, char *name);
short mem_type(short instruction);
short arith_type(short instruction);
short convertNum(short num);
void run();




main(int argc, char *argv[])
{
    short i, j;
    short statusNum;
    char line[MAXLINELENGTH];
    short lowMem, highMem;
    FILE *filePtr;

    if (argc != 2) {
	printf("error: usage: %s <machine-code file>\n", argv[0]);
	exit(1);
    }

    memset(&state, 0, sizeof(state_t));
    memset(&new, 0, sizeof(state_t));

    init_pipeline();
    init_cache(&state.iCache, IC_SIZE, IC_ASSOC, IC_BLOCK, "I-Cache");
    init_cache(&state.dCache, DC_SIZE, DC_ASSOC, DC_BLOCK, "D-Cache");
    init_bpredictor(&state.bPredictor, BP_TYPE, BP_SIZE, BP_TARG);

    /* read machine-code file into instruction/data memory (starting at address 0) */

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
	printf("error: can't open file %s\n", argv[1]);
	perror("fopen");
	exit(1);
    }

    for (state.numMemory=0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
	state.numMemory++) {
	if (sscanf(line, "%hx", &state.mainMemory[state.numMemory]) != 1) {
	    printf("error in reading address %hd\n", state.numMemory);
	    exit(1);
	}
	printf("memory[%hd]=%hx\n", state.numMemory, state.mainMemory[state.numMemory]);
    }

    printf("%hd memory words\n", state.numMemory);

    run();

    printState(&state);
    printStats(&stats);
}

void
init_pipeline()
{
    short i;

    state.pc=0;

    for (i=0; i<IBUFSIZE; i++) {
	state.IFID[i].valid = 0;
	state.IFID[i].instr = NOPINSTRUCTION; /* nop */
    }

    for (i=0; i<NUMPIPES; i++) {
	state.IDEX[i].instr = state.EXMEM[i].instr =
	    state.MEMWB[i].instr = state.WBEND[i].instr = NOPINSTRUCTION; /* nop */
    }
}

short opcode(short instruction)
{
    return (instruction >> OP_SHIFT) & 0x7;
}

short
field_ra(short instruction)
{
    return (instruction >> A_SHIFT) & 0x7;
}

short
field_rb(short instruction)
{
    return (instruction >> B_SHIFT) & 0x7;
}

short
field_rc(short instruction)
{
    return (instruction >> C_SHIFT) & 0x7;
}

short
field_imm(short instruction)
{
    return (instruction & 0x7F);
}

short
offset(short instruction)
{
    return convertNum(field_imm(instruction));
}

short
target(short instr)
{
    if (opcode(instr) == ADD || opcode(instr) == ADDI || opcode(instr) == NAND ||
	opcode(instr) == LW || opcode(instr) == LUI || opcode(instr) == JALR) {
	return field_ra(instr);
    } else {
	return 0;
    }
}

short
source1(short instr)
{
    if (opcode(instr) == ADD || opcode(instr) == ADDI || opcode(instr) == NAND ||
	opcode(instr) == BEQ || opcode(instr) == SW || opcode(instr) == LW) {
	return field_rb(instr);
    } else {
	return 0;
    }
}

short
source2(short instr)
{
    if (opcode(instr) == ADD || opcode(instr) == NAND) {
	return field_rc(instr);
    } else if (opcode(instr) == JALR) {
	return field_rb(instr);
    } else if (opcode(instr) == BEQ || opcode(instr) == SW) {
	return field_ra(instr);
    } else {
	return 0;
    }
}


short
mem_type(short instruction)
{
    return(opcode(instruction)==LW || opcode(instruction)==SW);
}

short
branch_type(short instruction)
{
    return(opcode(instruction)==BEQ || opcode(instruction)==JALR);
}

short
arith_type(short instruction)
{
    return(opcode(instruction)==ADD || opcode(instruction)==ADDI || opcode(instruction)==NAND || opcode(instruction)==LUI ||
    /* this part necessary to update the RF */
    opcode(instruction)==JALR);
}

short
convertNum(short num)
{
    /* convert a 7 bit number into a 32-bit or 64-bit Sun number */
    if (num & 0x40) {
	num -= 128;
    }
    return(num);
}

void
cache_fill(cache_t *cache, short addr, short *data, short *mainMemory)
{
    short i;
    short *blockp;
    short word_in_block;
    short set_in_cache;
    short blocknum;
    short block_in_set;

    set_t *sp;
    block_t *bp;

    blocknum = (addr >> cache->log_blocksize);
    word_in_block = (addr & cache->word_mask);
    set_in_cache = (blocknum % cache->numsets);

    sp = &cache->setp[ set_in_cache ];

    /* "randomly" choose a block within the set (if an associative cache),
     * but do not choose to replace the most-recently-used block */
    if (cache->assoc > 1) {
	block_in_set = random() % cache->assoc;
	while (block_in_set == sp->mru) {
	    block_in_set++;
	    block_in_set %= cache->assoc;
	}
    } else {
	block_in_set = 0;
    }

    bp = &sp->blockp[ block_in_set ];

    bp->tag = blocknum;
    bp->valid = 1;
    sp->mru = block_in_set;
    blockp = bp->datap;
    for (i=0; i<cache->blocksize; i++) {
	blockp[i] = mainMemory[i + (blocknum << cache->log_blocksize)];
    }

    if (data) {
	*data = mainMemory[addr];
    }

    return;
}

int
cache_write(cache_t *cache, short addr, short *data)
{
    short i, block_in_set;
    short *blockp;
    short word_in_block;
    short set_in_cache;
    short blocknum;

    set_t *sp;
    block_t *bp;

    blocknum = (addr >> cache->log_blocksize);
    word_in_block = (addr & cache->word_mask);
    set_in_cache = (blocknum % cache->numsets);

    sp = &cache->setp[ set_in_cache ];

    for (i=0, block_in_set=sp->mru; i<cache->assoc; i++, block_in_set=(block_in_set+1)%cache->assoc) {
	bp = &sp->blockp[ block_in_set ];
	if (bp->tag == blocknum && bp->valid) {
	    blockp = bp->datap;
	    blockp[word_in_block] = *data;
	    sp->mru = block_in_set;
	    return 1;
	}
    }

    return 0;
}

int
cache_read(cache_t *cache, short addr, short *data)
{
    short i, block_in_set;
    short *blockp;
    short word_in_block;
    short set_in_cache;
    short blocknum;

    set_t *sp;
    block_t *bp;

    blocknum = (addr >> cache->log_blocksize);
    word_in_block = (addr & cache->word_mask);
    set_in_cache = (blocknum % cache->numsets);

    sp = &cache->setp[ set_in_cache ];

    for (i=0, block_in_set=sp->mru; i<cache->assoc; i++, block_in_set=(block_in_set+1)%cache->assoc) {
	bp = &sp->blockp[ block_in_set ];
	if (bp->tag == blocknum && bp->valid) {
	    blockp = bp->datap;
	    *data = blockp[word_in_block];
	    sp->mru = block_in_set;
	    return 1;
	}
    }

    return 0;
}

void
print_cache(cache_t *cache)
{
    short i, j, k;
    set_t *setp;
    block_t *blockp;
    short *datap;

    printf("\tSET MRU\t", cache->assoc);
    for (i=0; i<cache->assoc; i++) {
	printf("Block-%hd:", i);
	for (j=0; j<cache->blocksize; j++) {
	    printf("\t");
	}
    }
    printf("\n");
    printf("\t");
    for (j=0; j<cache->assoc; j++) {
	printf("\tV TAG");
	for (k=0; k<cache->blocksize; k++) {
	    printf("\tData-%d", k);
	}
    }
    printf("\n");
    for (i=0; i<cache->numsets; i++) {
	setp = &cache->setp[i];
	printf("\t%02hd: %hd", i, setp->mru);
	for (j=0; j<cache->assoc; j++) {
	    blockp = &setp->blockp[j];
	    printf("\t%hd %04hx", blockp->valid, blockp->tag);
	    datap = blockp->datap;
	    for (k=0; k<cache->blocksize; k++) {
		printf("\t%04hx", datap[k]);
	    }
	}
	printf("\n");
    }
    return;
}

void
init_cache(cache_t *cache, short size, short assoc, short block, char *name)
{
    short i, j;
    set_t *sp;
    block_t *bp;

    switch (block) {
	case 2:
	    cache->log_blocksize = 1;
	    cache->word_mask = 1;
	break;
	case 4:
	    cache->log_blocksize = 2;
	    cache->word_mask = 3;
	break;
	case 8:
	    cache->log_blocksize = 3;
	    cache->word_mask = 7;
	break;
	default:
	    printf("ERROR: invalid blocksize %d\n", block);
	    exit(1);
	break;
    }

    cache->assoc = assoc;
    cache->blocksize = block;
    cache->numsets = size / block / assoc;

    cache->setp = (set_t *)calloc(cache->numsets, sizeof(set_t));
    if (!cache->setp) {
	perror("calloc sets");
	exit(1);
    }

    for (i=0; i<cache->numsets; i++) {
	sp = &cache->setp[i];
	sp->blockp = (block_t *)calloc(assoc, sizeof(block_t));
	if (!sp->blockp) {
	    perror("calloc block");
	    exit(1);
	}
	for (j=0; j<assoc; j++) {
	    bp = &sp->blockp[j];
	    bp->valid = 0;
	    bp->datap = (short *)calloc(block, sizeof(short));
	    if (!bp->datap) {
		perror("calloc data");
		exit(1);
	    }
	}
    }

    printf("%s:\n", name);
    printf("     numsets: %hd sets\n", cache->numsets);
    printf("    blocksiz: %hd words\n", cache->blocksize);
    printf("       assoc: %hd way\n", cache->assoc);
    printf("        size: %hd bytes\n", cache->numsets * cache->assoc * cache->blocksize * 2);
    printf("    wordmask: %hx\n", cache->word_mask);
    printf("    logblock: %hd\n", cache->log_blocksize);
}

/*
typedef struct branchPredictor {
    short type;
    short branchHistory[MAXPREDTABLE];
    short bhtSize;
    short patternHistory[MAXPREDTABLE];
    short phtSize;
    short branchTarget[MAXPREDTABLE];
    short btbSize;
} bpredictor_t;
*/

short
predict_btarget(bpredictor_t *pred, short pc, short *idx)
{
    if (idx) *idx = -1;
    switch (pred->type) {
	case NONE:
	    return pc+1;
	break;

	case ONE_LEVEL:
	case TWO_LEVEL:
	case STATIC:
	    if (pred->btbSize) {
		if (idx) *idx = pc & (pred->btbSize - 1);
		return pred->branchTarget[ *idx ];
	    } else {
		return 30000;	/* some huge, invalid number, so we can catch mistakes */
	    }
	break;

	default:
	    fprintf(stderr, "BIG ERROR: bad prediction type %d\n", pred->type);
	break;
    }
}

short
predict_bdirection(bpredictor_t *pred, short pc, short instr, short *bht_idx, short *pht_idx)
{
    short idx;
    if (bht_idx) *bht_idx = -1;
    if (pht_idx) *pht_idx = -1;
    switch (pred->type) {
	case NONE:
	    return 0;
	break;

	case TWO_LEVEL:
	    idx = pc & (pred->bhtSize - 1);
	    if (bht_idx) *bht_idx = idx;
	    pc = pred->branchHistory[ idx ];
	    /* FALL THROUGH to the ONE-LEVEL predictor */
	case ONE_LEVEL:
	    idx = *pht_idx = pc & (pred->phtSize - 1);
	    if (pht_idx) *pht_idx = idx;
	    if (pred->patternHistory[ idx ] & 0x2) {	/* check the most significant of 2 bits */
		return 1;
	    } else {
		return 0;
	    }
	break;

	case STATIC:
	    if (offset(instr) < 0) {
		return 1;
	    } else {
		return 0;
	    }
	break;

	default:
	    fprintf(stderr, "BIG ERROR: bad prediction type %d\n", pred->type);
	break;
    }
}

void
update_bpredictor(bpredictor_t *pred, short bht_idx, short pht_idx, short btb_idx, short direction, short target)
{
    switch (pred->type) {
	case NONE:		/* do nothing */
	break;

	case TWO_LEVEL:
	    if (bht_idx < 0 || bht_idx >= pred->bhtSize) {
		fprintf(stderr, "ERROR: bad index to branch history table: %d\n", bht_idx);
		exit(1);
	    }
	    pred->branchHistory[ bht_idx ] <<= 1;
	    pred->branchHistory[ bht_idx ] |= direction & 1;
	    pred->branchHistory[ bht_idx ] &= 0xf;	/* only keep the most significant FOUR bits */
	    /* FALL THROUGH to the ONE-LEVEL case */
	case ONE_LEVEL:
	    if (pht_idx < 0 || pht_idx >= pred->phtSize) {
		fprintf(stderr, "ERROR: bad index to pattern history table: %d\n", pht_idx);
		exit(1);
	    }
	    pred->patternHistory [ pht_idx ] += (direction ? 1 : -1);
	    if (pred->patternHistory [ pht_idx ] > 3) {
		pred->patternHistory [ pht_idx ] = 3;
	    } else if (pred->patternHistory [ pht_idx ] < 0) {
		pred->patternHistory [ pht_idx ] = 0;
	    }
	break;

	case STATIC:
	break;

	default:
	    fprintf(stderr, "BIG ERROR: bad prediction type %d\n", pred->type);
	break;
    }
    if (pred->btbSize && btb_idx >= 0) {
	pred->branchTarget[ btb_idx ] = target;
    }
}

/*
typedef struct branchPredictor {
    short type;
    short branchHistory[MAXPREDTABLE];
    short bhtSize;
    short patternHistory[MAXPREDTABLE];
    short phtSize;
    short branchTarget[MAXPREDTABLE];
    short btbSize;
} bpredictor_t;
*/

char *branchHistories[] = {
    "0000",
    "0001",
    "0010",
    "0011",
    "0100",
    "0101",
    "0110",
    "0111",
    "1000",
    "1001",
    "1010",
    "1011",
    "1100",
    "1101",
    "1110",
    "1111",
};

char *
historystr(short branch)
{
    if (branch < 0 || branch > 15) {
	return "INVALID HISTORY";
    } else {
	return branchHistories[branch];
    }
}

char *patterns[] = {
    "00",
    "01",
    "10",
    "11",
};

char *
patternstr(short pattern)
{
    if (pattern < 0 || pattern > 3) {
	return "INVALID PATTERN";
    } else {
	return patterns[pattern];
    }
}

void
print_bpredictor(bpredictor_t *pred)
{
    short i;
    switch (pred->type) {
	case NONE:
	    printf("\tNONE\n");
	break;

	case TWO_LEVEL:
	    printf("\tTWO-LEVEL with BTB of size %d\n", pred->btbSize);
	    printf("\tbranch history table:\n");
	    for (i=0; i<pred->bhtSize; i++) {
		printf("\t    %s\n", historystr(pred->branchHistory[i]));
	    }
	    printf ("\tpattern history table:\n");
	    for (i=0; i<pred->phtSize; i++) {
		printf("\t    %s\n", patternstr(pred->patternHistory[i]));
	    }
	break;

	case ONE_LEVEL:
	    printf("\tONE-LEVEL with BTB of size %d\n", pred->btbSize);
	    printf ("\tpattern history table:\n");
	    for (i=0; i<pred->phtSize; i++) {
		printf("\t    %s\n", historystr(pred->patternHistory[i]));
	    }
	break;

	case STATIC:
	    printf("\tSTATIC with BTB of size %d\n", pred->btbSize);
	break;

	default:
	    printf("BIG ERROR: bad prediction type %d\n", pred->type);
	    exit(0);
	break;
    }

    if (pred->type != NONE && pred->btbSize) {
	printf ("\tbranch target buffer:\n");
	for (i=0; i<pred->btbSize; i++) {
	    printf("\t    %04hx\n", pred->branchTarget[i]);
	}
    }

    return;
}

void
init_bpredictor(bpredictor_t *pred, short type, short size, short target)
{
    short i;

    pred->type = type;

    switch (type) {
	case NONE:		/* do nothing */
	break;

	case ONE_LEVEL:		/* set up the pattern history table */
	    pred->phtSize = size;
	    for (i=0; i<pred->phtSize; i++) {
		pred->patternHistory[i] = 1;
	    }
	break;

	case TWO_LEVEL:		/* set up pattern history and branch history tables */
	    pred->bhtSize = size;
	    for (i=0; i<pred->bhtSize; i++) {
		pred->branchHistory[i] = 0;
	    }
	    pred->phtSize = 16;
	    for (i=0; i<pred->phtSize; i++) {
		pred->patternHistory[i] = 1;
	    }
	break;

	case STATIC:		/* do nothing */
	break;

	default:
	    fprintf(stderr, "BIG ERROR: bad initial prediction type %d\n", type);
	    exit(0);
	break;
    }

    pred->btbSize = target;
    for (i=0; i<target; i++) {
	pred->branchTarget[ i ] = 0;
    }

    return;
}

#define WID	40
#define PRINT_FIELD2(label,struct,field) \
    printf("\t%s %-*hd%s %-*hd \n", \
	label, WID - strlen(label), state->struct[0].field, \
	label, WID - strlen(label), state->struct[1].field);

#define WID4	23
#define PRINT_FIELD4(label,struct,field) \
    printf("\t%s %-*hd%s %-*hd%s %-*hd%s %-*hd\n", \
	label, WID4 - strlen(label), state->struct[0].field, \
	label, WID4 - strlen(label), state->struct[1].field, \
	label, WID4 - strlen(label), state->struct[2].field, \
	label, WID4 - strlen(label), state->struct[3].field);

char PRINT_INSTRUCTION[100];

void
print_RRR(instr, name)
short instr;
char *name;
{
    sprintf(PRINT_INSTRUCTION, "%s %hd,%hd,%hd", name, field_ra(instr), field_rb(instr), field_rc(instr));
}

void
print_RRI(instr, name)
short instr;
char *name;
{
    sprintf(PRINT_INSTRUCTION, "%s %hd,%hd,%hd", name, field_ra(instr), field_rb(instr), offset(instr));
}

void
print_RR(instr, name)
short instr;
char *name;
{
    sprintf(PRINT_INSTRUCTION, "%s %hd,%hd", name, field_ra(instr), field_rb(instr));
}

void
print_RI(instr, name)
short instr;
char *name;
{
    sprintf(PRINT_INSTRUCTION, "%s %hd,0x%hx", name, field_ra(instr), instr & 0x3ff);
}

char *
printInstruction(short instr)
{
    if (instr == NOPINSTRUCTION) {
	sprintf(PRINT_INSTRUCTION, "nop");
    } else if (instr == HALTINSTRUCTION) {
	sprintf(PRINT_INSTRUCTION, "halt");
    } else if (opcode(instr) == ADD) {
	print_RRR(instr, "add");
    } else if (opcode(instr) == ADDI) {
	print_RRI(instr, "addi");
    } else if (opcode(instr) == NAND) {
	print_RRR(instr, "nand");
    } else if (opcode(instr) == LUI) {
	print_RI(instr, "lui");
    } else if (opcode(instr) == LW) {
	print_RRI(instr, "lw");
    } else if (opcode(instr) == SW) {
	print_RRI(instr, "sw");
    } else if (opcode(instr) == BEQ) {
	print_RRI(instr, "beq");
    } else if (opcode(instr) == JALR) {
	print_RR(instr, "jalr");
    } else {
	sprintf(PRINT_INSTRUCTION, "data: %hd", instr);
    }

    return PRINT_INSTRUCTION;
}

void
printState(state_t *state)
{
    short i;
    printf("@@@ state at instruction %ld, cycle %ld\n", stats.instructions, stats.cycles);
    printf("    pc %hd\n", state->pc);

    printf("    instruction cache:\n");
	print_cache(&state->iCache);
    printf("    data cache:\n");
	print_cache(&state->dCache);
    printf("    registers:\n");
	for (i=0; i<NUMREGS; i++) {
	    printf("\treg[ %hd ] %hd\n", i, state->reg[i]);
	}
    printf("    branch predictor:\n");
	print_bpredictor(&state->bPredictor);

    printf("    instruction window:\n");
    printf("\t%-*s %-*s %-*s %-*s \n", WID4, "IFID.0", WID4, "IFID.1", WID4, "IFID.2", WID4, "IFID.3");
	printf("\tinstruction %-*s", 12, printInstruction(state->IFID[0].instr));
	printf("instruction %-*s", 12, printInstruction(state->IFID[1].instr));
	printf("instruction %-*s", 12, printInstruction(state->IFID[2].instr));
	printf("instruction %-*s\n", 12, printInstruction(state->IFID[3].instr));
	PRINT_FIELD4("pcPlus1", IFID, pcPlus1)
	PRINT_FIELD4("offset", IFID, offset)
	PRINT_FIELD4("valid", IFID, valid)
	PRINT_FIELD4("patHistIdx", IFID, branchHistoryIndex)
	PRINT_FIELD4("brnHistIdx", IFID, patternHistoryIndex)
	PRINT_FIELD4("brnTargIdx", IFID, branchTargetIndex)
	PRINT_FIELD4("prdBrnTarg", IFID, predBranchTarget)
	PRINT_FIELD4("predTaken", IFID, predTaken)
	PRINT_FIELD4("branchTakn", IFID, branchTaken)

    printf("    %-*s %-*s \n", WID, "IDEX.0", WID, "IDEX.1");
	printf("\tinstruction %-*s", WID - 11, printInstruction(state->IDEX[0].instr));
	printf("instruction %-*s\n", WID - 11, printInstruction(state->IDEX[1].instr));
	PRINT_FIELD2("pcPlus1", IDEX, pcPlus1)
	PRINT_FIELD2("offset", IDEX, offset)
	PRINT_FIELD2("readRegA", IDEX, readRegA)
	PRINT_FIELD2("readRegB", IDEX, readRegB)
	PRINT_FIELD2("branchHistoryIndex", IDEX, branchHistoryIndex)
	PRINT_FIELD2("patternHistoryIndex", IDEX, patternHistoryIndex)
	PRINT_FIELD2("branchTargetIndex", IDEX, branchTargetIndex)
	PRINT_FIELD2("predBranchTarget", IDEX, predBranchTarget)
	PRINT_FIELD2("branchTaken", IDEX, branchTaken)
	PRINT_FIELD2("realBranchTarget", IDEX, realBranchTarget)

    printf("    %-*s %-*s \n", WID, "EXMEM.0", WID, "EXMEM.1");
	printf("\tinstruction %-*s", WID - 11, printInstruction(state->EXMEM[0].instr));
	printf("instruction %-*s\n", WID - 11, printInstruction(state->EXMEM[1].instr));
	PRINT_FIELD2("aluResult", EXMEM, aluResult)
	PRINT_FIELD2("readRegB", EXMEM, readRegB)

    printf("    %-*s %-*s \n", WID, "MEMWB.0", WID, "MEMWB.1");
	printf("\tinstruction %-*s", WID - 11, printInstruction(state->MEMWB[0].instr));
	printf("instruction %-*s\n", WID - 11, printInstruction(state->MEMWB[1].instr));
	PRINT_FIELD2("writeData", MEMWB, writeData)

    printf("    %-*s %-*s \n", WID, "WBEND.0", WID, "WBEND.1");
	printf("\tinstruction %-*s", WID - 11, printInstruction(state->WBEND[0].instr));
	printf("instruction %-*s\n", WID - 11, printInstruction(state->WBEND[1].instr));
	PRINT_FIELD2("writeData", WBEND, writeData)
}

void
run()
{
    short readRegA;
    short readRegB;
    short i, j, k, data, instr, icache_fetch;
    short pht_idx, bht_idx, btb_idx;
    short available_pipe;
    short cannot_issue;

    printf("iassoc = %d\n", state.iCache.assoc);
    printf("dassoc = %d\n", state.dCache.assoc);

    /* just for good measure, copy everything in state to new */
    memcpy(&new, &state, sizeof(state_t));

    while (1) {

	// printState(&state);

	/* --------------------- IF stage --------------------- */

	icache_fetch = 0;
	for (i=0; i<IBUFSIZE; i++) {
	    /* don't fetch into a buffer that already has a non-issued instruction */
	    if (state.IFID[i].valid) continue;

	    /* get the next instruction out of the cache into "instr" */
	    stats.icache_access++;
	    if (!cache_read(&new.iCache, state.pc, &instr)) {
		state.IFID[i].imiss = 1;
		stats.icache_miss++;
		stats.cycles += CACHE_MISS_PENALTY;
		cache_fill(&new.iCache, state.pc, &instr, &state.mainMemory[0]);
	    } else {
		state.IFID[i].imiss = 0;
	    }

	    new.IFID[i].instr = instr;
	    new.IFID[i].pcPlus1 = state.pc+1;
	    new.IFID[i].predBranchTarget = predict_btarget(&state.bPredictor, state.pc, &btb_idx);
	    new.IFID[i].predTaken = predict_bdirection(&state.bPredictor, state.pc, instr, &bht_idx, &pht_idx);
	    new.IFID[i].offset = offset(instr);
	    new.IFID[i].valid = 1;
	    new.IFID[i].branchHistoryIndex = bht_idx;
	    new.IFID[i].patternHistoryIndex = pht_idx;
	    new.IFID[i].branchTargetIndex = btb_idx;

	    if (branch_type(instr) && new.IFID[i].predTaken) {
		new.pc = new.IFID[i].predBranchTarget;
		new.IFID[i].branchTaken = 1;
		break;	/* don't continue fetching instructions */
	    } else {
		new.IFID[i].branchTaken = 0;
		state.pc++;
		new.pc = state.pc;
	    }

	    /* limit ourselves to 2 instructions fetched per clock cycle */
	    icache_fetch++;
	    if (icache_fetch > 1) break;

	}

	/* --------------------- ID stage --------------------- */

	/*
	 * issue algorithm:
	 *
	 * if either instruction in the IDEX registers is a LW,
	 * then no dependent instructions may follow.
	 *
	 * if there are true dependencies between two instructions,
	 * they may not issue simultaneously.
	 * 
	 * an instruction may not be issued ahead of another instruction
	 * if there are any dependencies between the two (RAW, WAW, WAR).
	 *
	 * don't let any instruction bypass a branch instruction.
	 * don't let a branch instruction bypass any instruction.
	 *
	 * don't let stores bypass loads or stores
	 * don't let loads bypass stores
	 */

	/* by default */
	memset(&new.IDEX[0], 0, sizeof(new.IDEX));
	new.IDEX[0].instr = new.IDEX[1].instr = NOPINSTRUCTION;
	for (i=0; i<IBUFSIZE; i++) {
	    if (!state.IFID[i].valid) {
		break;
	    }
	    memcpy(&new.IFID[i], &state.IFID[i], sizeof(IFID_t));
	}

	//fprintf(stderr, "------\n");

	available_pipe = 0;
	cannot_issue = 0;
	for (i=0; i<IBUFSIZE; i++) {

	    if (cannot_issue) {
		/* can only get here if we skipped over an instruction */
		/* stop the issue process if that instruction was a Branch */
		if (branch_type(state.IFID[i-1].instr)) {
		    break;
		}
	    }

	    cannot_issue = 0;

	    if (!state.IFID[i].valid) {
		break;
	    }

	    /*
	     * if there is a LW in the execute stage, we cannot issue
	     * any instruction that is dependent on the load
	     */
	    for (j=0; j<NUMPIPES; j++) {
		if (opcode(state.IDEX[j].instr) == LW 
		    && ( (source1(state.IFID[i].instr) != 0 && source1(state.IFID[i].instr) == target(state.IDEX[j].instr))
			 || (source2(state.IFID[i].instr) != 0 && source2(state.IFID[i].instr) == target(state.IDEX[j].instr)))) {
		    /* we cannot issue this instruction */
		    stats.load_use_interlocks++;
		    cannot_issue = 1;
		    break;
		}
	    }
	    if (cannot_issue) {
		//fprintf(stderr, "(load-use): %s\n", printInstruction(state.IFID[i].instr));
		continue;
	    }

	    /*
	     * make sure that we do not issue two instructions
	     * simultaneously that have inter-dependencies
	     */
	    for (j=0; j<available_pipe; j++) {		/* only executes if we have already issued one instruction */
		if (	/* check RAW dependencies */
			(source1(state.IFID[i].instr) != 0 && source1(state.IFID[i].instr) == target(new.IDEX[j].instr))
		     || (source2(state.IFID[i].instr) != 0 && source2(state.IFID[i].instr) == target(new.IDEX[j].instr))
#if 0
			/* check WAR dependencies */
		     || (source1(new.IDEX[j].instr) != 0 && source1(new.IDEX[j].instr) == target(state.IFID[i].instr))
		     || (source2(new.IDEX[j].instr) != 0 && source2(new.IDEX[j].instr) == target(state.IFID[i].instr))
#endif
			/* check WAW dependencies */
		     || (target(state.IFID[i].instr) != 0 && target(state.IFID[i].instr) == target(new.IDEX[j].instr))) {

		     cannot_issue = 1;
		     break;
		 }
	    }
	    if (cannot_issue) {
		//fprintf(stderr, "(inter-dependent): %s\n", printInstruction(state.IFID[i].instr));
		continue;
	    }

	    /*
	     * make sure that we do not bypass an instruction with
	     * which we have an inter-dependence ... this includes
	     * branches, loasd, and stores.
	     *
	     * if we DO find branches, then break, since we cannot go any further.
	     */
	    for (j=0; j<i; j++) {	/* if we are issuing out-of-order */
		if (!state.IFID[j].valid) continue;
		if (branch_type(state.IFID[i].instr) || branch_type(state.IFID[j].instr)) {
		    cannot_issue = 1;
		    //fprintf(stderr, "(BRANCH-bypass): %s\n", printInstruction(state.IFID[i].instr));
		    break;
		}
		if (	(opcode(state.IFID[i].instr) == LW && opcode(state.IFID[j].instr) == SW)
			|| (opcode(state.IFID[i].instr) == SW && opcode(state.IFID[j].instr) == LW)
			|| (opcode(state.IFID[i].instr) == SW && opcode(state.IFID[j].instr) == SW)) {
		    cannot_issue = 1;
		    //fprintf(stderr, "(MEM-bypass): %s\n", printInstruction(state.IFID[i].instr));
		    break;
		}
		if (	/* check RAW dependencies */
			(source1(state.IFID[i].instr) != 0 && source1(state.IFID[i].instr) == target(state.IFID[j].instr))
		     || (source2(state.IFID[i].instr) != 0 && source2(state.IFID[i].instr) == target(state.IFID[j].instr))
			/* check WAR dependencies */
		     || (source1(state.IFID[j].instr) != 0 && source1(state.IFID[j].instr) == target(state.IFID[i].instr))
		     || (source2(state.IFID[j].instr) != 0 && source2(state.IFID[j].instr) == target(state.IFID[i].instr))
			/* check WAW dependencies */
		     || (target(state.IFID[i].instr) == target(state.IFID[j].instr))) {

		     //fprintf(stderr, "(cant-bypass): %s\n", printInstruction(state.IFID[i].instr));
		     cannot_issue = 1;
		     break;
		}
	    }
	    if (cannot_issue) {
		continue;
	    }

	    /* we can issue this instruction */

	    //fprintf(stderr, "ISSUING: %s\n", printInstruction(state.IFID[i].instr));

	    new.IDEX[available_pipe].instr = state.IFID[i].instr;
	    new.IDEX[available_pipe].imiss = state.IFID[i].imiss;
	    new.IDEX[available_pipe].pcPlus1 = state.IFID[i].pcPlus1;
	    new.IDEX[available_pipe].offset = state.IFID[i].offset;
	    new.IDEX[available_pipe].readRegA = state.reg[source1(state.IFID[i].instr)];
	    new.IDEX[available_pipe].readRegB = state.reg[source2(state.IFID[i].instr)];
	    new.IDEX[available_pipe].predBranchTarget = state.IFID[i].predBranchTarget;
	    new.IDEX[available_pipe].realBranchTarget = state.IFID[i].pcPlus1 + state.IFID[i].offset;
	    new.IDEX[available_pipe].branchHistoryIndex = state.IFID[i].branchHistoryIndex;
	    new.IDEX[available_pipe].patternHistoryIndex = state.IFID[i].patternHistoryIndex;
	    new.IDEX[available_pipe].branchTargetIndex = state.IFID[i].branchTargetIndex;
	    new.IDEX[available_pipe].branchTaken = state.IFID[i].branchTaken;

	    memset(&new.IFID[i], 0, sizeof(IFID_t));
	    state.IFID[i].valid = 0;	/* hackery */
	    new.IFID[i].valid = 0;
	    new.IFID[i].instr = NOPINSTRUCTION;

	    available_pipe++;
	    if (available_pipe >= NUMPIPES) {
		break;
	    }

	}

	switch (available_pipe) {
	    case 0:
		stats.zero_instructions_issued++;
	    break;
	    case 1:
		stats.one_instructions_issued++;
	    break;
	    case 2:
		stats.two_instructions_issued++;
	    break;
	    default:
		fprintf(stderr, "ERROR: how many pipes do we have? [%hd] \n", available_pipe);
	    break;
	}

	for (i=0; i<IBUFSIZE-1; i++) {
	    if (new.IFID[i].valid) continue;

	    for (j=i+1; j<IBUFSIZE; j++) {
		if (new.IFID[j].valid) break;
	    }
	    if (j >= IBUFSIZE) break;
	    memcpy(&new.IFID[i], &new.IFID[j], sizeof(IFID_t));
	    memset(&new.IFID[j], 0, sizeof(IFID_t));
	    new.IFID[j].valid = 0;
	    new.IFID[j].instr = NOPINSTRUCTION;
	}

	/* --------------------- EX stage --------------------- */

	for (i=0; i<NUMPIPES; i++) {
	    new.EXMEM[i].instr = state.IDEX[i].instr;
	    new.EXMEM[i].imiss = state.IDEX[i].imiss;

	    readRegA = state.IDEX[i].readRegA;
	    readRegB = state.IDEX[i].readRegB;

	    /* deal with forwarding:
	     * look ahead at the other instructions in the pipeline to see
	     * who (if anyone) is generating the operands we need.  put the
	     * pointers into the forwarding slots */

	    if (source1(state.IDEX[i].instr) == 0) {
		/* do nothing */
	    } else if (source1(state.IDEX[i].instr) == target(state.EXMEM[0].instr)) {
		readRegA = state.EXMEM[0].aluResult;
	    } else if (source1(state.IDEX[i].instr) == target(state.EXMEM[1].instr)) {
		readRegA = state.EXMEM[1].aluResult;
	    } else if (source1(state.IDEX[i].instr) == target(state.MEMWB[0].instr)) {
		readRegA = state.MEMWB[0].writeData;
	    } else if (source1(state.IDEX[i].instr) == target(state.MEMWB[1].instr)) {
		readRegA = state.MEMWB[1].writeData;
	    } else if (source1(state.IDEX[i].instr) == target(state.WBEND[0].instr)) {
		readRegA = state.WBEND[0].writeData;
	    } else if (source1(state.IDEX[i].instr) == target(state.WBEND[1].instr)) {
		readRegA = state.WBEND[1].writeData;
	    }

	    if (source2(state.IDEX[i].instr) == 0) {
		/* do nothing */
	    } else if (source2(state.IDEX[i].instr) == target(state.EXMEM[0].instr)) {
		readRegB = state.EXMEM[0].aluResult;
	    } else if (source2(state.IDEX[i].instr) == target(state.EXMEM[1].instr)) {
		readRegB = state.EXMEM[1].aluResult;
	    } else if (source2(state.IDEX[i].instr) == target(state.MEMWB[0].instr)) {
		readRegB = state.MEMWB[0].writeData;
	    } else if (source2(state.IDEX[i].instr) == target(state.MEMWB[1].instr)) {
		readRegB = state.MEMWB[1].writeData;
	    } else if (source2(state.IDEX[i].instr) == target(state.WBEND[0].instr)) {
		readRegB = state.WBEND[0].writeData;
	    } else if (source2(state.IDEX[i].instr) == target(state.WBEND[1].instr)) {
		readRegB = state.WBEND[1].writeData;
	    }

	    /* by default */
	    new.EXMEM[i].aluResult = 0;
	    new.EXMEM[i].readRegB = readRegB;

	    if (opcode(state.IDEX[i].instr) == ADD) {
		new.EXMEM[i].aluResult = readRegA + readRegB;
	    } else if (opcode(state.IDEX[i].instr) == ADDI) {
		new.EXMEM[i].aluResult = readRegA + state.IDEX[i].offset;
	    } else if (opcode(state.IDEX[i].instr) == NAND) {
		new.EXMEM[i].aluResult = ~(readRegA & readRegB);
	    } else if (opcode(state.IDEX[i].instr) == LUI) {
		new.EXMEM[i].aluResult = (state.IDEX[i].instr << 6) & 0xffc0;
	    } else if (opcode(state.IDEX[i].instr) == JALR) {
		if (readRegB == state.IDEX[i].predBranchTarget && state.IDEX[i].branchTaken) {
		    /* we correctly predicted the branch target and we took it ... do nothing */
		    new.EXMEM[i].aluResult = state.IDEX[i].pcPlus1;
		} else {
		    /* we have an incorrect prediction in the pipe */
		    memset(&new.IFID, 0, sizeof(new.IFID));
		    memset(&new.IDEX, 0, sizeof(new.IDEX));
		    if (i == 0) {	/* stomp on instruction in pipe1 */
			memset(&state.IDEX[1], 0, sizeof(IDEX_t));
		    }
		    new.EXMEM[i].aluResult = state.IDEX[i].pcPlus1;
		    new.pc = readRegB;
		}
		update_bpredictor(&new.bPredictor,
				    state.IDEX[i].branchHistoryIndex,
				    state.IDEX[i].patternHistoryIndex,
				    state.IDEX[i].branchTargetIndex,
				    1,
				    readRegB);
	    } else if (opcode(state.IDEX[i].instr) == BEQ) {
		short badDirection = (state.IDEX[i].branchTaken != (readRegA == readRegB));
		short badTarget = (state.IDEX[i].predBranchTarget != state.IDEX[i].realBranchTarget);
		if (badDirection || (state.IDEX[i].branchTaken && badTarget)) {
		    /* stomp on bad instructions */
		    memset(&new.IFID, 0, sizeof(new.IFID));
		    memset(&new.IDEX, 0, sizeof(new.IDEX));
		    if (i == 0) {	/* stomp on instruction in pipe1 */
			memset(&state.IDEX[1], 0, sizeof(IDEX_t));
		    }

		    if (readRegA == readRegB) {
			new.pc = state.IDEX[i].realBranchTarget;
		    } else {
			new.pc = state.IDEX[i].pcPlus1;
		    }
		    if (badDirection) stats.branch_dir_mispredicts++;
		}

		if (badTarget) stats.branch_tgt_mispredicts++;
		stats.branches++;

		update_bpredictor(&new.bPredictor,
				    state.IDEX[i].branchHistoryIndex,
				    state.IDEX[i].patternHistoryIndex,
				    state.IDEX[i].branchTargetIndex,
				    (readRegA == readRegB),
				    state.IDEX[i].realBranchTarget);
		new.EXMEM[i].aluResult = (readRegA == readRegB);
	    } else if (mem_type(state.IDEX[i].instr)) {
		new.EXMEM[i].aluResult = readRegA + state.IDEX[i].offset;
		new.EXMEM[i].readRegB = readRegB; /* used for stores */
	    }
	}

	/* --------------------- MEM stage --------------------- */

	for (i=0; i<NUMPIPES; i++) {
	    new.MEMWB[i].instr = state.EXMEM[i].instr;
	    new.MEMWB[i].imiss = state.EXMEM[i].imiss;
	    new.MEMWB[i].writeData = state.EXMEM[i].aluResult;	// by default
	    if (opcode(state.EXMEM[i].instr) == LW) {
		if (!cache_read(&new.dCache, state.EXMEM[i].aluResult, &data)) {
		    stats.dcache_miss++;
		    stats.cycles += CACHE_MISS_PENALTY;
		    cache_fill(&new.dCache, state.EXMEM[i].aluResult, &data, &state.mainMemory[0]);
		}
		new.MEMWB[i].writeData = data;
		stats.dcache_access++;
	    } else if (opcode(state.EXMEM[i].instr) == SW) {
		data = state.EXMEM[i].readRegB;
		if (!cache_write(&new.dCache, state.EXMEM[i].aluResult, &data)) {
		    stats.dcache_miss++;
		    stats.cycles += CACHE_MISS_PENALTY;
		    cache_fill(&new.dCache, state.EXMEM[i].aluResult, (short *)NULL, &state.mainMemory[0]);
		    cache_write(&new.dCache, state.EXMEM[i].aluResult, &data);
		}
		new.mainMemory[state.EXMEM[i].aluResult] = data;
		stats.dcache_access++;
		new.MEMWB[i].writeData = state.EXMEM[i].aluResult;
	    }
	}

	/* --------------------- WB stage --------------------- */

	for (i=0; i<NUMPIPES; i++) {

	    /* check for halt */
	    if (state.MEMWB[i].instr == HALTINSTRUCTION) {
		printf("machine halted\n");
		printf("total of %ld instructions (%ld cycles) executed\n\n\n", stats.instructions, stats.cycles);
		return;
	    }

	    /* just to fill the pipeline register WBEND (for forwarding) */
	    new.WBEND[i].instr = state.MEMWB[i].instr;
	    new.WBEND[i].imiss = state.MEMWB[i].imiss;
	    new.WBEND[i].writeData = state.MEMWB[i].writeData;

	    new.reg[target(state.MEMWB[i].instr)] = state.MEMWB[i].writeData;
	    new.reg[0] = 0;
	}

	/* --------------------- end stage --------------------- */

	if (new.reg[0] != 0) {
	    fprintf(stderr, "ERROR: r0 != 0\n");
	    new.reg[0] = 0;
	}

	/* transfer new state into current state */
	memcpy(&state, &new, sizeof(state_t));

	/* --------------------- collect stats ----------------- */

	stats.cycles++;

	j=0;
	for (i=0; i<NUMPIPES; i++) {
	    if (state.WBEND[i].instr == NOPINSTRUCTION) {
		/*
		 * note that intentionally executed nops instructions do not get counted
		 * (therefore you cannot pad results by adding nop instructions to the code ...)
		 */
		j++;
	    } else {
		stats.instructions++;
		//printf ("%d %s\n", state.WBEND[i].imiss, printInstruction(state.WBEND[i].instr));
		if (stats.instructions % 10000 == 0) {
		    printf("committed %d instructions\n", stats.instructions);
		}
	    }
	}

	switch (j) {
	    case 0:
		stats.two_instructions_retired++;
	    break;
	    case 1:
		stats.one_instructions_retired++;
	    break;
	    case 2:
		stats.zero_instructions_retired++;
	    break;
	    default:
		fprintf(stderr, "ERROR: how many pipes do we have? [%hd] \n", j);
	    break;
	}

    }
}

/*
typedef struct {
    long cycles;
    long instructions;
    long icache_access;
    long icache_miss;
    long dcache_access;
    long dcache_miss;
    long branches;
    long branch_dir_mispredicts;
    long branch_tgt_mispredicts;
    long load_use_interlocks;
    long zero_instructions_issued;
    long one_instructions_issued;
    long two_instructions_issued;
} stats_t;
*/

#define FDIVIDE(a,b) (b == 0) ? ((float)a / 1.0) : ((float)a / (float)b)

void
printStats(stats_t *stats)
{
    long tot_issued = (stats->zero_instructions_issued + stats->one_instructions_issued + stats->two_instructions_issued);
    long tot_retired = (stats->zero_instructions_retired + stats->one_instructions_retired + stats->two_instructions_retired);

    printf("\nSTATS:\n");
    printf("------\n");
    printf("cycles: %ld\n", stats->cycles);
    printf("stalls: %ld\n", stats->cycles - stats->instructions);
    printf("instructions: %ld\n", stats->instructions);
    printf("icache_access: %ld\n", stats->icache_access);
    printf("icache_miss: %ld\n", stats->icache_miss);
    printf("dcache_access: %ld\n", stats->dcache_access);
    printf("dcache_miss: %ld\n", stats->dcache_miss);
    printf("branches: %ld\n", stats->branches);
    printf("branch_dir_mispredicts: %ld\n", stats->branch_dir_mispredicts);
    printf("branch_tgt_mispredicts: %ld\n", stats->branch_tgt_mispredicts);
    printf("load_use_interlocks: %ld\n", stats->load_use_interlocks);
    printf("zero_instructions_issued: %ld\n", stats->zero_instructions_issued);
    printf("one_instructions_issued: %ld\n", stats->one_instructions_issued);
    printf("two_instructions_issued: %ld\n", stats->two_instructions_issued);
    printf("zero_instructions_retired: %ld\n", stats->zero_instructions_retired);
    printf("one_instructions_retired: %ld\n", stats->one_instructions_retired);
    printf("two_instructions_retired: %ld\n", stats->two_instructions_retired);

    printf("-----------------------------------\n");
    printf("Total CPI: %.04f\n",
		FDIVIDE(stats->cycles, stats->instructions));
    printf("Instruction-issue IPC: %.04f\n",
		FDIVIDE((2 * stats->two_instructions_issued + stats->one_instructions_issued), tot_issued));
    printf("Instruction-retire IPC: %.04f\n",
		FDIVIDE((2 * stats->two_instructions_retired + stats->one_instructions_retired), tot_retired));
    printf("Icache miss rate: %.04f\n",
		FDIVIDE(stats->icache_miss, stats->icache_access));
    printf("Icache hit rate: %.04f\n",
		FDIVIDE((stats->icache_access - stats->icache_miss), stats->icache_access));
    printf("Dcache miss rate: %.04f\n",
		FDIVIDE((float)stats->dcache_miss, stats->dcache_access));
    printf("Dcache hit rate: %.04f\n",
		FDIVIDE((stats->dcache_access - stats->dcache_miss), stats->dcache_access));
    printf("Branch direction prediction miss rate: %.04f\n", 
		FDIVIDE(stats->branch_dir_mispredicts, stats->branches));
    printf("Branch target prediction miss rate: %.04f\n",
		FDIVIDE(stats->branch_tgt_mispredicts, stats->branches));
}

