#include <iostream.h>
#include <ctype.h>

class lowstream : public ostream {

    public:
    lowstream () {};
    lowstream ( ostream &os ) : ios ( os ), ostream ( os ) {};
    lowstream &operator << ( char *p );
    lowstream &operator << ( ostream &(*__f)( ostream & ) ) {
        __f( *this );
        return( *this );
    }

};

lowstream &lowstream::operator<< ( char *p ) {

    char     *tp;

    if( opfx() ) {
        //this << p;
        