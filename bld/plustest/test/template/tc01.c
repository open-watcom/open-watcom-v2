// access perms are still respected with templates
template <class T,int size,void (*err)()>
    class Stack {
	int index;
	T a[size];
	Stack() : index(0)
	{
	}
	T pop()
	{
	    if( index == 0 ) {
		(*err)();
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

void fatal()
{
    printf( "FAIL\n" );
}

Stack<int,10,fatal> s;

void main( void )
{
    s.push( 1 );
    s.push( 2 );
    s.pop();
    s.pop();
    s.pop();
}
