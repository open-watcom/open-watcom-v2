#include <string.h>


class str_data {
//private:	remarked this out so that hash fns can access string
public:
    char *      info;
public:
    str_data() {
        info = new char[1];
        *info = '\0';
    };

    str_data( const char * c ) {
        info = new char[strlen(c)+1];
        strcpy( info, c );
    };

    ~str_data() {
         delete [] info;
         info = 0;
     };

    str_data( const str_data& str_data ) {
        info = new char[strlen(str_data.info)+1];
        strcpy( info, str_data.info );
    };

    static unsigned hash_fn( const str_data& s ) {
	unsigned short c = 0;
	int len = strlen( s.info );

	for( int i = 0; i < len; i++ ) {
	    c <<= 1;
	    c += (s.info)[ i ];
	};
	return( c );
    };

    void operator=( const str_data& str_data ) {
        if( this != &str_data ) {
            delete [] info;
            info = new char[strlen(str_data.info)+1];
            strcpy( info, str_data.info );
        }
    };

    int operator==( const str_data& str_data ) const {
        return( strcmp( info, str_data.info ) == 0 );
    };

    int operator!=( const str_data& str_data ) const {
        return( strcmp( info, str_data.info ) != 0 );
    };

    int operator<( const str_data& str_data ) const {
        return( strcmp( info, str_data.info ) < 0 );
    };

    friend ostream& operator<<( ostream& stream, str_data strd ) {
        stream << strd.info;
        return( stream );
    };
};

// save some time and more important, make sure vectors, hash, and skip lists
// don't depend on anything from wclist.h
#ifndef _WCHASH_H_INCLUDED
#ifndef _WCSKIP_H_INCLUDED
#ifndef _WCVECTOR_H_INCLUDED
#include <wclist.h>	
class str_sdata : public WCSLink {
private:
    char *      info;
public:
    str_sdata() {
        info = new char[1];
        *info = '\0';
    };

    str_sdata( const char * c ) {
        info = new char[strlen(c)+1];
        strcpy( info, c );
    };

    ~str_sdata() {
         delete [] info;
         info = 0;
     };

    str_sdata( const str_sdata& str_sdata ) {
        info = new char[strlen(str_sdata.info)+1];
        strcpy( info, str_sdata.info );
    };

    void operator=( const str_sdata& str_sdata ) {
        if( this != &str_sdata ) {
            delete [] info;
            info = new char[strlen(str_sdata.info)+1];
            strcpy( info, str_sdata.info );
        }
    };

    int operator==( const str_sdata& str_sdata ) const {
        return( strcmp( info, str_sdata.info ) == 0 );
    };

    int operator<( const str_sdata& str_sdata ) const {
        return( strcmp( info, str_sdata.info ) < 0 );
    };

    friend ostream& operator<<( ostream& stream, str_sdata& strd ) {
        stream << strd.info;
        return( stream );
    };
};


class str_ddata : public WCDLink {
private:
    char *      info;
public:
    str_ddata() {
        info = new char[1];
        *info = '\0';
    };

    str_ddata( const char * c ) {
        info = new char[strlen(c)+1];
        strcpy( info, c );
    };

    ~str_ddata() {
         delete [] info;
         info = 0;
     };

    str_ddata( const str_ddata& str_ddata ) {
        info = new char[strlen(str_ddata.info)+1];
        strcpy( info, str_ddata.info );
    };

    void operator=( const str_ddata& str_ddata ) {
        if( this != &str_ddata ) {
            delete [] info;
            info = new char[strlen(str_ddata.info)+1];
            strcpy( info, str_ddata.info );
        }
    };

    int operator==( const str_ddata& str_ddata ) const {
        return( strcmp( info, str_ddata.info ) == 0 );
    };

    int operator<( const str_ddata& str_ddata ) const {
        return( strcmp( info, str_ddata.info ) < 0 );
    };

    friend ostream& operator<<( ostream& stream, str_ddata& strd ) {
        stream << strd.info;
        return( stream );
    };
};
#endif
#endif
#endif
