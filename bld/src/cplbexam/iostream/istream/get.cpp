#include <iostream.h>

int main( void ) {

    char    ch;

    cout << "Enter a string:" << endl;
    while( ( ch = cin.get() ) != '\n' ) {
        cout << ch;
    }
    cout << endl;
}
