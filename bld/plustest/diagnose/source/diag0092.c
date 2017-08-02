template <int size>
    void f1( int x[size] );
template <int size>
    void f1( int x[size] )
    {
    }
int a;
int b;
template <class F1,class F2,class F3>
    void f2( F1 *, F2 *, F3 *, F3 * );
template <class F1,class F3,class F2>
    void f2( F1 *, F3 *, F2 *, F2 * )
    {
	++a;
    }
template <class F2,class F3,class F1>
    void f2( F2 *, F3 *, F1 *, F1 * )
    {
	++b;
    }

void foo()
{
    int *p;

    f2(p,p,p,p);
}
