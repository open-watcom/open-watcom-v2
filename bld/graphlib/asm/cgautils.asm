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

        xref    BitCopy
        xref    BitReplace
        xref    BitAnd
        xref    BitOr
        xref    BitXor

        extrn   __PlotAct : word
        extrn   __Transparent : word

        modstart cgautils

        xdefp   _CoRep_
        xdefp   _CoXor_
        xdefp   _CoAnd_
        xdefp   _CoOr_
        xdefp   _MoveUp_
        xdefp   _MoveDown_
        xdefp   _Move1Left_
        xdefp   _Move2Left_
        xdefp   _Move1Right_
        xdefp   _Move2Right_
        xdefp   _Get1Dot_
        xdefp   _Get2Dot_
        xdefp   _Pix1Zap_
        xdefp   _Pix2Zap_
        xdefp   _Pix1Fill_
        xdefp   _Pix2Fill_
        xdefp   _Pix1Copy_
        xdefp   _Pix2Copy_
        xdefp   _Pix1Read_
        xdefp   _Pix2Read_
        xdefp   _CGAScanLeft_
        xdefp   _CGAScan1Right_
        xdefp   _CGAScan2Right_

;=========================================================================
;
;   Plotting primitives
;
;   Input       ES:EDI      screen memory
;               AL          colour
;               CH          mask
;
;=========================================================================

        db      E_CoXor-_CoXor_
_CoXor_:                        ; xor in new pixel
        xor     es:0[edi],al
E_CoXor:
        ret

        db      E_CoAnd-_CoAnd_
_CoAnd_:                        ; and in new pixel
        or      al,ch           ; mask on other bits
        and     es:0[edi],al    ; do and with memory
        xor     al,ch           ; restore al
E_CoAnd:
        ret

        db      E_CoRep-_CoRep_
_CoRep_:                        ; replace pixel
        mov     ah,es:[edi]     ; get current byte
        and     ah,ch           ; mask out current colour
        or      ah,al           ; OR in new colour
        mov     es:[edi],ah     ; replace byte
E_CoRep:
        ret

        db      E_CoOr-_CoOr_
_CoOr_:                         ; or in new pixel
        or      es:0[edi],al
E_CoOr:
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

        db      E_Move2Right-_Move2Right_
_Move2Right_:                   ; move right in medium-res mode
        ror     al,cl           ; shift colour pattern to the right
        ror     ch,cl           ; shift mask to the right
        sbb     edi,-1          ; increment di if start of new byte
E_Move2Right:
        ret                     ; return

        db      E_Move1Right-_Move1Right_
_Move1Right_:                   ; move right in high-res mode
        ror     al,1            ; shift colour pattern to the right
        ror     ch,1            ; shift mask to the right
        sbb     edi,-1          ; move to next byte if it is time
E_Move1Right:
        ret                     ; return

        db      E_Move2Left-_Move2Left_
_Move2Left_:                    ; move left in medium-res mode
        rol     al,cl           ; shift colour pattern to the left
        rol     ch,cl           ; shift mask to the left
        adc     edi,-1          ; decrement di if start of new byte
E_Move2Left:
        ret                     ; return

        db      E_Move1Left-_Move1Left_
_Move1Left_:                    ; move left in high-res mode
        rol     al,1            ; shift colour pattern to the left
        rol     ch,1            ; shift mask to the left
        adc     edi,-1          ; move to next byte if it is time
E_Move1Left:
        ret                     ; return

        db      E_MoveUp-_MoveUp_
_MoveUp_:                       ; move up 1 dot
ifdef _386                      ; can't use other method since with DOS4GW
        push    ebx             ; ... the segment is part of EDI
        mov     bx,di
        and     bx,8000h        ; keep high bit
        and     di,7fffh
endif
        sub     di,2000h        ; assume di >= 2000h
        _if     b               ; if di < 0
          add     di,2000h+2000h-80 ; add back 2000h + enough for next row
        _endif                  ; endif
ifdef _386
        or      di,bx
        pop     ebx
endif
E_MoveUp:
        ret                     ; return

        db      E_MoveDown-_MoveDown_
_MoveDown_:                     ; move down 1 dot
ifdef _386                      ; can't use other method since with DOS4GW
        push    ebx             ; ... the segment is part of EDI
        mov     bx,di
        and     bx,8000h        ; keep high bit
        and     di,7fffh
endif
        sub     di,2000h-80     ; assume di > $2000 (odd scan row)
        _if     b               ; if not
          add     di,2000h-80+2000h ; compensate for mistake & add $2000
        _endif                  ; endif
ifdef _386
        or      di,bx
        pop     ebx
endif
E_MoveDown:
        ret                     ; return

;=========================================================================
;
;   GetDot routines
;
;   Input       ES:EDI      screen memory
;               CL          bit position
;
;   Output      AX          colour of pixel at location
;
;=========================================================================

_Get1Dot_:
        mov     al,es:[edi]     ; get byte
        sub     cl,7            ; shift byte by ( 7 - bit_position )
        neg     cl              ; . . .
        shr     al,cl           ; right align pixel
        and     al,1            ; keep only 1 bit (1 bit per pixel)
        xor     ah,ah           ; clear high byte
        ret

_Get2Dot_:
        mov     al,es:[edi]      ; get byte
        sub     cl,6            ; shift byte by ( 6 - bit_position )
        neg     cl              ; . . .
        shr     al,cl           ; right align pixel
        and     al,3            ; keep only 2 bits (2 bits per pixel)
        xor     ah,ah           ; clear high byte
        ret

;=========================================================================
;
;   PixCopy routines
;
;   Input       ES:EDI,DH   screen memory
;               SI:EAX,DL   buffer to copy from
;               CX          number of pixels to copy
;
;=========================================================================

_Pix2Copy_:
        shl     ecx,1           ; # bits = 2 * # pixels

_Pix1Copy_:
        push    ds              ; save DS
        mov     ds,si           ; get SI:AX into DS:SI
        mov     esi,eax
        docall  SetupAction
        docall  BitCopy
        docall  BitReplace
        pop     ds
        ret

;=========================================================================
;
;   ReadRow routines
;
;   Input       ES:EDI      buffer to copy into
;               SI:EAX,DL   screen memory
;               CX          number of pixels to copy
;
;=========================================================================

_Pix2Read_:
        shl     ecx,1

_Pix1Read_:
        push    ds              ; save DS
        mov     ds,si           ; get SI:AX into DS:SI
        mov     esi,eax
        docall  BitCopy
        pop     ds
        ret


func_table      PlotJmp,<BitReplace,BitXor,BitAnd,BitOr>

SetupAction:
ifdef _386
        movzx   ebx,word ptr ss:__PlotAct
        jmp     cs:PlotJmp[ebx*4]
else
        mov     bx,ss:__PlotAct
        shl     bx,1
        jmp     cs:PlotJmp[bx]
endif

func_table      FillJmp,<_CoRep_,_CoXor_,_CoAnd_,_CoOr_>

;=========================================================================
;
;   Zap routines
;
;   Input       ES:EDI,DH   screen memory
;               AL          colour (unmasked)
;               BX          not used
;               CX          number of pixels to fill
;
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

_Pix1Zap_:                          ; zap fill ( 1 bit per pixel )
        push    ebp
        push    esi
        or      al,al               ; if given colour is zero
        _if     e
          xor     bl,bl             ; colour is zero
        _else
          mov     bl,0ffh           ; replicate 1 across byte
        _endif
        mov     bh,0ffh             ; mask is ff (affect all bits)
        jmp     short fill_1_common

_Pix1Fill_:                         ; fill with style ( 1 bit per pixel )
        push    ebp
        push    esi
        cmp     word ptr ss:__Transparent,0
        _if     e                   ; if _Transparent == 0
          mov     bh,0ffh           ; - bit mask is ff (affect all bits)
        _else                       ; else
          mov     bh,bl             ; - bit mask is fill mask (affect only the mask bits)
        _endif                      ; endif
        or      al,al               ; if colour is zero
        _if     e                   ; then
          mov     bl,al             ; - colour is zero
        _endif                      ; endif (else colour is fill mask)

fill_1_common:                      ; at this point bl = colour, bh = mask

ifdef _386
        movzx   esi,word ptr ss:__PlotAct
        mov     esi,cs:FillJmp[esi*4]
else                                ; load address of plot function
        mov     si,ss:__PlotAct
        shl     si,1
        mov     si,cs:FillJmp[si]
endif
        mov     ebp,ecx             ; get count in bp

        or      dh,dh               ; if bit_pos != 0
        _if     ne                  ; (doesn't start on byte boundary)
          mov     cl,dh             ; - get bit_pos
          mov     ah,80h            ; - initial value of mask
          shr     ah,cl             ; - shift mask into position
          xor     ch,ch             ; - ch will be mask for wanted bits
          _loop                     ; - loop
            or      ch,ah           ; - - put bit in
            dec     ebp             ; - - count--
            _quif   e               ; - - quif count == 0
            shr     ah,1            ; - - move to next bit
          _until    c               ; - until bit comes out
          mov     al,bl             ; - get colour
          and     al,ch             ; - keep bits of colour we want
          and     ch,bh             ; - keep only mask bits
          not     ch                ; - bit mask is complement
          call    esi               ; - plot the pixels
          inc     edi               ; - move to next byte
        _endif                      ; endif

        mov     edx,ebp             ; get count
        mov     cl,3                ; convert to byte count
        shr     edx,cl              ; . . .
        _if     ne                  ; if byte count != 0
          mov     al,bl             ; - get colour
          mov     ch,bh             ; - get mask
          not     ch                ; - . . .
          _guess                    ; - guess (can use fast method)
            cmp     word ptr ss:__PlotAct,0
            _quif   ne              ; - - quif if its not replace mode
            or      ch,ch           ; - -
            _quif   ne              ; - - quif if the mask is not zero
            mov     ecx,edx         ; - - use fast fill
            cld                     ; - -
            rep     stosb           ; - - fill buffer until( --count == 0 )
          _admit                    ; - admit (use slow method)
            _loop                   ; - - loop
              call    esi           ; - - - plot byte
              inc     edi           ; - - - move to next byte
              dec     edx           ; - - - count--
              _quif   e             ; - - - quif count == 0
            _endloop                ; - - endloop
          _endguess                 ; - endguess
        _endif                      ; endif

        and     ebp,7               ; if bit count != 0
        _if     ne                  ; then
          mov     ecx,8             ; - mask = ff << ( 8 - bit count )
          sub     ecx,ebp           ; - . . .
          mov     ch,0ffh           ; - ch will be mask for wanted bits
          shl     ch,cl             ; - . . .
          mov     al,bl             ; - get colour
          and     al,ch             ; - keep bits of colour we want
          and     ch,bh
          not     ch                ; - bit mask is complement
          call    esi               ; - plot the pixels
        _endif                      ; endif

        pop     esi
        pop     ebp
        ret

TwoBitTab   db  00000000b
            db  01010101b
            db  10101010b
            db  11111111b

_Pix2Zap_:                          ; Zap fill routine ( 2 bits per pixel )
        push    ebp
        push    esi
        mov     ebp,ecx             ; move count to bp
        mov     ah,dh               ; move bit offset into ah

        mov     ebx,offset TwoBitTab
        xlat    cs:TwoBitTab        ; get extended colour
        mov     dl,al               ; place colour into dx
        mov     dh,al
        mov     ebx,0ffffh          ; bit mask is ffff (affect all bits)
        jmp     fill_2_common

_Pix2Fill_:                         ; fill style routine ( 2 bits per pixel)
        push    ebp
        push    esi
        mov     ebp,ecx             ; move count to bp
        mov     ah,dh               ; move bit offset into ah

        mov     ecx,8               ; loop index
        xor     esi,esi             ; construct the extended fill mask
do_mask:                            ;   for 2 bpp
          shr     bl,1              ; check each bit in the fill mask
          _if     c
            or      esi,11B         ; bit is set
          _endif
          ror     si,1              ; rotate the mask (16 bits only)
          ror     si,1
        loop    do_mask
        mov     cl,bh               ; save mask offset

        mov     ebx,offset TwoBitTab
        xlat    cs:TwoBitTab        ; get extended colour
        mov     dl,al               ; place colour into dx
        mov     dh,al
        and     edx,esi             ; keep only bits in fill mask
        cmp     word ptr ss:__Transparent,0
        _if     e                   ; if _Transparent == 0
          mov     ebx,0ffffh        ; - bit mask is ffff (affect all bits)
        _else                       ; else
          mov     ebx,esi           ; - bit mask is fill mask (affect only the mask bits)
       _endif                       ; endif

        cmp     cl,3                ; if mask offset > 3
        _if     g                   ; then
          xchg    dh,dl             ; - flip colour and mask
          xchg    bh,bl             ; - to use other half of byte
        _endif                      ; endif

fill_2_common:
        ; at this point we now
        ;   bp - count
        ;   ah - bit offset
        ;   bx - bit mask
        ;   dx - colour mask
        ;   cx - scratch register
        ;   si - will hold address of plot routine

ifdef _386
        movzx   esi,word ptr ss:__PlotAct
        mov     esi,cs:FillJmp[esi*4]
else                                ; load address of plot function
        mov     si,ss:__PlotAct
        shl     si,1
        mov     si,cs:FillJmp[si]
endif
        or      ah,ah               ; if bit_pos != 0
        _if     ne                  ; (doesn't start on byte boundary)
          mov     cl,ah             ; - get bit_pos
          mov     ah,11000000b      ; - initial value of mask
          shr     ah,cl             ; - shift mask into position
          xor     ch,ch             ; - ch will be mask for wanted bits
          _loop                     ; - loop
            or      ch,ah           ; - - put bit in
            dec     ebp             ; - - count--
            _quif   e               ; - - quif count == 0
            shr     ah,1            ; - - move to next pixel
            shr     ah,1            ; - - . . .
          _until    c               ; - until bit comes out
          mov     al,dh             ; - get colour
          and     al,ch             ; - keep bits of colour we want
          and     ch,bh             ; - and with fill mask
          not     ch                ; - bit mask is complement
          call    esi               ; - plot the pixels
          inc     edi               ; - move to next byte
          xchg    dh,dl             ; - get ready for next byte
          xchg    bh,bl             ; - . . .
        _endif                      ; endif

        push    ebp                 ; save count
        shr     ebp,1               ; convert to byte count
        shr     ebp,1               ; . . .
        _if     ne                  ; if byte count != 0
          mov     al,dh             ; - get colour
          mov     ch,bh             ; - get mask
          not     ch                ; - . . .
          _guess                    ; - guess (can use fast method)
            cmp     word ptr ss:__PlotAct,0
            _quif   ne              ; - - quif if its not replace mode
            cmp     ebx,0ffffh      ; - -
            _quif   ne              ; - - quif if the mask is not ffff
            cmp     dl,dh           ; - -
            _quif   ne              ; - - quif if two parts of colour not same
            mov     ecx,ebp         ; - - use fast fill
            cld                     ; - -
            rep     stosb           ; - - fill buffer until( --count == 0 )
          _admit                    ; - admit (use slow method)
            _loop                   ; - - loop
              call    esi           ; - - - plot byte
              inc     edi           ; - - - move to next byte
              xchg    dh,dl         ; - - - get ready for next byte
              xchg    bh,bl         ; - - - . . .
              dec     ebp           ; - - - count--
              _quif   e             ; - - - quif count == 0
              mov     al,dh         ; - - - get colour
              mov     ch,bh         ; - - - get mask
              not     ch            ; - - - . . .
            _endloop                ; - - endloop
          _endguess                 ; - endguess
        _endif                      ; endif

        pop     ebp                 ; restore count
        and     ebp,3               ; if bit count != 0
        _if     ne                  ; then
          mov     ecx,8             ; - mask = ff << ( 8 - 2 * bit count )
          sub     ecx,ebp           ; - . . .
          sub     ecx,ebp           ; - . . .
          mov     ch,0ffh           ; - ch will be mask for wanted bits
          shl     ch,cl             ; - . . .
          mov     al,dh             ; - get colour
          and     al,ch             ; - keep bits of colour we want
          and     ch,bh             ; - and with fill mask
          not     ch                ; - bit mask is complement
          call    esi               ; - plot the pixels
        _endif                      ; endif

        pop     esi
        pop     ebp
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

_CGAScanLeft_:
        mov     ah,dl               ; put the border flag in ah
        inc     ebx
        not     ch
        mov     dl,es:[edi]         ; get starting byte
        _loop
          mov     dh,dl             ; restore full byte info
          and     dh,ch             ; isolate pixel by anding with bit mask
          cmp     dh,al             ; test if pixel is set
          _if     e                 ; pixel is same as color mask
            or      ah,ah           ; quit loop if paint until
            je      short done_scanleft
          _else
            or      ah,ah           ; quit loop if paint while
            jne     short done_scanleft
          _endif
          dec     ebx               ; move to next pixel
          cmp     esi,ebx           ; check for viewport boundary
          _quif   ge
          rol     al,cl             ; rotate color mask for new pixel
          rol     ch,cl             ; rotate bit mask for new pixel
          _if     c
            dec     edi             ; look at next byte
            mov     dl,es:[edi]
          _endif
        _endloop
done_scanleft:
        ret

_CGAScan1Right_:
        not     ch                  ; reverse complement of mask
        mov     dh,ch               ; build an extended right mask for 1st byte
        shl     dh,1
        neg     dh                  ; example : 00000100 -> 00000111
        not     dh
        _loop                       ; line up with byte boundary
          dec   ebx
          shl   ch,1
        _until  c
        or      al,al               ; build an extended colour mask
        _if     ne
          mov     al,0ffh
        _endif
        not     al                  ; complement of colour mask
        mov     ah,es:[edi]         ; load first byte
        xor     ah,al               ; build scan byte mask
        or      dl,dl               ; if( border_flag != 0 )
        _if     ne
          not     ax
        _endif
        and     ah,dh               ; border condition true in first byte
        jne     short done_CGAscan1right
        mov     ecx,esi             ; convert the pixel count inside
        sub     ecx,ebx             ; the viewport to a byte count
        dec     ecx                 ; in order to scan full bytes
        and     cl,0F8h
        add     ebx,ecx
        shr     ecx,1
        shr     ecx,1
        shr     ecx,1
        jle     short done_CGAscan1right  ; less than 8 pixels to scan
        inc     edi
        repe    scasb
        shl     ecx,1
        shl     ecx,1
        shl     ecx,1
        sub     ebx,ecx
        dec     edi
        mov     ah,es:[edi]
        xor     ah,al

done_CGAscan1right:
        _loop
          shl     ah,1
          _quif   c
          cmp     esi,ebx
          _quif   le
          inc     ebx
        _endloop
        ret

_CGAScan2Right_:
        mov     ah,al               ; build an extended colour mask for 4 pixels
        rol     ah,cl               ; and keep it in al
        or      al,ah
        mov     ah,al
        rol     ah,cl
        rol     ah,cl
        or      al,ah
        mov     ecx,ebx             ; adjust count for 1st byte
        and     ecx,3
        sub     ebx,ecx
        dec     ebx
        shl     cl,1                ; build an extended right mask for 1st byte
        mov     dh,0ffh             ; example : 00001100 -> 00001111
        shr     dh,cl
        or      dl,dl               ; if( border_flag != 0 )
        _if     ne
          mov     ch,10101010B      ; note: ch=0 from above, otherwise
        _endif
        _loop
          mov     ah,es:[edi]       ; read the next 4 pixels
          xor     ah,al             ; match with extended colour mask
          not     ah                ; merge the two bits representing each
          mov     cl,ah             ; pixel so that a one in the higher
          and     cl,10101010B      ; bit of the two will represent a
          and     ah,01010101B      ; match
          shl     ah,1
          and     ah,cl
          xor     ah,ch             ; check with border flag
          and     ah,dh             ; account for 1st byte boundary
          _quif   ne
          cmp     ebx,esi           ; check for clip region
          _if     ge
            mov     ebx,esi         ; return edge of clip region
            ret
          _endif
          inc     edi               ; move to next byte
          add     ebx,4             ; 4 pixels per byte
          mov     dh,0ffh           ; reset mask to full
        _endloop
        add     esi,4
        _loop                       ; process the last byte
          shl     ah,1
          _quif   c
          shl     ah,1
          cmp     esi,ebx
          _quif   le
          inc     ebx
        _endloop
        ret

;       extrn   __LineXInc : word
;       extrn   __LineYInc : word
;       extrn   __LineYMove : word
;
;=========================== Line Drawing =================================
;
;       ES:DI      pointer to screen memory
;       AL         colour mask
;       CH,CL      bit mask,bits per pixel
;       BX         line style
;       DX         number of pixels to draw
;       SI         decision variable
;
;==========================================================================
;
;        xdefp   _CGA_XLine_
;_CGA_XLine_:
;
;        push    bp
;        mov     bp,ss:__PlotAct         ; load the appropriate plot routine
;        or      bp,bp
;        _if     ne
;          shl     bp,1                    ; ...
;          mov     bp,FillJmp[bp]          ; ...
;        _endif
;topx:   _loop                           ; loop ( for each point )
;          rol     bx,1                  ; if dot is to be drawn
;          _if     c                     ; then
;            or      bp,bp
;            _if     ne
;              call    bp                  ; - plot the point
;            _else
;              mov     ah,es:[di]      ; get current byte
;              and     ah,ch           ; mask out current colour
;              or      ah,al           ; OR in new colour
;              mov     es:[di],ah      ; replace byte
;            _endif
;          _endif                        ; endif
;          dec     dx                    ; --count
;          _quif   l                     ; quif count < 0
;          ror     al,cl                 ; move right
;          ror     ch,cl                 ; . . .
;          sbb     di,-1                 ; . . .
;          sub     si,ss:__LineYInc      ; d -= dy
;          jg      topx                  ; if d <= 0
;          add     si,ss:__LineXInc      ; - d += dx
;          sub     di,ss:__LineYMove     ; - y += y_inc
;          _if     b                     ; - if di < 0
;            add     di,2000h+2000h-80   ; - - add back 2000h + enough for next row
;          _endif                        ; - endif
;        _endloop                        ; endloop
;        pop     bp
;        ret
;
;        xdefp   _CGA_YLine_
;_CGA_YLine_:
;
;        push    bp
;        mov     bp,ss:__PlotAct         ; load the appropriate plot routine
;        or      bp,bp
;        _if     ne
;          shl     bp,1                    ; ...
;          mov     bp,FillJmp[bp]          ; ...
;        _endif
;topy:   _loop                           ; loop ( for each point )
;          rol     bx,1                  ; if dot is to be drawn
;          _if     c                     ; then
;            or      bp,bp
;            _if     ne
;              call    bp                  ; - plot the point
;            _else
;              mov     ah,es:[di]      ; get current byte
;              and     ah,ch           ; mask out current colour
;              or      ah,al           ; OR in new colour
;              mov     es:[di],ah      ; replace byte
;            _endif
;          _endif                        ; endif
;          dec     dx                    ; --count
;          _quif   l                     ; quif count < 0
;          sub     di,ss:__LineYMove     ; y += y_inc
;          _if     b                     ; - if di < 0
;            add     di,2000h+2000h-80   ; - - add back 2000h + enough for next row
;          _endif                        ; - endif
;          sub     si,ss:__LineXInc      ; d -= dx
;          jg      topy                  ; if d <= 0
;          add     si,ss:__LineYInc      ; - d += dy
;          ror     al,cl                 ; move right
;          ror     ch,cl                 ; . . .
;          sbb     di,-1                 ; . . .
;        _endloop                        ; endloop
;        pop     bp
;        ret

        endmod cgautils
        end
