int a;
int b;
template <class F1,class F2,class F3>
    void f2( F1 *, F2 *, F3 *, F3 * )
    {
	++a;
    }
template <class F3,class F2,class F1>
    void f2( F1 *, F2 *, F3 *, F3 * )
    {
	++a;
    }

void foo()
{
    int *p;
    char *q;

    f2(p,p,q,q);
}
