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
;* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
;*               DESCRIBE IT HERE!
;*
;*****************************************************************************


                NAME    loadstd

extrn           DOSLOADMODULE   : far

public          StartProc_
public          LoadThisDLL_
public          EndLoadThisDLL_

_data segment byte public 'DATA'
extrn           _ThisDLLModHandle : word
_data ends

dgroup group _data

_text segment byte public 'CODE'

assume cs:_text, ds:dgroup

LoadThisDLL_    proc    near
        call    DOSLOADMODULE   ; load STD.DLL
        int     3               ; break point
EndLoadThisDLL_ label byte
LoadThisDLL_    endp

StartProc_      proc    far
                push    ds
                push    ax
                mov     ax,dgroup
                mov     ds,ax
                pop     ax
                mov     _ThisDLLModHandle,ax
                mov     ax,1
                pop     ds
                ret
StartProc_      endp


_text           ENDS

                END     StartProc_
