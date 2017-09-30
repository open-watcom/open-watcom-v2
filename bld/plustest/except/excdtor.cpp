// EXCDTOR -- test DTOR
//
// 92/11/27 -- J.W.Welch        -- defined

// #pragma on ( dump_tokens )

extern "C" char* printf( const char*, ... );

#define strdef( name )                          \
    int def;                                    \
    name() { printf( "%s CTOR\n", #name ); };   \
    ~name() { printf( "%s %d DTOR'ed\n", #name, def ); };

struct BASE1
{
    strdef( BASE1 )
    BASE1( int line ) : def(line) { printf( "BASE1 CTOR %d\n", line ); };
};

struct BASE2
{
    strdef( BASE2 )
    BASE2( int line ) : def(line) { printf( "BASE2 CTOR %d\n", line ); };
};

struct B12 : BASE1, BASE2
{
    strdef( B12 )
    B12( int line ) : def(line), BASE1(line), BASE2(line)
        { printf( "B12 CTOR %d\n", line ); };
};

struct B1V2 : BASE1, virtual BASE2
{
    strdef( B1V2 )
    B1V2( int line ) : def(line), BASE1(line), BASE2(line)
        { printf( "B1V2 CTOR %d\n", line ); };
};

struct DB12 : B12
{
    int def;
    DB12( int line ) : def(line), B12(line)
        { printf( "DB12 CTOR %d\n", line ); };
    DB12()
        { printf( "DB12 CTOR\n" ); };
    ~DB12();
};

int db12_flag = 1;

DB12::~DB12()
{
    if( db12_flag ) {
        printf( "DB12 %d -- throw\n", def );
        db12_flag = 0;
        throw 1234;
    } else {
        printf( "DB12 %d DTORed\n", def );
    }
}

void goop()
{
    DB12 b12( __LINE__ );
}

void crap()
{
    {
        int i, line;
        DB12 array[3];
        for( i = 0; i < 3; ++ i ) {
            line = __LINE__;
            array[i].def = line;
            array[i].B12::def = line;
            array[i].BASE1::def = line;
            array[i].BASE2::def = line;
        }
        db12_flag = 1;
        goto out;
    }
out:
    return;
}


int main()
{
    {
        B12 b12( __LINE__ );
    }
    {
        B1V2 b12( __LINE__ );
    }
    try {
        goop();
    } catch( int cot ) {
        printf( "Line %d -- caught %d\n", __LINE__, cot );
    }
    try {
        crap();
    } catch( int cot ) {
        printf( "Line %d -- caught %d\n", __LINE__, cot );
    }
    return( 0 );
}
