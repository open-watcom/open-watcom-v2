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
    const S<G,T,1,fail> *foo( G *, T * )
    {
	return 0;
    }

const S<int,int,1,fail> *foo( int *, int * );
