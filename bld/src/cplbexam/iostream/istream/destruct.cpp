#include <iostream.h>

void *operator new( size_t, istream *p ) { return( p ); }

void main( void ) {

    char    blob[512], buf[50];

    istream     *p = new ( blob ) istream ( cin );
    (*p).width( 50 );
    cout << "Enter a string:" << endl;
    *p >> buf;
    cout << "The string you have just entered:" << endl;
    cout << buf << endl;
    p->~istream();
}
