#define HL_NAME(n) mod_##n

#include "hl.h"

typedef struct {int a; int b;} MyThing;

HL_PRIM int HL_NAME(len_sqrt)(MyThing* t) {
	return t->a * t->a + t->b * t->b;
}

DEFINE_PRIM(_I32, len_sqrt, _BYTES); // _BYTES => MyThing*

