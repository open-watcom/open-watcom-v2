#include <iostream.h>

class mystream : public istream {
    public:
    mystream ( streambuf *str ) : ios ( str ), istream ( str ) {};
};

void main( void ) {

    char    ch;

    mystream    test ( cin.rdbuf() );
    cout << "Enter a string (press CTRL-Z):" << endl;
    while( (test >> ch).good() ) {
        cout << ch << flush;
    }
}
