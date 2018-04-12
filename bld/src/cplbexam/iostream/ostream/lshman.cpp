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
        //*((ostream*)this) << p;
        tp = p;
        while( *tp ) {
            *tp = tolower( *tp );
            tp++;
            *tp = toupper( *tp );
            tp++;
        }
        this->ostream::operator<<( p );
        osfx();
    }
    return ( *this );
}

int main( void ) {

    uplowstream    test ( cout );
    test << "Hello, Open Watcom C/C++ users!" << endl;      // manipulator endl
}
