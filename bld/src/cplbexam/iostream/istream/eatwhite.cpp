# include <iostream.h>
# include <fstream.h>

class mystream : public ifstream {

    public:
    mystream ( char *name ) : ifstream ( name ) {};
    mystream &operator >> ( char& );

};

mystream &mystream::operator >> ( char &ch ) {

    this->eatwhite();
    // *((istream *)this) >> ch;
    this->istream::operator >> ( ch );
    return *this;
}

void main( void ) {

    char    ch;

    mystream    test ( "temp.txt" );
    test.unsetf( ios::skipws );
    cout << "The content of \"temp.txt\" without spaces:" << endl;
    while( (test >> ch).good() ) {
        cout << ch;
    }
}
