#include <iostream.h>

void *operator new( size_t, iostream *p ) { return( p ); }

void main( void ) {

    char blob[512], buf[50];

    iostream *p = new ( blob ) iostream ( cin );
    (*p).width( sizeof buf );
    cout << "Enter a string:" << endl;
    *p >> buf;
    cout << "The string you have just entered:" << endl;
    cout << buf << endl;
    p->~iostream();
}
