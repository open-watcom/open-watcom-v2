#include <strstrea.h>

void main( void ) {

    istrstream    ss ( "abcdefg" );
    cout << ss.rdbuf() << endl;
}
