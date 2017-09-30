#include "fail.h"

struct foo {
    int a[];
};

struct foo const x = { 0, -1, -2, -3, -4, -5, -6 };
struct foo const y = { 0, -1, -2, -3, -4, -5, -6, -7 };
struct foo const z = { 0, -1, -2, -3, -4, -5 };

int main() {
    if( x.a[0] != -0 ) _fail;
    if( x.a[1] != -1 ) _fail;
    if( x.a[2] != -2 ) _fail;
    if( x.a[3] != -3 ) _fail;
    if( x.a[4] != -4 ) _fail;
    if( x.a[5] != -5 ) _fail;
    if( x.a[6] != -6 ) _fail;
    if( y.a[0] != -0 ) _fail;
    if( y.a[1] != -1 ) _fail;
    if( y.a[2] != -2 ) _fail;
    if( y.a[3] != -3 ) _fail;
    if( y.a[4] != -4 ) _fail;
    if( y.a[5] != -5 ) _fail;
    if( y.a[6] != -6 ) _fail;
    if( y.a[7] != -7 ) _fail;
    if( z.a[0] != -0 ) _fail;
    if( z.a[1] != -1 ) _fail;
    if( z.a[2] != -2 ) _fail;
    if( z.a[3] != -3 ) _fail;
    if( z.a[4] != -4 ) _fail;
    if( z.a[5] != -5 ) _fail;
    _PASS;
}
