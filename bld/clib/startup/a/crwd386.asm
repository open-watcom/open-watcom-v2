;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
;*
;*  ========================================================================
;*
;*    This file contains Original Code and/or Modifications of Original
;*    Code as defined in and that are subject to the Sybase Open Watcom
;*    Public License version 1.0 (the 'License'). You may not use this file
;*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
;*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
;*    provided with the Original Code and Modifications, and is also
;*    available at www.sybase.com/developer/opensource.
;*
;*    The Original Code and all software distributed under the License are
;*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
;*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
;*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
;*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
;*    NON-INFRINGEMENT. Please see the License for the specific language
;*    governing rights and limitations under the License.
;*
;*  ========================================================================
;*
;* Description:  C runtime read/write data (386 version).
;*
;*****************************************************************************


        name    crwdata

.387
.386p

        assume  nothing

DGROUP group _DATA

_TEXT segment byte public 'CODE'
        assume cs:_TEXT
__null_FPE_rtn proc near
        ret
__null_FPE_rtn endp
_TEXT ends

_DATA   segment dword public 'DATA'

        assume  DS:DGROUP

ifndef __NETWARE__
_dynend      dd 0               ; top of dynamic data area
_curbrk      dd 0               ; top of usable memory
endif
ifndef _NETWARE_LIBC
_LpCmdLine   dd 0               ; pointer to raw command line
_LpPgmName   dd 0               ; pointer to program name (for argv[0])
endif
ifndef __QNX__
ifndef __LINUX__
ifdef __NT__
_LpDllName   dd 0               ; pointer to dll name (for OS/2,WIN32)
_LpwCmdLine  dd 0               ; pointer to widechar raw command line
_LpwPgmName  dd 0               ; pointer to widechar program name (for argv[0])
_LpwDllName  dd 0               ; pointer to widechar dll name (for OS/2,WIN32)
endif
ifdef __OS2__
_LpDllName   dd 0               ; pointer to dll name (for OS/2,WIN32)
_LpwCmdLine  dd 0               ; pointer to widechar raw command line
_LpwPgmName  dd 0               ; pointer to widechar program name (for argv[0])
_LpwDllName  dd 0               ; pointer to widechar dll name (for OS/2,WIN32)
endif
ifdef __DOS__
_psp         dw 0,0             ; segment addr of program segment prefix
__x386_stacklow label   dword
endif
endif
endif
_STACKLOW  dd 0                 ; lowest address in stack
_STACKTOP  dd 0                 ; highest address in stack
__ASTACKSIZ dd 0                ; alternate F77 stack size
__ASTACKPTR dd 0                ; alternate F77 stack pointer
ifndef __NETWARE__
_cbyte     dd 0                 ; used by getch, getche
_cbyte2    dd 0                 ; used by getch, getche
__child    dd 0                 ; non-zero => a spawned process is running
endif
__no87     db 0                 ; non-zero => "NO87" environment var present
ifndef __NETWARE__
ifdef __DOS__
_Extender         db 0          ; see clib\h\extender.inc for values
_ExtenderSubtype  db 0          ; non-zero -> variant of extender
__X32VM    db 0                 ; non-zero => X-32VM DOS Extender
endif
ifdef __DOS__
_Envptr    df 0                 ; far environment pointer
else
_Envptr    dd 0                 ; offset part of environment pointer
endif
ifndef __QNX__
ifndef __LINUX__
_osmajor   db 0                 ; major DOS version number
_osminor   db 0                 ; minor DOS version number
ifdef __NT__
_osbuild   dw 0                 ; operating system build number
_osver     dd 0                 ; operating system build number
_winmajor  dd 0                 ; operating system major version number
_winminor  dd 0                 ; operating system minor version number
_winver    dd 0                 ; operating system version number
endif
endif
endif
endif
__FPE_handler dd __null_FPE_rtn ; FPE handler


ifndef __QNX__
ifndef __LINUX__
ifndef __NETWARE__
        public  "C",_osmajor
        public  "C",_osminor
ifdef __NT__
        public  "C",_osbuild
        public  "C",_osver
        public  "C",_winmajor
        public  "C",_winminor
        public  "C",_winver
endif
endif
ifdef __DOS__
        public  "C",_psp
        public  "C",_Extender
        public  "C",_ExtenderSubtype
        public  __X32VM
        public  __x386_stacklow
endif
ifdef __NT__
        public  "C",_LpDllName
        public  "C",_LpwCmdLine
        public  "C",_LpwPgmName
        public  "C",_LpwDllName
endif
ifdef __OS2__
        public  "C",_LpDllName
        public  "C",_LpwCmdLine
        public  "C",_LpwPgmName
        public  "C",_LpwDllName
endif
endif
endif
ifndef _NETWARE_LIBC
        public  "C",_LpCmdLine
        public  "C",_LpPgmName
endif
ifndef __NETWARE__
        public  "C",_dynend
        public  "C",_curbrk
endif
        public  "C",_STACKLOW
        public  "C",_STACKTOP
        public  __ASTACKSIZ
        public  __ASTACKPTR
ifndef __NETWARE__
        public  "C",_cbyte
        public  "C",_cbyte2
        public  __child
        public  "C",_Envptr
endif
        public  __no87
        public  "C",__FPE_handler

_DATA   ends

        end
