/*
 * tests if you are running in a windows dos box
 */
extern char IsWindows( void );
#pragma aux IsWindows = \
        0xb8 0x00 0x16  /* mov ax,1600h */ \
        0xcd 0x2f       /* int 2f */ \
        value [al];

main( void )
{
    char        rc;

    rc = IsWindows();
    if( rc == 0 || rc == 0x80 ) exit( 0 );
    exit( 1 );
}
