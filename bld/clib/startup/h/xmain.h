/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  (w)main function declaration.
*
****************************************************************************/


#if defined( __QNX__ )

extern int  main( int argc, char **argv, char **env );
#if defined( _M_I86 )
#pragma aux main __modify [__sp]
#else
#pragma aux main __modify [__esp]
#endif

#elif defined( _M_I86 ) && defined(__SW_BD)

extern int  main( void );

#elif defined( __LINUX__ )

extern int  main( int argc, char **argv, char **env );

#elif defined( __WIDECHAR__ ) && ( defined( __NT__ ) || defined( __OS2__ ) )

extern int  wmain( int wargc, wchar_t **wargv );

#else

extern int  main( int argc, char **argv );

#endif
