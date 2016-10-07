// Author: Tanner Grehawick
//	Email: tgrehawi@gmail.com
// Problem 2: Stack-based Computer Architecture Simulation

#ifndef _INCLUDE
	#define startops typedef enum
	#define op(name) name,
	#define endops ops
#else
	#undef startops
	#undef op
	#undef endops
	#define startops const char *opnames[OPS_COUNT] =
	#define op(name) #name,
	#define endops
#endif

#ifndef _INCLUDE

	#include <stdio.h>
	#include <stdlib.h>
	#include <ctype.h>
	#include <stdint.h>

	#define OPS_COUNT 16
	typedef int32_t number;

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


#define skipspace() while (*s && isspace(*s)) s++
#define parsestr() while (*s && !isspace(*s)) *(b++) = *(s++); *b = 0

int main(int argc, char **argv) {
	size_t n = 0;
	char *line;
	char *s, *b;
	char buf1[32], buf2[32];
	ssize_t status = getline(&line, &n, stdin);
	while (status > 0) {
		s = line;
		while (*s && *s != '#') s ++;
		if (*s == '#') *s = 0;
		s = line;
		skipspace();
		b = buf1; parsestr();
		skipspace();
		b = buf2; parsestr();
		status = getline(&line, &n, stdin);
	}
	free(line);
	return 0;
}

#endif
