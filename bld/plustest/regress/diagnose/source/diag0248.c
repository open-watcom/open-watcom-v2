//             B
//            /
//       A   C
//        \ /
//         D

// single level positive test of this is in overload/class05.cpp

class A { public: int a; };
class B { public: int b; };
class C : public B { public: int c; };
class D : public A, public C { public: int d; };

void f1( A ** );
void f1( B ** );

void f2( A **);
void f2( C **);

void f3( A * );
void f3( B * );

void g1( D **pd )
{
    f1( pd );	// should say can't get there from here
}

void g2( D **pd )
{
    f2( pd );	// should say can't get there form here NOT ambiguous
}


void g3( D *pd )
{
    f3( pd );	// OK
}

