template <class T>
    void zap( T *p, T *x )
    {
	*p = *x;
    }

void foo( char const *p )
{
    zap( p, p );
}
