// EXCFANCY -- fancy test that DTORs get done, regardless of where the
//             throw occurs
//
// 93/02/01     -- J.W.Welch        -- defined
// 93/12/21     -- J.W.Welch        -- test new[], delete[]

#include <stdlib.h>

#pragma inline_depth 0
//#pragma on ( dump_emit_ic )
extern "C" char* printf( char*, ... );
extern "C" int strcmp( const void*, const void* );

static const int iterBeg = 1;
static const int iterEnd = 12;

const char *ctored[32];
unsigned cted;

int count_down;
int error_count = 0;

void error( const char* msg )
{
    printf( "\n--%s--\n", msg );
    ++ error_count;
}

void CTORDTOR(
    const char *name,
    const char *rtn )
{
    printf( ",%s %s", rtn, name );
    -- count_down;
    if( count_down == 0 ) {
        printf( ",throw" );
        throw 234;
    }
}

void CTOR(
    const char *name )
{
    CTORDTOR( name, "CTOR" );
    ctored[ cted++ ] = name;
}

void DTOR(
    const char *name )
{
    if( 0 == cted ) {
        error( "DTOR underflow" );
    } else if( 0 != strcmp( name, ctored[ --cted ] ) ) {
        error( "DTOR failure" );
    }
    CTORDTOR( name, "DTOR" );
}


struct V1
{
    V1() { CTOR( "V1" ); }
    ~V1() { DTOR( "V1" ); }
};


struct V2
{
    V2() { CTOR( "V2" ); }
    ~V2() { DTOR( "V2" ); }
};


struct M1 : virtual V1, virtual V2
{
    M1() { CTOR( "M1" ); }
    ~M1() { DTOR( "M1" ); }
};


struct M2 : virtual V1, virtual V2
{
    M2() { CTOR( "M2" ); }
    ~M2() { DTOR( "M2" ); }
};


struct D : virtual V2, M1, M2
{
    V2 v2;
    D() { CTOR( "D" ); }
    ~D() { DTOR( "D" ); }
};

struct S : V1, V2
{
    S() { CTOR( "S" ); }
    ~S() { DTOR( "S" ); }
};

struct M
{
    V1 v1;
    V2 v2;
};

struct MemStack
{
    MemStack( size_t );
    ~MemStack();
    void* alloc( size_t );
    void free( void* );

    struct MemFrame {
        size_t size;
    };

    MemFrame* area;
    unsigned allocs;
};

MemStack::MemStack( size_t initial )
    : area( (MemFrame*)::operator new( initial ) )
    , allocs( 0 )
{
    area->size = initial - sizeof( MemFrame );
}

MemStack::~MemStack()
{
    ::operator delete( area );
}

void* MemStack::alloc( size_t size )
{
    void* retn;
    MemFrame* freed = area;
    size_t left = freed->size;
    if( left < ( size + sizeof( MemFrame ) ) ) {
        error( "MemStack overflow" );
        retn = NULL;
    } else {
        area->size = size;
        retn = (void*)( (char*)freed + sizeof( MemFrame ) );
        freed = (MemFrame*)( (char*)retn + size );
        freed->size = left - size - sizeof( MemFrame );
        area = freed;
        ++allocs;
    }
    return retn;
}

void MemStack::free( void* mem )
{
    if( allocs == 0 ) {
        error( "MemStack underflow" );
    } else {
        --allocs;
        MemFrame* freed = (MemFrame*)( (char*)mem - sizeof( MemFrame ) );
        if( (char*)area
         != ( (char*)freed + freed->size + sizeof( MemFrame ) ) ){
            error( "MemStack inconsistent" );
        } else {
            freed->size += area->size + sizeof( MemFrame );
            area = freed;
        }
    }
}

struct N : D
{
    N() { CTOR( "N" ); }
    ~N() { DTOR( "N" ); }
    void* operator new[]( size_t );
    void* operator new( size_t, MemStack& );
    void* operator new( size_t size ) { return ::operator new( size ); }
    void operator delete[]( void*, size_t );
};

void* N::operator new[]( size_t size ) {
    void* p = ::operator new[]( size + sizeof( int ) );
    int* ip = (int*)( (char*)p + size );
    *ip = 763;
    return p;
}    

void* N::operator new( size_t size, MemStack& mem ) {
    return mem.alloc( size );
}    

void N::operator delete[]( void* p, size_t size )
{
    int*ip = (int*)( (char*)p + size );
    if( *ip != 763 ) {
        error( "N::operator delete[] failed" );
    }
    ::operator delete[]( p );
}


void test_M()
{
    int i;

    cted = 0;
    for( i = 1; i <= 7; ++i ) {
        try {
            printf( "\n[%d]", i );
            count_down = i;
            cted = 0;
            M m;
        } catch( int j ) {
            if( j == 234 ) {
                if( cted == 0 ) {
                    printf( ",caught\n" );
                } else {
                    error( "missed DTOR" );
                }
            } else {
                error( "failed catch" );
                break;
            }
        }
    }
    printf( "\n" );
}


void test_S()
{
    int i;

    cted = 0;
    for( i = 1; i <= 7; ++i ) {
        try {
            printf( "\n[%d]", i );
            count_down = i;
            cted = 0;
            S m;
        } catch( int j ) {
            if( j == 234 ) {
                if( cted == 0 ) {
                    printf( ",caught\n" );
                } else {
                    error( "missed DTOR" );
                }
            } else {
                error( "failed catch" );
                break;
            }
        }
    }
    printf( "\n" );
}


void test_D()
{
    int i;

    cted = 0;
    for( i = 1; i <= 12; ++i ) {
        try {
            printf( "\n[%d]", i );
            count_down = i;
            cted = 0;
            D m;
        } catch( int j ) {
            if( j == 234 ) {
                if( cted == 0 ) {
                    printf( ",caught\n" );
                } else {
                    error( "missed DTOR" );
                }
            } else {
                error( "failed catch" );
                break;
            }
        }
    }
    printf( "\n" );
}


void test_N()
{
    int i;

    for( i = 1; i <= 12; ++i ) {
        try {
            printf( "\n[%d]", i );
            count_down = i;
            cted = 0;
            N*p = new N;
            printf( ",deleting" );
            delete p;
            printf( ",deleted" );
        } catch( int j ) {
            if( j == 234 ) {
                if( cted == 0 ) {
                    printf( ",caught\n" );
                } else {
                    error( "missed DTOR" );
                }
            } else {
                error( "failed catch" );
                break;
            }
        }
    }

    for( i = 1; i <= 20; ++i ) {
        try {
            printf( "\n[%d]", i );
            count_down = i;
            cted = 0;
            N*p = new N[3];
            printf( ",deleting" );
            delete[] p;
            printf( ",deleted" );
        } catch( int j ) {
            if( j == 234 ) {
                if( cted == 0 ) {
                    printf( ",caught\n" );
                } else {
                    error( "missed DTOR" );
                }
            } else {
                error( "failed catch" );
                break;
            }
        }
    }

    MemStack mem( 1024 );

    for( i = 1; i <= 12; ++i ) {
        try {
            printf( "\n[%d]", i );
            count_down = i;
            cted = 0;
            N*p = new( mem )( N );
            printf( ",deleting" );
            mem.free( p );
            printf( ",deleted" );
        } catch( int j ) {
            if( j == 234 ) {
                if( cted == 0 ) {
                    printf( ",caught\n" );
                } else {
                    error( "missed DTOR" );
                }
            } else {
                error( "failed catch" );
                break;
            }
        }
    }
}


int main()
{
    test_M();
    test_S();
    test_D();
    test_N();

    if( error_count == 0 ) {
        printf( "\n**** completed ok ****\n" );
	return( 0 );
    }
    printf( "\N**** %d errors detected ****\n", error_count );
    return( 1 );
}
// #pragma on (dump_scopes)
// #pragma on ( dump_exec_ic )
