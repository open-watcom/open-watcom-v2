struct NN1 {};

void ack3() {
    NN1 *p;
    
    NN1 *q = p ? ( p=p, p ) : p++;
}

struct XX9;

struct XX9 {
};

typedef int XX9;

// location for S::foo should be 'foo' not '::' (fix in FinishDeclarator)
struct X17 {
    void foo();
};

void X17::foo() {
}

void X17::foo() {
}

extern char x27[23];
char x27[] = { 'a' };

extern char x30[23];
char x30[];

void x33( int x1 )
{
    if( x1 ) {
	goto label;
    } else {
	label:;
    }
    if( x1 ) {
	looper:;
    } else {
	goto looper;
    }
}

class x47 {
};

x47 * x50()
{
    x47 it;
    return it;
}

int x56()
{
    UNDECLARED_TYPE v1;
    int v2 oops;
    struct S {
	UNDECLARED_TYPE2 m1;
    };
}
int x64;
int x65;
int x66()
{
    struct S {
	int v3 oops;
    };
}
int x72;
int x73;

// must be at the end of the file
extern void __assert( int, char *, char *, int );
#define assert(expr)	__assert(expr,#expr,__FILE__,__LINE__)
int end_of_the_file( int *x )
{
    assert( x == 1 ;
}
