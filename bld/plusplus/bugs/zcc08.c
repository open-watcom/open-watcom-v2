// yields internal error 47066
typedef int (pF)( char, float );
void f( const pF * );
void f( pF * );
