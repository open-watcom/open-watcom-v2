#include "fail.h"

struct A {
    char a,b,c,d;
};

struct A x = { 'a', 'b', 'c', 'd' };

struct A foo() {
    return x;
}

struct A /*inline*/ ifoo() {
    return x;
}

int main() {
    if( foo().a != 'a' ) fail(__LINE__);
    if( foo().b != 'b' ) fail(__LINE__);
    if( foo().c != 'c' ) fail(__LINE__);
    if( foo().d != 'd' ) fail(__LINE__);
    if( ifoo().a != 'a' ) fail(__LINE__);
    if( ifoo().b != 'b' ) fail(__LINE__);
    if( ifoo().c != 'c' ) fail(__LINE__);
    if( ifoo().d != 'd' ) fail(__LINE__);
    _PASS;
}
