int i;
int i;

typedef int (F)(int,int);

void *p = new void;
void *q = new F;
void *r = new const void;
void *s = new const int;
void *t = new volatile F;
void *u = new volatile int;

struct Z;
typedef struct X T;
struct X {
    ~T();
    ~Z();
};

union U;
struct U { int i; };

enum colour { RED, WHITE, BLUE };
typedef int colour;

struct id1 id2 { int i; };

struct Q {
    static int foo() const volatile;
    ~Q() const volatile;
};

int foo() const volatile;
int (*bar)() const volatile;

struct NEW1 {} ***foo( struct NEW2 {} ***p );

struct A1 {
    virtual int d( int );
};
struct A2 : A1 {
    int a = 0;
    int b( int ) = 0;
    int virtual c( int ) = 0;
    int d( int ) = 0;
};

not_a_function;

struct NF {
    not_a_function;
};
