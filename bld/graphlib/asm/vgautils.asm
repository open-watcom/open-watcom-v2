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

        xref    _CoXor_
        xref    _CoOr_

        extrn   __PlotAct : word
        extrn   __Transparent : word

        modstart vgautils

        xdefp   _MoveUp19_
        xdefp   _MoveDown19_
        xdefp   _MoveLeft19_
        xdefp   _MoveRight19_
        xdefp   _Rep19_
        xdefp   _And19_
        xdefp   _Zap19_
        xdefp   _GetDot19_
        xdefp   _Fill19_
        xdefp   _PixCopy19_
        xdefp   _PixRead19_
        xdefp   _ScanLeft19_
        xdefp   _ScanRight19_

;=========================================================================
;
;   Plotting primitives
;
;   Input       ES:EDI      screen memory
;               AL          colour
;               CH          mask
;
;=========================================================================

        db      E_And19-_And19_
_And19_:
        and     es:0[edi],al
E_And19:
        ret

        db      E_Rep19-_Rep19_
_Rep19_:
        mov     es:0[edi],al     ; replace pixel
E_Rep19:
        ret

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

        db      E_MoveUp19-_MoveUp19_
_MoveUp19_:                     ; move up one dot
        sub     edi,320         ; 320x200x256
E_MoveUp19:
        ret

        db      E_MoveDown19-_MoveDown19_
_MoveDown19_:                   ; move down one dot
        add     edi,320
E_MoveDown19:
        ret

        db      E_MoveLeft19-_MoveLeft19_
_MoveLeft19_:
        dec     edi
E_MoveLeft19:
        ret

        db      E_MoveRight19-_MoveRight19_
_MoveRight19_:
        inc     edi
E_MoveRight19:
        ret

;=========================================================================
;
;   GetDot routine
;
;   Input       ES:EDI      screen memory
;               CL          bit position
;
;   Output      AX          colour of pixel at location
;
;=========================================================================

_GetDot19_:
        mov     al,es:[edi]     ; get byte
        xor     ah,ah           ; clear high byte
        ret

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

func_table      VGAJmp,<_Rep19_,_CoXor_,_And19_,_CoOr_>

_Zap19_:

ifdef _386
        movzx   ebx,word ptr ss:__PlotAct
        or      ebx,ebx
        _if     e               ; if replace mode
          rep     stosb         ; - do fast fill
        _else
          mov     ebx,cs:VGAJmp[ebx*4]  ; load plot routine
else
        mov     bx,ss:__PlotAct
        or      bx,bx
        _if     e               ; if replace mode
          rep     stosb         ; - do fast fill
        _else
          shl     bx,1
          mov     bx,cs:VGAJmp[bx]      ; load plot routine
endif
zap_loop:                       ; loop
            call    ebx         ; - call the plot routine
            inc     edi         ; - move to next pixel on the right
          loop    zap_loop      ; until( --count == 0 )
        _endif
        ret

;=========================================================================
;
;   Fill routines
;
;   Input       ES:EDI,DH   screen memory
;               AL          colour (unmasked)
;               BH,BL       mask offset, fill mask
;               CX          number of pixels to fill
;
;==========================================================================

_Fill19_:
        mov     dl,cl               ; save the count
        mov     cl,bh               ; get the fill offset
        rol     bl,cl               ; adjust fill mask
        mov     cl,dl               ; restore the count
        mov     dl,bl               ; get mask in dl
ifdef _386
        movzx   ebx,word ptr ss:__PlotAct
        mov     ebx,cs:VGAJmp[ebx*4]; load the appropriate plot routine
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
              call    ebx           ; pixel has to be set
            _endif
            inc     edi             ; move to next pixel
          loop    style_loop
        _else
          mov     dh,al             ; save the colour mask
trans_loop:
            rol     dl,1            ; adjust fill style mask
            _if     c
              mov     al,dh         ; use colour mask
            _else
              xor     al,al         ; background color
            _endif
            call    ebx             ; plot the pixel
            inc     edi             ; move to next pixel
          loop    trans_loop        ; decrement the count
        _endif
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

_PixCopy19_:
        push    ds              ; save DS
        mov     ds,si           ; get SI:AX into DS:SI
        mov     esi,eax
        cmp     word ptr ss:__PlotAct,0
        _if     e
          rep     movsb         ; - do fast copy
        _else
          push    ebx
ifdef _386
          movzx   ebx,word ptr ss:__PlotAct
          mov     ebx,cs:VGAJmp[ebx*4]; load the appropriate plot routine
else
          mov     bx,ss:__PlotAct     ; load the appropriate plot routine
          shl     bx,1                ; ...
          mov     bx,cs:VGAJmp[bx]    ; ...
endif
copy_loop:                      ; loop
            lodsb               ; - get the next colour
            call    ebx         ; - call the plot routine
            inc     edi         ; - move to next pixel on the right
          loop    copy_loop     ; until( --count == 0 )
          pop     ebx
        _endif
        pop     ds
        ret

;=========================================================================
;
;   ReadRow routine
;
;   Input       ES:EDI      buffer to copy into
;               SI:EAX,DL   screen memory (SI:AX for 16-bit)
;               CX          number of pixels to copy
;
;=========================================================================

_PixRead19_:
        push    ds              ; save DS
        mov     ds,si           ; get SI:AX into DS:SI
        mov     esi,eax
        rep     movsb           ; do the copy
        pop     ds
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

_ScanLeft19_:
        mov     ecx,ebx
        sub     ecx,esi
        inc     ecx
        inc     ecx
        std                     ; use autodecrement
        or      dl,dl
        _if      ne             ; if scan while colour
          _loop                 ; do the scan until colour = al and count > 0
            dec   ecx           ; decrement the count
            _quif e             ; stop if cx == 0
            scasb               ; scan the byte
            _quif ne            ; ... while color == al
          _endloop
        _else                   ; else if scan until colour
          _loop                 ; do the scan while color = al and count > 0
            dec   ecx           ; decrement the count
            _quif e             ; stop if cx == 0
            scasb               ; scan the byte
            _quif e             ; ... until color == al
          _endloop
        _endif
        cld                     ; clear direction flag
        add     esi,ecx
        mov     ebx,esi
        ret

_ScanRight19_:
        mov     ecx,esi
        sub     ecx,ebx
        inc     ecx
        inc     ecx
        cld                     ; use autoincrement
        or      dl,dl
        _if      ne             ; if scan while colour
          repe    scasb
        _else                   ; else if scan until colour
          repne   scasb
        _endif
        sub     esi,ecx
        mov     ebx,esi
        ret

        endmod vgautils
        end
