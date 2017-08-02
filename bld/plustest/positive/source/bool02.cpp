#include "fail.h"

#ifdef __WATCOM_BOOL__

enum E { A,B,C,D };
void foo();

bool a[10];
const int x = 5;

int foo( E * p, int *q ) {
    int i = 0;
    a[i++] = p;
    a[i++] = C;
    a[i++] = *p;
    a[i++] = *q;
    a[i++] = x;
    return i;
}

int main() {
    E e( D );
    int i(__LINE__);
    int m = foo( &e, &i );
    for( int j = 0; j < m; ++j ) {
	if( a[j] != true ) _fail;
	if( a[j] == false ) _fail;
    }
    _PASS;
}
#else
ALWAYS_PASS
#endif
