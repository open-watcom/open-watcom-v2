template <class T>
    void zap( T __based(__segname("ASDF")) *p, T __based(__segname("ASDF")) *x )
    {
	*p = *x;
    }

void foo( char volatile __based(void) *p )
{
    zap( p, p );	// T should not bind
}
