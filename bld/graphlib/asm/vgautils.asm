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
;* Description:  Graphics library VGA specific code.
;*
;*****************************************************************************


include graph.inc

        xrefp   _CoXor_
        xrefp   _CoOr_

        extrn   __PlotAct : word
        extrn   __Transparent : word

        modstart vgautils,WORD

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
;   Input       ES:_EDI     screen memory
;            V1 AX          colour
;            V2 DX:AX/EAX   colour
;               CH          mask
;
;=========================================================================

        db      E_And19-_And19_
_And19_:
        and     es:0[_edi],al
E_And19:
        ret

        db      E_Rep19-_Rep19_
_Rep19_:
        mov     es:0[_edi],al     ; replace pixel
E_Rep19:
        ret

;=========================================================================
;
;   Movement primitives
;
;   Input       ES:_EDI     screen memory
;               AL          colour
;               CH          mask
;
;   Output      same        altered as per move
;
;=========================================================================

        db      E_MoveUp19-_MoveUp19_
_MoveUp19_:                     ; move up one dot
        sub     _edi,320         ; 320x200x256
E_MoveUp19:
        ret

        db      E_MoveDown19-_MoveDown19_
_MoveDown19_:                   ; move down one dot
        add     _edi,320
E_MoveDown19:
        ret

        db      E_MoveLeft19-_MoveLeft19_
_MoveLeft19_:
        dec     _edi
E_MoveLeft19:
        ret

        db      E_MoveRight19-_MoveRight19_
_MoveRight19_:
        inc     _edi
E_MoveRight19:
        ret

;=========================================================================
;
;   GetDot routine
;
;   Input       ES:_EDI     screen memory
;               CL          bit position
;
;   Output   V1 AX          colour of pixel at location
;            V2 DX:AX/EAX   colour of pixel at location
;
;=========================================================================

_GetDot19_:
ifdef VERSION2
ifdef __386__
        xor     eax,eax         ; clear dword
else
        xor     dx,dx           ; and high word
endif
endif
        mov     al,es:[_edi]    ; get byte
        xor     ah,ah           ; clear high byte
        ret

;=========================================================================
;
;   Zap routine
;
;   Input       ES:_EDI,DH  screen memory pixel position
;            V1 AX          colour (unmasked)
;            V2 SI:AX/EAX   colour (unmasked)
;               BX          not used
;               CX          number of pixels to fill
;               DL          not used
;
;=========================================================================

ifdef __386__
    VGAJmp dd _Rep19_, _CoXor_, _And19_, _CoOr_
else
    VGAJmp dw _Rep19_, _CoXor_, _And19_, _CoOr_
endif

_Zap19_:

ifdef __386__
        movzx   _ebx,word ptr ss:__PlotAct
        or      _ebx,_ebx
        _if     e               ; if replace mode
          rep     stosb         ; - do fast fill
        _else
          mov     _ebx,cs:VGAJmp[_ebx*4]  ; load plot routine
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
            call    _ebx         ; - call the plot routine
            inc     _edi         ; - move to next pixel on the right
          loop    zap_loop      ; until( --count == 0 )
        _endif
        ret

;=========================================================================
;
;   Fill routines
;
;   Input       ES:_EDI,DH  screen memory pixel position
;            V1 AX          colour (unmasked)
;            V2 SI:AX/EAX   colour (unmasked)
;               BH,BL       mask offset, fill mask
;               CX          number of pixels to fill
;               DL          not used
;
;==========================================================================

_Fill19_:
        mov     dl,cl               ; save the count
        mov     cl,bh               ; get the fill offset
        rol     bl,cl               ; adjust fill mask
        mov     cl,dl               ; restore the count
        mov     dl,bl               ; get mask in dl
ifdef __386__
        movzx   _ebx,word ptr ss:__PlotAct
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
            call    _ebx             ; plot the pixel
            inc     _edi             ; move to next pixel
          loop    trans_loop        ; decrement the count
        _endif
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

_PixCopy19_:
        push    ds              ; save DS
        mov     ds,si           ; get SI:AX into DS:SI
        mov     _esi,_eax
        cmp     word ptr ss:__PlotAct,0
        _if     e
          rep     movsb         ; - do fast copy
        _else
          push    _ebx
ifdef __386__
          movzx   _ebx,word ptr ss:__PlotAct
          mov     _ebx,cs:VGAJmp[_ebx*4]; load the appropriate plot routine
else
          mov     bx,ss:__PlotAct     ; load the appropriate plot routine
          shl     bx,1                ; ...
          mov     bx,cs:VGAJmp[bx]    ; ...
endif
copy_loop:                      ; loop
            lodsb               ; - get the next colour
            call    _ebx         ; - call the plot routine
            inc     _edi         ; - move to next pixel on the right
          loop    copy_loop     ; until( --count == 0 )
          pop     _ebx
        _endif
        pop     ds
        ret

;=========================================================================
;
;   ReadRow routine
;
;   Input       ES:_EDI      buffer to copy into
;               SI:_EAX,DL   screen memory (SI:AX for 16-bit)
;               CX          number of pixels to copy
;
;=========================================================================

_PixRead19_:
        push    ds              ; save DS
        mov     ds,si           ; get SI:AX into DS:SI
        mov     _esi,_eax
        rep     movsb           ; do the copy
        pop     ds
        ret

;=========================================================================
;
;   Scan routines
;
;   Input       ES:_EDI     screen memory
;            V1 AX          colour mask
;            V2 SI:AX/EAX   colour mask
;               CH          mask (CL may be bits per pixel)
;               BX          starting x-coordinate
;            V1 SI          ending x value (viewport boundary)
;            V2 stack/SI    ending x value (viewport boundary)
;               DL          0 if paint until colour, 1 if paint while
;
;   Output      BX          updated x-coordinate
;
;=========================================================================

_ScanLeft19_:
ifndef __386__
        push    bp
        mov     bp,sp
        mov     si,ss:[bp+4]
endif
        mov     _ecx,_ebx
        sub     _ecx,_esi
        inc     _ecx
        inc     _ecx
        std                     ; use autodecrement
        or      dl,dl
        _if      ne             ; if scan while colour
          _loop                 ; do the scan until colour = al and count > 0
            dec   _ecx           ; decrement the count
            _quif e             ; stop if cx == 0
            scasb               ; scan the byte
            _quif ne            ; ... while color == al
          _endloop
        _else                   ; else if scan until colour
          _loop                 ; do the scan while color = al and count > 0
            dec   _ecx           ; decrement the count
            _quif e             ; stop if cx == 0
            scasb               ; scan the byte
            _quif e             ; ... until color == al
          _endloop
        _endif
        cld                     ; clear direction flag
        add     _esi,_ecx
        mov     _ebx,_esi
ifndef __386__
        pop bp
endif
        ret

_ScanRight19_:
ifndef __386__
        push    bp
        mov     bp,sp
        mov     si,ss:[bp+4]
endif
        mov     _ecx,_esi
        sub     _ecx,_ebx
        inc     _ecx
        inc     _ecx
        cld                     ; use autoincrement
        or      dl,dl
        _if      ne             ; if scan while colour
          repe    scasb
        _else                   ; else if scan until colour
          repne   scasb
        _endif
        sub     _esi,_ecx
        mov     _ebx,_esi
ifndef __386__
        pop bp
endif
        ret

        endmod vgautils
        end
