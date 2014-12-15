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
* Description:  Implementation of Floating-point environment for C99.
*
****************************************************************************/


#include "variety.h"
#include <fenv.h>
#include <inttypes.h>


/***************************************************************************/

#ifdef __386__

/* 32-bit versions */

_WCRTDATA const fenv_t  __fenv_h_default_environment = {
    0x127f,   /*CW*/
    0,
    0,        /*SW*/
    0,
    0xffff,   /*TAG*/
    0,
    0,0,0,0,0,0,
};

/* Load floating-point environment from memory to hardware */
extern void fenv_load( const fenv_t *env );
#pragma aux fenv_load = \
    "fldenv     [eax]"  \
    parm [eax]          \
    modify exact nomemory [];

/* Store floating-point environment from hardware to memory */
extern void fenv_store( fenv_t *env );
#pragma aux fenv_store =    \
    "fstenv     [eax]"      \
    parm [eax]              \
    modify exact [];

/* Load floating-point control word from memory to hardware */
extern void fpcw_load( const uint16_t *control );
#pragma aux fpcw_load = \
    "fldcw      [eax]"  \
    parm [eax]          \
    modify exact nomemory [];

/* Store floating-point control word from hardware to memory */
extern void fpcw_store( uint16_t *control );
#pragma aux fpcw_store =    \
    "fnstcw     [eax]"      \
    parm [eax]              \
    modify exact [];

/* Store floating-point status word from hardware to memory */
extern void fpsw_store( uint16_t *status );
#pragma aux fpsw_store =    \
    "fnstsw     [eax]"      \
    parm [eax]              \
    modify exact [];

#else

/* 16-bit versions */

_WCRTDATA const fenv_t  __fenv_h_default_environment = {
    0x127f,   /*CW*/
    0,        /*SW*/
    0xffff,   /*TAG*/
    0,0,0,0,
};

#if defined( __SMALL__ ) || defined( __MEDIUM__ )
    /* no need to load seg regs */
    #define DATA_SEG
#else
    /* seg reg needs to be loaded to point to data */
    #define DATA_SEG    es
#endif

/* Load floating-point environment from memory to hardware */
extern void fenv_load( const fenv_t *env );
#pragma aux fenv_load = \
    "fldenv     [di]"   \
    parm [DATA_SEG di]  \
    modify exact nomemory [];

/* Store floating-point environment from hardware to memory */
extern void fenv_store( fenv_t *env );
#pragma aux fenv_store =    \
    "fstenv     [di]"       \
    parm [DATA_SEG di]      \
    modify exact [];

/* Load floating-point control word from memory to hardware */
extern void fpcw_load( const uint16_t *control );
#pragma aux fpcw_load = \
    "fldcw      [di]"   \
    parm [DATA_SEG di]  \
    modify exact nomemory [];

/* Store floating-point control word from hardware to memory */
extern void fpcw_store( uint16_t *control );
#pragma aux fpcw_store =    \
    "fnstcw     [di]"       \
    parm [DATA_SEG di]      \
    modify exact [];

/* Store floating-point status word from hardware to memory */
extern void fpsw_store( uint16_t *status );
#pragma aux fpsw_store =    \
    "fnstsw     [di]"       \
    parm [DATA_SEG di]      \
    modify exact [];

#endif

/* FPU wait */
extern void fwait( void );
#pragma aux fwait = \
    "fwait"         \
    modify exact [] nomemory;


/****************************************************************************
* exception handling functions
****************************************************************************/

/*
The feclearexcept function clears the supported floating-point exceptions
represented by its argument.
*/
_WCRTLINK int feclearexcept( int excepts )
/****************************************/
{
    fenv_t      env;

    fenv_store( &env );
    env.status_word &= ~excepts;
    fenv_load( &env );
    return( 0 );
}

/*
The fegetexceptflag function stores an implementation-defined
representation of the states of the floating-point status flags
indicated by the argument excepts in the object pointed to by
the argument flagp.
*/
_WCRTLINK int fegetexceptflag( fexcept_t *flagp, int excepts )
/************************************************************/
{
    uint16_t    status;

    fpsw_store( &status );
    *flagp = excepts & status & FE_ALL_EXCEPT;
    return( 0 );
}

/*
The feraiseexcept function raises the supported floating-point
exceptions represented by its argument. The order in which
these floating-point exceptions are raised is unspecified, except
as stated in F.7.6. Whether the feraiseexcept function additionally
raises the inexact floating-point exception whenever it raises the
overflow or underflow floating-point exception is implementation-defined.
*/
_WCRTLINK int feraiseexcept( int excepts )
/****************************************/
{
    fenv_t      env;

    fenv_store( &env );
    env.status_word |= excepts & FE_ALL_EXCEPT;
    fenv_load( &env );
    fwait();    /* Make sure exception gets triggered now */
    return( 0 );
}

/*
feenableexcept is an OW extension to unmask the given fpu exception
*/
_WCRTLINK void feenableexcept( int excepts )
/******************************************/
{
    uint16_t    status;

    fpcw_store( &status );
    status &= ~FE_ALL_EXCEPT;
    status |= excepts ^ FE_ALL_EXCEPT;
    fpcw_load( &status );
}

/*
fedisableexcept is an OW extension to mask the given fpu exception
*/
_WCRTLINK void fedisableexcept( int excepts )
/*******************************************/
{
    uint16_t    status;

    fpcw_store( &status );
    status &= ~FE_ALL_EXCEPT;
    status |= excepts;
    fpcw_load( &status );
}

/*
The fesetexceptflag function sets the floating-point status flags
indicated by the argument excepts to the states stored in the object
pointed to by flagp. The value of *flagp shall have been set by a
previous call to fegetexceptflag whose second argument represented
at least those floating-point exceptions represented by the argument
excepts. This function does not raise floating-point exceptions,
but only sets the state of the flags.
*/
_WCRTLINK int fesetexceptflag( const fexcept_t *flagp, int excepts )
/******************************************************************/
{
    fenv_t      env;

    fenv_store( &env );
    env.status_word &= ~FE_ALL_EXCEPT;
    env.status_word |= excepts & *flagp & FE_ALL_EXCEPT;
    fenv_load( &env );
    return( 0 );
}

/*
The fetestexcept function determines which of a specified
subset of the floatingpoint exception flags are currently set.
The excepts argument specifies the floatingpoint status flags
to be queried.
Returns: the value of the bitwise OR of the floating-point
exception macros corresponding to the currently set floating-point
exceptions included in excepts.
*/
_WCRTLINK int fetestexcept( int excepts )
/***************************************/
{
    fexcept_t   curr;

    fegetexceptflag( &curr, excepts );
    return( curr );
}

/****************************************************************************
* rounding mode functions
****************************************************************************/

/*
The fegetround function gets the current rounding direction.
*/
_WCRTLINK int fegetround( void )
/******************************/
{
    uint16_t    status;

    fpcw_store( &status );
    return( status & 0x0C00 );
}

/*
The fesetround function sets the current rounding direction.
*/
_WCRTLINK int fesetround( int round )
/***********************************/
{
    uint16_t    status;

    switch( round ) {
    case FE_TONEAREST:
    case FE_DOWNWARD:
    case FE_TOWARDZERO:
    case FE_UPWARD:
        fpcw_store( &status );
        status &= ~0x0C00;
        status |= (uint16_t)round;
        fpcw_load( &status );
        return( 0 );
    }
    return( 1 );
}

/****************************************************************************
* FPU environment functions
****************************************************************************/

/*
The fegetenv function stores the current floating-point environment
in the object pointed to by envp.
*/
_WCRTLINK int fegetenv( fenv_t *envp )
/************************************/
{
    fenv_store( envp );
    return( 0 );
}

/*
The feholdexcept function saves the current floating-point environment
in the object pointed to by envp, clears the floating-point status
flags, and then installs a non-stop (continue on floating-point exceptions)
mode, if available, for all floating-point exceptions.
Returns: zero if and only if non-stop floating-point exception handling
was successfully installed.
*/
_WCRTLINK int feholdexcept( fenv_t *envp )
/****************************************/
{
    uint16_t    status;

    fenv_store( envp );
    status = envp->control_word | FE_ALL_EXCEPT;
    fpcw_load( &status );
    return( 0 );
}

/*
The fesetenv function establishes the floating-point environment represented
by the object pointed to by envp. The argument envp shall point to an object
set by a call to fegetenv or feholdexcept, or equal a floating-point
environment macro. Note that fesetenv merely installs the state of the
floating-point status flags represented through its argument, and does not
raise these floating-point exceptions.
*/
_WCRTLINK int fesetenv( const fenv_t *envp )
/******************************************/
{
    fenv_load( envp );
    return( 0 );
}

/*
The feupdateenv function saves the currently raised floating-point exceptions
in its automatic storage, installs the floating-point environment represented
by the object pointed to by envp, and then raises the saved floating-point
exceptions. The argument envp shall point to an object set by a call to
feholdexcept or fegetenv, or equal a floating-point environment macro.
*/
_WCRTLINK int feupdateenv( const fenv_t *envp )
/*********************************************/
{
    uint16_t    status;

    fpsw_store( &status );
    fenv_load( envp );
    feraiseexcept( status & FE_ALL_EXCEPT );
    return( 0 );
}
