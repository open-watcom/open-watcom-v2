#include <iostream.h>
#include <ctype.h>

#define BUF_SIZ 90

class upstream : public istream {

    public:
    static ios::fmtflags my_toupper_bit;
    upstream (streambuf *sptr) : ios( sptr ), istream( sptr ) {};
    upstream &operator>> (char *p);
};

ios::fmtflags upstream::my_toupper_bit;

upstream &upstream::operator>> (char *p) {

    getline( p, BUF_SIZ );
    if( (this->flags() & upstream::my_toupper_bit) != 0 ) {
        while( *p ) {
            *p = toupper( *p );
            p++;
        }
    }
    return (*this);
}

void main( void ) {

    char buf[BUF_SIZ];

    upstream test( cin.rdbuf() );
    upstream::my_toupper_bit = ios::bitalloc();
    test.setf( upstream::my_toupper_bit );
    cout << "Enter a string:" << endl;
    test >> buf;
    cout << buf << endl;
    test.unsetf( upstream::my_toupper_bit );
    cout << "Enter another string:" << endl;
    test >> buf;
    cout << buf << endl;
}
