// inline functions inside
template <class T,int size,void (*err)()>
    struct Stack {
	int index;
	T a[size];
	Stack() : index(0)
	{
	}
	T pop()
	{
	    if( index == 0 ) {
		(*err)();
		return -1;
	    }
	    return a[--index];
	}
	void push( T x )
	{
	    if( index == size ) {
		(*err)();
	    }
	    a[index++] = x;
	}
    };

extern "C" int printf( char *, ... );

void ok()
{
    printf( "PASS\n" );
}

Stack<int,10,ok> s;

void main( void )
{
    s.push( 1 );
    s.push( 2 );
    s.pop();
    s.pop();
    if( s.pop() != -1 ) {
	printf( "FAIL\n" );
    }
}
