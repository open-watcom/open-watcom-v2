// TEST EXECUTION OF CLASS FEATURES
//
// 92/02/08 -- J.W.Welch        -- defined

#include <stdio.h>

static int error_count;
static const int TRUE  = 1;
static const int FALSE = 0;

class BASE
{
    int private_b;
    static int private_static_b;
    int private_fun();
  public:
    int public_b;
    int public_fun();
    void check( int, int, char * );
    BASE();
    BASE( int, int );
};

BASE::BASE()
{
    private_b = 1;
    public_b = 1;
}

BASE::BASE( int priv, int pub )
{
    private_b = priv;
    public_b = pub;
}

void BASE::check( int priv, int pub, char *msg )
{
    if( ( priv != private_b ) || ( pub != public_b ) ) {
        printf( "%d %d %s\n", private_b, public_b, msg );
        ++ error_count;
    }
}

class DERIVED : public BASE
{
    int private_d;
    static int private_static_d;
    int private_fun();
  public:
    int public_d;
    int public_fun();
    void check( int, int, char * );
    DERIVED();
    DERIVED( int, int );
};

DERIVED::DERIVED()
{
    private_d = 1;
    public_d = 1;
    BASE::check( 1, 1, "DERIVED(): BASE NOT ( 1, 1 )" );
}

DERIVED::DERIVED( int priv, int pub )
{
    private_d = priv;
    public_d = pub;
    BASE::check( 1, 1, "DERIVED(int,int): BASE NOT ( 1, 1 )" );
}

void DERIVED::check( int priv, int pub, char *msg )
{
    if( ( priv != private_d ) || ( pub != public_d ) ) {
        printf( "%d %d %s\n", private_d, public_d, msg );
        ++ error_count;
    }
}

class DTOR : public BASE
{
  public:
    int vars[3];
    ~DTOR();
};

int dtor_called = FALSE;

DTOR::~DTOR()
{
    dtor_called = TRUE;
}

int main( void )
{
    BASE b0( 1, 1 );        // change to no arg.s later
    BASE b1( 3, 4 );

    b0.check( 1, 1, "BASE() failed" );
    b1.check( 3, 4, "BASE( 3, 4 ) failed" );
    b0 = b1;
    b0.check( 3, 4, "b0 = b1 failed" );

    DERIVED d0( 1, 1 );     // change to no arg.s later
    DERIVED d1( 5, 6 );

    d0.check( 1, 1, "DERIVED() failed" );
    d1.check( 5, 6, "DERIVED( 3, 4 ) failed" );
    d0 = d1;
    d0.check( 5, 6, "d0 = d1 failed" );

    {   DTOR dummy;
        dtor_called = FALSE;
        dummy = dummy;
    }
    if( ! dtor_called ) {
        printf( "DTOR not dtor'd properly\n" );
        ++ error_count;
    }

    {   class DTOR_DERIVED : public DTOR
        {
          public:
            int x;
        };
        DTOR_DERIVED dummy;
        dtor_called = FALSE;
        dummy.x = 18;
    }
    if( ! dtor_called ) {
        printf( "DTOR not dtor'd properly\n" );
        ++ error_count;
    }

    if( error_count == 0 ) {
        printf( "CHKCL -- passed all tests\n\n" );
    } else {
        printf( "CHKCL -- %d errors noted\n\n", error_count );
    }
    return( error_count != 0 );
}
