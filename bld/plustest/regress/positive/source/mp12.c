#include "fail.h"

struct S {
};
typedef void (S::* mp) ( void );

mp ack() {
    return mp();
}
mp foo() {
    return 0;
}

mp zz;

int main() {
    if( ack() != 0 ) _fail;
    if( foo() != 0 ) _fail;
    if( ack() != zz ) _fail;
    if( foo() != zz ) _fail;
    _PASS;
}
