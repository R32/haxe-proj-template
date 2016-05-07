#include "neko.h"

C_FUNCTION_BEGIN
/* if NEKO_SOURCES || NEKO_STANDALONE then  EXTERN = EXPORT else  EXTERN = IMPORT; see "neko.h" */
EXTERN int fib(int n);
EXTERN int tfib(int a, int b, int n);

C_FUNCTION_END