#include "fail.h"
// 93/12/07 -- jww -- changed Int::Int, Int::operator= to take const ref arg.s

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct Int {
   int val;
   Int (int x = 0)         { val = x;     }
   Int (Int const  &x)     { val = x.val; }
   Int &operator= (Int const &x) { val = x.val; return *this; }
   operator int ()         { return val;  }
};
class Exp : public Int {
   Exp (Int x): Int (x) {}
   friend Exp operator* (Int x);
   friend Int operator* (Int x, Exp y);
};

Exp operator* (Int x) {
    return Exp (x);
}

Int operator* (Int x, Int y) {
    return Int (x.val * y.val);
}

Int operator* (Int x, Exp y) {
    assert (y.val >= 0);
    int z = 1;
    for (int i = 1; i <= y.val; i++) z *= x.val;
    return Int (z);
}

int main () {
    Int x (2);
    Int y (3);
    Int z;
    z = x ** y;
    if( z != 8 ) fail(__LINE__);
    _PASS;
}
