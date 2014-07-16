#include "fail.h"
#include <stdlib.h>

typedef struct _STABLE {
    int size;
} STABLE, *PSTABLE;

void Dummy(unsigned long hTable)
{
	PSTABLE pSTable = (PSTABLE)hTable; 

	if (0x12345678 == hTable)
	   return;
	
	pSTable->size = 100;
} 

int main()
{
    unsigned long hTable;

    hTable=strtoul("12345678", NULL, 16);
    Dummy(hTable);
    _PASS;
}
