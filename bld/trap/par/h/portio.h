#if defined(__NT__)
        // We have direct port I/O for NT and Win9x
        extern unsigned outp( unsigned, unsigned );
        extern unsigned inp( unsigned );
#elif defined(__OS2__)
  #ifndef NOREMAPIO
    #if defined( _M_I86 )
        #define inp input_port
        #define outp output_port
    #else
        #define inp input_port
        #define outp output_port
    #endif
  #endif
    #ifdef _M_I86
        extern unsigned __far output_port( unsigned, unsigned );
        extern unsigned __far input_port( unsigned );
    #else
        extern unsigned short __far16 __pascal DosPortAccess( unsigned short, unsigned short, unsigned short, unsigned short );
        extern unsigned short __far16 __pascal output_port( unsigned short, unsigned short );
        extern unsigned short __far16 __pascal input_port( unsigned short );
    #endif
#elif defined(__NETWARE__)
    extern char inp( int );
    #pragma aux inp = "in al,dx" \
        __parm __routine    [__dx] \
        __value             [__al]
    extern void outp( int, int );
    #pragma aux outp = "out dx,al" \
        __parm __routine [__dx] [__ax]
#else
    #include <conio.h>
#endif


