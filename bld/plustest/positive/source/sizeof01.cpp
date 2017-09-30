#include "fail.h"

char c;
char &r = c;

enum E { A, B, C, D };

int main()
{
    if(( sizeof E() == sizeof A ) &&
       ( sizeof c == sizeof( char ) ) &&
       ( sizeof r == sizeof( char ) )
    ) {
    } else {
	fail(__LINE__);
    }
    _PASS;
}
