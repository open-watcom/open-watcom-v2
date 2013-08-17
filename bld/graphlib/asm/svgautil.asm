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
;* Description:  Utility routines for SuperVGA modes.
;*
;*****************************************************************************


include graph.inc


        xref    _Zap19_
        xref    _Fill19_
        xref    _PixCopy19_
        xref    _PixRead19_

        extrn   __PlotAct : word
        extrn   __Transparent : word
        extrn   __VGAPage : byte
ifdef VERSION2
        extrn   __VGABytesPerPixel : word
endif
        extrn   __VGAStride : word
        extrn   __VGAGran : byte
        extptr  __SetVGAPage

        modstart svgautil,WORD

        xdefp   _MoveUp100_
        xdefp   _MoveDown100_
        xdefp   _MoveUp128_
        xdefp   _MoveDown128_
        xdefp   _MoveLeft256_
        xdefp   _MoveRight256_
        xdefp   _MoveUp640_
        xdefp   _MoveDown640_
        xdefp   _MoveUp800_
        xdefp   _MoveDown800_
        xdefp   _MoveUp1024_
        xdefp   _MoveDown1024_
ifdef VERSION2
        xdefp   _MoveUpVESA_
        xdefp   _MoveDownVESA_
        xdefp   _MoveLeftVESA_
        xdefp   _MoveRightVESA_
endif
;;      xdefp   _MoveUp1280_    ; this mode is untested
;;      xdefp   _MoveDown1280_
;;      xdefp   _MoveLeftWord_
;;      xdefp   _MoveRightWord_
ifdef VERSION2
        xdefp   _RepWord_
        xdefp   _XorWord_
        xdefp   _AndWord_
        xdefp   _OrWord_
        xdefp   _RepTByte_
        xdefp   _XorTByte_
        xdefp   _AndTByte_
        xdefp   _OrTByte_
        xdefp   _RepDWord_
        xdefp   _XorDWord_
        xdefp   _AndDWord_
        xdefp   _OrDWord_
        xdefp   _GetDotWord_
        xdefp   _GetDotTByte_
        xdefp   _GetDotDWord_
        xdefp   _ZapWord_
        xdefp   _ZapDWord_
        xdefp   _ZapTByte_
        xdefp   _FillWord_
        xdefp   _FillDWord_
        xdefp   _FillTByte_
        xdefp   _PixCopyWord_
        xdefp   _PixReadWord_
        xdefp   _PixCopyDWord_
        xdefp   _PixReadDWord_
        xdefp   _PixCopyTByte_
        xdefp   _PixReadTByte_
        xdefp   _ScanLeftWord_
        xdefp   _ScanRightWord_
        xdefp   _ScanLeftDWord_
        xdefp   _ScanRightDWord_
        xdefp   _ScanLeftTByte_
        xdefp   _ScanRightTByte_
else
;;      xdefp   _RepWord_
;;      xdefp   _XorWord_
;;      xdefp   _AndWord_
;;      xdefp   _OrWord_
;;      xdefp   _GetDotWord_
;;      xdefp   _ZapWord_
;;      xdefp   _FillWord_
;;      xdefp   _PixCopyWord_
;;      xdefp   _PixReadWord_
;;      xdefp   _ScanLeftWord_
;;      xdefp   _ScanRightWord_
endif
        xdefp   _Zap256_
        xdefp   _Fill256_
        xdefp   _PixCopy256_
        xdefp   _PixRead256_
        xdefp   _ScanLeft256_
        xdefp   _ScanRight256_
        xdefp   _PageVESA_
        xdefp   _PageVideo7_
        xdefp   _PageParadise_
        xdefp   _PageATI_
        xdefp   _PageTseng3_
        xdefp   _PageTseng4_
        xdefp   _PageOak_
        xdefp   _PageTrident_
        xdefp   _PageChips_
        xdefp   _PageGenoa_
        xdefp   _PageS3_
        xdefp   _PageCirrus_
        xdefp   _PageViper_


;=========================================================================
;
;   SuperVGA page setting functions
;
;   Input       AL          page number
;
;   (see the book 'Advanced Programmer's Guide to Super VGAs'
;    for description of these routines.
;
;=========================================================================

_PageVESA_:
        cmp     al,ss:__VGAPage ; if page already selected
        _if     ne              ; then
          mov     ss:__VGAPage,al ; remember new page
          push    _ebx
          push    _ecx
          push    _edx
          mov     cl,ss:__VGAGran
          shl     al,cl
          xor     ah,ah
          mov     dx,ax
          mov     ax,4f05h
          xor     bx,bx
          int     10h           ; Page window a (write)
          mov     ax,4f05h
          mov     bl,1
          int     10h           ; Page window b (read)
          pop     _edx
          pop     _ecx
          pop     _ebx
        _endif
        _retf

_PageVideo7_:
        cmp     al,ss:__VGAPage ; if page already selected
        _if     ne              ; then
          mov     ss:__VGAPage,al ; remember new page
          mov     ah,al
          push    _edx
          cmp     byte ptr ss:__VGAGran,16
          _if     e             ; different paging mechanism for 16/256 cols
            mov     dx,3c4h
            mov     al,0f6h
            out     dx,al
            inc     dx
            in      al,dx
            and     al,0f0h
            or      al,ah
            shl     ah,1
            shl     ah,1
            or      al,ah
            out     dx,al
          _else
            mov     dx,3cch
            in      al,dx
            and     al,not 20h
            and     ah,2
            shl     ah,1
            shl     ah,1
            shl     ah,1
            shl     ah,1
            or      al,ah
            mov     dx,3c2h
            out     dx,al
            mov     dx,3c4h
            mov     al,0f9h
            mov     ah,ss:__VGAPage
            and     ah,1
            out     dx,ax
            mov     al,0f6h
            out     dx,al
            inc     dx
            in      al,dx
            and     al,0f0h
            mov     ah,ss:__VGAPage
            and     ah,0ch
            or      al,ah
            shr     ah,1
            shr     ah,1
            or      al,ah
            out     dx,al
          _endif
          pop     _edx
        _endif
        _retf

_PageParadise_:
_PageCirrus_:
        cmp     al,ss:__VGAPage ; if page already selected
        _if     ne              ; then
          mov     ss:__VGAPage,al ; remember new page
          mov     ah,al
          shl     ah,1
          shl     ah,1
          shl     ah,1
          shl     ah,1
          mov     al,9
          push    _edx
          mov     dx,3ceH
          out     dx,ax
          pop     _edx
        _endif
        _retf

_PageATI_:
        cmp     al,ss:__VGAPage ; if page already selected
        _if     ne              ; then
          mov     ss:__VGAPage,al ; remember new page
          mov     ah,al
          cli
          push    _edx
          mov     _edx,1ceH
          mov     al,0b2H
          out     dx,al
          inc     dl
          in      al,dx
          and     al,0e1h
          shl     ah,1
          or      ah,al
          mov     al,0b2H
          dec     dl
          out     dx,ax
          pop     _edx
          sti
        _endif
        _retf

_PageTseng3_:
        cmp     al,ss:__VGAPage ; if page already selected
        _if     ne              ; then
          mov     ss:__VGAPage,al ; remember new page
          mov     ah,al
          shl     ah,1
          shl     ah,1
          shl     ah,1
          or      al,ah
          or      al,40h
          push    _edx
          mov     dx,3CDh
          out     dx,al
          pop     _edx
        _endif
        _retf

_PageViper_:
_PageTseng4_:
        cmp     al,ss:__VGAPage ; if page already selected
        _if     ne              ; then
          mov     ss:__VGAPage,al ; remember new page
          mov     ah,al
          shl     ah,1
          shl     ah,1
          shl     ah,1
          shl     ah,1
          or      al,ah
          push    _edx
          mov     dx,3CDh
          out     dx,al
          pop     _edx
        _endif
        _retf

_PageOak_:
        cmp     al,ss:__VGAPage ; if page already selected
        _if     ne              ; then
          mov     ss:__VGAPage,al ; remember new page
          mov     ah,al
          shl     ah,1
          shl     ah,1
          shl     ah,1
          shl     ah,1
          or      al,ah
          push    _edx
          mov     dx,3dfh
          out     dx,al
          pop     _edx
        _endif
        _retf

_PageTrident_:
        cmp     al,ss:__VGAPage ; if page already selected
        _if     ne              ; then
          mov     ss:__VGAPage,al ; remember new page
          mov     ah,al
          xor     ah,02h
          push    _edx
          mov     dx,3c4h
          mov     al,0eh
          out     dx,al
          inc     dx
          in      al,dx
          and     al,0f0h
          or      al,ah
          out     dx,al
          pop     _edx
        _endif
        _retf

_PageChips_:
        cmp     al,ss:__VGAPage ; if page already selected
        _if     ne              ; then
          mov     ss:__VGAPage,al ; remember new page
          push    _ecx
          mov     cl,ss:__VGAGran
          shl     al,cl
          mov     ah,al
          push    _edx
          mov     dx,03d6h
          mov     al,10h
          out     dx,al
          inc     dx
          in      al,dx
          and     al,0c0h
          or      al,ah
          out     dx,al
          pop     _edx
          pop     _ecx
        _endif
        _retf

_PageGenoa_:
        cmp     al,ss:__VGAPage ; if page already selected
        _if     ne              ; then
          mov     ss:__VGAPage,al ; remember new page
          mov     ah,al
          shl     al,1
          shl     al,1
          shl     al,1
          or      ah,al
          push    _edx
          mov     dx,03c4h
          mov     al,6
          out     dx,al
          inc     dx
          in      al,dx
          and     al,80h
          or      al,40h
          or      al,ah
          out     dx,al
          pop     _edx
        _endif
        _retf

_PageS3_:
        cmp     al,ss:__VGAPage ; if page already selected
        _if     ne              ; then
          mov     ss:__VGAPage,al ; remember new page
          mov     ah,al
          cmp     byte ptr ss:__VGAGran,16 ; shift page in 16 colour mode
          _if     e
            shl     ah, 1
            shl     ah, 1
          _endif
          mov     al,35h
          push    _edx
          mov     dx,03d4h
          out     dx,al
          inc     dx
          in      al,dx
          and     ax,0ff0h
          or      al,ah
          out     dx,al
          pop     _edx
        _endif
        _retf

;=========================================================================
;
;   Movement primitives
;
;   Input       ES:_EDI      screen memory
;               AL          colour
;               CH          mask
;
;   Output      same        altered as per move
;
;=========================================================================

        db      E_MoveUp100-_MoveUp100_
_MoveUp100_:                    ; move up one dot
        sub     _edi,100         ; 800x600x16
E_MoveUp100:
        ret

        db      E_MoveDown100-_MoveDown100_
_MoveDown100_:                  ; move down one dot
        add     _edi,100
E_MoveDown100:
        ret

        db      E_MoveUp128-_MoveUp128_
_MoveUp128_:                    ; move up one dot
        sub     di,128          ; 1024x768x16
        _if     c               ; if < 0
          push    _eax           ; - move to previous page
          mov     al,ss:__VGAPage
          dec     al
          doicall __SetVGAPage
          pop     _eax
        _endif
E_MoveUp128:
        ret

        db      E_MoveDown128-_MoveDown128_
_MoveDown128_:                  ; move down one dot
        add     di,128
        _if     c               ; if < 0
          push    _eax           ; - move to next page
          mov     al,ss:__VGAPage
          inc     al
          doicall __SetVGAPage
          pop     _eax
        _endif
E_MoveDown128:
        ret

        db      E_MoveLeft256-_MoveLeft256_
_MoveLeft256_:                  ; 256 colour SuperVGA modes
        sub     di,1
        _if     c               ; if < 0
          push    _eax           ; - move to previous page
          mov     al,ss:__VGAPage
          dec     al
          doicall __SetVGAPage
          pop     _eax
        _endif
E_MoveLeft256:
        ret

        db      E_MoveRight256-_MoveRight256_
_MoveRight256_:                 ; 256 colour SuperVGA modes
        add     di,1
        _if     c               ; if < 0
          push    _eax           ; - move to next page
          mov     al,ss:__VGAPage
          inc     al
          doicall __SetVGAPage
          pop     _eax
        _endif
E_MoveRight256:
        ret

        db      E_MoveUp640-_MoveUp640_
_MoveUp640_:                    ; 640x???, 256 colour SuperVGA modes
        sub     di,ss:__VGAStride
        _if     c               ; if < 0
          push    _eax           ; - move to previous page
          mov     al,ss:__VGAPage
          dec     al
          doicall __SetVGAPage
          pop     _eax
        _endif
E_MoveUp640:
        ret

        db      E_MoveDown640-_MoveDown640_
_MoveDown640_:                  ; 640x???, 256 colour SuperVGA modes
        add     di,ss:__VGAStride
        _if     c               ; if < 0
          push    _eax           ; - move to next page
          mov     al,ss:__VGAPage
          inc     al
          doicall __SetVGAPage
          pop     _eax
        _endif
E_MoveDown640:
        ret

        db      E_MoveUp800-_MoveUp800_
_MoveUp800_:                    ; 800x600, 256 colour SuperVGA modes
        sub     di,ss:__VGAStride
        _if     c               ; if < 0
          push    _eax           ; - move to previous page
          mov     al,ss:__VGAPage
          dec     al
          doicall __SetVGAPage
          pop     _eax
        _endif
E_MoveUp800:
        ret

        db      E_MoveDown800-_MoveDown800_
_MoveDown800_:                  ; 800x600, 256 colour SuperVGA modes
        add     di,ss:__VGAStride
        _if     c               ; if < 0
          push    _eax           ; - move to next page
          mov     al,ss:__VGAPage
          inc     al
          doicall __SetVGAPage
          pop     _eax
        _endif
E_MoveDown800:
        ret

        db      E_MoveUp1024-_MoveUp1024_
_MoveUp1024_:                   ; 1024x768, 256 colour SuperVGA modes
_MoveUpVESA_:                  ; any colour SuperVGA modes
        sub     di,ss:__VGAStride
        _if     c               ; if < 0
          push    _eax           ; - move to previous page
          mov     al,ss:__VGAPage
          dec     al
          doicall __SetVGAPage
          pop     _eax
        _endif
E_MoveUp1024:
        ret

        db      E_MoveDown1024-_MoveDown1024_
_MoveDown1024_:                  ; 1024x768, 256 colour SuperVGA modes
_MoveDownVESA_:                  ; any colour SuperVGA modes
        add     di,ss:__VGAStride
        _if     c               ; if < 0
          push    _eax           ; - move to next page
          mov     al,ss:__VGAPage
          inc     al
          doicall __SetVGAPage
          pop     _eax
        _endif
E_MoveDown1024:
        ret

ifdef VERSION2
        db      E_MoveLeftVESA-_MoveLeftVESA_
_MoveLeftVESA_:                  ; any colour SuperVGA modes
        sub     di,ss:__VGABytesPerPixel
        _if     c               ; if < 0
          push    _eax           ; - move to previous page
          mov     al,ss:__VGAPage
          dec     al
          doicall __SetVGAPage
          pop     _eax
        _endif
E_MoveLeftVESA:
        ret

        db      E_MoveRightVESA-_MoveRightVESA_
_MoveRightVESA_:                 ; VESA colour SuperVGA modes
        add     di,ss:__VGABytesPerPixel
        _if     c               ; if < 0
          push    _eax           ; - move to next page
          mov     al,ss:__VGAPage
          inc     al
          doicall __SetVGAPage
          pop     _eax
        _endif
E_MoveRightVESA:
        ret
endif

;;        db      E_MoveUp1280-_MoveUp1280_
;;_MoveUp1280_:                   ; 640x480, 32768 colour SuperVGA modes
;;        sub     di,1280
;;        _if     c               ; if < 0
;;          push    _eax           ; - move to previous page
;;          mov     al,ss:__VGAPage
;;          dec     al
;;          doicall __SetVGAPage
;;          pop     _eax
;;        _endif
;;E_MoveUp1280:
;;        ret
;;
;;        db      E_MoveDown1280-_MoveDown1280_
;;_MoveDown1280_:                  ; 640x480, 32768 colour SuperVGA modes
;;        add     di,1280
;;        _if     c               ; if < 0
;;          push    _eax           ; - move to next page
;;          mov     al,ss:__VGAPage
;;          inc     al
;;          doicall __SetVGAPage
;;          pop     _eax
;;        _endif
;;E_MoveDown1280:
;;        ret
;;
;;        db      E_MoveLeftWord-_MoveLeftWord_
;;_MoveLeftWord_:                  ; 32768 colour (16 bit) SuperVGA modes
;;        sub     di,2
;;        _if     c               ; if < 0
;;          push    _eax           ; - move to previous page
;;          mov     al,ss:__VGAPage
;;          dec     al
;;          doicall __SetVGAPage
;;          pop     _eax
;;        _endif
;;E_MoveLeftWord:
;;        ret
;;
;;        db      E_MoveRightWord-_MoveRightWord_
;;_MoveRightWord_:                 ; 32768 colour (16 bit) SuperVGA modes
;;        add     di,2
;;        _if     c               ; if < 0
;;          push    _eax           ; - move to next page
;;          mov     al,ss:__VGAPage
;;          inc     al
;;          doicall __SetVGAPage
;;          pop     _eax
;;        _endif
;;E_MoveRightWord:
;;        ret
;;
;;=========================================================================
;;
;;   Plotting primitives
;;
;;   Input       ES:_EDI      screen memory
;;            V1 AX           colour
;;            V2 DX:AX/EAX    colour
;;               CH           mask
;;
;;=========================================================================

ifdef VERSION2

        db      E_AndWord-_AndWord_
_AndWord_:
        and    word ptr es:0[_edi],ax
E_AndWord:
        ret

        db      E_RepWord-_RepWord_
_RepWord_:
        mov    word ptr es:0[_edi],ax     ; replace pixel
E_RepWord:
        ret

        db      E_XorWord-_XorWord_
_XorWord_:
        xor    word ptr es:0[_edi],ax     ; replace pixel
E_XorWord:
        ret

        db      E_OrWord-_OrWord_
_OrWord_:
        or     word ptr es:0[_edi],ax     ; replace pixel
E_OrWord:
        ret

ifdef __386__

;;PIXEL in EAX

        db      E_AndDWord-_AndDWord_
_AndDWord_:
        and    dword ptr es:0[_edi],eax
E_AndDWord:
        ret

        db      E_RepDWord-_RepDWord_
_RepDWord_:
        mov    dword ptr es:0[_edi],eax     ; replace pixel
E_RepDWord:
        ret

        db      E_XorDWord-_XorDWord_
_XorDWord_:
        xor    dword ptr es:0[_edi],eax     ; replace pixel
E_XorDWord:
        ret

        db      E_OrDWord-_OrDWord_
_OrDWord_:
        or     dword ptr es:0[_edi],eax     ; replace pixel
E_OrDWord:
        ret

else

;;PIXEL in DX:AX
        db      E_AndDWord-_AndDWord_
_AndDWord_:
        and    word ptr es:0[_edi],ax
        and    word ptr es:2[_edi],dx
E_AndDWord:
        ret

        db      E_RepDWord-_RepDWord_
_RepDWord_:
        mov    word ptr es:0[_edi],ax     ; replace pixel
        mov    word ptr es:2[_edi],dx     ; replace pixel
E_RepDWord:
        ret

        db      E_XorDWord-_XorDWord_
_XorDWord_:
        xor    word ptr es:0[_edi],ax     ; replace pixel
        xor    word ptr es:2[_edi],dx     ; replace pixel
E_XorDWord:
        ret

        db      E_OrDWord-_OrDWord_
_OrDWord_:
        or     word ptr es:0[_edi],ax     ; replace pixel
        or     word ptr es:2[_edi],dx     ; replace pixel
E_OrDWord:
        ret

endif



byte_op     macro instr, rg
        instr    byte ptr es:0[_edi],rg
            endm

next_byte     macro instr, incrmnt
         instr    di, incrmnt
        _if     c               ; if < 0
          push    _eax           ; - move to next page
          mov     al,ss:__VGAPage
          ifidn instr,add
              inc     al
          else
              dec     al
          endif
          doicall __SetVGAPage
          pop     _eax
        _endif
              endm


_up1_:
        next_byte add, 1
        _retf

_down2_:
        next_byte sub, 2
        _retf


tb_op        macro instr
ifdef __386__
;;init regs for 386
        push   edx
        push   eax
        pop    dx
        pop    dx
endif
;;pixels are not necessarily aligned...
        byte_op instr, al
        doicall _up1_
;        next_byte add, 1
        byte_op instr, ah
        doicall _up1_
;        next_byte add, 1
        byte_op instr, dl
        doicall _down2_
;        next_byte sub, 2
ifdef __386__
;;and restore
        pop    edx
endif
              endm

;;for 3 byte pixels (is that ever used?)
        db      E_AndTByte-_AndTByte_
_AndTByte_:

        tb_op and

E_AndTByte:

        ret

        db      E_RepTByte-_RepTByte_
_RepTByte_:
        tb_op mov
        
E_RepTByte:
        ret

        db      E_XorTByte-_XorTByte_
_XorTByte_:
        tb_op xor
        
E_XorTByte:
        ret

        db      E_OrTByte-_OrTByte_
_OrTByte_:
        tb_op or
        
E_OrTByte:
        ret

endif

;;;=========================================================================
;;;
;;;   GetDot routine
;;;
;;;   Input       ES:_EDI      screen memory
;;;               CL           bit position
;;;
;;;   Output   V1 AX           colour of pixel at location
;;;            V2 DX:AX/EAX    colour of pixel at location
;;;
;;;=========================================================================

ifdef VERSION2

_GetDotWord_:
ifdef __386__
        xor     eax,eax
else
        xor     dx,dx
endif
        mov     ax, word ptr es:[_edi]     ; get word (colour)
        ret

_GetDotTByte_:
ifdef __386__
;;init regs for 386
        push   edx
endif
        xor    dx,dx
;;pixels are not necessarily aligned...
        mov     al, byte ptr es:[_edi]
        next_byte add, 1
        mov     ah, byte ptr es:[_edi]
        next_byte add, 1
        mov     dl, byte ptr es:[_edi]
        next_byte sub, 2
ifdef __386__
;;and restore
        push   dx
        push   ax
        pop    eax
        pop    edx
endif

        ret

_GetDotDWord_:
ifdef __386__
        mov     eax, dword ptr es:[_edi]     ; get dword (colour)
else
        mov     ax, word ptr es:[_edi]     ; get word (colour)
        mov     dx, word ptr es:2[_edi]     ; get word (colour)
endif
        ret

endif

;=========================================================================
;
;   Zap routine
;
;   Input       ES:_EDI,DH   screen memory pixel position
;            V1 AX           colour (unmasked)
;            V2 SI:AX/EAX    colour (unmasked)
;               BX           not used, may be fill mask
;               CX           number of pixels to fill
;               DL           not used
;
;=========================================================================

ifdef VERSION2

ifdef __386__
    VGAJmp dd _RepWord_, _XorWord_, _AndWord_, _OrWord_
else
    VGAJmp dw _RepWord_, _XorWord_, _AndWord_, _OrWord_
endif

;func_table    VGAJmp, <_RepWord_,_XorWord_,_AndWord_,_OrWord_>

_ZapWord_:
        _movzx   _ebx,di          ; check for end of page
        shl     cx, 1           ; - calc bytes = 2 * pix
        add     bx,cx           ; . . .
        _guess                  ; guess : need to do in two parts
          _quif   nc            ; - quif if no overflow
          _quif   e             ; - quif if the result was zero
          push    _ebx           ; - remember # of bytes remaining
          _movzx   _ecx,di        ; - calc # bytes left on this line
          neg     cx            ; - . . .
          shr     cx, 1         ; - convert to # of pix
          push    _edi           ; - save offset of screen memory
          call    zap_word      ; - zap remainder of this page
          pop     _edi           ; - restore offset of screen memory
          xor     di, di         ; - reset lower part of offset to 0
          pop     _ecx           ; - get remaining # of pixels
          push    _eax           ; - move to next page
          mov     al, ss:__VGAPage
          inc     al
          doicall __SetVGAPage
          pop     _eax
        _endguess               ; endguess
      shr     cx, 1           ; calc # of pix, then fall into zap_word
zap_word:
ifdef __386__
        _movzx   _ebx, word ptr ss:__PlotAct
        or      _ebx,_ebx
        _if     e               ; if replace mode
          rep     stosw         ; - do fast fill
        _else
          mov     _ebx, cs:VGAJmp[_ebx*4]  ; load plot routine
else
        mov     bx,ss:__PlotAct
        or      bx,bx
        _if     e               ; if replace mode
          rep     stosw         ; - do fast fill
        _else
          shl     bx, 1
          mov     bx,cs:VGAJmp[bx]      ; load plot routine
endif
zap_loop:                       ; loop
            call    _ebx         ; - call the plot routine
            inc     _edi         ; - move to next pixel on the right
            inc     _edi         ; . . .
          loop    zap_loop      ; until( --count == 0 )
        _endif
        ret

ifdef __386__
    VGADWJmp dd _RepDWord_, _XorDWord_, _AndDWord_, _OrDWord_
else
    VGADWJmp dw _RepDWord_, _XorDWord_, _AndDWord_, _OrDWord_
endif

_ZapDWord_:
        _movzx   _ebx,di          ; check for end of page
        shl     cx, 1           ; - calc bytes = 2 * pix
        shl     cx, 1           ; - calc bytes = 2 * pix
        add     bx,cx           ; . . .
        _guess                  ; guess : need to do in two parts
          _quif   nc            ; - quif if no overflow
          _quif   e             ; - quif if the result was zero
          push    _ebx           ; - remember # of bytes remaining
          _movzx   _ecx,di        ; - calc # bytes left on this line
          neg     cx            ; - . . .
          shr     cx, 1         ; - convert to # of pix
          shr     cx, 1         ; - convert to # of pix
          push    _edi           ; - save offset of screen memory
          call    zap_dword      ; - zap remainder of this page
          pop     _edi           ; - restore offset of screen memory
          xor     di, di         ; - reset lower part of offset to 0
          pop     _ecx           ; - get remaining # of pixels
          push    _eax           ; - move to next page
          mov     al, ss:__VGAPage
          inc     al
          doicall __SetVGAPage
          pop     _eax
        _endguess               ; endguess
      shr     cx, 1           ; calc # of pix, then fall into zap_word
      shr     cx, 1           ; calc # of pix, then fall into zap_word
zap_dword:
ifdef __386__
        _movzx   _ebx, word ptr ss:__PlotAct
        or      _ebx,_ebx
        _if     e               ; if replace mode
          rep     stosd         ; - do fast fill
          ret
        _else
          mov     _ebx, cs:VGADWJmp[_ebx*4]  ; load plot routine
        _endif
else
        mov     bx,ss:__PlotAct
        shl     bx, 1
        mov     bx,cs:VGADWJmp[bx]      ; load plot routine
endif

ifndef __386__
            push    dx
            mov     dx,si
endif

zap_dloop:                       ; loop

            call    _ebx         ; - call the plot routine
            add     _edi,4         ; - move to next pixel on the right
            loop    zap_dloop      ; until( --count == 0 )

ifndef __386__
            pop    dx
endif
        ret

ifdef __386__
    VGATBJmp dd _RepTByte_, _XorTByte_, _AndTByte_, _OrTByte_
else
    VGATBJmp dw _RepTByte_, _XorTByte_, _AndTByte_, _OrTByte_
endif

_ZapTByte_:
ifdef __386__
        _movzx   _ebx, word ptr ss:__PlotAct
        mov     _ebx, cs:VGATBJmp[_ebx*4]  ; load plot routine
else
        mov     bx,ss:__PlotAct
        shl     bx, 1
        mov     bx,cs:VGATBJmp[bx]      ; load plot routine
endif

ifndef __386__
            push    dx
            mov     dx,si
endif

zap_tbloop:                       ; loop

            call    _ebx         ; - call the plot routine
            next_byte add, 3
            loop    zap_tbloop      ; until( --count == 0 )

ifndef __386__
            pop    dx
endif
        ret

endif

_Zap256_:
        _movzx   _ebx,di          ; check for end of page
        add      bx,cx           ; . . .
        _guess                  ; guess : need to do in two parts
          _quif   nc            ; - quif if no overflow
          _quif   e             ; - quif if the result was zero
          push    _ebx           ; - remember # of pixels for next time
          _movzx  _ecx,di        ; - calc # left on this line
          neg     cx            ; - . . .
          push    _edi           ; - save offset of screen memory
          call    _Zap19_       ; - zap remainder of this page
          pop     _edi           ; - restore offset of screen memory
          xor     di,di         ; - reset lower part of offset to 0
          pop     _ecx           ; - get remaining # of pixels
          push    _eax           ; - move to next page
          mov     al,ss:__VGAPage
          inc     al
          doicall __SetVGAPage
          pop     _eax
        _endguess               ; endguess
        jmp     _Zap19_         ; zap remaining points

;=========================================================================
;
;   Fill routines
;
;   Input       ES:_EDI,DH   screen memory pixel position
;            V1 AX           colour (unmasked)
;            V2 SI:AX/EAX    colour (unmasked)
;               BH,BL        mask offset, fill mask
;               CX           number of pixels to fill
;               DL           not used
;
;=========================================================================

ifdef VERSION2

_FillWord_:
        push    _esi             ; save si
        _movzx   _esi,di          ; check for end of page
        shl   cx, 1           ; convert pixels to bytes
        add     si,cx           ; . . .
        _guess                  ; guess : need to do in two parts
          _quif   nc            ; - quif if no overflow
          _quif   e             ; - quif if the result was zero
          push    _esi           ; - remember # of bytes for next time
          _movzx   _ecx,di        ; - calc # of bytes left on this line
          neg     cx            ; - . . .
          shr     cx, 1         ; - convert bytes to pixels
          push    _edi           ; - save offset of screen memory
          call    fill_word     ; - fill remainder of this page
          pop     _edi           ; - restore offset of screen memory
          xor     di,di         ; - reset lower part of offset to 0
          mov     bl,dl         ; - remember updated bit mask
          xor     bh,bh         ; - (offset will be 0)
          pop     _ecx           ; - get # of remaining bytes
          push    _eax           ; - move to next page
          mov     al,ss:__VGAPage
          inc     al
          doicall __SetVGAPage
          pop     _eax
        _endguess               ; endguess
        shr     _ecx, 1                ; convert bytes to pixels
        call    fill_word       ; fill remaining points
        pop     _esi
        ret
fill_word:
        mov     dl,cl               ; save the count
        mov     cl,bh               ; get the fill offset
        rol     bl,cl               ; adjust fill mask
        mov     cl,dl               ; restore the count
        mov     dl,bl               ; get mask in dl
ifdef __386__
        _movzx   _ebx,word ptr ss:__PlotAct
        mov     _ebx,cs:VGAJmp[_ebx*4]; load the appropriate plot routine
else
        mov     bx,ss:__PlotAct     ; load the appropriate plot routine
        shl     bx,1                ; ...
        mov     bx,cs:VGAJmp[bx]    ; ...
endif
        cmp     word ptr ss:__Transparent,0     ; check for transparency
        _if     ne
style_loop:
            rol     dl,1            ; adjust fill style mask
            _if     c
              call    _ebx           ; pixel has to be set
            _endif
            inc     _edi             ; move to next pixel
            inc     _edi             ; . . .
          loop    style_loop
        _else
          mov     si,ax             ; save the colour mask
trans_loop:
            rol     dl,1            ; adjust fill style mask
            _if     c
              mov     ax,si         ; use colour mask
            _else
              xor     ax,ax         ; background color
            _endif
            call    _ebx             ; plot the pixel
            inc     _edi             ; move to next pixel
            inc     _edi             ; . . .
          loop    trans_loop        ; decrement the count
        _endif
        ret

_FillDWord_:
ifndef __386__
        push    bp
        mov     bp,sp
endif
        push    _esi             ; save si
        _movzx   _esi,di          ; check for end of page
        shl   cx, 1           ; convert pixels to bytes
        shl   cx, 1           ; convert pixels to bytes
        add     si,cx           ; . . .
        _guess                  ; guess : need to do in two parts
          _quif   nc            ; - quif if no overflow
          _quif   e             ; - quif if the result was zero
          push    _esi           ; - remember # of bytes for next time
          _movzx   _ecx,di        ; - calc # of bytes left on this line
          neg     cx            ; - . . .
          shr     cx, 1         ; - convert bytes to pixels
          shr     cx, 1         ; - convert bytes to pixels
          push    _edi           ; - save offset of screen memory
ifndef __386__
          mov    si,[bp-2]       ;get color hi word
endif
          call    fill_dword     ; - fill remainder of this page
          pop     _edi           ; - restore offset of screen memory
          xor     di,di         ; - reset lower part of offset to 0
          mov     bl,dl         ; - remember updated bit mask
          xor     bh,bh         ; - (offset will be 0)
          pop     _ecx           ; - get # of remaining bytes
          push    _eax           ; - move to next page
          mov     al,ss:__VGAPage
          inc     al
          doicall __SetVGAPage
          pop     _eax
        _endguess               ; endguess
        shr     _ecx, 1                ; convert bytes to pixels
        shr     _ecx, 1                ; convert bytes to pixels
ifndef __386__
          mov    si,[bp-2]       ;get color hi word
endif
        call    fill_dword       ; fill remaining points
        pop     _esi
ifndef __386__
        pop     bp
endif
        ret
fill_dword:
        mov     dl,cl               ; save the count
        mov     cl,bh               ; get the fill offset
        rol     bl,cl               ; adjust fill mask
        mov     cl,dl               ; restore the count
        mov     dl,bl               ; get mask in dl
ifdef __386__
        _movzx   _ebx,word ptr ss:__PlotAct
        mov     _ebx,cs:VGADWJmp[_ebx*4]; load the appropriate plot routine
else
        mov     bx,ss:__PlotAct     ; load the appropriate plot routine
        shl     bx,1                ; ...
        mov     bx,cs:VGADWJmp[bx]    ; ...
endif
        cmp     word ptr ss:__Transparent,0     ; check for transparency
        _if     ne
style_loop_dw:
            rol     dl,1            ; adjust fill style mask
            _if     c
ifndef __386__
            push    _edx
            mov     _edx,_esi           ; use colour mask hi
endif
              call    _ebx           ; pixel has to be set
ifndef __386__
            pop    _edx
endif
            _endif
            add     _edi,4           ; move to next pixel
          loop    style_loop_dw
        _else

          push _esi  ;;color hi
          mov  _esi,_eax ;;color lo
trans_loop_dw:
            rol     dl,1            ; adjust fill style mask
ifndef __386__
            push    _edx
endif
            _if     c
              mov     _eax,_esi           ; use colour mask
ifndef __386__
              mov     dx,ss:[bp-2]       ; use colour mask hi
endif
            _else
              xor     _eax,_eax         ; background color
ifndef __386__
              xor     _edx,_edx         ; background color
endif
            _endif
            call    _ebx             ; plot the pixel
            add     _edi,4           ; move to next pixel
ifndef __386__
            pop    _edx
endif
          loop    trans_loop_dw        ; decrement the count
          mov _eax,_esi ;;color lo
          pop _esi  ;;color hi
        _endif
        ret


_FillTByte_:
        mov     dl,cl               ; save the count
        mov     cl,bh               ; get the fill offset
        rol     bl,cl               ; adjust fill mask
        mov     cl,dl               ; restore the count
        mov     dl,bl               ; get mask in dl
ifdef __386__
        _movzx   _ebx,word ptr ss:__PlotAct
        mov     _ebx,cs:VGATBJmp[_ebx*4]; load the appropriate plot routine
else
        mov     bx,ss:__PlotAct     ; load the appropriate plot routine
        shl     bx,1                ; ...
        mov     bx,cs:VGATBJmp[bx]    ; ...
endif
        cmp     word ptr ss:__Transparent,0     ; check for transparency
        _if     ne
style_loop_tb:
            rol     dl,1            ; adjust fill style mask
            _if     c
ifndef __386__
            push    _edx
            mov     _edx,_esi           ; use colour mask hi
endif
              call    _ebx           ; pixel has to be set
ifndef __386__
            pop    _edx
endif
            _endif
          next_byte add, 3           ; move to next pixel
          loop    style_loop_tb
        _else

          push _esi  ;;color hi
          mov  _esi,_eax ;;color lo
ifndef __386__
          push    bp
          mov     bp,sp
endif
trans_loop_tb:
            rol     dl,1            ; adjust fill style mask
ifndef __386__
            push    _edx
endif
            _if     c
              mov     _eax,_esi           ; use colour mask
ifndef __386__
              mov     dx,[bp+2]       ; use colour mask hi
endif
            _else
              xor     _eax,_eax         ; background color
ifndef __386__
              xor     _edx,_edx         ; background color
endif
            _endif
            call    _ebx             ; plot the pixel
            next_byte add, 3           ; move to next pixel
ifndef __386__
            pop    _edx
endif
          loop    trans_loop_tb        ; decrement the count
ifndef __386__
          pop    bp
endif
          mov _eax,_esi ;;color lo
          pop _esi  ;;color hi
        _endif
ifndef __386__
        pop     bp
endif
        ret

endif

_Fill256_:
        push    _esi             ; save si
        _movzx   _esi,di          ; check for end of page
        add     si,cx           ; . . .
        _guess                  ; guess : need to do in two parts
          _quif   nc            ; - quif if no overflow
          _quif   e             ; - quif if the result was zero
          push    _esi           ; - remember # of pixels for next time
          _movzx   _ecx,di        ; - calc # left on this line
          neg     cx            ; - . . .
          push    _edi           ; - save offset of screen memory
          call    _Fill19_      ; - fill remainder of this page
          pop     _edi           ; - restore offset of screen memory
          xor     di,di         ; - reset lower part of offset to 0
          mov     bl,dl         ; - remember updated bit mask
          xor     bh,bh         ; - (offset will be 0)
          pop     _ecx           ; - get # of remaining pixels
          push    _eax           ; - move to next page
          mov     al,ss:__VGAPage
          inc     al
          doicall __SetVGAPage
          pop     _eax
        _endguess               ; endguess
        call    _Fill19_        ; fill remaining points
        pop     _esi
        ret

;=========================================================================
;
;   PixCopy routine
;
;   Input       ES:_EDI,DH   screen memory
;               SI:_EAX,DL   buffer to copy from
;               CX          number of pixels to copy
;
;=========================================================================

ifdef VERSION2

_PixCopyTByte_:
        push cx
        shl  cx, 1           ; triple the size and treat as byte sized pixels
ifdef __386__
        add  cx,ss:[esp]
        add  esp,2
else
        push bp
        mov  bp,sp
        add  cx,ss:[bp+2]
        pop  bp
        add  sp,2
endif
        jmp pcopy_common
_PixCopyDWord_:
        shl   cx, 1           ; double the size and treat as word sized pixels
_PixCopyWord_:
        shl   cx, 1           ; convert pix to # of bytes
pcopy_common:
        push    _ebx
        _movzx   _ebx,di          ; check for end of page
        add     bx,cx           ; . . .
        _guess                  ; guess : need to do in two parts
          _quif   nc            ; - quif if no overflow
          _quif   e             ; - quif if the result was zero
          push    _ebx           ; - remember # of bytes for next time
          _movzx   _ecx,di        ; - calc # of bytes left on this line
          neg     cx            ; - . . .
          push    _esi           ; - save segment of buffer
          push    _edi           ; - save offset of screen memory
          call    _PixCopy19_   ; - copy remainder of this page
          pop     _edi           ; - restore offset of screen memory
          xor     di,di         ; - reset lower part of offset to 0
          mov     _eax,_esi       ; - update offset of buffer
          pop     _esi           ; - reload segment of buffer
          pop     _ecx           ; - get remaining # of bytes
          push    _eax           ; - move to next page
          mov     al,ss:__VGAPage
          inc     al
          doicall __SetVGAPage
          pop     _eax
        _endguess               ; endguess
        call    _PixCopy19_     ; copy remaining pixels
        pop     _ebx
        ret

endif

_PixCopy256_:
        push    _ebx
        _movzx   _ebx,di          ; check for end of page
        add     bx,cx           ; . . .
        _guess                  ; guess : need to do in two parts
          _quif   nc            ; - quif if no overflow
          _quif   e             ; - quif if the result was zero
          push    _ebx           ; - remember # of pixels for next time
          _movzx   _ecx,di        ; - calc # left on this line
          neg     cx            ; - . . .
          push    _esi           ; - save segment of buffer
          push    _edi           ; - save offset of screen memory
          call    _PixCopy19_   ; - copy remainder of this page
          pop     _edi           ; - restore offset of screen memory
          xor     di,di         ; - reset lower part of offset to 0
          mov     _eax,_esi       ; - update offset of buffer
          pop     _esi           ; - reload segment of buffer
          pop     _ecx           ; - get remaining # of pixels
          push    _eax           ; - move to next page
          mov     al,ss:__VGAPage
          inc     al
          doicall __SetVGAPage
          pop     _eax
        _endguess               ; endguess
        call    _PixCopy19_     ; copy remaining pixels
        pop     _ebx
        ret

;=========================================================================
;
;   ReadRow routine
;
;   Input       ES:_EDI      buffer to copy into
;               SI:_EAX,DL   screen memory
;               CX          number of pixels to copy
;
;=========================================================================

ifdef VERSION2

_PixReadTByte_:
        push cx
        shl  cx, 1           ; triple the size and treat as byte sized pixels
ifdef __386__
        add  cx,ss:[esp]
        add  esp,2
else
        push bp
        mov  bp,sp
        add  cx,ss:[bp+2]
        pop  bp
        add  sp,2
endif
        jmp pread_common
_PixReadDWord_:
        shl   cx, 1           ; double the size and treat as word sized pixels
_PixReadWord_:
        shl   cx, 1           ; convert pixels to # of bytes
pread_common:
        push    _ebx
        _movzx   _ebx,ax          ; check for end of page
        add     bx,cx           ; . . .
        _guess                  ; guess : need to do in two parts
          _quif   nc            ; - quif if no overflow
          _quif   e             ; - quif if the result was zero
          push    _ebx           ; - remember # of bytes for next time
          _movzx   _ecx,ax        ; - calc # of bytes left on this line
          neg     cx            ; - . . .
          push    _esi           ; - save segment of screen memory
          push    _eax           ; - save offset of screen memory
          call    _PixRead19_   ; - copy remainder of this page
          pop     _eax           ; - restore offset of screen memory
          xor     ax,ax         ; - reset lower part of offset to 0
          pop     _esi           ; - reload segment of screen memory
          pop     _ecx           ; - get remaining # of bytes
          push    _eax           ; - move to next page
          mov     al,ss:__VGAPage
          inc     al
          doicall __SetVGAPage
          pop     _eax
        _endguess               ; endguess
        call    _PixRead19_     ; copy remaining pixels
        pop     _ebx
        ret

endif

_PixRead256_:
        push    _ebx
        _movzx   _ebx,ax          ; check for end of page
        add     bx,cx           ; . . .
        _guess                  ; guess : need to do in two parts
          _quif   nc            ; - quif if no overflow
          _quif   e             ; - quif if the result was zero
          push    _ebx           ; - remember # of pixels for next time
          _movzx   _ecx,ax        ; - calc # left on this line
          neg     cx            ; - . . .
          push    _esi           ; - save segment of screen memory
          push    _eax           ; - save offset of screen memory
          call    _PixRead19_   ; - copy remainder of this page
          pop     _eax           ; - restore offset of screen memory
          xor     ax,ax         ; - reset lower part of offset to 0
          pop     _esi           ; - reload segment of screen memory
          pop     _ecx           ; - get remaining # of pixels
          push    _eax           ; - move to next page
          mov     al,ss:__VGAPage
          inc     al
          doicall __SetVGAPage
          pop     _eax
        _endguess               ; endguess
        call    _PixRead19_     ; copy remaining pixels
        pop     _ebx
        ret

;=========================================================================
;
;   Scan routines
;
;   Input       ES:_EDI      screen memory
;            V1 AX           colour mask
;            V2 SI:AX/EAX    colour mask
;               CH           mask (CL may be bits per pixel)
;               BX           starting x-coordinate
;            V1 SI           ending x value (viewport boundary)
;            V2 stack/SI     ending x value (viewport boundary)
;               DL           0 if paint until colour, 1 if paint while
;
;   Output      BX          updated x-coordinate
;
;=========================================================================

ifdef VERSION2

_ScanLeftWord_:
ifndef __386__
        push    bp
        mov     bp,sp
        mov     si,ss:[bp+4]         ; get starting byte
endif
        inc     _ebx
        _loop
          cmp     ax,word ptr es:[_edi]  ; check for colour
        _if     e                     ; pixel is same as color mask
            or      dl,dl               ; quit loop if paint until
            je      short done_scanleftword
          _else
            or      dl,dl               ; quit loop if paint while
            jne     short done_scanleftword
          _endif
          dec     _ebx                   ; move to next pixel
          cmp     _ebx,_esi               ; check for viewport boundary
          _quif   le
          sub     di,2                  ; move left
          _if     c                     ; if < 0
            push    _eax                 ; - move to previous page
            mov     al,ss:__VGAPage
            dec     al
            doicall __SetVGAPage
            pop     _eax
          _endif
        _endloop
done_scanleftword:
ifndef __386__
        pop  bp
endif
        ret

_ScanLeftDWord_:
ifndef __386__
        push    bp
        mov     bp,sp
endif
        inc     _ebx
        _loop
ifdef  __386__
          cmp     eax,dword ptr es:[_edi]  ; check for colour
else
          cmp     ax,word ptr es:[_edi]  ; check for colour
          jne sld1
          cmp     si,word ptr es:[_edi+2]  ; check for colour
endif
          jne sld1
            or      dl,dl               ; quit loop if paint until and equals
            je      short done_scanleftdword
          jmp sld2
          sld1:
            or      dl,dl               ; quit loop if paint while ad not equal
            jne     short done_scanleftdword
          sld2:
          dec     _ebx                   ; move to next pixel
ifndef  __386__
          cmp     _ebx,ss:[bp+4]               ; check for viewport boundary
else
          cmp     _ebx,_esi               ; check for viewport boundary
endif
          _quif   le
          sub     di,4                  ; move left
          _if     c                     ; if < 0
            push    _eax                 ; - move to previous page
            mov     al,ss:__VGAPage
            dec     al
            doicall __SetVGAPage
            pop     _eax
          _endif
        _endloop
done_scanleftdword:
ifndef __386__
        pop  bp
endif
        ret

_ScanLeftTByte_:
        inc     _ebx
ifndef __386__
;;get si lo byte into dh somehow
        push    bp
        mov     bp,sp
        push si
        mov  dh,ss:[bp-2]
        pop si
else
;;get eax byte 2 into dh
        push eax
        mov  dh,ss:[esp+2]
        pop eax
endif
        next_byte add, 3
        _loop
          next_byte sub, 1
          cmp     dh,byte ptr es:[_edi]  ; check for colour
          jne slt5

          next_byte sub, 1
          cmp     ah,byte ptr es:[_edi]  ; check for colour
          jne slt4

          next_byte sub, 1
          cmp     al,byte ptr es:[_edi]  ; check for colour
          jne slt3

            or      dl,dl               ; quit loop if paint until and equals
            je      short done_scanlefttbyte
          jmp slt2
          slt5:
          next_byte sub, 1
          slt4:
          next_byte sub, 1
          slt3:
            or      dl,dl               ; quit loop if paint while ad not equal
            jne     short done_scanlefttbyte
          slt2:
          dec     _ebx                   ; move to next pixel
ifndef  __386__
          cmp     _ebx,ss:[bp+4]               ; check for viewport boundary
else
          cmp     _ebx,_esi               ; check for viewport boundary
endif
          _quif   le
        _endloop
done_scanlefttbyte:
ifndef __386__
        pop bp
endif
        ret

endif

_ScanLeft256_:
ifdef VERSION2
ifndef __386__
        push    bp
        mov     bp,sp
        mov     si,ss:[bp+4]
endif
endif
        inc     _ebx
        _loop
          cmp     al,es:[_edi]       ; check for colour
          _if     e                 ; pixel is same as color mask
            or      dl,dl           ; quit loop if paint until
            je      short done_scanleft
          _else
            or      dl,dl           ; quit loop if paint while
            jne     short done_scanleft
          _endif
          dec     _ebx               ; move to next pixel
          cmp     _ebx,_esi           ; check for viewport boundary
          _quif   le
          sub     di,1              ; move left
          _if     c                 ; if < 0
            push    _eax             ; - move to previous page
            mov     al,ss:__VGAPage
            dec     al
            doicall __SetVGAPage
            pop     _eax
          _endif
        _endloop
done_scanleft:
ifdef VERSION2
ifndef __386__
        pop bp
endif
endif
        ret

ifdef VERSION2

_ScanRightWord_:
ifndef __386__
        push    bp
        mov     bp,sp
        mov     si,ss:[bp+4]
endif
        dec     _ebx
        _loop
          cmp     ax,word ptr es:[_edi]  ; check for colour
          _if     e                     ; pixel is same as color mask
            or      dl,dl               ; quit loop if paint until
            je      done_scanrightword
          _else
            or      dl,dl               ; quit loop if paint while
            jne     done_scanrightword
          _endif
          inc     _ebx                   ; move to next pixel
          cmp     _ebx,_esi               ; check for viewport boundary
          _quif   ge
          add     di,2                  ; move right
          _if     c                     ; if < 0
            push    _eax                 ; - move to next page
            mov     al,ss:__VGAPage
            inc     al
            doicall __SetVGAPage
            pop     _eax
          _endif
        _endloop
done_scanrightword:
ifndef __386__
        pop bp
endif
        ret

_ScanRightDWord_:
ifndef __386__
        push    bp
        mov     bp,sp
endif
        dec     _ebx
        _loop
ifdef  __386__
          cmp     eax,dword ptr es:[_edi]  ; check for colour
else
          cmp     ax,word ptr es:[_edi]  ; check for colour
          jne srd1
          cmp     si,word ptr es:[_edi+2]  ; check for colour
endif
          jne srd1
            or      dl,dl               ; quit loop if paint until and equals
            je      short done_scanrightdword
          jmp srd2
          srd1:
            or      dl,dl               ; quit loop if paint while ad not equal
            jne     short done_scanrightdword
          srd2:
          inc     _ebx                   ; move to next pixel
ifndef  __386__
          cmp     _ebx,ss:[bp+4]               ; check for viewport boundary
else
          cmp     _ebx,_esi               ; check for viewport boundary
endif
          _quif   ge
          add     di,4                  ; move right
          _if     c                     ; if < 0
            push    _eax                 ; - move to previous page
            mov     al,ss:__VGAPage
            inc     al
            doicall __SetVGAPage
            pop     _eax
          _endif
        _endloop
done_scanrightdword:
ifndef __386__
        pop bp
endif
        ret

_ScanRighTByte_:
        inc     _ebx
ifndef __386__
;;get si lo byte into dh somehow
        push    bp
        mov     bp,sp
        push si
        mov  dh,ss:[bp-2]
        pop si
else
;;get eax byte 2 into dh
        push eax
        mov  dh,ss:[esp+2]
        pop eax
endif
        _loop

          cmp     al,byte ptr es:[_edi]  ; check for colour
          jne srt3

          next_byte add, 1
          cmp     ah,byte ptr es:[_edi]  ; check for colour
          jne srt4

          next_byte add, 1
          cmp     dh,byte ptr es:[_edi]  ; check for colour
          jne srt5
          next_byte add, 1
            or      dl,dl               ; quit loop if paint until and equals
            je      short done_scanrighttbyte
          jmp srt2
          srt3:
          next_byte add, 1
          srt4:
          next_byte add, 1
          srt5:
          next_byte add, 1
            or      dl,dl               ; quit loop if paint while ad not equal
            jne     short done_scanrighttbyte
          srt2:
          inc     _ebx                   ; move to next pixel
ifndef  __386__
          cmp     _ebx,ss:[bp+4]               ; check for viewport boundary
else
          cmp     _ebx,_esi               ; check for viewport boundary
endif
          _quif   ge
        _endloop
done_scanrighttbyte:
ifndef __386__
        pop bp
endif
        ret

endif

_ScanRight256_:
ifdef VERSION2
ifndef __386__
        push    bp
        mov     bp,sp
        mov     si,ss:[bp+4]
endif
endif
        dec     _ebx
        _loop
          cmp     al,es:[_edi]       ; check for colour
          _if     e                 ; pixel is same as color mask
            or      dl,dl           ; quit loop if paint until
            je      done_scanright
          _else
            or      dl,dl           ; quit loop if paint while
            jne     done_scanright
          _endif
          inc     _ebx               ; move to next pixel
          cmp     _ebx,_esi           ; check for viewport boundary
          _quif   ge
          add     di,1              ; move right
          _if     c                 ; if < 0
            push    _eax             ; - move to next page
            mov     al,ss:__VGAPage
            inc     al
            doicall __SetVGAPage
            pop     _eax
          _endif
        _endloop
done_scanright:
ifdef VERSION2
ifndef __386__
        pop bp
endif
endif
        ret


ifdef VERSION2

;      xdefp   _BresLine_
;; to let wdis come up with the actual byte values...
;;   for 16 bit case..

;_BresLine_:
;                push    bp
;                mov     bp,sp
;                push    si
;                mov     si,[bp+6]        ;...cause it's far call..
;    BL1:         rol     bx,1             ;            ... check line style
;                jnc     BL2
;                push    dx               ;.... save ctr
;                mov     dx,[bp-2]        ;.... get hi color word
;;    ( L1: )     [ inline code for plot function ]
;                pop     dx               ;.... restore ctr
;    BL2:         dec     dx
;                jl      BL3
;;                [ inline code for major function ]
;                sub     si,0x800f
;                jg      BL1
;                add     si,0x800f
;;                [ inline code for minor function ]
;                jmp     BL1
;    BL3:        pop     si
;                pop     bp
;               retf

endif

        endmod svgautil
        end
