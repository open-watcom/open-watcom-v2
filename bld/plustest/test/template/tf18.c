template <class T1,class T2,int s,void (*err)()>
    struct S {
	T1 v1[s];
	T2 v2[s];
	S()
	{
	    err();
	}
    };

extern void fail();

template <class G,class T>
    volatile S<G,T,1,fail> *foo( G *, T * )
    {
	static S<G,T,1,fail> x;
	return &x;
    }

void bar( int *p )
{
    foo( p, p )->v1[0] = 0;
}
