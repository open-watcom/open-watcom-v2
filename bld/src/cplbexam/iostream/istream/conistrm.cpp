#include <iostream.h>

class mystream : public istream {
    public:
    mystream ( istream &is ) : ios ( is ), istream ( is ) {};
};

void main( void ) {

    char    ch;

    mystream    test ( cin );
    // CRTL-Z is used to mark an EOF
    cout << "Enter a string (press CRTL-Z):" << endl;
    while( (test >> ch).good() ) {
        cout << ch << flush;
    }
}
