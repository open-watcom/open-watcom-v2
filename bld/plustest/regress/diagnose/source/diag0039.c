class C {
    void foo( void );
    int u0;
    union {
	int u1;
	union {
	    int u2;
	};
    };
public:
    int u3;
    union {
	int u4;
	union {
	    int u5;
	};
    };
};

void C::foo( void )
{
    u0 = 1;
    u1 = 1;
    u2 = 1;
    u3 = 1;
    u4 = 1;
    u5 = 1;
}

void foo( C *p )
{
    p->u0 = 1;
    p->u1 = 1;
    p->u2 = 1;
    p->u3 = 1;
    p->u4 = 1;
    p->u5 = 1;
}
