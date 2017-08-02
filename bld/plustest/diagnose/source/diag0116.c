void x1 () throw (struct S)
{
	struct S { int mbr; };  // completes previous?
}
void x5 () throw (enum E1 { red, green, blue });
void x6 () throw (enum E2 { cyan, magenta, yellow });

#pragma template_depth ack
#pragma template_depth 13
template <class T>
    void foo( T & x )
    {
	struct D {
	    T &x;
	    D( T & x ) : x(x) {}
	} dummy(x);

	foo( dummy );
    }

void foo()
{
    int x;

    foo(x);
}
