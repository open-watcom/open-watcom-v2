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
;* Description:  Win32 helper routines for TLS. 
;*
;*****************************************************************************


.387
.386p
                PUBLIC  __tls_region
                PUBLIC  __tls_used
                PUBLIC  __tls_index
                PUBLIC  __tls_start
                PUBLIC  __tls_end
                EXTRN   __tls_array:BYTE

DGROUP          GROUP   CONST,CONST2,_DATA,_BSS
TLS             GROUP   '.tls','.tls$','.tls$ZZZ'

_TEXT           SEGMENT BYTE PUBLIC USE32 'CODE'
                ASSUME CS:_TEXT, DS:DGROUP, SS:DGROUP
__tls_region:
    push      edx
    mov       eax,dword ptr fs:__tls_array
    mov       edx,dword ptr __tls_index
    mov       eax,dword ptr [eax+edx*4]
    sub       eax,dword ptr __tls_used
    pop       edx
    ret
_TEXT           ENDS

CONST           SEGMENT DWORD PUBLIC USE32 'DATA'
CONST           ENDS

CONST2          SEGMENT DWORD PUBLIC USE32 'DATA'
CONST2          ENDS

_DATA           SEGMENT DWORD PUBLIC USE32 'DATA'
__tls_used:
                DD      __tls_start
                DD      __tls_end
                DD      __tls_index
                DD      0
                DD      0
                DD      0

_DATA           ENDS

_BSS            SEGMENT DWORD PUBLIC USE32 'BSS'
                ORG 0x00000000
__tls_index     LABEL   BYTE
                ORG 0x00000004
_BSS            ENDS

.tls            SEGMENT DWORD PUBLIC USE32 'TLS'
__tls_start:
                DD      __tls_start
.tls            ENDS

.tls$           SEGMENT DWORD PUBLIC USE32 'TLS'
.tls$           ENDS

.tls$ZZZ        SEGMENT DWORD PUBLIC USE32 'TLS'
__tls_end:
                DD      __tls_end
.tls$ZZZ        ENDS

                END
