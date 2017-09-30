typedef unsigned size_t;
void * operator new( size_t, size_t = 1 );

struct T {
    void operator delete( void *, size_t = sizeof(T) );
    void *operator new( size_t, size_t = -1 );
};

void foo()
{
    T*p1 = new T;
    T*p2 = new (2) T;
}
