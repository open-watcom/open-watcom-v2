#include <iostream.h>

void main( void ) {

    char    c[] = "Hello my world\0";

    for( int i = 0; c[i] != '\0'; i++ ) {
        cout.put( c[i] );
    }
        cout << endl;
}
