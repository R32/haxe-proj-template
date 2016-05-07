#include <stdio.h>
#include <time.h>
#include "fib.h"

#define CC 39
#define SCC "39"

int main(int argc, char *argv[]){
	clock_t t0 = clock();
	printf("fib: %d\n", fib(CC));
	printf("fib("SCC") ms: %d\n", clock() - t0);

	t0 = clock();
	printf("tfib: %d\n", tfib(1, 1, CC));
	printf("tfib(1, 1, "SCC") ms: %d\n", clock() - t0);
	return 0;
}
