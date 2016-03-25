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
* Description:  Interface for 'ex' emulation routines.
*
****************************************************************************/


#ifndef _EX_INCLUDED
#define _EX_INCLUDED

/*
 * constants
 */
enum {
    #define PICK(a,b) b,
    #include "excmds.h"
    #undef PICK
};


/*
 * external defs (from exdata.c)
 */
extern char _NEAR TokensEx[];

/*
 * function prototypes
 */
/* ex.c */
extern vi_rc    EnterExMode( void );
extern vi_rc    ProcessEx( linenum, linenum, bool, int, const char * );

/* exappend.c */
extern vi_rc    Append( linenum, bool );
extern vi_rc    AppendAnother( const char * );

#endif
