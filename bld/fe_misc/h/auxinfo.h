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

pick( M_DEFAULT,  "default",  &DefaultInfo  )
pick( M_CDECL,    "cdecl",    &CdeclInfo    )
pick( M_PASCAL,   "pascal",   &PascalInfo   )
pick( M_FORTRAN,  "fortran",  &FortranInfo  )
pick( M_SYSCALL,  "syscall",  &SyscallInfo  )
pick( M_STDCALL,  "stdcall",  &StdcallInfo  )
pick( M_FASTCALL, "fastcall", &FastcallInfo )
pick( M_OPTLINK,  "optlink",  &OptlinkInfo  )
pick( M_SYSTEM,   "system",   &SyscallInfo  )
pick( M_WATCALL,  "watcall",  &WatcallInfo  )
pick( M_UNKNOWN,  NULL,       NULL          )
