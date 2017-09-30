// fails 16-bit compiler -d2
#include "fail.h"
 
volatile unsigned long ch[32];

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
