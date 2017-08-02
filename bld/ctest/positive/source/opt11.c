// fails 16/32-bit X86 compiler -d2
#include "fail.h"

#ifdef __I86__
volatile unsigned long ch[32];
#else
volatile unsigned long long ch[32];
#endif

void Inc(int chan);

int main()
{
    ch[5] = 0;
    Inc(5);
    if( ch[5] != 1 ) _fail;
    _PASS;
}

void Inc(int chan)
{
    ch[chan]++;
}
