#include "fail.h"

unsigned new_value = 0x1000;

struct V1 {
    int v1;
    void check1( int check, int line )
    {
	if( v1 != check ) {
	    fail(line);
	}
    }
    virtual void x1( int check, int line )
    {
	check1( check, line );
    }
    virtual void y1( int check, int line )
    {
	check1( check, line );
    }
};

struct V2 {
    int v2;
    void check2( int check, int line )
    {
	if( v2 != check ) {
	    fail(line);
	}
    }
    virtual void x2( int check, int line )
    {
	check2( check, line );
    }
    virtual void y2( int check, int line )
    {
	check2( check, line );
    }
};

struct X1 : virtual V1 {
    X1()
    {
	v1 = ++new_value;
	x1( v1, __LINE__ );
    }
    virtual void x1( int check, int line )
    {
	V1::x1( check, line );
    }
};

struct X2 : virtual V2 {
    X2()
    {
	v2 = ++new_value;
	x2( v2, __LINE__ );
    }
    virtual void x2( int check, int line )
    {
	V2::x2( check, line );
    }
};

struct Y1 : virtual V1 {
    Y1()
    {
	v1 = ++new_value;
	y1( v1, __LINE__ );
    }
    virtual void y1( int check, int line )
    {
	V1::y1( check, line );
    }
};

struct Y2 : virtual V2 {
    Y2()
    {
	v2 = ++new_value;
	y2( v2, __LINE__ );
    }
    virtual void y2( int check, int line )
    {
	V2::y2( check, line );
    }
};

struct P : virtual X1, virtual Y2, virtual Y1, virtual X2 {
    P()
    {
	V1 *pv1 = this;
	V2 *pv2 = this;
	v1 = ++new_value;
	pv1->x1( v1, __LINE__ );
	v2 = ++new_value;
	pv2->x2( v2, __LINE__ );
	v1 = ++new_value;
	pv1->y1( v1, __LINE__ );
	v2 = ++new_value;
	pv2->y2( v2, __LINE__ );
    }
};
struct Z : P, virtual X2, virtual Y1, virtual Y2, virtual X1 {
    Z()
    {
	V1 *pv1 = this;
	V2 *pv2 = this;
	v1 = ++new_value;
	pv1->x1( v1, __LINE__ );
	v2 = ++new_value;
	pv2->x2( v2, __LINE__ );
	v1 = ++new_value;
	pv1->y1( v1, __LINE__ );
	v2 = ++new_value;
	pv2->y2( v2, __LINE__ );
    }
};
struct QQ : virtual Z {
};

int main()
{
    static QQ dummy;

    _PASS;
}
