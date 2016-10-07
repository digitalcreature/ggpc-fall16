OPTIONS=-Wall -g
problem1: clean1
	gcc problem1.d/problem1.c -o problem1 $(OPTIONS)
problem2: clean2
	gcc problem2.d/problem2.c -o problem2 $(OPTIONS)
problem3: clean3
	gcc problem3.d/problem3.c -o problem3 $(OPTIONS)

clean1:
	rm -f problem1
clean2:
	rm -f problem1
clean3:
	rm -f problem1
