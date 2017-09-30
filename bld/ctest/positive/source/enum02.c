#include "fail.h"

typedef enum E { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, va, vb, vc, vd, ve, vf } E;

void check( E e, int i, unsigned line )
{
    if( ((int)(e)) != i ) fail(line);
}

int main() {
    int xx;
    E i;

    for( i = v0, xx=0; i != vf; ++i, ++xx ) {
	check( i, xx, __LINE__ );
    }
    for( i = vf, xx = 0x0f; i != v0; --i, --xx ) {
	check( i, xx, __LINE__ );
    }
    for( i = v0, xx=0; i != vf; i+=1, xx+=1 ) {
	check( i, xx, __LINE__ );
    }
    for( i = (xx=0x0f,vf); i != v0; i-=1, xx-=1 ) {
	check( i, xx, __LINE__ );
    }
    _PASS;
}
