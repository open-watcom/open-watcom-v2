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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifdef __386__
#define _nheapgrow()
#define _nheapshrink()
#endif

#define InitialStack() \
    { \
        _nheapgrow(); \
        sp = GetSP(); \
        stackptr = _nmalloc( MIN_STACK_K*1024 ); \
        if( stackptr == NULL ) { \
            exit( 1 ); \
        } \
        SetSP( stackptr+MIN_STACK_K*1024-16 ); \
        _STACKLOW = (unsigned) stackptr; \
        _STACKTOP = (unsigned) (stackptr+MIN_STACK_K*1024-16); \
    }

#define FinalStack() \
    { \
        SetSP( sp ); \
        _nfree( stackptr ); \
        while( 1 ) { \
            stackptr2 = _nmalloc( StackK*1024 ); \
            if( stackptr2 == NULL ) { \
                StackK--; \
                if( StackK < MIN_STACK_K ) { \
                    QuitEditor( ERR_NO_MEMORY ); \
                } \
            } else { \
                break; \
            } \
        } \
        SetSP( stackptr2+StackK*1024-16 ); \
        _STACKLOW = (unsigned) stackptr2; \
        _STACKTOP = (unsigned) (stackptr2+StackK*1024-16); \
        _nheapshrink(); \
    }

static char near *stackptr,near *stackptr2;
static char near *sp;

extern unsigned _STACKLOW;
extern unsigned _STACKTOP;
