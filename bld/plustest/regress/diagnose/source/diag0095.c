template <class T,int size,void (*err)()>	// template not declared
void Stack<T,size,err>::foo( void )
{}
template <class T,int size,void (*err)()>
int T::* Stack<size,T,err>::give_index()	// bad parms to Stack<>
{ return 0; }
template <class T,int size,void (*err)()>
int T::* Stack<T,size,err>::give_index()	// member pointer declarator
{ return 0; }
template <class T,int size,void (*err)()>	// fn not member
Stack<T,size,err>::Stack(int x) : index(x)
{ }
template <class T,int size,void (*err)()>	// generic type as return type
T Stack<T,size,err>::pop(int,char)
{ return a[0]; }
template <class T,int size,void (*err)()>	// OK
void Stack<T,size,err>::push( T x )
{ a[0] = x; }
template <class T,int size,void (*err)()>	// starts off like a class template
class X Stack<T,size,err>::dummy( void )
{
}
template <class T,int size,void (*err)()>	// static member
int Stack<T,size,err>::qw = 1;

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
