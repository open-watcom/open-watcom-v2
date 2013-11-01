;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;* Copyright (c) 2004-2013 The Open Watcom Contributors. All Rights Reserved.
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
;* Description:  DPMI memory auto lock/unlock routines
;*
;*****************************************************************************

.386

include mdef.inc
include xinit.inc

public  __autolock

__autolock equ 1234h

DPMILockLinearRegion    equ 600h
DPMIUnlockLinearRegion  equ 601h

DGROUP  group   _DATA, _AUTOLOCKB, _AUTOLOCK, _AUTOLOCKE

_TEXT   segment word    public  use32   'CODE'

automemlock proc    near    private  uses eax ebx ecx edx esi edi
        mov     edx,offset _start
loop1:
        cmp     edx,offset _end
        jae     end1
        mov     ebx,dword ptr [edx]
        mov     cx,bx
        shr     ebx,16
        mov     esi,dword ptr [edx+4]
        mov     di,si
        shr     esi,16
        mov     ax,DPMILockLinearRegion
        int     31h
        jc      end1
        add     edx,8
        jmp     loop1
end1:
        ret         
automemlock endp

automemunlock proc    near    private  uses eax ebx ecx edx esi edi
        mov     edx,offset _start 
loop2:
        cmp     edx,offset _end
        jae     end2
        mov     ebx,dword ptr [edx]
        mov     cx,bx
        shr     ebx,16
        mov     esi,dword ptr [edx+4]
        mov     di,si
        shr     esi,16
        mov     ax,DPMIUnlockLinearRegion
        int     31h
        jc      end2
        add     edx,8
        jmp     loop2
end2:
        ret         
automemunlock endp

_TEXT   ends

_DATA   segment dword   public  use32   'DATA'
_DATA   ends

_AUTOLOCKB  segment dword   public  use32   'DATA'
_start:
_AUTOLOCKB  ends

_AUTOLOCK   segment dword   public  use32   'DATA'
_AUTOLOCK   ends

_AUTOLOCKE  segment dword   public    use32  'DATA'
_end:
_AUTOLOCKE  ends

xinit   automemlock,   INIT_PRIORITY_EXIT - 1
xfini   automemunlock, INIT_PRIORITY_EXIT - 1

end
