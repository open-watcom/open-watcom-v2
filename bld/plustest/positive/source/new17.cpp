#include "fail.h"

unsigned order;
unsigned count;

struct Dtorable {
    int _order;
    Dtorable();
    Dtorable( Dtorable const & );
    ~Dtorable();
};

Dtorable::Dtorable()
    : _order( ++order )
{
}

Dtorable::Dtorable( Dtorable const & )
    : _order( ++order )
{
}

Dtorable::~Dtorable()
{
    if( order != _order ) _fail;
    -- order;
}

struct Composed {
    Dtorable d1;
    Dtorable d2;
};

int count_other = 0;

struct AnotherDtorable {
    AnotherDtorable();
    ~AnotherDtorable();
};

AnotherDtorable::AnotherDtorable()
{
    ++ count_other;
}

AnotherDtorable::~AnotherDtorable()
{
    -- count_other;
}


char some_data[ sizeof( Composed ) ];

//inline
Composed* ctor( Composed const & src )
{
    AnotherDtorable stuff_1;
    AnotherDtorable stuff_2;
    AnotherDtorable stuff_3;
    Composed* ptr = new( some_data ) Composed( src );
    return ptr;
}


int main()
{
    if( order != 0 ) _fail;
    {
        Composed comp;
        Composed* ptr = ctor( comp );
        ptr->~Composed();
    }
    if( order != 0 ) _fail;
    _PASS;
}
