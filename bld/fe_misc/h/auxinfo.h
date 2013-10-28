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
* Description:  define values for pragma aux magic words
*
****************************************************************************/

pick( M_DEFAULT,  "\0\0default", &DefaultInfo  )
pick( M_CDECL,    "__cdecl",     &CdeclInfo    )
pick( M_PASCAL,   "__pascal",    &PascalInfo   )
pick( M_FORTRAN,  "__fortran",   &FortranInfo  )
pick( M_SYSCALL,  "__syscall",   &SyscallInfo  )
pick( M_STDCALL,  "__stdcall",   &StdcallInfo  )
pick( M_FASTCALL, "__fastcall",  &FastcallInfo )
pick( M_OPTLINK,  "__optlink",   &OptlinkInfo  )
pick( M_SYSTEM,   "__system",    &SyscallInfo  )
pick( M_WATCALL,  "__watcall",   &WatcallInfo  )
pick( M_UNKNOWN,  NULL,          NULL          )
