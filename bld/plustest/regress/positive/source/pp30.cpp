#include "fail.h"

#define FOO /*
testing */	1

int hi = FOO;

int main() {
    if( hi != 1 ) fail(__LINE__);
    _PASS;
}
