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
* Description:  List of internal commands.
*
****************************************************************************/


/* this list must be in alpha order */

pick( BREAK )
pick( CALL )
pick( CD )
#if defined( __OS2__ ) || defined( __NT__ ) || defined( __UNIX__ )
pick( CHCP )
#endif
pick( CHDIR )
pick( CLS )
#if defined( __DOS__ )
pick( COMMAND )
#endif
pick( COPY )
#if defined( __DOS__ )
pick( CTTY )
#endif
pick( DATE )
pick( DEL )
#if defined( __OS2__ ) || defined( __NT__ ) || defined( __UNIX__ )
pick( DETACH )
#endif
pick( DIR )
#if defined( __OS2__ ) || defined( __NT__ ) || defined( __UNIX__ )
pick( DPATH )
#endif
pick( ECHO )
#if defined( __OS2__ ) || defined( __NT__ ) || defined( __UNIX__ )
pick( ENDLOCAL )
#endif
pick( ERASE )
#if defined( __OS2__ ) || defined( __NT__ ) || defined( __UNIX__ )
pick( EXIT )
pick( EXTPROC )
#endif
pick( FOR )
pick( GOTO )
pick( IF )
pick( MD )
pick( MKDIR )
#if defined( __OS2__ ) || defined( __NT__ )
pick( MOVE )
#endif
pick( PATH )
pick( PAUSE )
pick( PROMPT )
pick( RD )
pick( REM )
pick( REN )
pick( RENAME )
pick( RM )
pick( RMDIR )
pick( SET )
#if defined( __OS2__ ) || defined( __NT__ ) || defined( __UNIX__ )
pick( SETLOCAL )
pick( SHIFT )
pick( START )
#endif
pick( TIME )
pick( TYPE )
pick( VER )
pick( VERIFY )
pick( VOL )
