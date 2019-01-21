#include <iostream.h>
#include <ctype.h>

class upstream : public istream {

    public:
    upstream ( streambuf *sptr ) : ios( sptr ), istream( sptr ) {};
    upstream &operator>> ( char *p );

};

upstream &upstream::operator>> ( char *p ) {

    if( ipfx() ) {
        //this >> p;
        //*((istream*)this) >> p;
        this->istream::operator>>( p );
        while( *p ) {
            *p = toupper( *p );
            p++;
        }
        isfx();
    }
    return ( *this );
}

int main( void ) {

    char    buf[50];

    upstream    test ( cin.rdbuf() );
    cout << "Enter a string:" << endl;
    test.width( sizeof buf );
    test >> buf;
    cout << "The string you have just entered in upper case: " << buf << endl;
}
