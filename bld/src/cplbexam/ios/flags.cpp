#include <iostream.h>

void main( void ) {

    cout.setf( ios::hex );
    cout.setf( ios::unitbuf );
    cout << "The current value of ios::fmtflags in hex. is "
         << cout.flags() << endl;
    cout.flags( 0x3142 );
    cout << "The current value of ios::fmtflags in hex. is "
         << cout.flags() << endl;
}

