#include <iostream.h>
#include <fstream.h>

void main( void ) {

    char    buf[50];

    fstream    test ( "temp.txt",ios::in|ios::out );
    cin.unsetf( ios::skipws );
    cout << "Write a sentence, then press CTRL-Z:" << endl;
    cin >> test.rdbuf();               // saves the sentence into a file
    test.seekg( 0 );
    cout << "The sentence have been saved in \"temp.txt\"." << endl;
    cout << "The content of \"temp.txt\":" << endl;
    while( (test >> buf).good() ) {
        cout << buf << " " << flush;
    }
}
