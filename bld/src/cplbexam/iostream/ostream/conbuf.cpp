#include <iostream.h>

class mystream : public ostream {
    public:
    mystream ( streambuf *str ) : ios ( str ), ostream ( str ) {};
};

int main( void ) {

    mystream    test ( cout.rdbuf() );
    test << "Just for fun!" << endl;
}

