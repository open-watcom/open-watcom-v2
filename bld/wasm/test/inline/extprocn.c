#ifdef __cplusplus
extern "C" {
#endif

extern void near procn( void );

extern void calln( void);
#pragma aux calln = "call procn"

void test( void )
{
    calln();
}

#ifdef __cplusplus
}
#endif
