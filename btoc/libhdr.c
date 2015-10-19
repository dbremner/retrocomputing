#include <setjmp.h>

#define true -1
#define false 0
#define TRUE true
#define FALSE false
#define bytesperword 4
#define BYTESPERWORD 4
#define MAXINT 0x7FFFFFFF
#define MININT 0x80000000
#define ENDSTREAMCH (-1)

typedef long word;
typedef char* string;

extern word (*wrch_fn)();
extern word (*rdch_fn)();

extern word *getvec();

extern word result2;
