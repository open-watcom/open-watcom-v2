#if !defined( ARCH ) || ( ARCH != 386 )
    #error system dependent test for 386
#else

// 93/12/13 (jww) fixed

// compile -mh -w3
// generates two warnings for same source location
// grb
char *p;
char *r;
int i = p - r;
#error no warning anymore


#endif
