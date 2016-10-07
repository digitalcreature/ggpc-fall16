#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef unsigned int integer;
typedef unsigned long long natural;

typedef struct segment {
	integer a;
	integer b;
} segment;

typedef unsigned char bool;

void *malloc_checked(size_t size) {
	void *block = malloc(size);
	if (block) {
		return block;
	}
	else {
		fprintf(stderr, "Could not allocate block of size %ld\n", size);
		exit(1);
		return NULL;
	}
}

void *realloc_checked(void *block, size_t size) {
	block = realloc(block, size);
	if (block) {
		return block;
	}
	else {
		fprintf(stderr, "Could not allocate block of size %ld\n", size);
		exit(1);
		return NULL;
	}
}

#define malloc malloc_checked
#define realloc realloc_checked

#define INITIAL_CAP 16
#define LOAD_FACTOR 1.5f

//yeah im using globals, fight me
segment *segs;							//dynamic array
natural segcount;				//dynamic array count (how many are stored)
natural segcap;		//dynamic array capacity (how much room has been allocated)

void init() {
	if (segs) free(segs);
	segs = malloc(INITIAL_CAP * sizeof(segment));
	segcount = 0;
	segcap = INITIAL_CAP;
}

natural end() {
	natural total = 0;
	natural i;
	for (i = 0; i < segcount; i ++) {
		total += (segs[i].b - segs[i].a);
	}
	free(segs);
	segs = NULL;
	printf("%llu\n", total);
	return total;
}

void adjust(natural newcount) {
	natural oldcap = segcap;
	while (newcount > segcap) {
		segcap *= LOAD_FACTOR;
	}
	if (oldcap != segcap) {
		realloc(segs, segcap);
	}
}

bool in(integer x, segment s) {
	return (x >= s.a) && (x <= s.b);
}


void insert(segment s, natural i) {
	adjust(segcount + 1);
	segcount ++;
	natural j;
	for (j = i + 1; j < segcount; j ++) {
		segs[j] = segs[j - 1];
	}
	segs[i] = s;
}

void delete(natural i) {
	for (i = i; i < -- segcount; i ++) {
		segs[i] = segs[i + 1];
	}
}

void addseg(segment s) {
	if (s.a == s.b) return; //zero length segments dont add anything
	natural i;
	for (i = 0; i < segcount; i ++) {
		segment *seg = segs + i;
		bool a_in = in(s.a, *seg);
		bool b_in = in(s.b, *seg);
		if (a_in && b_in) return;
		if (b_in) {

		}
	}
	//case 1: no overlap
	adjust(segcount + 1);
	segs[segcount ++] = s;
}

segment stoseg(char *s) {
		#define skipspace() for ( s = s; isspace(*s); s++) {}
		#define parseinteger(i) for (s = s; isdigit(*s); s ++) { *i *= 10; *i += *s-'0'; }
	segment seg;
	seg.a = 0;
	seg.b = 0;
	skipspace();
	parseinteger(&seg.a);
	skipspace();
	parseinteger(&seg.b);
	// make sure the endpoints are correctly ordered
	if (seg.b < seg.a) {
		integer c = seg.a;
		seg.a = seg.b;
		seg.b = c;
	}
	return seg;
		#undef skipspace
		#undef parseinteger
}

bool isblanks(char *s) {
	for (s = s; *s; s ++) {
		if (!isspace(*s)) {
			return 0;
		}
	}
	return 1;
}

int main(int argc, char **argv) {
	size_t n = 0;
	char *line;
	ssize_t status = getline(&line, &n, stdin);
	while (status > 0) {
		if (isblanks(line)) {
			//finish this run
			if (segs != NULL) {
				end();
			}
		}
		else {
			if (!segs) {
				init();
			}
			segment seg = stoseg(line);
			addseg(seg);
		}
		status = getline(&line, &n, stdin);
	}
	if (segs) end();
	return 0;
}
