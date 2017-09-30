extern void import( const void * );

static void fn() {}

void external()
{
    const void * foo = fn;
    import( foo );
}

#error this should be the only message
