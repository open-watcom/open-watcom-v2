#include "fail.h"
// Test DTORing withing Constructors
//
// 94/03/02 -- J.W.Welch        -- defined


#define myfail( m ) ( puts( m ), fail(__LINE__) )

int ct_S;
int ct_T;

struct base {
    int id;
    int type;

    base( int i, int t ) : id(i), type(t) {}

};

struct base_link : base {
    base_link *last;
    static base_link* list;

    base_link( int i, int t ) : base( i, t ), last( list )
    {
        list = this;
    }

    base_link( const base_link& src ) : base( src ), last( list )
    {
        list = this;
    }

    ~base_link()
    {
        if( list != this ) {
            myfail( "DTORing out of order" );
            show( "dtoring:" );
            this->show( "list:   " );
        } else {
            list = list->last;
        }
    }

    void show( const char* text )
    {
        printf( "%s id=%d type=%c\n", text, id, type );
    }

};

base_link* base_link::list = 0;

struct S : base_link {

    S( int i ) : base_link( i, 'S' )
    {
        ++ ct_S;
    }

    S( const S& src ) : base_link( src )
    {
        ++ ct_S;
    }

    ~S()
    {
        -- ct_S;
    }

};

struct T : base_link {
    S s100;
    S s200;

    T( int i )
        : base_link( i, 'T' )
        , s100( 100 + i )
        , s200( 200 + i )
    {
        int j;
        for( j = 0; j < i; ++j ) {
            S temp( 300 );
            if( j == 1 ) break;
        }
        ++ ct_T;
    }

    T( const T& src )
        : base_link( src )
        , s100( src.s100 )
        , s200( src.s200 )
    {
        ++ ct_T;
    }

    ~T()
    {
        -- ct_T;
    }

};


int main()
{
    {
        S s1(1);
        T t1(1);
        S s2(2);
        T t2(2);
    }

    _PASS;
}
