#include <fstream.h>

void main( void ) {

    char    s[10];

    ifstream     input ( "temp.txt" );
    input >> s;
    cout << "The first string in the file \"temp.txt\" is \""
         << s << "\"" << endl;
    input >> s;
    cout << "The second string in the file \"temp.txt\" is \""
         << s << "\"" << endl;
}
