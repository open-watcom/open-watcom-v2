// Note that each occurence of j here should imply its own scope
// i.e. this is NOT a case of multiple declaration as Borland seems to think.

void main( void )
{
    for( ; ; ) int j;           // statement cannot be a declaration
    do int j; while( 1 );       // ditto
    while( 1 ) int j;           // ditto
}
