??=error trigraph ??= test
int * operator new []( int x )
{
    return new int[x];
}
struct S {
    void *operator new []()
    {
	return new char;
    }
    void operator delete []( void *, double )
    {
    }
};
static void operator delete []( void *p )
{
    ::delete p;
}
int operator delete( void * ){ return 0; }
int operator delete []( void * ){ return 0; }
void operator delete( int );
void operator delete []( int );
#if 1
// error should be on the line before
// line
// line
// line
// line
// line
// line
