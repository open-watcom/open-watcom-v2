static int x;
extern inline foo( void ) { x = 1; }

extern enum colour { RED, WHITE, BLUE };

int bar( void ) { return RED; }

extern "C" enum X { X1, X2, X3 };
enum Y { Y1, Y2, Y3 } extern "C" "+" "+";

int r( void );

static inline int main()
{
    return( r() );
}

int recursion = 2;
int r( void )
{
    if( recursion-- > 0 ) {
	main();
    }
    return( recursion );
}

int (*sam( void ))( void )
{
    return main;
}
