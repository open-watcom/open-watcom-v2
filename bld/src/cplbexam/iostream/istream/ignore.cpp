#include <iostream.h>

void main( void ) {

    char    s[20];
    int     num = 3;

    cout << "Enter a string:" << endl;
    cin.ignore( num ) >> s;
    cout << "The string you have just entered after discarding the first "
         << num << " characters: " << endl;
    cout << s << endl;
}
