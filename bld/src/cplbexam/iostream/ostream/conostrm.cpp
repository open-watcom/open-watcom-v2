#include <iostream.h>

class mystream : public ostream {
    public:
    mystream ( ostream &os ) : ios ( os ), ostream ( os ) {}
};

int main( void ) {

    mystream    test ( cout );
    test << "Hello my world!" << endl;
}

