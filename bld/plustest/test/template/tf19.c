template <class T,int s,void (*err)()>
    struct S {
	T v1[s];
	S()
	{
	    err();
	}
    };

extern void fail();

template <class Q>
    const S<Q,1,fail> *foo( Q *, Q * )
    {
	return 0;
    }

const S<int,1,fail> *foo( int *, int * );
const S<char,1,fail> *foo( char *, char * );

void foo( int *p , char *q )
{
    foo(p,p);
    foo(p,p);
    foo(q,q);
    foo(q,q);
}
