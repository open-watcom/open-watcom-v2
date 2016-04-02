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
* Description:  Run-time exception handling.
*
****************************************************************************/


#include "ftnstd.h"
#include <signal.h>
#if defined( __DOS__ ) || defined( __WINDOWS__ )
  #include <dos.h>
#elif defined( _M_IX86 )
  #include <i86.h>
#endif
#if defined( __OS2__ )
    #define INCL_DOSMISC
  #if defined( _M_I86 )
    #define __OS2_286__
  #else
    #define __OS2_386__
  #endif
#endif
#include "rtstack.h"
#include "frtdata.h"
#include "fthread.h"
#include "xfflags.h"
#if defined( __DOS__ ) || defined( __WINDOWS__ )
  #include "extender.h"
#endif
#include "ftextfun.h"
#include "errcod.h"
#include "rundat.h"
#if defined( __NT__ )
  #include "enterdb.h"
#endif
#include "thread.h"
#include "fptraps.h"
#include "rttraps.h"


#if defined( __OS2_286__ )
  #define       _handler        __interrupt __pascal __far
#elif defined( __DOS__ ) || defined( __WINDOWS__ )
  #define       _handler        __interrupt
#else
  #define       _handler
#endif

typedef void            (*fsig_func)( intstar4 );

#if defined( __WINDOWS_386__ )

#elif defined( __OS2_286__ ) || defined( __DOS__ ) || defined( __WINDOWS__ )
  extern byte           IntOverFlow;
         void           (* __UserIOvFlHandler)(intstar4) = { (fsig_func)SIG_DFL };
  extern byte           IntDivBy0;
         void           (* __UserIDivZHandler)(intstar4) = { (fsig_func)SIG_DFL };
  static void           (_handler *ISave)(void);
  static void           (_handler *ZSave)(void);
#endif

#if defined( __DOS__ )
  extern byte           BreakVector;
         void           (* __UserBreakHandler)(intstar4) = { (fsig_func)SIG_DFL };
  static void           (_handler *CBSave)(void);
 #if defined( __386__ )
  static unsigned long  CBRealSave;
 #endif
#endif

#if defined( __OS2_386__ ) || defined( __NT__ )
  extern byte           __ExceptionHandled;
  static void           IOvFlSignal(int);
#endif

#if (defined( __DOS__ ) || defined( __WINDOWS__ )) && defined( __386__ )
  void _movestack( unsigned_32 );
  #pragma aux _movestack = \
        "push ds"            \
        "pop  ss"             \
        "mov  esp,eax"        \
        parm [eax] modify [esp];

  #include "realvec.h"

#endif


#if defined( __WINDOWS__ )

#else

static  void    ProcessBreak( void ) {
//==============================

    _RWD_XcptFlags &= ~XF_ERR_MASK;
    _RWD_XcptFlags |= XF_LIMIT_ERR | XF_KO_INTERRUPT;
    if( _RWD_XcptFlags & XF_IO_INTERRUPTABLE ) {
        _RWD_XcptFlags |= XF_IO_INTERRUPTED;
 #if defined( __OS2_386__ ) || defined( __NT__ )
    } else {
        __ExceptionHandled = 0;
 #endif
    }
}

static  void    BreakSignal( int sig ) {
//===================================================

    sig = sig;
    signal( SIGINT, BreakSignal );
 #if defined( __OS2__ ) || defined( __NT__ )
    signal( SIGBREAK, BreakSignal );
 #endif
    ProcessBreak();
}

#endif

#if defined( __DOS__ )
static  void    _handler BreakHandler( void ) {
//=============================================

    _enable();
    if( __UserBreakHandler != (fsig_func)SIG_DFL ) {
        if( (__UserBreakHandler != (fsig_func)SIG_IGN) &&
            (__UserBreakHandler != (fsig_func)SIG_ERR) ) {
            __UserBreakHandler( SIGBREAK );
        }
    } else {
        ProcessBreak();
    }
}
#endif


#if defined( __WINDOWS_386__ )

#elif defined( __DOS__ ) || defined( __WINDOWS__ ) || defined( __OS2__ ) || defined( __NT__ )

static  void    ProcessIDivZ( void ) {
//==============================

    RTErr( KO_IDIV_ZERO );
}

static  void    ProcessIOvFl( void ) {
//==============================

    // Set flag so that we report an overflow when we read an integer
    // regardless of whether user wants integer overflows reported
    _RWD_XcptFlags |= XF_IOVERFLOW;
}
#endif

#if defined( __WINDOWS_386__ )

#elif defined( __OS2_386__ ) || defined( __NT__ )

static  void    IDivZSignal( int sig ) {
//===================================================

    sig = sig;
    ProcessIDivZ();
}

static  void    IOvFlSignal( int sig ) {
//===================================================

    sig = sig;
    // re-signal exception since we may get many overflows
    // before we actually report the condition
    signal( SIGIOVFL, IOvFlSignal );
    ProcessIOvFl();
}

#elif defined( __DOS__ ) || defined( __WINDOWS__ ) || defined( __OS2_286__ )

static  void    _handler IDivZHandler( void ) {
//=============================================

 #if defined( __DOS__ ) || defined( __WINDOWS__ )
    _enable();
  #ifdef __386__
    // Under pharlap, ss != ds when this interrupt is taken.
    // But in order for our error reporting code to work we have to
    // set ss = ds.
    _movestack( _STACKLOW+512 );
  #endif
 #endif
    if( __UserIDivZHandler != (fsig_func)SIG_DFL ) {
        if( (__UserIDivZHandler != (fsig_func)SIG_IGN) &&
            (__UserIDivZHandler != (fsig_func)SIG_ERR) ) {
            __UserIDivZHandler(SIGIDIVZ);
        }
    } else {
        ProcessIDivZ();
    }
}

static  void    _handler IOvFlHandler( void ) {
//=============================================

 #if defined( __DOS__ ) || defined( __WINDOWS__ )
    _enable();
 #endif
    if( __UserIOvFlHandler != (fsig_func)SIG_DFL ) {
        if( (__UserIOvFlHandler != (fsig_func)SIG_IGN) &&
            (__UserIOvFlHandler != (fsig_func)SIG_ERR) ) {
            __UserIOvFlHandler(SIGIOVFL);
        }
    } else {
        ProcessIOvFl();
    }
}
#endif

static  void    AbnormalTerm( int dummy ) {
//==============================

    RTErr( CP_TERMINATE );
}

int     __EnableF77RTExceptionHandling( void ) {
//==============================================

#if defined( __NT__ )
    return( !DebuggerPresent() );
#else
    return( 1 );
#endif
}

void    R_TrapInit( void ) {
//====================

    int enable_excpt;

    enable_excpt = __EnableF77RTExceptionHandling();

    if( enable_excpt ) {
        signal( SIGSEGV, AbnormalTerm );
        signal( SIGILL, AbnormalTerm );
    }
#if !defined( __WINDOWS__ )
    signal( SIGINT, BreakSignal );
#endif
#if defined( __DOS__ )
    CBSave = _dos_getvect( BreakVector );
    _dos_setvect( BreakVector, BreakHandler );
  #if defined( __386__ )
    if( _IsPharLap() ) {
        CBRealSave = _dos_getrealvect( BreakVector );
        _dos_setvectp( BreakVector, BreakHandler );
    }
  #endif
    ISave = _dos_getvect( IntOverFlow );
    _dos_setvect( IntOverFlow, IOvFlHandler );
    ZSave = _dos_getvect( IntDivBy0 );
    _dos_setvect( IntDivBy0, IDivZHandler );
#elif defined( __WINDOWS__ )
 #if !defined( __386__ )
    ISave = _dos_getvect( IntOverFlow );
    _dos_setvect( IntOverFlow, IOvFlHandler );
    ZSave = _dos_getvect( IntDivBy0 );
    _dos_setvect( IntDivBy0, IDivZHandler );
 #endif
#elif defined( __OS2_386__ ) || defined( __NT__ )
    signal( SIGBREAK, BreakSignal );
    if( enable_excpt ) {
        signal( SIGIDIVZ, IDivZSignal );
        signal( SIGIOVFL, IOvFlSignal );
    }
#elif defined( __OS2_286__ )
    signal( SIGBREAK, BreakSignal );
    DosSetVec( IntOverFlow, (PFN)IOvFlHandler, (PFN FAR *)&ISave );
    DosSetVec( IntDivBy0, (PFN)IDivZHandler, (PFN FAR *)&ZSave );
#endif
    FPTrapInit();
}

void    R_TrapFini( void ) {
//====================

#if defined( __DOS__ )
    _dos_setvect( BreakVector, CBSave );
 #if defined( __386__ )
    if( _IsPharLap() ) {
        _dos_setrealvect( BreakVector, CBRealSave );
    }
 #endif
    _dos_setvect( IntOverFlow, ISave );
    _dos_setvect( IntDivBy0, ZSave );
#elif defined( __WINDOWS__ )
 #if !defined( __386__ )
    _dos_setvect( IntOverFlow, ISave );
    _dos_setvect( IntDivBy0, ZSave );
 #endif
#elif defined( __OS2_286__ )
    DosSetVec( IntOverFlow, (PFN)ISave, (PFN FAR *)&ISave );
    DosSetVec( IntDivBy0, (PFN)ZSave, (PFN FAR *)&ZSave );
#endif
    FPTrapFini();
}

#if 0
extern void             (*_ExceptionInit)( void );
extern void             (*_ExceptionFini)( void );

void    __InitExceptionVectors( void ) {
//================================

    _ExceptionInit = &R_TrapInit;
    _ExceptionFini = &R_TrapFini;
}
#endif
