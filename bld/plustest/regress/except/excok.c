// TEST EXCEPTION HANDLING
//
// 92/11/13 -- J.W.Welch    -- defined
// 95/09/06 -- J.W.Welch    -- change pass-through test for 11.0 unexpected


// #pragma on (debug_names)
// #pragma on (dump_ptree)
// #pragma on (dump_tokens)
// #pragma on (print_ptree)
// #pragma on (dump_emit_ic)
#pragma warning 549 10


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <except.h>

int catches = 0;
int throws = 0;
int thr_line = 0;

void no_handler()
{
    printf( "No handler at line %d\n", thr_line );
    abort();
}

enum E { e1, e2, e3, e4 };



void test_scalars()
{
#define test_scalar( type, value, prt ) \
        try {                           \
            thr_line = __LINE__;        \
            ++ throws;                  \
            throw (type)value;          \
        } catch( type val ) {           \
            ++ catches;                 \
            if( val != value ) {        \
                printf( "failure[%d]: " ## prt ## "\n", __LINE__, val ); \
            }                           \
        }
    test_scalar( char, 'A', "%c" );
    test_scalar( signed char, 'B', "%c" );
    test_scalar( unsigned char, 'C', "%c" );
    test_scalar( wchar_t, 15, "%c" );
    test_scalar( signed short, 14, "%d" );
    test_scalar( unsigned short, 15, "%d" );
    test_scalar( signed int, 16, "%d" );
    test_scalar( unsigned int, 17, "%d" );
    test_scalar( signed long, 18, "%d" );
    test_scalar( unsigned long, 19, "%d" );
    test_scalar( float, 14.67F, "%d" );
    test_scalar( double, 15.97, "%d" );
    test_scalar( long double, 13.47L, "%d" );
    test_scalar( E, e3, "%d" );
    test_scalar( int*, &catches, "%d" );
    test_scalar( void*, &catches, "%d" );
    test_scalar( const void*, &catches, "%d" );
    test_scalar( volatile void*, &catches, "%d" );
    test_scalar( const volatile void*, &catches, "%d" );
    test_scalar( void*, 0, "%d" );
    test_scalar( const void*, 0, "%d" );
    test_scalar( volatile void*, 0, "%d" );
    test_scalar( const volatile void*, 0, "%d" );
#undef test_scalar
}

struct base0
{   int v[7];
};

struct base
{
    int b[3];
};

struct der : public base0, public base
{
    int d[5];
};

struct vir : virtual public base
{
    int v[3];
};

static base bv;
static der  dv;
static vir  vv;

struct dtorable : public der
{   dtorable();
    ~dtorable();
};

dtorable::dtorable()
{
    printf( "dtorable CTOR\n" );
    fflush( stdout );
}

dtorable::~dtorable()
{
    printf( "dtorable DTOR\n" );
    fflush( stdout );
}

static dtorable dt;

int sameClass( void* m1, void* m2, int sz )
{
    return 0 == memcmp( m1, m2, sz );
}


void init_class()
{
    int i;
    for( i = 0; i < 5; ++i ) {
        dv.d[i] = i;
    }
    for( i = 0; i < 3; ++i ) {
        dv.b[i] = 10+i;
        dt.b[i] = 10+i;
        bv.b[i] = 20+i;
        vv.b[i] = 30+i;
    }
}


void test_class()
{
#define test_clss( thr, cat, catv, src, size ) \
        try {                           \
            thr_line = __LINE__;        \
            ++ throws;                  \
            throw thr;                  \
        } catch( cat ) {                \
            ++ catches;                 \
            if( ! sameClass( &catv, &src, size ) ) { \
                printf( "class failure[%d]\n", __LINE__ ); \
            }                           \
        }

    test_clss( bv, base bvc, bvc, bv, sizeof(bv) );
    test_clss( dv, base bvc, bvc, dv.b[0], sizeof(bv.b) );
    test_clss( (base)bv, base bvc, bvc, bv, sizeof(bv) );
    test_clss( (der)dv, base bvc, bvc, dv.b[0], sizeof(bv.b) );
    test_clss( bv, base& bvc, bvc, bv, sizeof(bv) );
    test_clss( dv, base& bvc, bvc, dv.b[0], sizeof(bv.b) );
    test_clss( (base)bv, base& bvc, bvc, bv, sizeof(bv) );
    test_clss( (der)dv, base& bvc, bvc, dv.b[0], sizeof(bv.b) );
    test_clss( dt, base bvc, bvc, dv.b[0], sizeof(bv.b) );
    test_clss( vv, vir vvc, vvc, vv, sizeof(vv) );
    test_clss( vv, base bvc, bvc, vv.b[0], sizeof(vv.b) );

#undef test_clss
}


void test_cl_ptrs()
{
#define test_cl_ptr( type, value, prt ) \
        try {                           \
            thr_line = __LINE__;        \
            type val = value;           \
            ++ throws;                  \
            throw (type)val;            \
        } catch( type val ) {           \
            ++ catches;                 \
            if( val != value ) {        \
                printf( "cl ptr failure[%d]: " ## prt ## "\n", __LINE__, val ); \
            }                           \
        }

    test_cl_ptr( der*, &dv, "%x" );
    test_cl_ptr( base*, &dv, "%x" );
    test_cl_ptr( void*, &dv, "%x" );
    test_cl_ptr( const void*, &dv, "%x" );
    test_cl_ptr( volatile void*, &dv, "%x" );
    test_cl_ptr( const volatile void*, &dv, "%x" );
    test_cl_ptr( der*, 0, "%x" );
    test_cl_ptr( base*, 0, "%x" );
    test_cl_ptr( void*, 0, "%x" );
    test_cl_ptr( const void*, 0, "%x" );
    test_cl_ptr( volatile void*, 0, "%x" );
    test_cl_ptr( const volatile void*, 0, "%x" );

#undef test_cl_ptr

#define test_cl_ptr( type, value, prt ) \
        try {                           \
            thr_line = __LINE__;        \
            type val = value;           \
            ++ throws;                  \
            throw val;                  \
        } catch( type val ) {           \
            ++ catches;                 \
            if( val != value ) {        \
                printf( "cl ptr failure[%d]: " ## prt ## "\n", __LINE__, val ); \
            }                           \
        }

    test_cl_ptr( der*, &dv, "%x" );
    test_cl_ptr( base*, &dv, "%x" );
    test_cl_ptr( void*, &dv, "%x" );
    test_cl_ptr( const void*, &dv, "%x" );
    test_cl_ptr( volatile void*, &dv, "%x" );
    test_cl_ptr( const volatile void*, &dv, "%x" );

#undef test_cl_ptr
}


void unexp()
{
    throw 675L;
}

void test_unexpected() throw( long )
{
    throw 15;
}

void my_term() throw()
{
    printf( "EXCOK: %d catches completed\n", throws );
    exit( 0 );
}

void test_term()
{
    throw 56;
}

void passthru() throw( int )
{
    throw char(69);
}

void throw_int()
{
    throw 124;
}

void test_passthru()
{
    PFV unex;

    unex = set_unexpected( throw_int );
    try {
        passthru();
    } catch( int ) {
        printf( "passthru ok\n" );
    }
    set_unexpected( unex );
}

void partial()
{
    try {
        throw 765;
    } catch ( int i ) {
        if( i != 765 ) {
            printf( "partial failed\n" );
        }
        throw;
    }
}

void test_rethrow()
{
    try {
        partial();
    } catch( int i ) {
        if( i != 765 ) {
            printf( "test_rethrow failed\n" );
        }
    }
}

void test_basics()
{
    try {
        ++throws;
        throw 1234;
    } catch( float ) {
        printf( "Failed: %d\n", __LINE__ );
    } catch( int ) {
        ++catches;
    }
    try {
        ++throws;
        throw 1234;
    } catch( float ) {
        printf( "Failed: %d\n", __LINE__ );
    } catch( ... ) {
        ++catches;
    }
    try {
        printf( "try without catch\n" );
    } catch( ... ) {
        printf( "Failed: %d\n", __LINE__ );
    }
    try {
        try {
            ++throws;
            throw "crap";
        } catch( char* it ) {
            ++catches;
            if( 0 == strcmp( it, "crap" ) ) {
                ++throws;
                throw 345;
            } else {
                printf( "Failed: %d\n", __LINE__ );
            }
        }
    } catch( int ) {
        ++catches;
    }
    try {
        try {
            ++throws;
            throw 7658;
        } catch( int it ) {
            ++catches;
            if( it == 7658 ) {
                ++throws;
                throw;
            } else {
                printf( "Failed: %d\n", __LINE__ );
            }
        }
    } catch( int it ) {
        ++catches;
        if( it != 7658 ) {
            printf( "Failed: %d\n", __LINE__ );
        }
    }
}

int main()
{
    printf( "EXCOK: start\n" );
    set_terminate( &no_handler );
    test_basics();
    test_scalars();
    init_class();
    test_class();
    test_cl_ptrs();
    test_passthru();
    test_rethrow();
    try {
        ++throws;
        set_unexpected( &unexp );
        test_unexpected();
    } catch( long l ) {
        ++catches;
        if( l != 675L ) {
            printf( "unexpected failure %l\n", l );
        }
    }
    if( throws != catches ) {
        printf( "catches != throws\n" );
    }
    set_terminate( &my_term );
    test_term();

    printf( "terminate did not abort\n" );
    return( 0 );
}


// #pragma on (dump_exec_ic)

