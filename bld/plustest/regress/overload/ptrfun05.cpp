#include "dump.h"

#if __WATCOM_REVISION__ >= 8
struct S {

    int foo( int ) { GOOD; return 0; } ;	// (1)
    static int foo( double ) { return 0; }; 	// (2)
    int moo( int ) { GOOD; return 0; }; 	// (3)

    static int goo( double ) { return 0; }; 	// (4)
    void extra();
    void extra2();
    static int data1;
    double data2;
    double data3;
};

static int S::data1 = 0;

typedef int (S::* MPTR)(int);

/////////////////////////
// data initialization //
/////////////////////////
//    ptr vs mbrptr
///   individual vs. array
////  & vs no &
///// function vs data
////////////////////////

//   member pointer
///  individual
//// using &
MPTR f1 = &S::moo;	// not overloaded (3)
MPTR f2 = &S::foo;	// overloaded (1)
double S::*d1 = &S::data2;
//// not using &
/////// see ptrfun06 which is compiled -ze ///////

///  array
//// using &
int (S::*arr[2])(int) = { &S::moo,	// overloadeded (3)
                          &S::foo };	// not overloaded (1)
double S::*darr[2] = { &S::data2, &S::data3 };

//// not using &
/////// see ptrfun06, which is compiled -ze ///////

//   pointer
///  individual
//// using &
void S::extra(void) // need to be in scope of S to just use goo and &goo
{
int (*f5)(double) = &goo; // not overloaded (4)
int (*f6)(double) = &foo; // overloaded (2)
int *d3 = &data1;
//// not using &
int (*f7)(double) = goo; // not overloaded (4)
int (*f8)(double) = foo; // overloaded (4)
///  array
//// using &
int (*arr2[2])(double) = { &goo,	// not overloaded (4)
    			   &foo };	// overlaoded (2)
int *darr2[2] = { &data1, &data1 };
//// not using &
int (*arr3[2])(double) = { goo,		// not overloaded (4)
    			   foo };	// overlaoded (2)

}

void f( int (S::*p)(int) )
{
    S s;

    (s.*p)(0);
}

int main()
{
    f( f1 );
    CHECK_GOOD(8);
    f( f2 );
    CHECK_GOOD(6+8);
    f( arr[0] );
    CHECK_GOOD(8+6+8);
    f( arr[1] );
    CHECK_GOOD(6+8+6+8);
    return errors != 0;
}
#else
int main( void )
{
  FORCE_GOOD(8);
  CHECK_GOOD(8);
  FORCE_GOOD(6);
  CHECK_GOOD(6+8);
  FORCE_GOOD(8);
  CHECK_GOOD(8+6+8);
  FORCE_GOOD(6);
  CHECK_GOOD(6+8+6+8);
  return errors != 0;
}


#endif
