#define NEKO_SOURCES
#include "fib.h"

int fib(int n){
	if(n < 2) return 1;
	else return fib(n-1) + fib(n-2);
}

int tfib(int a, int b, int n){
	if(n==0) return a;
	else return tfib(b, a+b, n-1);
}

static value nfib(value n){
	val_check(n,int);
	return alloc_int(tfib(1, 1, val_int(n)));
}

DEFINE_PRIM(nfib, 1);
