#include "fail.h"

struct S {
    S( int x ) : a(x) {}
    int a;
};

int main()
{
    if( ((S)-1).a != -1 ) fail(__LINE__);
    _PASS;
}
