#include <iostream.h>
#include <ctype.h>

class uplowstream : public ostream {

    public:
    uplowstream () {};
    uplowstream ( ostream &os ) : ios ( os ), ostream ( os ) {};
    uplowstream &operator << ( char *p );
    uplowstream &operator << ( ostream &(*__f)( ostream & ) ) {
        __f( *this );
        return( *this );
    }

};

uplowstream &uplowstream::operator<< ( char *p ) {

    char    *tp;

    if( opfx() ) {
        //this << p;
        