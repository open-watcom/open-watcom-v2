typedef int F( int );

int x3( int )	// OK
{
    return 0;
}

F x8		// error
{
    return 0;
}

F x13;
int x13( int )	// OK
{
    return 0;
}

int x19( int );
F x19		// error
{
    return 0;
}

void (*x25( int a, int b ))( int c, int d )
{
    ++a;	// OK
    ++b;	// OK
    ++c;	// error
    ++d;	// error
    return 0;
}

void (*x34())( int c, int d )
{
    ++a;	// error
    ++b;	// error
    ++c;	// error
    ++d;	// error
    return 0;
}

void (*x43(void))( int c, int d )
{
    ++a;	// error
    ++b;	// error
    ++c;	// error
    ++d;	// error
    return 0;
}
