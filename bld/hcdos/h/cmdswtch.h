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
* Description:  Command line option handling macros.
*
****************************************************************************/


#if defined( __UNIX__ )
    #define _IsCmdSwitch( str )    ((str)[0] == '-')
#else
    #define _IsCmdSwitch( str )    ((str)[0] == '-'  ||  (str)[0] == '/')
#endif

// The following macro allows you to access a value for a parameter either
// using the remainder of the current parameter, or the next parameter.
// Eg. for "-z4" and "-z 4", this macro will return a pointer to the
// "4" in either case.
// NOTE: this macro MODIFIES the 2nd parameter (the argument number).

#define _argvalue( argv, i )    (argv[i][2] == '\0' ? argv[++i] : &argv[i][2])
