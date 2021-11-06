;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


;
;
include mdef.inc
include struct.inc

        xrefp   "C",__set_errno_dos
        modstart lfnerror

        xdefp   "C", __lfnerror_0
;
        defp    __lfnerror_0
        sbb     DX,DX           ; set DX = -1 by carry
        _if     z               ; if not carry
          cmp   AX,7100h        ; - if AX equal 7100h
          _if   z               ; - then
            mov DX,-1           ; - - set DX = -1
          _else                 ; - else
            sub AX,AX           ; - - set AX = 0
          _endif                ; - endif
        _endif                  ; endif
        ret                     ; return DX:AX to caller
        endproc __lfnerror_0

        xdefp   "C", __lfnerror_ax
;
        defp    __lfnerror_ax
        sbb     DX,DX           ; set DX = -1 by carry
        _if     z               ; if not carry
          cmp   AX,7100h        ; - if AX equal 7100h
          _if   z               ; - then
            mov DX,-1           ; - - set DX = -1
          _endif                ; - endif
        _endif                  ; endif
        ret                     ; return DX:AX to caller
        endproc __lfnerror_ax

        endmod
        end
