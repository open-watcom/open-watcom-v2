template <class T>
    int foo( int * );
template <class T>
    T bar( int * );
template <class T>
    T sam( T * );
template <class T>
    T x[1];
template <class T>
    class {
    };

template <class T,class Q>
    class DD {
	T a;
	Q x;
    };

template <class T,class Q>
    class UU;

template <class T,class W>
    void f1( DD<T,int> * );

template <class T,class W>
    void f2( DD<T,W> * );

template <class T,class W>
    void f3( UU<T,int> * );

template <class T,class W>
    void f4( UU<T,W> * );

template <class T,class W>
    void f5( T W::* );

template <class T,class W>
    void f6a( int W::* );

template <class T,class W>
    void f6b( T T::* );

template <class T,class W>
    void f7( T (*)( W, W * ) );

template <class T,class W>
    void f8( T (*)( T, W * ) );

template <class F3,class F2,class F1>
    void f9( F3 *, F2 *, F1 *, F1 * );
template <class F1,class F3,class F2>
    void f9( F1 *, F3 *, F2 *, F2 * )
    {
    }
template <class F2,class F3,class F1>
    void f9( F2 *, F3 *, F1 *, F1 * )
    {
    }

void foo()
{
}
