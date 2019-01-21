#include <strstrea.h>

int main( void ) {

    istrstream    ss ( "abcdefg" );
    cout << ss.rdbuf() << endl;
}
