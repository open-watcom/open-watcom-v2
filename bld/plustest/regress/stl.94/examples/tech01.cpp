#include <stdio.h>
#include <iostream.h>
#include <set.h>

#if __WATCOM_REVISION__ >= 8

class A {
public:
     operator int () const {return 1;}
};

typedef set< A,less<A> > setA;

int main () {
    set< setA,less<setA> > sets;
    puts( __FILE__ );
    return 0;
}

#else
int main () {
    puts( __FILE__ );
    return 0;
}
#endif
