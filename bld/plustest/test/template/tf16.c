extern "C" int printf( char *, ... );

template <class T,class W,int s1,int *p1,int s2,int *p2>
    struct Stack {
	Stack *n;
	T v;
	W x;
	Stack();
	void test( int a[s1][s2] );
    };

template <class X,class W,int s1,int *p1,int s2,int *p2>
Stack<X,W,s1,(int [s1][s2])0,s2,p2>::Stack() : n(0)
{
}

template <class X,class W,int s1,int *p1,int s2,int *p4>
void Stack<X,W,s1,p1,s2,p2>::test( int a[s1][sq] )
//		        ^^ not ignored       ^^ gets ignored
{
}

int x;

template <class U,class W>
    U pop( Stack<U,W,1,&x,2,&x> **p )
    {
	Stack<U,W,1,&x,2,&x> *e = *p;
	if( e ) {
	    U v = e->v;
	    *p = e->n;
	    return v;
	}
	return 0x123a;
    }

template <class U,class W>
    void push( Stack<U,W,2,&x,1,&x> **p, Stack<U,W,1,&x,2,&x> *e )
    {
	e->n = *p;
	*p = e;
    }

Stack<int,char,1,&x,2,&x> *head;
Stack<int,char,1,&x,2,&x> *elt;

int pop()
{
    return pop( &head );
}

void main()
{
    push( &head, elt );
    if( pop() == 0x123a ) {
	printf( "PASS\n" );
    } else {
	printf( "FAIL\n" );
    }
}
