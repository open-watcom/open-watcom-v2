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


        page    64,110
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;<>
;<> NGRABLOW:   Grab low memory for the overlay loader
;<>
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

        include struct.inc
        include novlldr.inc

        name ngrablow

        extrn __OVLAREALIST__:near
        extrn _ovl_addarea_:far

THRESHOLD equ 2         ; only allocate blocks larger than this

_TEXT   segment byte '_OVLCODE' PUBLIC
        assume cs:_TEXT

;
; void far _ovl_grablow( unsigned must_leave )
;
        public _ovl_grablow_
_ovl_grablow_ proc far
;
;       Grab low memory leaving at least must_leave paras for caller.
;
        push    ax
        push    bx
        push    cx
        push    dx
        push    es
        _guess
          test  ax,ax           ; check if size 0
          _if ne
            xchg bx,ax          ; allocate a chunk that satisfies must_leave
            mov ah,48h          ; dos alloc memory request
            int 21h             ; . . .
          _endif
          _quif c               ; fatal error (c=0 after test ax,ax)
          mov   cx,ax           ; save segment we've been given (or null)
          _loop
            mov ah,48h          ; dos alloc memory request
            mov bx,0ffffh       ; find out largest segment size
            int 21h             ; . . .
            cmp bx,THRESHOLD    ; check if block too small
            _quif be            ; . . .
            mov dx,bx           ; save the size
            mov ah,48h          ; dos alloc memory request
            int 21h             ; allocate the block
            _quif c             ; some sort of fatal error occured
            call  _ovl_addarea_ ; add the area
          _endloop
          test  cx,cx           ; check if we have to free a seg
          _if   ne
            mov es,cx           ; - give back segment we saved for user
            mov ah,49h          ; - dos free memory request
            int 21h             ; - . . .
          _endif
        _endguess
        pop     es
        pop     dx
        pop     cx
        pop     bx
        pop     ax
        ret
_ovl_grablow_ endp

_TEXT   ends

        end
