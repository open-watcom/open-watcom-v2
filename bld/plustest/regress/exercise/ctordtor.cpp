#include <stdio.h>

struct Msg
{
private:
    int class_no;
    static int next_class_no;
public:
    void msg( char * );
    Msg();
};
static int Msg::next_class_no = 0;

Msg::Msg() : class_no( next_class_no )
{
    ++ next_class_no;
}

void Msg::msg(                  // PRINT MESSAGE
    char *text )                // - text
{
    printf( "%3d -- %s\n", class_no, text );
#if __WATCOM_REVISION__ < 8
    fflush( stdout );
#endif
}


struct NullClass : virtual Msg  // CLASS WITH NO DATA
{
    NullClass();
    ~NullClass();
};

NullClass::NullClass()          // CTOR
{
    this->msg( "NullClass CTOR" );
}

NullClass::~NullClass()         // DTOR
{
    this->msg( "NullClass DTOR" );
}


struct ElemClass : virtual Msg  // CLASS WITH DATA
{
private:
    int value;
public:
    ElemClass();
    ~ElemClass();
};

ElemClass::ElemClass()          // CTOR
{
    this->msg( "ElemClass CTOR" );
}

ElemClass::~ElemClass()         // DTOR
{
    this->msg( "ElemClass DTOR" );
}


struct ArrayClass : virtual Msg // CLASS WITH ARRAYS
{
private:
    NullClass na[5];
    ElemClass ea[3][2];
public:
    ArrayClass();
    ~ArrayClass();
};


ArrayClass::ArrayClass()        // CTOR
{
    this->msg( "ArrayClass CTOR" );
}

ArrayClass::~ArrayClass()       // DTOR
{
    this->msg( "ArrayClass DTOR" );
}


ArrayClass pub;

void foo( void )
{
    static ArrayClass stat;
}

int main( void )
{
    ArrayClass aut;
    foo();
    return 0;
}
