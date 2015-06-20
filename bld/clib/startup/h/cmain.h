/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2015 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  _CMain function declaration.
*
****************************************************************************/


#if defined( __QNX__ )
  #if defined( _M_I86 )
    extern void _CMain( void __near *free, unsigned short n, struct _proc_spawn __near *cmd, unsigned short stk_bot, pid_t pid );
    #pragma aux _CMain "_*" parm [bx] [cx] [di] [dx] [ax];
  #else
    extern void _CMain( int argc, char **argv, char **arge );
    #pragma aux _CMain "_*" parm [eax] [edx] [ebx];
  #endif
#else
  #if defined( _M_I86 )
    #if defined(__SW_BD)
        extern int  _CMain( void );
    #else
        extern void _CMain( void );
    #endif
    #pragma aux _CMain  "_*"
  #else
    extern void  __CMain( void );
    extern void  __wCMain( void );
    #if defined( _M_IX86 )
      #pragma aux __CMain  "*"
      #pragma aux __wCMain "*"
    #endif
  #endif
#endif
