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


/*
 * scary low-level hack coming up....
 * this is used in functions which need a stack.
 * this makes a lot of assumptions about code generation, and assumes that
 * the user _really_ knows whats going on.
 * be _very_, _very_ careful when using these functions, and make _sure_ that
 * you do not blow the stack when using them!!!!
*/

extern unsigned_32 _stkpop( void );
extern void _stkpush( unsigned_32 );

#if defined(__386__)
#pragma aux     _stkpop = 0x58 /* pop eax */\
                        parm value [eax] modify nomemory [esp];
#pragma aux     _stkpush = 0x50 /* push eax */\
                        parm caller [eax] modify nomemory [esp];

#else                                           /* 16-bit land */
#pragma aux     _stkpop = 0x58 /* pop ax */\
                          0x5A /* pop dx */\
                        parm value [ax dx] modify nomemory [sp];
#pragma aux     _stkpush = 0x52 /* push dx */\
                          0x50  /* pop ax */\
                        parm caller [ax dx] modify nomemory [sp];
#endif
