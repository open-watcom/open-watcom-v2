/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  Default Win32 exception handlers.
*
****************************************************************************/


#include "variety.h"
#include <windows.h>
#include <excpt.h>
#include <float.h>
#include <signal.h>
#include <unistd.h>
#include "iomode.h"
#include "rtdata.h"
#include "sigfunc.h"
#include "fpusig.h"
#include "excptwnt.h"
#include "clearfpe.h"
#include "ntconio.h"
#include "thread.h"
#include "_xtoa.h"


LONG WINAPI __ReportException( EXCEPTION_POINTERS *rec );
int __cdecl __ExceptionFilter( LPEXCEPTION_RECORD ex,
                               LPVOID establisher_frame,
                               LPCONTEXT context,
                               LPVOID dispatch_context );

__sig_func  (*__oscode_check_func)( int, long ) = NULL;
int         (*__raise_func)( int )              = NULL;
unsigned char   __ExceptionHandled;
unsigned char   __ReportInvoked;

static int _my_GetActiveWindow( void ) {
    HANDLE hdl;
    typedef HWND (__stdcall *gaw_type)( void );
    gaw_type gaw;
    HWND rc = 0;

    hdl = LoadLibrary( "USER32.DLL" );
    if( hdl != 0 ) {
        gaw = (gaw_type) GetProcAddress( hdl, "GetActiveWindow" );
        if( gaw != 0 ) {
            rc = (*gaw)();
        }
    }
    return( rc != 0 );
}

static void fmt_hex( char *buf, char *fmt, void *hex ) {

    char *ptr = NULL;
    unsigned long value;

    /*
     * find the end of the buffer
     */
    while( *buf != '\0' ) {
        buf++;
    }

    /*
     * copy the format string into the buffer
     */
    for( ;; ) {
        *buf = *fmt;
        if( *fmt == '\0' ) break;
        if( *fmt == '0' && *(fmt+1) == 'x' ) {
            /* memorize the location of the hex field */
            ptr = buf+9;
        }
        buf++;
        fmt++;
    }

    /*
     * format the hex value, if a location was found
     */
    if( ptr != NULL ) {
        value = (unsigned long)hex;
        while( value ) {
            *ptr = __Alphabet[value & 0xf];
            value = value >> 4;
            ptr--;
        }
    }
}

/*
 * Make function external so we can set it manually
 */

LONG WINAPI __ReportException( EXCEPTION_POINTERS *rec )
{
    EXCEPTION_RECORD  *ex = rec->ExceptionRecord;
#if defined( _M_IX86 )
    CONTEXT           *context = rec->ContextRecord;
    DWORD             *sp;
    int               i;
#endif
    char              buff[256];
    DWORD             written;

    // if we are active then we've done crashed ourselves.
    if( __ReportInvoked )
        return( EXCEPTION_CONTINUE_SEARCH );

    __ReportInvoked = 1;    // indicate that we ran

    /*
     * Test to see if there is an active window.
     */
    if( _my_GetActiveWindow() || ( __NTConsoleOutput() == (HANDLE)-1 )) {
        return( EXCEPTION_CONTINUE_SEARCH );
    }

    /*
     * prepare the mesage buffer
     */
    buff[0] = '\0';

    /*
     * Lets see what caused the exception.
     */
    switch( ex->ExceptionCode ) {
#if defined( _M_IX86 )
    case STATUS_FLOAT_STACK_CHECK:
        if( context->FloatSave.StatusWord & SW_C1 ) {
            fmt_hex( buff, "The instruction at 0x00000000 caused a "
                "stack overflow floating point\nexception.\n",
                ex->ExceptionAddress );
        } else {
            fmt_hex( buff, "The instruction at 0x00000000 caused a "
                "stack underflow floating point\nexception.\n",
                ex->ExceptionAddress );
        }
        break;
#elif defined( __AXP__ )
        // no alpha specific floating point exceptions
#elif defined( __PPC__ )
        // no ppc specific floating point exceptions
#endif
    case STATUS_FLOAT_DENORMAL_OPERAND:
        fmt_hex( buff, "The instruction at 0x00000000 caused a denormal "
            "operand floating point\nexception.\n",
            ex->ExceptionAddress );
        break;
    case STATUS_FLOAT_DIVIDE_BY_ZERO:
        fmt_hex( buff, "The instruction at 0x00000000 caused a division "
            "by zero floating point\nexception.\n",
            ex->ExceptionAddress );
        break;
    case STATUS_FLOAT_INEXACT_RESULT:
        fmt_hex( buff, "The instruction at 0x00000000 caused an inexact "
            "value floating point\nexception.\n",
            ex->ExceptionAddress );
        break;
    case STATUS_FLOAT_OVERFLOW:
        fmt_hex( buff, "The instruction at 0x00000000 caused an overflow "
            "floating point exception.\n",
            ex->ExceptionAddress );
        break;
    case STATUS_FLOAT_UNDERFLOW:
        fmt_hex( buff, "The instruction at 0x00000000 caused an underflow "
            "floating point exception.\n",
            ex->ExceptionAddress );
        break;
    case STATUS_FLOAT_INVALID_OPERATION:
        fmt_hex( buff, "The instruction at 0x00000000 caused an invalid "
            "operation floating point\nexception.\n",
            ex->ExceptionAddress );
        break;
#if defined( __AXP__ )
    case STATUS_DATATYPE_MISALIGNMENT:
        fmt_hex( buff, "The instruction at 0x00000000 tried to reference ",
            ex->ExceptionAddress );
        fmt_hex( buff, "unaligned data at 0x00000000.\n",
            (void *)(ex->ExceptionInformation[2]) );
        break;
#endif
    case STATUS_ACCESS_VIOLATION:
        fmt_hex( buff, "The instruction at 0x00000000 referenced memory ",
            ex->ExceptionAddress );
        fmt_hex( buff, "at 0x00000000.\nThe memory could not be ",
            (void *)(ex->ExceptionInformation[1]) );
        if( ex->ExceptionInformation[0] == 0 ) {
            fmt_hex( buff, "read.\n", 0 );
        } else {
            fmt_hex( buff, "written.\n", 0 );
        }
        break;
    case STATUS_PRIVILEGED_INSTRUCTION:
        fmt_hex( buff, "A privileged instruction was executed at "
            "address 0x00000000.\n", ex->ExceptionAddress );
        break;
    case STATUS_ILLEGAL_INSTRUCTION:
        fmt_hex( buff, "An illegal instruction was executed at "
            "address 0x00000000.\n", ex->ExceptionAddress );
        break;
    case STATUS_INTEGER_DIVIDE_BY_ZERO:
        fmt_hex( buff, "An integer divide by zero was encountered at "
            "address 0x00000000.\n", ex->ExceptionAddress );
        break;
    case STATUS_STACK_OVERFLOW:
        fmt_hex( buff, "A stack overflow was encountered at address "
            "0x00000000.\n", ex->ExceptionAddress );
        break;
    default:
        fmt_hex( buff, "The program encountered exception 0x00000000 at ",
            (void *)ex->ExceptionCode );
        fmt_hex( buff, "address 0x00000000 and\ncannot continue.\n",
            ex->ExceptionAddress );
        break;
    }

    WriteFile( NT_STDERR_FILENO, buff, strlen( buff ), &written, NULL );

#if defined( _M_IX86 )
    buff[0] = '\0';
    fmt_hex( buff, "Exception fielded by 0x00000000\n", __ReportException );
    WriteFile( NT_STDERR_FILENO, buff, strlen(buff), &written, NULL );
    buff[0] = '\0';
    if( context->ContextFlags & CONTEXT_INTEGER ) {
        fmt_hex( buff, "EAX=0x00000000 ", (void *)context->Eax );
        fmt_hex( buff, "EBX=0x00000000 ", (void *)context->Ebx );
        fmt_hex( buff, "ECX=0x00000000 ", (void *)context->Ecx );
        fmt_hex( buff, "EDX=0x00000000\n", (void *)context->Edx );
        fmt_hex( buff, "ESI=0x00000000 ", (void *)context->Esi );
        fmt_hex( buff, "EDI=0x00000000 ", (void *)context->Edi );
    }
    if( context->ContextFlags & CONTEXT_CONTROL ) {
        fmt_hex( buff, "EBP=0x00000000 ", (void *)context->Ebp );
        fmt_hex( buff, "ESP=0x00000000\n", (void *)context->Esp );
        fmt_hex( buff, "EIP=0x00000000 ", (void *)context->Eip );
        fmt_hex( buff, "EFL=0x00000000 ", (void *)context->EFlags );
        fmt_hex( buff, "CS =0x00000000 ", (void *)context->SegCs );
        fmt_hex( buff, "SS =0x00000000\n", (void *)context->SegSs );
    }
    if( context->ContextFlags & CONTEXT_SEGMENTS ) {
        fmt_hex( buff, "DS =0x00000000 ", (void *)context->SegDs );
        fmt_hex( buff, "ES =0x00000000 ", (void *)context->SegEs );
        fmt_hex( buff, "FS =0x00000000 ", (void *)context->SegFs );
        fmt_hex( buff, "GS =0x00000000\n", (void *)context->SegGs );
    }
    WriteFile( NT_STDERR_FILENO, buff, strlen(buff), &written, NULL );
    buff[0] = '\0';
    if( context->ContextFlags & CONTEXT_CONTROL ) {
        sp = (DWORD *)context->Esp;
        fmt_hex( buff, "Stack dump (SS:ESP)\n", 0 );
        for( i = 1; i <= 72; i++) {
            if(( (long)sp & 0x0000FFFF ) == 0 ) {
                fmt_hex( buff, "-stack end\n", 0 );
            } else {
                fmt_hex( buff, "0x00000000 ", GetFromSS( sp ) );
            }
            if(( i % 6 ) == 0 ) {
                fmt_hex( buff, "\n", 0 );
            }
            WriteFile( NT_STDERR_FILENO, buff, strlen( buff ), &written, NULL );
            buff[0] = '\0';
            if(( (long)sp & 0x0000FFFF ) == 0 )
                break;
            sp++;
        }
    }
#endif

    return( EXCEPTION_EXECUTE_HANDLER );
}


void __DefaultExceptionHandler( void )
{
    LPTOP_LEVEL_EXCEPTION_FILTER top_filter;

    // This routine is called whenever a new process begins.
    // Install an exception handler and then check to see if we already
    // have one. If we did, set it back to the previous one. This ensures
    // we always have one set up and permits apps to install their own.

    top_filter = SetUnhandledExceptionFilter( __ReportException );
    if( top_filter != NULL ) {
        SetUnhandledExceptionFilter( top_filter );
    }
}


// Note: this needs to be cdecl for Win32s and Windows 95
//       Windows NT doesn't care if it is cdecl or stdcall

int __cdecl __ExceptionFilter( LPEXCEPTION_RECORD ex,
                               LPVOID establisher_frame,
                               LPCONTEXT context,
                               LPVOID dispatch_context )
{
    int          sig;
    int          fpe;
#if defined( _M_IX86 )
    char        *eip;
    status_word  sw;
    DWORD        tw;
#endif
    EXCEPTION_POINTERS rec;
    LONG         rv;

    /*
     * unused parms
     */
    dispatch_context  = dispatch_context;
    establisher_frame = establisher_frame;

    /*
     * Test some conditions we can immediately resolve.
     */
    if( ex->ExceptionFlags & UNWINDING )
        return( ExceptionContinueSearch );

    /*
     * Lets see what caused the exception.
     */
    switch( ex->ExceptionCode ) {
#if defined( _M_IX86 )
    case STATUS_FLOAT_STACK_CHECK:
        if( context->FloatSave.StatusWord & SW_C1 ) {
            fpe = FPE_STACKOVERFLOW;
        } else {
            fpe = FPE_STACKUNDERFLOW;
        }
        break;
#elif defined( __AXP__ )
        // no alpha specific floating point exceptions
#elif defined( __PPC__ )
        // no ppc specific floating point exceptions
#else
#error *** Platform Not Supported ***
#endif

    case STATUS_FLOAT_DENORMAL_OPERAND:
        fpe = FPE_DENORMAL;
        break;
    case STATUS_FLOAT_DIVIDE_BY_ZERO:
        fpe = FPE_ZERODIVIDE;
        break;
    case STATUS_FLOAT_INEXACT_RESULT:
        fpe = FPE_INEXACT;
        break;
    case STATUS_FLOAT_OVERFLOW:
        fpe = FPE_OVERFLOW;
        break;
    case STATUS_FLOAT_UNDERFLOW:
        fpe = FPE_UNDERFLOW;
        break;
    case STATUS_FLOAT_INVALID_OPERATION:
        fpe = FPE_INVALID;
#if defined( _M_IX86 )
        eip = (char *)context->FloatSave.ErrorOffset;

        if( *(unsigned short *)eip == 0xfad9 ) {        // caused by "fsqrt"
            fpe = FPE_SQRTNEG;
        } else if( *(unsigned short *)eip == 0xf1d9 ) { // caused by "fyl2x"
            fpe = FPE_LOGERR;
        } else if( *(unsigned short *)eip == 0xf8d9 ) { // caused by "fprem"
            fpe = FPE_MODERR;
        } else if( *(unsigned short *)eip == 0xf5d9 ) { // caused by "fprem1"
            fpe = FPE_MODERR;
        } else {
            if(( eip[0] == (char)0xdb ) || ( eip[0] == (char)0xdf )) {
                if(( eip[1] & 0x30 ) == 0x10 ) {        // caused by "fist(p)"
                    fpe = FPE_IOVERFLOW;
                }
            }
            if( !( eip[0] & 0x01 ) ) {
                if(( eip[1] & 0x30 ) == 0x30 ) {        // "fdiv" or "fidiv"
                    tw    = context->FloatSave.TagWord & 0x0000ffff;
                    sw.sw = context->FloatSave.StatusWord & 0x0000ffff;

                    if((( tw >> (sw.b.st << 1) ) & 0x01 ) == 0x01 ) {
                        fpe = FPE_ZERODIVIDE;
                    }
                }
            }
        }
#endif
        break;
    default:
        fpe = -1;
        break;
    }

    /*
     * If fpe != -1 then we have an identified floating point exception.
     * If there is a handler, invoke it.
     */
    if( fpe != -1 ) {
        __ExceptionHandled = 1;
        _ClearFPE();

        if( __sigfpe_handler( fpe ) != -1 ) {
            if( __ExceptionHandled ) {
#if defined( _M_IX86 )
                context->FloatSave.StatusWord &=
                    ~( SW_BUSY | SW_XCPT_FLAGS | SW_IREQ );
#elif defined( __AXP__ )
                ((unsigned long *)&context->Fpcr)[1] &=
                    ~(FPCR_SUMMARY_BIT | FPCR_XCPT_FLAGS);
#elif defined( __PPC__ )
                // Can we do something here?
#else
#error *** Platform Not Supported ***
#endif
                return( ExceptionContinueExecution );
            }
        }
    } else if( __raise_func ) {
        /*
         * If the signal handling code is linked in then we need to see if the
         * user has installed a signal handler.
         */
        __sig_func  func;

        for( sig = 1; sig <= __SIGLAST; sig++ ) {
            func = __oscode_check_func( sig, ex->ExceptionCode );
            if( func != NULL ) {
                if(( func == SIG_IGN ) || ( func == SIG_DFL ) || ( func == SIG_ERR )) {
                    break;
                }
                __ExceptionHandled = 1;
                (*__raise_func)( sig );
                if( __ExceptionHandled ) { // User has fixed up state
                    return( ExceptionContinueExecution );
                }
            }
        }
    }

    rec.ExceptionRecord = ex;
    rec.ContextRecord   = context;

    __ReportInvoked = 0;    // indicate our own last-chance handler has not run

    // Call __ReportException or an application-installed handler.
    // NOTE: if running under a debugger, the handler will NOT be called
    //       and it returns EXCEPTION_CONTINUE_SEARCH
    //
    // Possible rv values:
    //   -1 => EXCEPTION_CONTINUE_EXECUTION
    //    0 => EXCEPTION_CONTINUE_SEARCH
    //   +1 => EXCEPTION_EXECUTE_HANDLER
    //

    rv = UnhandledExceptionFilter( &rec );
    if( rv == EXCEPTION_EXECUTE_HANDLER ) {
        ExitProcess( -1 );
    } else if( rv == EXCEPTION_CONTINUE_EXECUTION ) {
        return( ExceptionContinueExecution );
    }
    return( ExceptionContinueSearch );
}


void __NewExceptionFilter( REGISTRATION_RECORD *rr )
{
    // This routine is called whenever a new process/thread begins.

    __XCPTHANDLER = rr;
#if defined( __386__ )
    rr->RegistrationRecordPrev = (LPVOID)GetFromFS( 0 );
    rr->RegistrationRecordFilter = __ExceptionFilter;
    PutToFS( (DWORD)rr, 0 );
#elif defined( __AXP__ )
    // __ExceptionFilter() installed in __NTMain()'s pdata
#elif defined( __PPC__ )
    // No idea yet.
#endif

}

void __DoneExceptionFilter( void )
{
#if defined( __386__ )
    REGISTRATION_RECORD *rr;
    rr = __XCPTHANDLER;
    if( rr ) {
        PutToFS( (DWORD)rr->RegistrationRecordPrev, 0 );
    }
#elif defined( __AXP__ )
    // Nothing to do
#elif defined( __PPC__ )
    // No idea yet.
#endif

    __XCPTHANDLER = NULL;
}
