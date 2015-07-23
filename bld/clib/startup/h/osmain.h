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
* Description:  ....Main OS specific function declarations.
*
****************************************************************************/


#if defined( __OS2__ )

  #if defined( _M_I86 )
    extern int _OS2Main( char _WCI86FAR *stklow, char _WCI86FAR *stktop, unsigned envseg, unsigned cmdoff );
    #pragma aux _OS2Main "_*" parm caller [ dx ax ] [ cx bx ]
  #else
    extern void __OS2Main( unsigned hmod, unsigned reserved, char *env, char *cmd );
    extern void __wOS2Main( unsigned hmod, unsigned reserved, char *env, char *cmd );
    #if defined(_M_IX86)
      #pragma aux __wOS2Main "*" parm caller []
      #pragma aux __OS2Main "*" parm caller []
    #endif
  #endif

#elif defined( __RDOS__ ) || defined( __RDOSDEV__ )

  extern void __RdosMain( void );
  #pragma aux __RdosMain  "*"

#elif defined( __LINUX__ )

  extern void __cdecl _LinuxMain( int argc, char **argv, char **arge );

#elif defined( __NT__ )

  extern void __NTMain( void );
  extern void __wNTMain( void );
  #if defined(_M_IX86)
    #pragma aux __NTMain "*"
    #pragma aux __wNTMain "*"
  #endif

#endif
