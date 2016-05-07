#include <stdio.h>
#include <time.h>

#define CC 39
#define SCC "39"

int fib(int n){
	if(n < 2) return 1;
	else return fib(n-1) + fib(n-2);
}

int tfib(int a, int b, int n){
	if(n==0) return a;
	else return tfib(b, a+b, n-1);
}

int main(int argc, char *argv[]){
	clock_t t0 = clock();
	printf("fib: %d\n", fib(CC));
	printf("fib("SCC") ms: %d\n", clock() - t0);

	t0 = clock();
	printf("tfib: %d\n", tfib(1, 1, CC));
	printf("tfib(1, 1, "SCC") ms: %d\n", clock() - t0);
	return 0;
}
