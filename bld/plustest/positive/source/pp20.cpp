#include "fail.h"

#if 0
#ifdef ack$2
#endif
#ifndef ack$$M#
#endif
#else
#if 1
int x;
#elif 1$3
#endif
#endif

int main() {
    ++x;
    if( x != 1 ) fail(__LINE__);
    _PASS;
}
