// binding of exception-specs in function templates
template <class T>
    T foo( T x, T y ) throw(int,T)
    {
	return x + y;
    }

int foo( int z )
{
    return foo(z,z);
}
