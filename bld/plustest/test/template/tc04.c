// specialization of a class template
extern "C" int printf( char *, ... );

void ok()
{
    printf( "PASS\n" );
}

template <class T,int size,void (*err)()>
    struct Stack {
	int a;
    };
struct Stack<int,10,ok> {
    int a[200];
};
Stack<int,10,ok> s;
void main( void )
{
    if( sizeof(s) == (200*sizeof(int)) ) {
	ok();
    } else {
	printf( "FAIL\n" );
    }
}
