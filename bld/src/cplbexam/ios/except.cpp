#include <iostream.h>
#include <fstream.h>

void main( void ) {

    fstream     test ( "temp.txt", ios::in );
    test.exceptions( ios::failbit );
    try {
        test >> "Hello my world";      /* failbit will be set if "temp.txt"
                                          is empty */
    }
    catch( ios::failure ) {
        cout << "The previous operation on the stream fails!" << endl;
    }
}
