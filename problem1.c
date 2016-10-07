// Author: Tanner Grehawick
//	Email: tgrehawi@gmail.com
// Problem 1: Overlapping Segments

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef unsigned int number;	//keepin stuff standard (and readable)

// number line segment struct
// represents a segment of the number line from a to b, inclusive
typedef struct segment {
	number a;	// first endpoint
	number b;	// last endpoint
} segment;

typedef unsigned char bool;	//alright this one is just for readability

// always check your allocs!
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

// ...and your reallocs
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

// like they arent even there....
#define malloc malloc_checked
#define realloc realloc_checked

// yeah im using globals, fight me
segment *segs;		// segment list (dynamic array)
number segcount;	// segment count (how many segments in the list)
number segcap;		// segment capacity (how many segments room has been allocated for)

// initial capacity for segment list
#define INITIAL_CAP 16

// start a new segment list
void start() {
	if (segs) free(segs);
	segs = malloc(INITIAL_CAP * sizeof(segment));
	segcap = INITIAL_CAP;
	segcount = 0;
}

// finish the current segment list
// print the total area covered by the segments
number end() {
	number total = 0;
	number i;
	for (i = 0; i < segcount; i ++) {
		total += (segs[i].b - segs[i].a);
	}
	free(segs);
	segs = NULL;
	printf("%u\n", total);
	return total;
}

// adjust the capacity of the segment list to accomodate at least a certain count
void adjust(number count) {
	number newcap = segcap;
	while (count >= newcap) {
		newcap *= 1.5f;
	}
	if (newcap != segcap) {
		//only realloc if necessary
		realloc(segs, newcap * sizeof(segment));
	}
}

// returns true if segment a is contained by segment b,
//	i.e., both of a's endpoints are inside b's (inclusive)
bool seginseg(segment a, segment b) {
	return
		(a.a >= b.a) && (a.a <= b.b) &&
		(a.b >= b.a) && (a.b <= b.b);
}

// insert segment s into segment list at index i
// existing segments from i to segcount are shifted to the right by 1
void insert(segment s, number i) {
	adjust(++ segcount);
	number j;
	for (j = segcount - 1; j > i; j --) {
		segs[j] = segs[j - 1];
	}
	segs[i] = s;
}

/*	add the next segment to the list. handles overlap by ensuring the rules of the list are respected
 	the rules of the list are as follows:
		1) no segments in the list overlap
		2) segments are in acscending order
	new segments fall into one of three categories with respect to existing segments:
		1) no overlap: insert new segment at correct index in list.
		2) zero length / fully contained by an existing segment: discard
		3) overlaps 1 or more existing segments:
				first find the first and last exisitng segments that the new segment overlaps
				(these can be the same segment if only one overlaps)
				then replace all of these segments with the new segment, shortening the list if necessary.
				if needed, the new segment is extended to match the full coverage of the old segments it replaces.
*/
void addseg(segment new) {
	if (new.a == new.b) return; //zero length segments dont add anything. discard
	segment *A = NULL;	//pointer to the first overlapped existing segment
	segment *B = NULL;	//pointer to the last overlapped existing segment
	number i;
	for (i = 0; i < segcount; i++) {
		segment seg = segs[i];
		if (seg.b < new.a) continue;	//skip existing segments before new
		if (seginseg(new, seg)) return;	//if new is contained in seg, theres nothing to add. discard
		if (A == NULL) {
			// we are still looking for the first segment to replace
			if (seg.a <= new.b) {		// seg overlaps new
				A = segs + i;
				B = A;	//A is the first candidate for B
				if (seg.a < new.a) {
					// if the existing segment started before the new one, extend the new one to match
					new.a = seg.a;
				}
			}
			else {							// seg is after new
				//new doesnt overlap any segments, so just insert it and return
				insert(new, i);
				return;
			}
		}
		if (A != NULL) {
			// we found A, now were looking for B
			if (new.b >= seg.a) {
				// if there is an overlap, the segment is the newest B candidate
				B = segs + i;
				if (new.b <= seg.b) {
					// if the new segment ends before the existing segment, then the existing segment
					// must be the last overlapping segment.
					// make sure that the new segment ends when it does, and not too soon.
					new.b = seg.b;
					break;	// we have A and B, time to replace them
				}
			}
			else {
				// if there is no overlap, this means that the new segment ended between this segment
				// and the last one, so the last one is the final overlapped segment (B).
				break;	// we have A and B, time to replace them
			}
		}
	}
	// if we made it this far, it means either:
	// 1) we found a range of 1 or more segments that the new segment overlaps
	// 2) we went through the entire list and this segment is after all existing segments
	//		(or the list is empty)
	if (A != NULL) {
		// if we found a range of segments to replace, do so.
		*A = new;			// assign the new segment at the beginning of the range
		if (A != B) {
			// if there is more than one segment to replace,
			// shift down the rest of them to close the gap
			segment *end = segs + segcount;		// pointer to the (previous) last segment in the list
			segcount -= (B - A);						// adjust segment count
			while (++A != end && ++B != end) {
				//shift everything over
				*A = *B;
			}
		}
	}
	else {
		// if all else fails, insert the segment at the end of the list where it belongs.
		insert(new, segcount);
	}
}

//parse a segment from a string
segment stoseg(char *s) {
	// defines. i am aware of sscanf, but this is more flexible (without using a regex lib)
		#define skipspace() for ( s = s; isspace(*s); s ++) {}
		#define parsenumber(i) for (s = s; isdigit(*s); s ++) { *i *= 10; *i += *s-'0'; }
	segment seg;
	seg.a = 0;
	seg.b = 0;
	skipspace();				//skip any leading space
	parsenumber(&seg.a);		//parse the first end point
	skipspace();				//skip any space in the middle
	parsenumber(&seg.b);		//parse the second end point
	// (the rest of the string is ignored)

	// make sure the endpoints are correctly ordered
	if (seg.b < seg.a) {
		number c = seg.a;
		seg.a = seg.b;
		seg.b = c;
	}
	return seg;
	//undef them defs
		#undef skipspace
		#undef parsenumber
}

//return true if s is only whitespace
bool isblanks(char *s) {
	for (s = s; *s; s ++) {
		if (!isspace(*s)) {
			return 0;
		}
	}
	return 1;
}

//main routine
int main(int argc, char **argv) {
	size_t n = 0;	// giving getline a size of 0 handle its own dynamic allocs
	char *line;		// pointer to line buffer (dynamically allocated by getline)
	ssize_t status = getline(&line, &n, stdin);	// get the first line
	while (status > 0) {
		if (isblanks(line)) {
			if (segs != NULL) {
				// if we reached a blank line and we still have a segment list,
				// finish it. (otherwise just wait until you get a non-blank line again)
				end();
			}
		}
		else {
			if (!segs) {
				//if we don't have a list of segments, this is the first element.
				//start anew
				start();
			}
			//parse the next segment and add it to the list
			addseg(stoseg(line));
		}
		//get the next line
		status = getline(&line, &n, stdin);
	}
	if (segs) end();	//wrap up the last segment list if its still around
	free(line);	// ...i still have to manually free the last line buffer
	return 0;
}
