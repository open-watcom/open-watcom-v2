// Ideally the code generator can nudge the front-end when it detects:
//      (1) may not be initialized
//      (2) is not initialized
// AFS

void case_1( int x )
{
    int i;
    int y;

    y = 1;
    for(;;) {
        if( y ) {
            i = 0;
            y = 0;
        } else {
            ++i;
        }
        if( x ) {
            ++y;
        }
    }
}

void case_2( int x )
{
    int     i;

    while( x ) {
        i++;
    }
}
