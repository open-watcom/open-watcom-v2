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


include graph.inc


        xref    _Zap19_
        xref    _Fill19_
        xref    _PixCopy19_
        xref    _PixRead19_

        extrn   __PlotAct : word
        extrn   __Transparent : word
        extrn   __VGAPage : byte
        extrn   __VGAGran : byte
        extptr  __SetVGAPage

        modstart svgautil

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
;;      xdefp   _MoveUp1280_    ; this mode is untested
;;      xdefp   _MoveDown1280_
;;      xdefp   _MoveLeftWord_
;;      xdefp   _MoveRightWord_
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
          push    ebx
          push    ecx
          push    edx
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
          pop     edx
          pop     ecx
          pop     ebx
        _endif
        _retf

_PageVideo7_:
        cmp     al,ss:__VGAPage ; if page already selected
        _if     ne              ; then
          mov     ss:__VGAPage,al ; remember new page
          mov     ah,al
          push    edx
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
          pop     edx
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
          push    edx
          mov     dx,3ceH
          out     dx,ax
          pop     edx
        _endif
        _retf

_PageATI_:
        cmp     al,ss:__VGAPage ; if page already selected
        _if     ne              ; then
          mov     ss:__VGAPage,al ; remember new page
          mov     ah,al
          cli
          push    edx
          mov     edx,1ceH
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
          pop     edx
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
          push    edx
          mov     dx,3CDh
          out     dx,al
          pop     edx
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
          push    edx
          mov     dx,3CDh
          out     dx,al
          pop     edx
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
          push    edx
          mov     dx,3dfh
          out     dx,al
          pop     edx
        _endif
        _retf

_PageTrident_:
        cmp     al,ss:__VGAPage ; if page already selected
        _if     ne              ; then
          mov     ss:__VGAPage,al ; remember new page
          mov     ah,al
          xor     ah,02h
          push    edx
          mov     dx,3c4h
          mov     al,0eh
          out     dx,al
          inc     dx
          in      al,dx
          and     al,0f0h
          or      al,ah
          out     dx,al
          pop     edx
        _endif
        _retf

_PageChips_:
        cmp     al,ss:__VGAPage ; if page already selected
        _if     ne              ; then
          mov     ss:__VGAPage,al ; remember new page
          push    ecx
          mov     cl,ss:__VGAGran
          shl     al,cl
          mov     ah,al
          push    edx
          mov     dx,03d6h
          mov     al,10h
          out     dx,al
          inc     dx
          in      al,dx
          and     al,0c0h
          or      al,ah
          out     dx,al
          pop     edx
          pop     ecx
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
          push    edx
          mov     dx,03c4h
          mov     al,6
          out     dx,al
          inc     dx
          in      al,dx
          and     al,80h
          or      al,40h
          or      al,ah
          out     dx,al
          pop     edx
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
          push    edx
          mov     dx,03d4h
          out     dx,al
          inc     dx
          in      al,dx
          and     ax,0ff0h
          or      al,ah
          out     dx,al
          pop     edx
        _endif
        _retf

;=========================================================================
;
;   Movement primitives
;
;   Input       ES:EDI      screen memory
;               AL          colour
;               CH          mask
;
;   Output      same        altered as per move
;
;=========================================================================

        db      E_MoveUp100-_MoveUp100_
_MoveUp100_:                    ; move up one dot
        sub     edi,100         ; 800x600x16
E_MoveUp100:
        ret

        db      E_MoveDown100-_MoveDown100_
_MoveDown100_:                  ; move down one dot
        add     edi,100
E_MoveDown100:
        ret

        db      E_MoveUp128-_MoveUp128_
_MoveUp128_:                    ; move up one dot
        sub     di,128          ; 1024x768x16
        _if     c               ; if < 0
          push    eax           ; - move to previous page
          mov     al,ss:__VGAPage
          dec     al
          doicall __SetVGAPage
          pop     eax
        _endif
E_MoveUp128:
        ret

        db      E_MoveDown128-_MoveDown128_
_MoveDown128_:                  ; move down one dot
        add     di,128
        _if     c               ; if < 0
          push    eax           ; - move to next page
          mov     al,ss:__VGAPage
          inc     al
          doicall __SetVGAPage
          pop     eax
        _endif
E_MoveDown128:
        ret

        db      E_MoveLeft256-_MoveLeft256_
_MoveLeft256_:                  ; 256 colour SuperVGA modes
        sub     di,1
        _if     c               ; if < 0
          push    eax           ; - move to previous page
          mov     al,ss:__VGAPage
          dec     al
          doicall __SetVGAPage
          pop     eax
        _endif
E_MoveLeft256:
        ret

        db      E_MoveRight256-_MoveRight256_
_MoveRight256_:                 ; 256 colour SuperVGA modes
        add     di,1
        _if     c               ; if < 0
          push    eax           ; - move to next page
          mov     al,ss:__VGAPage
          inc     al
          doicall __SetVGAPage
          pop     eax
        _endif
E_MoveRight256:
        ret

        db      E_MoveUp640-_MoveUp640_
_MoveUp640_:                    ; 640x???, 256 colour SuperVGA modes
        sub     di,640
        _if     c               ; if < 0
          push    eax           ; - move to previous page
          mov     al,ss:__VGAPage
          dec     al
          doicall __SetVGAPage
          pop     eax
        _endif
E_MoveUp640:
        ret

        db      E_MoveDown640-_MoveDown640_
_MoveDown640_:                  ; 640x???, 256 colour SuperVGA modes
        add     di,640
        _if     c               ; if < 0
          push    eax           ; - move to next page
          mov     al,ss:__VGAPage
          inc     al
          doicall __SetVGAPage
          pop     eax
        _endif
E_MoveDown640:
        ret

        db      E_MoveUp800-_MoveUp800_
_MoveUp800_:                    ; 800x600, 256 colour SuperVGA modes
        sub     di,800
        _if     c               ; if < 0
          push    eax           ; - move to previous page
          mov     al,ss:__VGAPage
          dec     al
          doicall __SetVGAPage
          pop     eax
        _endif
E_MoveUp800:
        ret

        db      E_MoveDown800-_MoveDown800_
_MoveDown800_:                  ; 800x600, 256 colour SuperVGA modes
        add     di,800
        _if     c               ; if < 0
          push    eax           ; - move to next page
          mov     al,ss:__VGAPage
          inc     al
          doicall __SetVGAPage
          pop     eax
        _endif
E_MoveDown800:
        ret

        db      E_MoveUp1024-_MoveUp1024_
_MoveUp1024_:                   ; 1024x768, 256 colour SuperVGA modes
        sub     di,1024
        _if     c               ; if < 0
          push    eax           ; - move to previous page
          mov     al,ss:__VGAPage
          dec     al
          doicall __SetVGAPage
          pop     eax
        _endif
E_MoveUp1024:
        ret

        db      E_MoveDown1024-_MoveDown1024_
_MoveDown1024_:                  ; 1024x768, 256 colour SuperVGA modes
        add     di,1024
        _if     c               ; if < 0
          push    eax           ; - move to next page
          mov     al,ss:__VGAPage
          inc     al
          doicall __SetVGAPage
          pop     eax
        _endif
E_MoveDown1024:
        ret

;;        db      E_MoveUp1280-_MoveUp1280_
;;_MoveUp1280_:                   ; 640x480, 32768 colour SuperVGA modes
;;        sub     di,1280
;;        _if     c               ; if < 0
;;          push    eax           ; - move to previous page
;;          mov     al,ss:__VGAPage
;;          dec     al
;;          doicall __SetVGAPage
;;          pop     eax
;;        _endif
;;E_MoveUp1280:
;;        ret
;;
;;        db      E_MoveDown1280-_MoveDown1280_
;;_MoveDown1280_:                  ; 640x480, 32768 colour SuperVGA modes
;;        add     di,1280
;;        _if     c               ; if < 0
;;          push    eax           ; - move to next page
;;          mov     al,ss:__VGAPage
;;          inc     al
;;          doicall __SetVGAPage
;;          pop     eax
;;        _endif
;;E_MoveDown1280:
;;        ret
;;
;;        db      E_MoveLeftWord-_MoveLeftWord_
;;_MoveLeftWord_:                  ; 32768 colour (16 bit) SuperVGA modes
;;        sub     di,2
;;        _if     c               ; if < 0
;;          push    eax           ; - move to previous page
;;          mov     al,ss:__VGAPage
;;          dec     al
;;          doicall __SetVGAPage
;;          pop     eax
;;        _endif
;;E_MoveLeftWord:
;;        ret
;;
;;        db      E_MoveRightWord-_MoveRightWord_
;;_MoveRightWord_:                 ; 32768 colour (16 bit) SuperVGA modes
;;        add     di,2
;;        _if     c               ; if < 0
;;          push    eax           ; - move to next page
;;          mov     al,ss:__VGAPage
;;          inc     al
;;          doicall __SetVGAPage
;;          pop     eax
;;        _endif
;;E_MoveRightWord:
;;        ret
;;
;;;=========================================================================
;;;
;;;   Plotting primitives
;;;
;;;   Input       ES:EDI      screen memory
;;;               AL          colour
;;;               CH          mask
;;;
;;;=========================================================================
;;
;;        db      E_AndWord-_AndWord_
;;_AndWord_:
;;        and    word ptr es:0[edi],ax
;;E_AndWord:
;;        ret
;;
;;        db      E_RepWord-_RepWord_
;;_RepWord_:
;;        mov    word ptr es:0[edi],ax     ; replace pixel
;;E_RepWord:
;;        ret
;;
;;        db      E_XorWord-_XorWord_
;;_XorWord_:
;;        xor    word ptr es:0[edi],ax     ; replace pixel
;;E_XorWord:
;;        ret
;;
;;        db      E_OrWord-_OrWord_
;;_OrWord_:
;;        or     word ptr es:0[edi],ax     ; replace pixel
;;E_OrWord:
;;        ret
;;
;;;=========================================================================
;;;
;;;   GetDot routine
;;;
;;;   Input       ES:EDI      screen memory
;;;               CL          bit position
;;;
;;;   Output      AX          colour of pixel at location
;;;
;;;=========================================================================
;;
;;_GetDotWord_:
;;        mov     ax, word ptr es:[edi]     ; get word (colour)
;;        ret

;=========================================================================
;
;   Zap routine
;
;   Input       ES:EDI,DH   screen memory
;               AL          colour (unmasked)
;               BX          not used
;               CX          number of pixels to fill
;
;=========================================================================

;;func_table    VGAJmp, <_RepWord_,_XorWord_,_AndWord_,_OrWord_>
;;
;;_ZapWord_:
;;        movzx   ebx,di          ; check for end of page
;;      shl     cx, 1           ; - calc bytes = 2 * pix
;;        add     bx,cx           ; . . .
;;        _guess                  ; guess : need to do in two parts
;;          _quif   nc            ; - quif if no overflow
;;          _quif   e             ; - quif if the result was zero
;;          push    ebx           ; - remember # of bytes remaining
;;          movzx   ecx,di        ; - calc # bytes left on this line
;;          neg     cx            ; - . . .
;;        shr     cx, 1         ; - convert to # of pix
;;          push    edi           ; - save offset of screen memory
;;          call    zap_word      ; - zap remainder of this page
;;          pop     edi           ; - restore offset of screen memory
;;          xor     di, di         ; - reset lower part of offset to 0
;;          pop     ecx           ; - get remaining # of pixels
;;          push    eax           ; - move to next page
;;          mov     al, ss:__VGAPage
;;          inc     al
;;          doicall __SetVGAPage
;;          pop     eax
;;        _endguess               ; endguess
;;      shr     cx, 1           ; calc # of pix, then fall into zap_word
;;zap_word:
;;ifdef _386
;;        movzx   ebx, word ptr ss:__PlotAct
;;        or      ebx,ebx
;;        _if     e               ; if replace mode
;;          rep     stosw         ; - do fast fill
;;        _else
;;          mov     ebx, cs:VGAJmp[ebx*4]  ; load plot routine
;;else
;;        mov     bx,ss:__PlotAct
;;        or      bx,bx
;;        _if     e               ; if replace mode
;;          rep     stosw         ; - do fast fill
;;        _else
;;          shl     bx, 1
;;          mov     bx,cs:VGAJmp[bx]      ; load plot routine
;;endif
;;zap_loop:                       ; loop
;;            call    ebx         ; - call the plot routine
;;            inc     edi         ; - move to next pixel on the right
;;            inc     edi         ; . . .
;;          loop    zap_loop      ; until( --count == 0 )
;;        _endif
;;        ret

_Zap256_:
        movzx   ebx,di          ; check for end of page
        add     bx,cx           ; . . .
        _guess                  ; guess : need to do in two parts
          _quif   nc            ; - quif if no overflow
          _quif   e             ; - quif if the result was zero
          push    ebx           ; - remember # of pixels for next time
          movzx   ecx,di        ; - calc # left on this line
          neg     cx            ; - . . .
          push    edi           ; - save offset of screen memory
          call    _Zap19_       ; - zap remainder of this page
          pop     edi           ; - restore offset of screen memory
          xor     di,di         ; - reset lower part of offset to 0
          pop     ecx           ; - get remaining # of pixels
          push    eax           ; - move to next page
          mov     al,ss:__VGAPage
          inc     al
          doicall __SetVGAPage
          pop     eax
        _endguess               ; endguess
        jmp     _Zap19_         ; zap remaining points

;=========================================================================
;
;   Fill routines
;
;   Input       ES:EDI,DH   screen memory
;               AL          colour (unmasked)
;               BH,BL       mask offset, fill mask
;               CX          number of pixels to fill
;
;=========================================================================

;;_FillWord_:
;;        push    esi             ; save si
;;        movzx   esi,di          ; check for end of page
;;        shl   cx, 1           ; convert pixels to bytes
;;        add     si,cx           ; . . .
;;        _guess                  ; guess : need to do in two parts
;;          _quif   nc            ; - quif if no overflow
;;          _quif   e             ; - quif if the result was zero
;;          push    esi           ; - remember # of bytes for next time
;;          movzx   ecx,di        ; - calc # of bytes left on this line
;;          neg     cx            ; - . . .
;;          shr     cx, 1         ; - convert bytes to pixels
;;          push    edi           ; - save offset of screen memory
;;          call    fill_word     ; - fill remainder of this page
;;          pop     edi           ; - restore offset of screen memory
;;          xor     di,di         ; - reset lower part of offset to 0
;;          mov     bl,dl         ; - remember updated bit mask
;;          xor     bh,bh         ; - (offset will be 0)
;;          pop     ecx           ; - get # of remaining pixels
;;          push    eax           ; - move to next page
;;          mov     al,ss:__VGAPage
;;          inc     al
;;          doicall __SetVGAPage
;;          pop     eax
;;        _endguess               ; endguess
;;        shr     ecx, 1                ; convert bytes to pixels
;;        call    fill_word       ; fill remaining points
;;        pop     esi
;;        ret
;;fill_word:
;;        mov     dl,cl               ; save the count
;;        mov     cl,bh               ; get the fill offset
;;        rol     bl,cl               ; adjust fill mask
;;        mov     cl,dl               ; restore the count
;;        mov     dl,bl               ; get mask in dl
;;ifdef _386
;;        movzx   ebx,word ptr ss:__PlotAct
;;        mov     ebx,cs:VGAJmp[ebx*4]; load the appropriate plot routine
;;else
;;        mov     bx,ss:__PlotAct     ; load the appropriate plot routine
;;        shl     bx,1                ; ...
;;        mov     bx,cs:VGAJmp[bx]    ; ...
;;endif
;;        cmp     word ptr ss:__Transparent,0     ; check for transparency
;;        _if     ne
;;style_loop:
;;            rol     dl,1            ; adjust fill style mask
;;            _if     c
;;              call    ebx           ; pixel has to be set
;;            _endif
;;            inc     edi             ; move to next pixel
;;            inc     edi             ; . . .
;;          loop    style_loop
;;        _else
;;          mov     si,ax             ; save the colour mask
;;trans_loop:
;;            rol     dl,1            ; adjust fill style mask
;;            _if     c
;;              mov     ax,si         ; use colour mask
;;            _else
;;              xor     ax,ax         ; background color
;;            _endif
;;            call    ebx             ; plot the pixel
;;            inc     edi             ; move to next pixel
;;            inc     edi             ; . . .
;;          loop    trans_loop        ; decrement the count
;;        _endif
;;        ret

_Fill256_:
        push    esi             ; save si
        movzx   esi,di          ; check for end of page
        add     si,cx           ; . . .
        _guess                  ; guess : need to do in two parts
          _quif   nc            ; - quif if no overflow
          _quif   e             ; - quif if the result was zero
          push    esi           ; - remember # of pixels for next time
          movzx   ecx,di        ; - calc # left on this line
          neg     cx            ; - . . .
          push    edi           ; - save offset of screen memory
          call    _Fill19_      ; - fill remainder of this page
          pop     edi           ; - restore offset of screen memory
          xor     di,di         ; - reset lower part of offset to 0
          mov     bl,dl         ; - remember updated bit mask
          xor     bh,bh         ; - (offset will be 0)
          pop     ecx           ; - get # of remaining pixels
          push    eax           ; - move to next page
          mov     al,ss:__VGAPage
          inc     al
          doicall __SetVGAPage
          pop     eax
        _endguess               ; endguess
        call    _Fill19_        ; fill remaining points
        pop     esi
        ret

;=========================================================================
;
;   PixCopy routine
;
;   Input       ES:EDI,DH   screen memory
;               SI:EAX,DL   buffer to copy from
;               CX          number of pixels to copy
;
;=========================================================================

;;_PixCopyWord_:
;;        push    ebx
;;        movzx   ebx,di          ; check for end of page
;;        shl   cx, 1           ; convert pix to # of bytes
;;        add     bx,cx           ; . . .
;;        _guess                  ; guess : need to do in two parts
;;          _quif   nc            ; - quif if no overflow
;;          _quif   e             ; - quif if the result was zero
;;          push    ebx           ; - remember # of bytes for next time
;;          movzx   ecx,di        ; - calc # of bytes left on this line
;;          neg     cx            ; - . . .
;;          push    esi           ; - save segment of buffer
;;          push    edi           ; - save offset of screen memory
;;          call    _PixCopy19_   ; - copy remainder of this page
;;          pop     edi           ; - restore offset of screen memory
;;          xor     di,di         ; - reset lower part of offset to 0
;;          mov     eax,esi       ; - update offset of buffer
;;          pop     esi           ; - reload segment of buffer
;;          pop     ecx           ; - get remaining # of bytes
;;          push    eax           ; - move to next page
;;          mov     al,ss:__VGAPage
;;          inc     al
;;          doicall __SetVGAPage
;;          pop     eax
;;        _endguess               ; endguess
;;        call    _PixCopy19_     ; copy remaining pixels
;;        pop     ebx
;;        ret

_PixCopy256_:
        push    ebx
        movzx   ebx,di          ; check for end of page
        add     bx,cx           ; . . .
        _guess                  ; guess : need to do in two parts
          _quif   nc            ; - quif if no overflow
          _quif   e             ; - quif if the result was zero
          push    ebx           ; - remember # of pixels for next time
          movzx   ecx,di        ; - calc # left on this line
          neg     cx            ; - . . .
          push    esi           ; - save segment of buffer
          push    edi           ; - save offset of screen memory
          call    _PixCopy19_   ; - copy remainder of this page
          pop     edi           ; - restore offset of screen memory
          xor     di,di         ; - reset lower part of offset to 0
          mov     eax,esi       ; - update offset of buffer
          pop     esi           ; - reload segment of buffer
          pop     ecx           ; - get remaining # of pixels
          push    eax           ; - move to next page
          mov     al,ss:__VGAPage
          inc     al
          doicall __SetVGAPage
          pop     eax
        _endguess               ; endguess
        call    _PixCopy19_     ; copy remaining pixels
        pop     ebx
        ret

;=========================================================================
;
;   ReadRow routine
;
;   Input       ES:EDI      buffer to copy into
;               SI:EAX,DL   screen memory
;               CX          number of pixels to copy
;
;=========================================================================

;;_PixReadWord_:
;;        push    ebx
;;        movzx   ebx,ax          ; check for end of page
;;      shl     cx, 1           ; convert pixels to # of bytes
;;        add     bx,cx           ; . . .
;;        _guess                  ; guess : need to do in two parts
;;          _quif   nc            ; - quif if no overflow
;;          _quif   e             ; - quif if the result was zero
;;          push    ebx           ; - remember # of bytes for next time
;;          movzx   ecx,ax        ; - calc # of bytes left on this line
;;          neg     cx            ; - . . .
;;          push    esi           ; - save segment of screen memory
;;          push    eax           ; - save offset of screen memory
;;          call    _PixRead19_   ; - copy remainder of this page
;;          pop     eax           ; - restore offset of screen memory
;;          xor     ax,ax         ; - reset lower part of offset to 0
;;          pop     esi           ; - reload segment of screen memory
;;          pop     ecx           ; - get remaining # of bytes
;;          push    eax           ; - move to next page
;;          mov     al,ss:__VGAPage
;;          inc     al
;;          doicall __SetVGAPage
;;          pop     eax
;;        _endguess               ; endguess
;;        call    _PixRead19_     ; copy remaining pixels
;;        pop     ebx
;;        ret

_PixRead256_:
        push    ebx
        movzx   ebx,ax          ; check for end of page
        add     bx,cx           ; . . .
        _guess                  ; guess : need to do in two parts
          _quif   nc            ; - quif if no overflow
          _quif   e             ; - quif if the result was zero
          push    ebx           ; - remember # of pixels for next time
          movzx   ecx,ax        ; - calc # left on this line
          neg     cx            ; - . . .
          push    esi           ; - save segment of screen memory
          push    eax           ; - save offset of screen memory
          call    _PixRead19_   ; - copy remainder of this page
          pop     eax           ; - restore offset of screen memory
          xor     ax,ax         ; - reset lower part of offset to 0
          pop     esi           ; - reload segment of screen memory
          pop     ecx           ; - get remaining # of pixels
          push    eax           ; - move to next page
          mov     al,ss:__VGAPage
          inc     al
          doicall __SetVGAPage
          pop     eax
        _endguess               ; endguess
        call    _PixRead19_     ; copy remaining pixels
        pop     ebx
        ret

;=========================================================================
;
;   Scan routines
;
;   Input       ES:EDI      screen memory
;               AL          colour mask
;               CH          mask (CL may be bits per pixel)
;               BX          starting x-coordinate
;               SI          ending x value (viewport boundary)
;               DL          0 if paint until colour, 1 if paint while
;
;   Output      BX          updated x-coordinate
;
;=========================================================================

;;_ScanLeftWord_:
;;        inc     ebx
;;        _loop
;;          cmp     ax,word ptr es:[edi]  ; check for colour
;;        _if     e                     ; pixel is same as color mask
;;            or      dl,dl               ; quit loop if paint until
;;            je      short done_scanleftword
;;          _else
;;            or      dl,dl               ; quit loop if paint while
;;            jne     short done_scanleftword
;;          _endif
;;          dec     ebx                   ; move to next pixel
;;          cmp     ebx,esi               ; check for viewport boundary
;;          _quif   le
;;          sub     di,2                  ; move left
;;          _if     c                     ; if < 0
;;            push    eax                 ; - move to previous page
;;            mov     al,ss:__VGAPage
;;            dec     al
;;            doicall __SetVGAPage
;;            pop     eax
;;          _endif
;;        _endloop
;;done_scanleftword:
;;        ret

_ScanLeft256_:
        inc     ebx
        _loop
          cmp     al,es:[edi]       ; check for colour
          _if     e                 ; pixel is same as color mask
            or      dl,dl           ; quit loop if paint until
            je      short done_scanleft
          _else
            or      dl,dl           ; quit loop if paint while
            jne     short done_scanleft
          _endif
          dec     ebx               ; move to next pixel
          cmp     ebx,esi           ; check for viewport boundary
          _quif   le
          sub     di,1              ; move left
          _if     c                 ; if < 0
            push    eax             ; - move to previous page
            mov     al,ss:__VGAPage
            dec     al
            doicall __SetVGAPage
            pop     eax
          _endif
        _endloop
done_scanleft:
        ret

;;_ScanRightWord_:
;;        dec     ebx
;;        _loop
;;          cmp     ax,word ptr es:[edi]  ; check for colour
;;          _if     e                     ; pixel is same as color mask
;;            or      dl,dl               ; quit loop if paint until
;;            je      done_scanrightword
;;          _else
;;            or      dl,dl               ; quit loop if paint while
;;            jne     done_scanrightword
;;          _endif
;;          inc     ebx                   ; move to next pixel
;;          cmp     ebx,esi               ; check for viewport boundary
;;          _quif   ge
;;          add     di,2                  ; move right
;;          _if     c                     ; if < 0
;;            push    eax                 ; - move to next page
;;            mov     al,ss:__VGAPage
;;            inc     al
;;            doicall __SetVGAPage
;;            pop     eax
;;          _endif
;;        _endloop
;;done_scanrightword:
;;        ret

_ScanRight256_:
        dec     ebx
        _loop
          cmp     al,es:[edi]       ; check for colour
          _if     e                 ; pixel is same as color mask
            or      dl,dl           ; quit loop if paint until
            je      done_scanright
          _else
            or      dl,dl           ; quit loop if paint while
            jne     done_scanright
          _endif
          inc     ebx               ; move to next pixel
          cmp     ebx,esi           ; check for viewport boundary
          _quif   ge
          add     di,1              ; move right
          _if     c                 ; if < 0
            push    eax             ; - move to next page
            mov     al,ss:__VGAPage
            inc     al
            doicall __SetVGAPage
            pop     eax
          _endif
        _endloop
done_scanright:
        ret

        endmod svgautil
        end
