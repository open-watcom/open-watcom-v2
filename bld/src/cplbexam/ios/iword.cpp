#include <iostream.h>
#include <strstream.h>

void main( void ) {

    int       index1, index2;
    float     dollar1, dollar2;
    char      ch;

    istrstream     currency ( "23.4 26.6" );
    index1 = currency.xalloc();
    currency.iword( index1 ) = 36;
    index2 = currency.xalloc();
    currency.iword( index2 ) = 157;
    currency >> dollar1;
    currency >> dollar2;
    cout << "Canadian currency: CAN " << ( ch = currency.iword( index1 ) )
         << dollar1 << endl;
    cout << "Japanese currency: " << ( ch = currency.iword( index2 ) )
         << dollar2 << endl;
}
