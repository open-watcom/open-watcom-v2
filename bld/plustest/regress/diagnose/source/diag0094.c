template <class T>
    void zap( T volatile *p, T volatile *x )
    {
	*p = *x;
    }

__segment s;

void foo( char const volatile __based(s) *p )
{
    zap( p, p );	// T should bind to 'char const __based(s)'
}
