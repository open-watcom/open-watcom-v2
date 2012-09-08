#define _NO_SLIB

#include <stdio.h>
#include <process.h>
#include <unistd.h>
#include <signal.h>
#include <sys/osinfo.h>

#define USAGE   "usage: emu87 [-f] &\n" \
                "  -f forces installation even if 80(x)87 is present\n"

#define Msg( string ) write( 2, string, sizeof( string ) - 1 )

#define EMU16_16        "/bin/emu87_16"
#define EMU16_32        "/bin/emu87_32"
#define EMU32_32        "/bin/emu387"

main( int argc, char *argv[] )
{
    struct  _osinfo     info;
    unsigned            i;

    if( argc > 1 && argv[1][0] == '?' ) {
        Msg( USAGE );
        exit( 0 );
    }
    /* ignore everything */
    for( i = _SIGMIN; i < _SIGMAX; ++i ) {
        signal( i, SIG_IGN );
    }
    qnx_osinfo( 0, &info );
    if( info.sflags & _PSF_32BIT ) {
        argv[0] = EMU32_32;
        if( spawnv( P_NOWAIT, argv[0], argv ) == -1 ) {
            Msg( "can not spawn 32-bit emulator\n" );
        }
        argv[0] = EMU16_32;
    } else {
        argv[0] = EMU16_16;
    }
    if( spawnv( P_NOWAIT, argv[0], argv ) == -1 ) {
        Msg( "can not spawn 16-bit emulator\n" );
    }
    exit( 0 );
}
