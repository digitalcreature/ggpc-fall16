// Author: Tanner Grehawick
//	Email: tgrehawi@gmail.com
// Problem 2: Stack-based Computer Architecture Simulation

// WARNING: preprocessor bs ahead.
// this source file #includes itself at some point. when it does, IS_HEADER is defined (to differentiate it)
// this was done so that i could accomplish header shenanigans within the single file limitation.
// it's not at all necessary but it doesn't hurt performance or even readability to a very large extent
// (although youll be the judge of that)

// also, i think it's neat B)

#ifndef IS_HEADER
	//	if this isnt a header, set up these macros to declare an enum
	#define startops typedef enum
	#define op(name) OP_##name,
	#define endops operator
#else
	//if this IS a header, reset them to declare a constant string array to go alongside the enum.
	#undef startops
	#undef op
	#undef endops
	#define startops const char *opname[OPS_COUNT] =
	#define op(name) #name,
	#define endops
#endif

//from here on out, everything within an #ifndef IS_HEADER is normal source code.

#ifndef IS_HEADER

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
	#include <stdint.h>

	#define OPS_COUNT 16
	#define ID_MAXLEN 32
	#define STACK_SIZE 32
	#define LABELS_MAX 32
	#define INSTR_INITCAP 32

	typedef int32_t number;
	typedef uint32_t address;

	// check yer mallocs!
	void *malloc_checked(size_t size) {
		void *block;
		if ((block = malloc(size))) return block;
		fprintf(stderr, "Could not allocate block of size %ld\n", size);
		exit(1); return NULL;
	}
	// ... and yer reallocs!
	void *realloc_checked(void *block, size_t size) {
		if ((block = realloc(block, size))) return block;
		fprintf(stderr, "Could not allocate block of size %ld\n", size);
		exit(1); return NULL;
	}
	// its a secret to everyone
	#define malloc malloc_checked
	#define realloc realloc_checked

#endif

//and here is the aforementioned bs.

//this little preprocessor bs declares:
// - "operator" enum type
// - "opname" constant string array
// this is because this little section of code is included twice, each with a different set of macros (defined at the top of the file)
startops {
	op(PUSH) op(POP) op(DUPE) op(PEEK)
	op(ADD) op(SUB) op(MUL) op(DIV)
	op(ROT) op(BRANCH) op(CALL) op(BRZ)
	op(BGZ) op(BLZ) op(RETURN) op(HCF)
} endops;

#ifndef IS_HEADER

	//and heres the self include.
	//all this does is expand the macros above with different definitions
	//so that one piece of code creates 2 definitions.
	#define IS_HEADER
	#include "problem2.c"
	#undef IS_HEADER

	//now leaving: preprocessor horror

	//now entering: typedef land

// a single instruction in a program
typedef struct instruction {
	operator op;	//the operator this instruction uses
	struct {		// <-- anonumous struct for the "arg" instruction parameter
		enum {	// <-- anonymous enum for the type of arg
			ARG_NONE,	//this instruction has no arg associated with it
			ARG_NUM,		//this instruction has a number arg
			ARG_LABEL,	//this instruction has an unlinked label id arg
			ARG_ADDRESS	//this instruction's label id arg has been linked to the appropiate address
		} type;
		union {	// <-- anon union for the value of arg
			number n;				//number value is ARG_NUM
			char i[ID_MAXLEN];	//label id value if ARG_LABEL
			address a;				//label address value if ARG_ADDRESS
		};
	} arg;
} instruction;

// a single label in a program
typedef struct label {
	address address;		//the address to which this label points
	char id[ID_MAXLEN];	//the id "name" of this label
} label;

// a single token parsed from a source file
typedef struct token {
	enum {		// <-- anon enum for the type of this token
		TOKEN_EMPTY,	//an empty token, such as a blank or malformed line
		TOKEN_INSTRUCTION,	//an instruction token, of the form: "OPNAME [optional arg]"
		TOKEN_LABEL				//a label declaration token, of the form: "label:"
	} type;
	union {		// <-- anon union for the value of this token
		instruction instr;	//instruction value if TOKEN_INSTRUCTION
		label lbl;				//label value if TOKEN_LABEL
	};
} token;

//an entire program
typedef struct program {
	label labels[LABELS_MAX];		//this programs labels
	address labelcount;				//label count
	number stack[STACK_SIZE];		//stack
	address stackp;					//stack pointer
	instruction *instructions;		//instructions (dynamic array)
	address instrcount;				//instruction count
	address instrcap;					//the capacity if the instructions dynamic array
	address instrp;					//instruction pointer
} program;

//parse a sigle line into a token
token parseline(char *line) {
	char *s, *b;	//string and buffer pointers, used in macros
		#define skipspace() while (*s && isspace(*s)) s++	//skip whitespace
		//parse non-whitespace into a buffer (starting at b)
		#define parsestr() while (*s && !isspace(*s)) *(b++) = *(s++); *b = 0
	char buf1[ID_MAXLEN], buf2[ID_MAXLEN]; //string buffers
	// first of all, rewrite the first instanc of "#" (the comment character) with a null terminator.
	// this deletes comments so they dont get parsed
	s = line;								//start at the beginning of the line
	while (*s && *s != '#') s ++;		//position pointer at first "#" (if it exists, EOL otherwise)
	*s = 0;									//rewrite with 0. (this does nothing if there is no "#"; s is already pointing to 0)
	//then, read the first 2 whitespace delimited strings into buffers 1 and 2.
	//if there are less than 2 whitespace delimited strings present, buffer 2 or both buffers will hold empty strings.
	s = line;					//start at the beginning of the decommented line
	skipspace();				//skip any leading whitespace
	b = buf1; parsestr();	//parse the first string into buffer 1
	skipspace();				//skip intermeddiate whitespace
	b = buf2; parsestr();	//parse the second string into buffer 2
		//we don't need these macros any more
		#undef skipspace
		#undef parsestr
	token tkn;
	if (!buf1[0]) {
		//if the first buffer is empty, so is this line.
		tkn.type = TOKEN_EMPTY;
		return tkn;
	}
	int i;
	for (i = 0; i < OPS_COUNT; i ++) {
		//search through all the operators and see if the first buffer holds one by name.
		if (strcmp(buf1, opname[i]) == 0) {
			//if so, this is an instruction token
			tkn.type = TOKEN_INSTRUCTION;
			tkn.instr.op = i;							//the operator enum value is the same as i here
			//the second buffer holds the arg
			if (!buf2[0]) {
				//if buffer 2 is empty, there is no arg
				tkn.instr.arg.type = ARG_NONE;
			}
			else {
				//if an int scan succeeds, the arg is a number
				if (sscanf(buf2, "%d", &(tkn.instr.arg.n))) {
					tkn.instr.arg.type = ARG_NUM;
				}
				else {
					//otherwise, take the full value of buf2 as a label identifier
					strcpy(tkn.instr.arg.i, buf2);
					tkn.instr.arg.type = ARG_LABEL;
				}
			}
			return tkn;
		}
	}
	//if no operator name matched buffer 1, its possible this is a label declaration.
	int b1len = strlen(buf1);
	if (b1len > 1 && buf1[b1len - 1] == ':') {
		//if the last character is a colon, this is a valid label declaration.
		buf1[b1len - 1] = 0;	//strip off the colon
		tkn.type = TOKEN_LABEL;
		strcpy(tkn.lbl.id, buf1);
		tkn.lbl.address = 0;	//the address of the token can't be assigned until it is added to a program
		return tkn;
	}
	//if everything failed, this token is malformed. so lets just call it empty
	tkn.type = TOKEN_EMPTY;
	return tkn;
}

//initialize a new empty program
program *newprogram() {
	program *p = malloc(sizeof(program));
	p->labelcount = 0;
	p->stackp = 0;
	p->instrcount = 0;
	p->instrcap = INSTR_INITCAP;
	p->instructions = malloc(sizeof(instruction) * INSTR_INITCAP);
	p->instrp = 0;
	return p;
}

//free the memory of an existing program, destroying it
void freeprogram(program *p) {
	free(p->instructions);
	free(p);
}

// add a token to a program
// if the token is TOKEN_EMPTY, it is discarded and the program is unaffected.
// if token is TOKEN_LABEL, the label is added to the program with the next address that will be filled with an instruction.
// if token is TOKEN_INSTRUCTION, the instruction is added to the end of the instruction list.
void addtoken(program *p, token t) {
	switch (t.type) {
		case TOKEN_INSTRUCTION:
			if (p->instrcount == p->instrcap) {
				//before adding the instruction, make sure we have enough room by growing the dynamic array if needed
				p->instrcap*= 1.5f;
				realloc(p->instructions, p->instrcap * sizeof(instruction));
			}
			//append the instruction
			p->instructions[p->instrcount++] = t.instr;
			break;
		case TOKEN_LABEL:
			t.lbl.address = p->instrcount;	//give the label the next address that wilkl be allocated
			p->labels[p->labelcount++] = t.lbl; //and add it to the labels array
			break;
		case TOKEN_EMPTY:
			break; //do nothing
	}
}

// link the program's instructions to its labels.
// for each instruction with an ARG_LABEL, see if a label with that name exists
// in the program. if so, change the arg to ARG_ADDRESS with the address of that label.
// if no label exists by that name, leave the arg as is.
void link(program *p) {
	address i, j;
	for (i = 0; i < p->instrcount; i ++) {
		instruction *instr = p->instructions + i;
		if (instr->arg.type == ARG_LABEL) {
			for (j = 0; j < p->labelcount; j ++) {
				label *lbl = p->labels + j;
				if (strcmp(instr->arg.i, lbl->id) == 0) {
					instr->arg.type = ARG_ADDRESS;
					instr->arg.a = lbl->address;
					break;
				}
			}
		}
	}
}

//run the program!
void run(program *p) {
	//some handy dandy macros (as usual)
		#define s p->stack					// s <=> stack
		#define sp p->stackp					// sp <=> stack pointer
		#define ip p->instrp					// ip <=> instruction pointer
		#define addr instr->arg.a			// addr <=> address argument of current instruction
		#define num instr->arg.n			// num <=> number argument of current instruction
		#define push(v) (s[sp ++] = v)	// put v on top of stack
		#define pop() (-- sp)				// delete top value of stack (do not return it)
		#define peek() (s[sp - 1])			// return the top value of stack (do not alter stack)
	number a, b;	//operand locals; for use when neede
	address i;		//for that one for-loop down there
	while (ip < p->instrcount) {
		instruction *instr = p->instructions + ip;
		char ipinc = 1;			// instruction pointer increment
		switch(instr->op) {
			//stack operators. pretty self explanatory
			case OP_PUSH:		push(num); break;
			case OP_POP:		pop(); break;
			case OP_PEEK:		printf("%d\n", peek()); break;
			//duplicate top value (peek n push)
			case OP_DUPE:		a = peek(); push(a); break;
			//arithmetic (pop 2 operands, push 1 result)
			case OP_ADD:		a = peek(); pop(); b = peek(); pop(); push(a + b); break;
			case OP_SUB:		a = peek(); pop(); b = peek(); pop(); push(a - b); break;
			case OP_MUL:		a = peek(); pop(); b = peek(); pop(); push(a * b); break;
			case OP_DIV:		a = peek(); pop(); b = peek(); pop(); push(a / b); break;
			//rotate top n values up 1
			case OP_ROT:
				a = peek();
				for (i = sp - 1; i > sp - num; i --) {
					s[i] = s[i - 1];
				}
				s[sp - num] = a;
				break;
			//jump operators (notice how they set ipinc to 0: this is so that the destination address isnt skipped)
			case OP_CALL:		push(ip + 1); //CALL is the same as BRANCH, just push next instruction pointer first
			case OP_BRANCH:	ip = addr; ipinc = 0; break;
			//jump-if operators (just look at the campatison operators)
			case OP_BRZ:		if (peek() == 0) { ip = addr; ipinc = 0; } break;
			case OP_BGZ:		if (peek() > 0) { ip = addr; ipinc = 0; } break;
			case OP_BLZ:		if (peek() < 0) { ip = addr; ipinc = 0; } break;
			//jump to the address on top of the stack (remove this value)
			case OP_RETURN:	ip = peek(); pop(); ipinc = 0; break;
			//end execution
			case OP_HCF:		ip = p->instrcount; ipinc = 0; break;
		}
		ip += ipinc;
	}
		// get rid of the handy dandy macros :c
		#undef s
		#undef sp
		#undef ip
		#undef addr
		#undef num
		#undef push
		#undef pop
		#undef peek
}

//main routine
int main(int argc, char **argv) {
	size_t n = 0;	// use n = 0 for getline so we dont have to malloc ourselves
	char *line;		// pointer to lien buffer
	program *p = newprogram(); // init program
	ssize_t status = getline(&line, &n, stdin); // get first line
	while (status > 0) {
		// until we run out of them, tokenize lines and add them to the program
		token t = parseline(line);
		addtoken(p, t);
		status = getline(&line, &n, stdin);
	}
	free(line);	// manually free the last line
	link(p);	// link the program
	run(p);	// run the program
	freeprogram(p);	// delete the program
	return 0;
}

#endif
