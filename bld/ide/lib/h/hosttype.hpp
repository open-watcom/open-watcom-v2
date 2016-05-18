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
* Description:  Host type definition for IDE config file.
*
****************************************************************************/

//    enum          type    batchserv       editor       DLL     parms  pathsep descr
pick( HOST_WINDOWS, 'w',    "BATCHBOX.PIF", "weditviw", true,   "",     '\\',   "Windows 3.x" )
pick( HOST_PM,      'o',    "BATSERV.EXE",  "epmlink",  true,   "",     '\\',   "OS/2 PM" )
pick( HOST_NT,      'n',    "BATSERV.EXE",  "weditviw", true,   "",     '\\',   "Windows NT" )
pick( HOST_WINOS2,  's',    "BATSERV.EXE",  "epmlink",  true,   "",     '\\',   "Win-OS/2" )
pick( HOST_WIN95,   '9',    "BATSERV.EXE",  "weditviw", true,   "",     '\\',   "Windows 95" )
pick( HOST_J_WIN,   'j',    "BATCHBOX.PIF", "notepad",  false,  "%f",   '\\',   "Japanese Windows 3.x (on IBM)" )
pick( HOST_NEC_WIN, '8',    "BATCHBOX.PIF", "notepad",  false,  "%f",   '\\',   "Japanese Windows 3.x (NEC PC98)" ) // obsolete
pick( HOST_AXP_NT,  'a',    "BATSERV.EXE",  "weditviw", true,   "",     '\\',   "Dec Alpha (Windows NT)" )
pick( HOST_DOS,     'd',    NULL,           NULL,       false,  "",     '\\',   "DOS" )
pick( HOST_LINUX,   'x',    NULL,           NULL,       false,  "",     '/',    "Linux" )
pick( HOST_UNIX,    'u',    NULL,           NULL,       false,  "",     '/',    "UNIX" )

