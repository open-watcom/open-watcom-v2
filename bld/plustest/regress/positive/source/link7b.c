extern "C" int ack( void *p )
{
    p = p;
    return 'a';
}
extern "C" int foo( void *p )
{
    p = p;
    return 'f';
}
extern "C" int bar( void )
{
    return 'b';
}

#if defined( M_I86 ) || defined( M_I386 )

#pragma aux ack "__S__ack_v_i";
#pragma aux foo "__S__foo_v_i";
#pragma aux bar "__S__bar_v_i";

#endif
