struct Q {
    enum E { X1 = sizeof( Q ), X2, X3 };
};

void foo( void )
{
    struct C {
	static int a;
    };
}

void bar( void )
{
    extern int bar1[];
    int bar2( int * );
    bar2( &bar1[0] );
}

typedef int T[];

T x;

class W;

struct Z {
    W q;
    int r[];
    int again;
    int again;
    static int zs1[];
    static int zs2[];
    static int zs3[];
    static int zs3[];
    int non_static_1;
    int non_static_2;
};

int Z::non_static_1;

int Z::non_static_2 = 1;

int Z::zs1[];

static int Z::zs2[] = { 1, 2, 3 };

NOT_A_TYPE i_think_this_is_an_id;
