// out of line member functions not visible (they're in T6b.c)
template <class T,int size,void (*err)()>
    struct Stack {
	int index;
	T a[size];
	Stack();
	T pop();
	void push( T x );
    };
#if 0
template <class T,int size,void (*err)()>
Stack<T,size,err>::Stack() : index(0)
{
}
template <class T,int size,void (*err)()>
T Stack<T,size,err>::pop()
{
    if( index == 0 ) {
	(*err)();
	return -1;
    }
    return a[--index];
}
template <class T,int size,void (*err)()>
void Stack<T,size,err>::push( T x )
{
    if( index == size ) {
	(*err)();
    }
    a[index++] = x;
}
#endif

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
