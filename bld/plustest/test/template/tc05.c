// specialization of a class template
// (no one can handle this)
extern "C" int printf( char *, ... );

void ok()
{
    printf( "PASS\n" );
}

template <class T,int size,void (*err)()>
    struct Stack {
	int index;
	T a[size];
	Stack();
	T pop();
	void push( T x );
    };
template <class T,int size,void (*err)()>
Stack<T,size,err>::Stack() : index(0)
{
    printf( "FAIL\n" );
}
template <class T,int size,void (*err)()>
T Stack<T,size,err>::pop()
{
    printf( "FAIL\n" );
}
template <class T,int size,void (*err)()>
void Stack<T,size,err>::push( T x )
{
    printf( "FAIL\n" );
}

struct Stack<int,10,ok> {
    int a[200];
    int index;
    Stack();
    int pop();
    void push( int x );
};
Stack<int,10,ok>::Stack() : index(0)
{
    a[0] = 1;
}
int Stack<int,10,ok>::pop()
{
    a[1] = 2;
    return 0;
}
void Stack<int,10,ok>::push( int x )
{
    a[2] = 3;
}

Stack<int,10,ok> s;

void main( void )
{
    s.push(1);
    s.pop();
    if( s.a[0] != 1 ) {
	printf( "FAIL\n" );
    } else if( s.a[1] != 2 ) {
	printf( "FAIL\n" );
    } else if( s.a[2] != 3 ) {
	printf( "FAIL\n" );
    } else {
	ok();
    }
}
