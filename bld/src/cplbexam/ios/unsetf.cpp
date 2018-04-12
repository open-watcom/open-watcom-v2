#include <iostream.h>

int main( void ) {

    char    ch;

    cout << "Write a sentence:" << endl;
    cin.unsetf( ios::skipws );
    while( (cin >> ch).good() && ch != '\n' ) {
        cout << ch;
    }
    cout << endl;
}
