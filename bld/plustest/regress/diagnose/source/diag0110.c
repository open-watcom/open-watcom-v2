template <class T>
    void foo( T x )
    {
	x.a = 1;
    }

void foo( double );

void foo()
{
    foo(1);
}
