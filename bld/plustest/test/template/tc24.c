// some complex binding (Borland fails this one)
template <class T>
    struct T1 {
	T f1;
    };

template <class T>
    struct T2 {
	T f2;
    };

template <class T>
    struct T3 {
	T f3;
    };

template <class T>
    struct TR1 {
	T f1;
    };

template <class T>
    struct TR2 {
	T f2;
    };

template <class T>
    struct TR3 {
	T f3;
    };

template <class T>
    TR1< TR2< TR3<T *> *> > foo( T, T1< T2< T3<T> > > )
    {
	TR1< TR2< TR3<T *> *> > x;
	return x;
    }


void foo()
{
    T1< T2< T3<int> > > nested;
    foo( 0, nested );
}
