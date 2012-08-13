#ifdef __cplusplus
extern "C" {
#endif

extern void far procf( void );

extern void callf( void);
#pragma aux callf = "call procf"

void test( void )
{
    callf();
}

#ifdef __cplusplus
}
#endif
