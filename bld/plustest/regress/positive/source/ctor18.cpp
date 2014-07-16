#include "fail.h"
#include <stddef.h>

struct CD {
    void *sig;
    CD();
    ~CD();
};

int ctor;
void *this_check;
void *this_check2;

CD::CD() : sig(this)
{
    ++ctor;
}

CD::~CD()
{
    if( this != this_check ) {
	if( this != this_check2 ) {
	    fail(__LINE__);
	} else {
	    if( this != sig ) fail(__LINE__);
	}
    } else {
	if( sig != this_check ) fail(__LINE__);
    }
    --ctor;
    if( ctor < 0 ) {
	fail(__LINE__);
	ctor = 0;
    }
}

struct CD11 {
    CD x;
    CD y;
};
struct CD01 {
    int x;
    CD y;
};
struct CD10 {
    CD y;
    int x;
};
struct CD00 {
    int y;
    int x;
};

struct CD01_0 {
    CD01 x;
    int y;
};
struct CD01_i {
    int y;
    CD01 x;
};
struct CD10_0 {
    CD10 x;
    int y;
};
struct CD10_i {
    int y;
    CD10 x;
};

void foo()
{
    {
	CD00 v00;
    }
    {
	CD01 v01;
	this_check = ((char*)&v01) + sizeof(int);
    }
    {
	CD10 v10;
	this_check = ((char*)&v10);
    }
    {
	CD11 v11;
	this_check = ((char*)&v11);
	this_check2 = ((char*)&v11) + offsetof( CD11, y );
    }
    this_check2 = NULL;
    {
	CD01_0 v01_0;
	this_check = ((char*)&v01_0) + sizeof( int );
    }
    {
	CD01_i v01_i;
	this_check = ((char*)&v01_i) + 2 * sizeof( int );
    }
    {
	CD10_0 v10_0;
	this_check = ((char*)&v10_0);
    }
    {
	CD10_i v10_i;
	this_check = ((char*)&v10_i) + sizeof( int );
    }
}

int main()
{
    foo();
    _PASS;
}
