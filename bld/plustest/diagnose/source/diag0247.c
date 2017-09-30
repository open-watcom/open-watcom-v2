// test one 

// f1(int, Y1) is not prevented from being viable when called with f(X1,Y1)
// just because it's first parameter X1 can converted to int via two different
// UDCFs (which tie).  Therefore, f1 is viable, as is f2, and they tie, so they
// are ambiguous.

struct X1
{
    int i;
    X1(int j) : i(j) {};
    operator short();
    operator char();
};

struct Y1
{
    int i;
    Y1(int j) : i(j) {};
    operator short();
};

void f1( int, Y1 );
void f1( X1, int);

test_one()
{
    X1 x1(1);
    Y1 y1(1);

    f1( x1, y1 ); // in fact, it should find ambiguity, not pass this
}

// test two 

// Argument passing is copy initialization, so ctors and UDCF can be used
// in the class to class case.
// B2 can go to A2 via ctor, or to C2 via UDCF, so f2 is ambiguous for
// the paramters B2.

class B2;
class A2 { public: A2( B2 ); };
class C2 {};
class B2 { public: operator C2(); };

A2::A2(B2) {}

void f2(A2) {}
void f2(C2) {}

test_two()
{
    B2 b2;

    f2(b2);
}


// test three (old udcnv2)

// B3 can go to A3 via UDCF or ctor, or B3 can go to int via UDCF.
// As a result, f3 is ambiguous ( f3(A3) is not made non-viable just
// because it's first paramter is ambiguous).

struct B3;

struct A3 {
    A3() {};
    A3( B3 ) {};
};

struct B3 {
    operator A3 () { return A3(); }
    operator int () { return 0; }
};

void f3( A3 ) {}
void f3( int ) {}
void f3( long ) {} // needs a standard conversion also, so shouldn't be listed
		   // as a possible function

test_three()
{
    B3 b3;
    f3( b3 );
}

// test four

// from footnote in section [over.best.ics]
// B4 can go to A4 via ctor of UDCF, and to C4 via ctor only.  F4(B4) is
// not made non-viable becuase it's first parameter is ambiguous.

struct B4;
struct A4 { A4( B4& ) ; };
struct B4 { operator A4() ; };
struct C4 { C4( B4& ) ; };

void f4(C4) {}
void f4(A4) {}

test_four()
{
    B4 b4;
    f4(b4);
}

// test five
// B5 goes to C5 via ctor or UDCF

class B5;
class C5 { public : C5( B5& ); };
class B5 { public : operator C5(); };

void f5(C5);

test_five()
{
    B5 b5;
    f5(b5);
}

// test 6

// B6 can go to A6 via UDCF or ctor, or B6 can go to A6 via UDCF.
// As a result, f6 is ambiguous ( f6(A6) is not made non-viable just
// because it's first paramter is ambiguous).

struct B6;

struct A6 {
    A6( B6 ) {};
};

struct C6 {};

struct B6 {
    operator A6 ();
    operator C6 ();
};

void f6( A6 ) {}
void f6( C6 ) {}

test_six()
{
    B6 b6;
    f6( b6 );
}


// mainline

void main()
{
    test_one();
    test_two();
    test_three();
    test_four();
    test_five();
    test_six();
}

