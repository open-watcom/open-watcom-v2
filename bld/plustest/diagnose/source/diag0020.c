typedef unsigned size_t;

int *operator new( int );		// bad return, bad first arg

void *operator new( size_t );
void *operator new( size_t, int );

int operator delete ( int * );		// bad return, bad first arg

void operator delete ( void *, size_t );// 2 args not allowed in file scope

struct S1 {
    int operator delete ( int * );	// bad first arg
};

struct S2 {
    void operator delete ( int *, int );	// both args bad
};

struct S3 {
    void operator delete ( void *, size_t );	// OK
};

struct S4 {
    void operator delete ( void * );
    void operator delete ( void *, size_t );	// cannot be overloaded
};
