#include "fail.h"

struct B {
};
struct D : B {
};
struct E1 : D {
};
struct F1 : D {
};
struct F2 : F1 {
};

void foo( void * ){}
void foo( D * ){ fail(__LINE__); }

void bar( void * ){ fail(__LINE__); }
void bar( B * ){}

void sam( F1 * ) {}
void sam( D * ) { fail(__LINE__); }

int main() {
    bar( (D*) 0 );	// bar( B * )
    foo( (B*) 0 );	// foo( void * )
    sam( (F2*) 0 );	// sam( F1 * );
    _PASS;
}
