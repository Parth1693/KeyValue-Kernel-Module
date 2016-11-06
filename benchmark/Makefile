all: test1 test2 test3 test4 test5 benchmark validate 

test1: test1.c
	$(CC) -g -O0 test1.c -o test1 -lckv
test2: test2.c
	$(CC) -pthread -g -O0 test2.c -o test2 -lckv
test3: test3.c
	$(CC) -g -O0 test3.c -o test3 -lckv
test4: test4.c
	$(CC) -g -O0 test4.c -o test4 -lckv
test5: test5.c
	$(CC) -pthread -g -O0 test5.c -o test5 -lckv
benchmark: benchmark.c
	$(CC) -g -O0 benchmark.c -o benchmark -lckv

validate: validate.c
	$(CC) -g -O0 validate.c -o validate -lckv

clean:
	rm -f benchmark validate test1 test2 test3 test4 test5