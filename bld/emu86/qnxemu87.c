#define _NO_SLIB

#ifdef __386__
/*
 * Stub routines/variable to save space
 */

int __kererr(int errcode)
{
    errcode = errcode;
    return( -1 );
}

#pragma aux __FiniRtns "*"
void __FiniRtns()
{
}

unsigned _endheap;
#endif
#if defined(__386__) || __WATCOMC__ >= 900
unsigned _STACKTOP;
unsigned _STACKLOW;
unsigned _curbrk;
#endif

#include <string.h>
#include <sys/io_msg.h>
#include <sys/proc_msg.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <sys/utsname.h>
#include <errno.h>
#include <i86.h>

extern void __interrupt __far __int7();
#pragma aux __int7 "*";

#if defined(QNX16)
#define EMULATOR "emu87_16"
#define EMU "16-bit emulator"
#elif !defined(__386__)
#define EMULATOR "emu87_32"
#define EMU "16-bit emulator"
#else
#define EMULATOR "emu387"
#define EMU "32-bit emulator"
#endif

char    _32bit;

#define USAGE   "usage: " EMULATOR " [-f] &\n" \
                "  -f forces installation even if 80(x)87 is present\n"

#define Msg( string ) write( 2, string, sizeof( string ) - 1 )

#ifdef __386__
#define __FAR
#else
#define __FAR   __far
#endif


int (__send)( pid, msg1, msg2, nbytes1, nbytes2 )
pid_t       pid;
void __FAR    *msg1;
void __FAR    *msg2;
unsigned    nbytes1;
unsigned    nbytes2;
{
    struct _mxfer_entry xmsg1;
    struct _mxfer_entry xmsg2;

    _setmx(&xmsg1, msg1, nbytes1);
    _setmx(&xmsg2, msg2, nbytes2);

    return( __sendmx( pid, 1, 1, &xmsg1, &xmsg2 ) );
}

pid_t (__receive)( pid, msg, nbytes )
pid_t        pid;
void __FAR    *msg;
unsigned     nbytes;
{
    struct _mxfer_entry xmsg;

    _setmx(&xmsg, msg, nbytes);

    return( __receivemx( pid, 1, &xmsg ) );
}


#pragma off(unreferenced);
#ifdef __386__
#pragma aux _CMain "_*";
_CMain(int argc, char **argv, char **arge)
{
    int install;
    char    *cp;

    cp = argc >= 2 ? argv[1] : "";
#else
#pragma aux _CMain "_*" parm [bx] [cx] [di] [dx]; /* left to right.*/
                        /* ( free, n,  cmd, stk_bot  ) */

void _CMain( free, n, cmd, stk_bot )
    void     __near *free;      /* start of free space                  */
    short unsigned   n;         /* number of bytes                      */
    struct _proc_spawn __near *cmd;/* pointer to spawn msg              */
    short unsigned  stk_bot;    /* bottom of stack                      */
{
    int         install;
    char        *cp;
    int         argc;

    argc = cmd->argc;
    cp = cmd->data;
    while( *cp ) ++cp; /* skip executable name */
    ++cp;
    while( *cp ) ++cp; /* skip argv[0] */
    ++cp;
#endif
    if( *cp == '?' ) {
        Msg( USAGE );
        __qnx_exit( 0 );
    }
    install = 1;
    if( has_fpu() ) install = 0;
    if( argc >= 2 ) {
#if 0
        while( *cp ) ++cp; /* skip command name */
        ++cp;
#endif
        if( cp[0] == '-' && cp[1] == 'f' ) {
            if( !install ) {
                Msg( EMU " force install\n" );
            }
            install = 1;
        }
    }
    if( install ) main();
    __qnx_exit( 0 );
}
#pragma on(unreferenced);

__Null_Argv() {} /* to satisfy main_entry */
__exit_with_msg() {}
__fatal_runtime_error() {}

main()
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
        __qnx_exit( 2 );
    }

    msg.s.type    = _PROC_EMUL87;
    msg.s.segment = FP_SEG(__int7);
    msg.s.offset  = FP_OFF(__int7);
    msg.s.nbytes  = 128;    /* size of emulator save area */
    Send( PROC_PID, &msg.s, &msg.r, sizeof(msg.s), sizeof(msg.r) );
    if( msg.r.status != EOK ) {
        Msg( "unable to attach " EMU "\n" );
        __qnx_exit( 2 );
    }
    /* close all file descriptors to release console */
    for( i = 0; i < 10; ++i ) {
        close( i );
    }
    for(;;) {
        Receive( 0, &msg, 0 );
    }
}


void __qnx_exit( status )
    int status;
{
    struct _proc_terminate msg;

    msg.type = _PROC_TERMINATE;
    msg.signum = 0;
    msg.xstatus = (long)status;
    Send( PROC_PID, &msg, &msg, sizeof(msg), 0 );
    /* The send will NOT return */
}


(qnx_fd_detach)(fd)
int fd;
    {
    union sfd {
        struct _proc_fd         s;
        struct _proc_fd_reply1  r;
        } msg;

    memset(&msg.s, 0, sizeof(msg.s));
    msg.s.type = _PROC_FD;
    msg.s.subtype = _PROC_SUB_DETACH;
    msg.s.fd = fd;

    if(Send(PROC_PID, &msg.s, &msg.r, sizeof(msg.s), sizeof(msg.r)) == -1) {
        return(-1);
        }

    if(msg.r.status) {
        errno = msg.r.status;
        return(-1);
        }

    return(0);
    }


int (close)(fd)
int     fd;
    {
    union _close
        {
        struct _io_close        s;
        struct _io_close_reply  r;
        }       msg;
    struct _mxfer_entry mx_entry[1];

    /*
     *  Set up the message header.
     */
    msg.s.type = _IO_CLOSE;
    msg.s.fd   = fd;
    msg.s.zero = 0;
    _setmx( &mx_entry[0], &msg, sizeof(msg) );

    if(Sendfdmx(fd, 1, 1, &mx_entry, &mx_entry) == -1)
        {
        qnx_fd_detach(fd);
        return(-1);
        }

    if(msg.r.status != EOK)
        {
        errno = msg.r.status;
        qnx_fd_detach(fd);
        return(-1);
        }

    qnx_fd_detach(fd);
    return(EOK);
    }


int (write)(fd, buf, nbytes)
    int              fd;
    const void      *buf;
    unsigned int     nbytes;
{
    register unsigned   count = 0;
    union _write
        {
        struct _io_write        s;
        struct _io_write_reply  r;
        }               msg;
    struct _mxfer_entry mx_entry[2];

    do
        {
        /*
         *  Set up the message header.
         */
        msg.s.type    = _IO_WRITE;
        msg.s.fd      = fd;
        msg.s.nbytes  = nbytes - count;
        msg.s.zero    = 0;

        _setmx(&mx_entry[0],&msg,sizeof(struct _io_write)-sizeof(msg.s.data));

        /*
         *  Setup the message data description.
         */
        _setmx( &mx_entry[1], (const char *)buf + count, nbytes - count );

        if(Sendfdmx(fd, 2, 1, &mx_entry, &mx_entry) == -1)
            {
            errno = EINTR;
            return(-1);
            }

        if(msg.r.status != EOK  &&  msg.r.status != EMORE)
            {
            errno = msg.r.status;
            return(-1);
            }

        count += msg.r.nbytes;
        }
    while(count < nbytes  &&  msg.r.status == EMORE);

    return(count);
}

static int has_fpu()
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
