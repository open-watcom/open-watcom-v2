#include <iostream.h>
#include <ctype.h>

class upstream : public istream {

    public:
    upstream ( streambuf *sptr ) : ios(), istream() { ios::init( sptr ); };
    upstream &operator>> ( char *p );

};

upstream &upstream::operator>> ( char *p ) {

    if( ipfx() ) {
        //this >> p;
        