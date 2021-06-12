#define _NO_SLIB

#include <string.h>
#include <unistd.h>
#include <sys/io_msg.h>
#include <sys/proc_msg.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <sys/utsname.h>
#include <errno.h>
#include <i86.h>
#include "initfini.h"
#include "cmain.h"
#include "exitwmsg.h"
#include "owqnx.h"


#ifdef _M_I86
#define __FAR   __far
#else
#define __FAR
#endif

#if defined( QNX16 )
#define EMULATOR "emu87_16"
#define EMU      "16-bit emulator"
#elif defined( QNX32 )
#define EMULATOR "emu87_32"
#define EMU      "16-bit emulator"
#else
#define EMULATOR "emu387"
#define EMU      "32-bit emulator"
#endif

#define USAGE   "usage: " EMULATOR " [-f] &\n" \
                "  -f forces installation even if 80(x)87 is present\n"

#define Msg( string ) write( 2, string, sizeof( string ) - 1 )

extern void __interrupt __int7();
#pragma aux __int7 "*";

/***************************************
 *
 * Stub variable to save space
 */
#ifdef __386__
unsigned _endheap;
#endif

unsigned _STACKTOP;
unsigned _STACKLOW;
unsigned _curbrk;
/***************************************/

static char    _32bit;

/***************************************
 *
 * Stub routines to save space
 */
#ifdef __386__
int __kererr( int errcode )
{
    errcode = errcode;
    return( -1 );
}

#pragma aux __FiniRtns "*"
void __FiniRtns() {}
#endif

//void __Null_Argv() {} /* to satisfy main_entry */
__declspec(aborts) void __exit_with_msg( char __FAR *msg, int rc )
{
    /* unused parameters */ (void)msg; (void)rc;
}
__declspec(aborts) void __fatal_runtime_error( char __FAR *msg, int rc )
{
    /* unused parameters */ (void)msg; (void)rc;
}
/***************************************/

int main( void )
{
    union {
        struct _proc_emul87         s;
        struct _proc_emul87_reply   r;
    }           msg;
    int         i;

#ifdef QNX16
    if( _32bit ) {
#else
    if( !_32bit ) {
#endif
        Msg( "incorrect process manager for " EMU "\n" );
        return( 2 );
    }

    msg.s.type    = _PROC_EMUL87;
    msg.s.segment = _FP_SEG( __int7 );
    msg.s.offset  = _FP_OFF( __int7 );
    msg.s.nbytes  = 128;    /* size of emulator save area */
    Send( PROC_PID, &msg.s, &msg.r, sizeof( msg.s ), sizeof( msg.r ) );
    if( msg.r.status != EOK ) {
        Msg( "unable to attach " EMU "\n" );
        return( 2 );
    }
    /* close all file descriptors to release console */
    for( i = 0; i < 10; ++i ) {
        close( i );
    }
    for( ;; ) {
        Receive( 0, &msg, 0 );
    }
    return( 0 );
}

static int has_fpu( void )
{
    union {
        struct _proc_osinfo         s;
        struct _proc_osinfo_reply   r;
    } msg;

    msg.s.type = _PROC_OSINFO;

    Send( PROC_PID, &msg.s, &msg.r, sizeof( msg.s ), sizeof( msg.r ) );
    if( msg.r.data.sflags & _PSF_32BIT ) {
        _32bit = 1;
    } else {
        _32bit = 0;
    }
    return( msg.r.data.fpu != 0 );
}

#ifdef __386__
void _CMain( int argc, char **argv, char **arge )
{
    int         install;
    char        *cp;
    int         rc;

    cp = ( argc >= 2 ) ? argv[1] : "";
#else
void _CMain(
    void                __near *free,   /* start of free space  */
    short unsigned      n,              /* number of bytes      */
    struct _proc_spawn  __near *cmd,    /* pointer to spawn msg */
    short unsigned      stk_bot,        /* bottom of stack      */
    pid_t               my_pid )        /* my pid               */
{
    int         install;
    char        *cp;
    int         argc;
    int         rc;

    argc = cmd->argc;
    cp = cmd->data;
    while( *cp != '\0' )        /* skip executable name */
        ++cp;
    ++cp;
    while( *cp != '\0' )        /* skip argv[0] */
        ++cp;
    ++cp;
#endif
    if( *cp == '?' ) {
        Msg( USAGE );
        __qnx_exit( 0 );
    }
    install = 1;
    if( has_fpu() )
        install = 0;
    if( argc >= 2 ) {
#if 0
        while( *cp != '\0' )    /* skip command name */
            ++cp;
        ++cp;
#endif
        if( cp[0] == '-' && cp[1] == 'f' ) {
            if( !install ) {
                Msg( EMU " force install\n" );
            }
            install = 1;
        }
    }
    rc = 0;
    if( install )
        rc = main();
    __qnx_exit( rc );
}
