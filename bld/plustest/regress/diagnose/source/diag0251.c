// compiled -za

struct S {

    static int foo( double ); 	// (2)
    int foo( int );		// (1)
    int moo( int ); 		// (3)

    static int goo( double ); 	// (4)

    int goo( int (S::*)( int ) );	// will match (1)
    int goo( int (*)( double ) );	// will match (2)

    void fun() {
        goo( &S::foo );	// ambig 
    }
    static int data1;
    double data2;
    double data3;
    extra();
};

struct T {
    int foo( int );	
    int moo( int ); 
    int (T::*f3)(int) = &T::foo;	// error
    int (T::*f11)(int) = &T::moo;	// error
};

int (S::*f1)(double) = &S::foo; // error: ptr to fn -> mptr illegal
				// note: ***error message misleading***
				//        just picks last foo for source in error message
				// MSDEV: error
int (S::*f2)(double) = &S::moo;	// error: selects (2), same as above
				// MSDEV: error
				// note: ***error message misleading***

int (*f4)(int) = &S::foo; // overloaded
			  // MSDEV: error
			  // error: mptr -> ptr to fn illegal

int S::extra(void)
{
  int *d4 = data1;
  int *darr3[2] = { data1, data1 };
  return 0;
}

// ***next two should give error with -za but needs to be fixed***
int (S::*f5)(int) = S::moo; // not overloaded (3)
int (S::*f6)(int) = S::foo; // overloaded (1)
double S::*d2 = S::data3;	// error

int (S::*f7)(int) = moo; // error
int (S::*f8)(int) = foo; // error
double S::*d3 = data3; 	 // error

void main()
{
    S s;

    s.fun();
}

double S::*dbarr[2] = { S::data2, S::data3 };	// error
double S::*dbarr2[2] = { &data2, &data3 };	// error
double S::*d4 = S::data3;	// error

// from [over.over]  (13.4)
struct X {
        int f(int);
	static int f(long);
};
int    (*p2)(int) = &X::f;	// error: mismatch
int (X::*p4)(long) = &X::f;	// error: mismatch
int (X::*p5)(int) = &(X::f);	// error: wrong syntax for pointer to member


int f(double);
int f(int);
int (*pfe)(...) = &f;		// error: type mismatch
