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
* Description:  wmake preprocessor directive definition
*
****************************************************************************/


/* this must be lexically sorted */

/*      text        enum         */
pick( "define",     D_DEFINE )
pick( "else",       D_ELSE )
pick( "endif",      D_ENDIF )
pick( "error",      D_ERROR )
pick( "if",         D_IF )      // MS Compatibility Directive.  NMAKE binary and string operations.
pick( "ifdef",      D_IFDEF )
pick( "ifeq",       D_IFEQ )
pick( "ifeqi",      D_IFEQI )
pick( "ifndef",     D_IFNDEF )
pick( "ifneq",      D_IFNEQ )
pick( "ifneqi",     D_IFNEQI )
pick( "include",    D_INCLUDE )
pick( "inject",     D_INJECT )
pick( "loaddll",    D_LOADDLL )
pick( "message",    D_MESSAGE )
pick( "undef",      D_UNDEF )
