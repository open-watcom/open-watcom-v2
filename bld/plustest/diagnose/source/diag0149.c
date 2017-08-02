struct X1 {};

void x3( ... );

void x3( ... )
{
}

void x7( X1, ... );

void x7( X1, ... )
{
}

void x11( int &, ... );

void x11( int &, ... )
{
}

struct X15 {
    X15( int, int *, int, char * );
};

void x19( int &i )
{
    X15( 0, i, 0, 0 );
    new X15( 0, i, 0, 0 );
}

static int x25(int = 0);
static int x26(int = 0);
static int x27(int = 0);

void x29()
{
    int x = x25();
    int y = x26(1);
}

static int x25(int i)
{
    return i;
}

static int x26(int i)
{
    return i;
}

static int x27(int i)
{
    return i;
}

static int x50( int );

static int x50( int );

static int x50( int x )
{
    return x;
}

static int x50( int );

static int x50( int );

static int x63( int );

static int x63( int );

static int x63( int );

static int x63( int );

extern int x71;
extern int x71;
int x71 = 1;
extern int x71;
extern char x71;

extern int x77;
extern int x77;
extern int x77;
extern char x77;
