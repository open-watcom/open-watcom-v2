#include <iostream.h>
#include <strstream.h>

void main( void ) {

    int      index1 , index2;
    char     s[10];

    istrstream     greet ( "Hello," );
    index1 = greet.xalloc();
    greet.pword( index1 ) = " Watcom C compiler users";
    index2 = greet.xalloc();
    greet.pword( index2 ) = " Watcom C++ compiler users";
    greet >> s;
    cout << s << (char *)greet.pword( index1 )  << endl;
    cout << s << (char *)greet.pword( index2 )  << endl;
}
