// Author: Tanner Grehawick
//	Email: tgrehawi@gmail.com
// Problem 2: Stack-based Computer Architecture Simulation

#ifndef _INCLUDE
	#define startops typedef enum
	#define op(name) name,
	#define endops operator
#else
	#undef startops
	#undef op
	#undef endops
	#define startops const char *opname[OPS_COUNT] =
	#define op(name) #name,
	#define endops
#endif

#ifndef _INCLUDE

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

	void *malloc_checked(size_t size) {
		void *block;
		if ((block = malloc(size))) return block;
		fprintf(stderr, "Could not allocate block of size %ld\n", size);
		exit(1); return NULL;
	}
	void *realloc_checked(void *block, size_t size) {
		if ((block = realloc(block, size))) return block;
		fprintf(stderr, "Could not allocate block of size %ld\n", size);
		exit(1); return NULL;
	}
	#define malloc malloc_checked
	#define realloc realloc_checked

#endif

startops {
	op(PUSH) op(POP) op(DUPE) op(PEEK)
	op(ADD) op(SUB) op(MUL) op(DIV)
	op(ROT) op(BRANCH) op(CALL) op(BRZ)
	op(BGZ) op(BLZ) op(RETURN) op(HCF)
} endops;

#ifndef _INCLUDE
	#define _INCLUDE
	#include "problem2.c"

	typedef struct instruction {
		operator op;
		struct {
			enum { NONE, NUMBER, ID, ADDRESS } type;
			union {
				number n;
				char i[ID_MAXLEN];
				address a;
			};
		} arg;
	} instruction;

	typedef struct label {
		address address;
		char id[ID_MAXLEN];
	} label;

	typedef struct token {
		enum { EMPTY, INSTRUCTION, LABEL } type;
		union {
			instruction instr;
			label lbl;
		};
	} token;

	typedef struct program {
		label labels[LABELS_MAX];
		address labelcount;
		number stack[STACK_SIZE];
		address stackp;
		instruction *instructions;
		address instrcount;
		address instrcap;
		address instrp;
	} program;

token parseline(char *line) {
	int i;
	char *s, *b;
	char buf1[ID_MAXLEN], buf2[ID_MAXLEN];
		#define skipspace() while (*s && isspace(*s)) s++
		#define parsestr() while (*s && !isspace(*s)) *(b++) = *(s++); *b = 0
	s = line;
	while (*s && *s != '#') s ++;
	if (*s == '#') *s = 0;
	s = line;
	skipspace();
	b = buf1; parsestr();
	skipspace();
	b = buf2; parsestr();
		#undef skipspace
		#undef parsestr
	token tkn;
	if (!buf1[0]) {
		tkn.type = EMPTY;
		return tkn;
	}
	for (i = 0; i < OPS_COUNT; i ++) {
		if (strcmp(buf1, opname[i]) == 0) {
			tkn.type = INSTRUCTION;
			tkn.instr.op = i;
			if (!buf2[0]) {
				tkn.instr.arg.type = NONE;
			}
			else {
				if (sscanf(buf2, "%d", &(tkn.instr.arg.n))) {
					tkn.instr.arg.type = NUMBER;
				}
				else {
					strcpy(tkn.instr.arg.i, buf2);
					tkn.instr.arg.type = ID;
				}
			}
			return tkn;
		}
	}
	int b1len = strlen(buf1);
	if (b1len > 1 && buf1[b1len - 1] == ':') {
		buf1[b1len - 1] = 0;
		tkn.type = LABEL;
		strcpy(tkn.lbl.id, buf1);
		tkn.lbl.address = 0;
		return tkn;
	}
	tkn.type = EMPTY;
	return tkn;
}

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

void freeprogram(program *p) {
	free(p->instructions);
	free(p);
}

void addtoken(program *p, token t) {
	switch (t.type) {
		case INSTRUCTION:
			if (p->instrcount == p->instrcap) {
				p->instrcap*= 1.5f;
				realloc(p->instructions, p->instrcap);
			}
			p->instructions[p->instrcount++] = t.instr;
			break;
		case LABEL:
			t.lbl.address = p->instrcount;
			p->labels[p->labelcount++] = t.lbl;
			break;
		case EMPTY:
			break;
	}
}

void link(program *p) {
	address i, j;
	for (i = 0; i < p->instrcount; i ++) {
		instruction *instr = p->instructions + i;
		if (instr->arg.type == ID) {
			for (j = 0; j < p->labelcount; j ++) {
				label *lbl = p->labels + j;
				if (strcmp(instr->arg.i, lbl->id) == 0) {
					instr->arg.type = ADDRESS;
					instr->arg.a = lbl->address;
					break;
				}
			}
		}
	}
}

void run(program *p) {
		#define s p->stack
		#define sp p->stackp
		#define ip p->instrp
		#define addr i->arg.a
		#define num i->arg.n
		#define push(v) (s[sp ++] = v)
		#define pop() (-- sp)
		#define peek() (s[sp - 1])
	number a, b;
	address j;
	while (ip < p->instrcount) {
		instruction *i = p->instructions + ip;
		char inc = 1;
		switch(i->op) {
			case PUSH:		push(num); break;
			case POP:		pop(); break;
			case DUPE:		a = peek(); push(a); break;
			case PEEK:		printf("%d\n", peek()); break;
			case ADD:		a = peek(); pop(); b = peek(); pop(); push(a + b); break;
			case SUB:		a = peek(); pop(); b = peek(); pop(); push(a - b); break;
			case MUL:		a = peek(); pop(); b = peek(); pop(); push(a * b); break;
			case DIV:		a = peek(); pop(); b = peek(); pop(); push(a / b); break;
			case ROT:
				a = peek();
				for (j = sp - 1; j > sp - num; j --) {
					s[j] = s[j - 1];
				}
				s[sp - num] = a;
				break;
			case BRANCH: ip = addr; inc = 0; break;
			case CALL:		push(ip); ip = addr; inc = 0; break;
			case BRZ:		if (peek() == 0) { ip = addr; inc = 0; } break;
			case BGZ:		if (peek() > 0) { ip = addr; inc = 0; } break;
			case BLZ:		if (peek() < 0) { ip = addr; inc = 0; } break;
			case RETURN:	ip = peek(); pop(); break;
			case HCF:		ip = p->instrcount; inc = 0; break;
		}
		ip += inc;
	}
		#undef s
		#undef sp
		#undef ip
		#undef addr
		#undef num
		#undef push
		#undef pop
		#undef peek
}

int main(int argc, char **argv) {
	size_t n = 0;
	char *line;
	program *p = newprogram();
	ssize_t status = getline(&line, &n, stdin);
	while (status > 0) {
		token t = parseline(line);
		addtoken(p, t);
		status = getline(&line, &n, stdin);
	}
	free(line);
	link(p);
	run(p);
	freeprogram(p);
	return 0;
}

#endif
