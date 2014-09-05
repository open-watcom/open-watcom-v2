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
;* Description:  Graphics library EGA specific code.
;*
;*****************************************************************************


include graph.inc

        extrn   __PlotAct : word
        extrn   __Transparent : word

        modstart egautils,WORD

        xdefp   _EGAMoveUpHi_
        xdefp   _EGAMoveUpLo_
        xdefp   _EGAMoveDownHi_
        xdefp   _EGAMoveDownLo_
        xdefp   _EGAMoveLeft_
        xdefp   _EGAMoveRight_
        xdefp   _EGARep_
        xdefp   _EGAGetDot_
        xdefp   _EGAGetDotEO_
        xdefp   _EGAGetDotMono_
        xdefp   _EGAZap_
        xdefp   _EGAZapEO_
        xdefp   _EGAZapMono_
        xdefp   _EGAFill_
        xdefp   _EGAFillEO_
        xdefp   _EGAFillMono_
        xdefp   _EGAPixCopy_
        xdefp   _EGAPixCopyEO_
        xdefp   _EGAPixCopyMono_
        xdefp   _EGAReadRow_
        xdefp   _EGAReadRowEO_
        xdefp   _EGAReadRowMono_
        xdefp   _EGAScanLeft_
        xdefp   _EGAScanLeftEO_
        xdefp   _EGAScanLeftMono_
        xdefp   _EGAScanRight_
        xdefp   _EGAScanRightEO_
        xdefp   _EGAScanRightMono_

;=========================================================================
;
;   EGA constants
;
;=========================================================================

SEQADDR =       03C4h
GRADDR  =       03CEh

;   EGA sequence registers

MAPMASK =       02h

;   EGA control registers

SRREG   =       00h
ESRREG  =       01h
CMPREG  =       02h
ACTN    =       03h
RMSEL   =       04h
MODEREG =       05h
DONTCARE =      07h
EGAMASK =       08h

;   EGA mode values

WRITE_MODE_0    =       00h
WRITE_MODE_1    =       01h
WRITE_MODE_2    =       02h
READ_MODE_0     =       00h
READ_MODE_1     =       08h
EVEN_ODD_MODE   =       10h

MonoTab db      00000000B       ; black
        db      00000011B       ; video
        db      00001100B       ; blink
        db      00001111B       ; bright

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

        db      E_EGAMoveUpHi-_EGAMoveUpHi_
_EGAMoveUpHi_:
        sub     _edi,80          ; move up in 640 pixel wide mode
E_EGAMoveUpHi:
        ret

        db      E_EGAMoveDownHi-_EGAMoveDownHi_
_EGAMoveDownHi_:
        add     _edi,80          ; move down in 640 pixel wide mode
E_EGAMoveDownHi:
        ret

        db      E_EGAMoveUpLo-_EGAMoveUpLo_
_EGAMoveUpLo_:
        sub     _edi,40          ; move up in 320 pixel wide mode
E_EGAMoveUpLo:
        ret

        db      E_EGAMoveDownLo-_EGAMoveDownLo_
_EGAMoveDownLo_:
        add     _edi,40          ; move down in 320 pixel wide mode
E_EGAMoveDownLo:
        ret

        db      E_EGAMoveLeft-_EGAMoveLeft_
_EGAMoveLeft_:                  ; move left 1 pixel
        rol     ch,1
        sbb     _edi,0
E_EGAMoveLeft:
        ret

        db      E_EGAMoveRight-_EGAMoveRight_
_EGAMoveRight_:                 ; move right 1 pixel
        ror     ch,1
        adc     _edi,0
E_EGAMoveRight:
        ret

;=========================================================================
;
;   Plotting primitive
;
;   Input       ES:_EDI     screen memory
;            V1 AX          colour
;            V2 DX:AX/EAX   colour
;               CH          mask
;
;=========================================================================

        db      E_EGARep-_EGARep_
_EGARep_:
        push    _edx
        mov     cl,al           ; copy colour
        mov     al,EGAMASK
        mov     _edx,GRADDR
        mov     ah,ch           ; mask
        out     dx,ax           ; set the set/reset register to colour
        mov     al,cl           ; get colour back
        xchg    cl,es:[_edi]     ; read to latch data, output data
        pop     _edx
E_EGARep:
        ret

;=========================================================================
;
;   GetDot routines
;
;   Input       ES:_EDI     screen memory
;               CL          bit position
;
;   Output   V1 AX          colour of pixel at location
;            V2 DX:AX/EAX   colour of pixel at location
;
;=========================================================================

_EGAGetDotMono_:        ; for mono ega cards with > 64K
ifdef VERSION2
ifdef __386__
        xor     eax,eax
else
        xor     dx,dx
endif
endif
        push    _ebx
        mov     _ebx,0202h       ; bh = first bit plane, bl = increment
        jmp     GetDotCommon

_EGAGetDotEO_:          ; even/odd modes
ifdef VERSION2
ifdef __386__
        xor     eax,eax
else
        xor     dx,dx
endif
endif
        push    _ebx
        mov     _ebx,0202h       ; bh = first bit plane, bl = increment
        test    _edi,1           ; if odd plane
        _if     ne              ; then
          inc     bh            ; - use planes 3 & 1 instead of 2 & 0
        _endif                  ; endif
        jmp     GetDotCommon

_EGAGetDot_:
ifdef VERSION2
ifdef __386__
        xor     eax,eax
else
        xor     dx,dx
endif
endif
        push    _ebx
        mov     _ebx,0301h       ; bh = first bit plane, bl = increment
GetDotCommon:
        push    _edx             ; save dx
        mov     _edx,GRADDR      ; load graphics register
        mov     ch,80h          ; prepare bit mask
        shr     ch,cl           ; . . .
        mov     al,RMSEL        ; select read map select register
        out     dx,al           ; . . .
        inc     _edx             ; point to data register

        xor     cl,cl           ; colour = 0
        mov     al,bh           ; plane = first bit plane
        _loop                   ; loop
          shl     cl,1          ; - move colour bit over
          out     dx,al         ; - select plane
          test    ch,es:[_edi]   ; - if bit is on in byte
          _if     ne            ; - then
            or      cl,1        ; - - set bit in pixel
          _endif                ; - endif
          sub     al,bl         ; - plane -= plane increment
        _until  l               ; until( plane < 0 )

        mov     al,cl           ; get colour in AX
        xor     ah,ah           ; clear high byte
        pop     _edx             ; restore dx
        pop     _ebx             ; and bx
        ret

;=========================================================================
;
;   PixCopy routines
;
;   Input       ES:_EDI,DH  screen memory
;               SI:_EAX,DL  buffer to copy from
;               CX          number of pixels to copy
;
;=========================================================================

_EGAPixCopyEO_:
        push    _ebp             ; save registers
        push    ds
        docall  pcopy_common
        mov     ah,0100B        ; plane mask
        _loop
          mov     al,ah
          test    _edi,1         ; if odd address, shift mask left 1
          _if     ne
            shl     al,1
          _endif
          push    _ecx           ; save offsets
          push    _ebp           ; put count on stack
          push    _edi           ; put dest addr on stack
          push    _esi           ; put scr addr on stack
          docall  start_put
          mov     ch,dh
          _loop                 ; main loop transferring full bytes
            mov     al,ah       ; get mask
            test    _edi,1       ; if odd address, shift mask over 1
            _if     ne
              shl     al,1
            _endif
            mov     _edx,SEQADDR+1
            out     dx,al       ; set bit mask reg
            mov     dh,ch
            mov     dl,[_esi]    ; get next byte
            inc     _esi         ; increment through source buffer
            mov     ch,dl
            shl     _edx,cl      ; do bit offset
            sub     _ebp,8       ; 8 pixels/byte
            cmp     _ebp,8
            _quif   l
            xchg    dh,es:[_edi] ; write out byte (need to xchg to load latches)
            inc     _edi         ; increment through screen memory
          _endloop
          docall  finish_put
          pop     _esi           ; restore src
          add     _esi,_ebx       ; add offset to next plane
          pop     _edi           ; restore destination
          pop     _ebp           ; restore count
          pop     _ecx           ; restore offsets
          shr     ah,1          ; next bit plane
          shr     ah,1
        _until  e
done_pixcopy:
        mov     _edx,SEQADDR     ; set map mask register to all planes
        mov     ax,0Fh*256+MAPMASK
        out     dx,ax
        pop     ds
        pop     _ebp
        ret

_EGAPixCopyMono_:
        push    _ebp             ; save registers
        push    ds
        docall  pcopy_common
        mov     al,0100B        ; set map mask to start at bit plane 2
        docall  put_bitplane
        mov     al,0001B        ; then do bit plane 0
        docall  put_bitplane
        jmp     done_pixcopy

_EGAPixCopy_:
        push    _ebp             ; save registers
        push    ds
        docall  pcopy_common
        mov     ah,1000B        ; set map mask to start at bit plane 3
        _loop
          mov     al,ah
          docall  put_bitplane
          shr     ah,1          ; next bit plane
        _until  c
        jmp     done_pixcopy

pcopy_common:
        mov     ds,si           ; get source in ds:si
        mov     _esi,_eax
        mov     _ebp,_ecx         ; save count
        mov     cl,dh           ; move dest bit offset
        mov     ch,dl           ; move src bit offset
        mov     _edx,GRADDR      ; get graphics address
        mov     ax,(READ_MODE_0+WRITE_MODE_0)*256+MODEREG    ; set write mode
        out     dx,ax
        mov     al,EGAMASK
        out     dx,al           ; select mask register
        mov     _edx,SEQADDR
        mov     al,MAPMASK      ; select sequencer map mask register
        out     dx,al
        ret

put_bitplane:
        push    _ecx             ; save offsets
        push    _ebp             ; put count on stack
        push    _edi             ; put dest addr on stack
        push    _esi             ; put scr addr on stack
        docall  start_put
        cmp     cl,0
        _if     e
          mov     _ecx,_ebp
          and     _ebp,111B      ; bp mod 8
          shr     _ecx,1         ; divide count by 8 pixels per byte
          shr     _ecx,1
          shr     _ecx,1
          dec     _ecx
          jle     short finished ; quit if count is zero
          dec     _esi           ; back up 1
          cmp     word ptr ss:__PlotAct,0
          _if     e             ; if plot action is replace
            rep     movsb
          _else
transfer_bytes:
            mov     al,[_esi]
            inc     _esi
            xchg    al,es:[_edi]
            inc     _edi
            loop    transfer_bytes ; until --cx = 0
          _endif
          or      _ebp,_ebp       ; if more pixels to go
          _if     ne
            mov     dh,[_esi]    ; - load into dh for finish_put()
          _endif
        _else
          _loop                 ; main loop transferring full bytes
            mov     al,[_esi]     ; get next byte
            inc     _esi
            mov     dl,al
            shl     _edx,cl      ; do bit offset
            sub     _ebp,8       ; 8 pixels/byte
            cmp     _ebp,8
            jl      short finished
            xchg    dh,es:[_edi] ; write out byte (need to xchg to load latches)
            inc     _edi         ; increment through screen memory
            mov     dh,al
          _endloop
        _endif
finished:
        docall  finish_put
        pop     _esi             ; restore src
        add     _esi,_ebx         ; add offset to next plane
        pop     _edi             ; restore destination
        pop     _ebp             ; restore count
        pop     _ecx             ; restore offsets
        ret

start_put:
        mov     _edx,SEQADDR+1
        out     dx,al           ; set bit plane mask reg
        mov     al,0ffh         ; compose bit mask in al
        shr     al,cl           ; shift mask by dest offset
        mov     dl,cl           ; add cl to bp by moving to dx
        xor     dh,dh
        add     _ebp,_edx
        cmp     _ebp,8
        _if     l               ; if only one byte to write,
          xchg    _ecx,_ebp       ; then move # of pixels to cl and
          mov     ch,0ffh       ; build end mask in ch
          shr     ch,cl
          not     ch
          and     al,ch         ; combine end of mask with beginning
          mov     _ecx,_ebp       ; restore offsets
          mov     _ebp,8         ; write only this pixel
        _endif
        mov     _edx,GRADDR+1    ; set MASK reg
        out     dx,al
        mov     dh,[_esi]        ; get first byte
        inc     _esi             ; increment src ptr
        mov     dl,[_esi]        ; get second byte
        push    _edx             ; save 2 bytes from src
        xchg    cl,ch
        shl     _edx,cl           ; calculate bit offsets
        xchg    cl,ch
        shr     _edx,cl
        xchg    dh,es:[_edi]     ; load latches and write first byte
        inc     _edi             ; increment dest ptr
        mov     _edx,GRADDR+1    ; set full mask
        mov     al,0ffh
        out     dx,al
        pop     _edx             ; restore 2 first bytes
        xchg    cl,ch
        sub     cl,ch           ; calculate new offset
        _if     l
          add     cl,8          ; src - dest + 8
        _else
          inc     _esi           ; increment src ptr
          mov     dh,dl         ; place second byte into first
        _endif
        ret                     ; dh contains next byte

finish_put:
        or      _ebp,_ebp
        _if     ne              ; if any pixels left
          mov     _ecx,_ebp       ; then move # of pixels to cl
          mov     al,0ffh       ; and make mask for end in ah
          shr     al,cl
          not     al
          mov     ch,dh         ; move last byte to ch
          mov     dx,GRADDR+1
          out     dx,al
          xchg    ch,es:[_edi]   ; load latches and write last byte
        _endif
        ret

;=========================================================================
;
;   ReadRow routines
;
;   Input       ES:_EDI      buffer to copy into
;               SI:_EAX,DL   screen memory
;               CX          number of pixels to copy
;
;=========================================================================

_EGAReadRowEO_:
        push    _ebp             ; save registers
        push    ds
        call    get_common
        mov     ah,2            ; plane number
        _loop
          push  _ebp             ; push count
          push  _esi             ; push source
          xor   al,al
          ror   _esi,1
          adc   al,ah
          rol   _esi,1
          out   dx,al           ; out plane # to RMSEL reg
          mov   bh,[_esi]        ; get first byte
          inc   _esi
          _loop
            xor     al,al
            ror     _esi,1           ; see if on even or odd byte
            adc     al,ah
            out     dx,al           ; out plane # to RMSEL reg
            rol     _esi,1
            mov     bl,[_esi]        ; get byte from screen
            inc     _esi             ; increment source ptr
            mov     ch,bl           ; save byte
            shl     _ebx,cl
            mov     es:[_edi],bh     ; move shifted byte into buffer
            mov     bh,ch
            inc     _edi             ; increment through buffer
            sub     _ebp,8           ; number of pixels/byte
          _until    le
          pop   _esi
          pop   _ebp
          sub   ah,2
        _until  l
        pop     ds
        pop     _ebp
        ret

_EGAReadRowMono_:
        push    _ebp             ; save registers
        push    ds
        docall  get_common
        mov     al,2            ; get bit plane 2
        docall  get_bitplane
        xor     al,al           ; and bit plane 0
        docall  get_bitplane
        pop     ds
        pop     _ebp
        ret

_EGAReadRow_:
        push    _ebp             ; save registers
        push    ds
        docall  get_common
        mov     bl,3            ; start on bit plane 3
        _loop
          mov       al,bl
          docall    get_bitplane
          dec       bl
        _until   l
        pop     ds
        pop     _ebp
        ret

get_common:
        mov     ds,si           ; get source in ds:si
        mov     _esi,_eax
        mov     _ebp,_ecx         ; save count in bp
        mov     cl,dl           ; move source bit offset (i.e. which pixel)
        mov     _edx,GRADDR
        mov     al,RMSEL
        out     dx,al           ; select read map sel register
        inc     _edx             ; point to GRADDR+1
        ret

get_bitplane:
        push    _ebp             ; push count
        push    _esi             ; push source
        out     dx,al           ; out plane # to RMSEL reg
        or      cl,cl
        _if     e               ; if no offset
          mov     _ecx,_ebp
          add     _ecx,7
          shr     _ecx,1
          shr     _ecx,1
          shr     _ecx,1
          rep     movsb         ; copy cx bytes from ds:si to es:di
        _else
          mov     ah,[_esi]       ; get first byte at ds:si into ah
          inc     _esi
          _loop
            mov     al,[_esi]
            inc     _esi
            mov     bh,al
            shl     _eax,cl
            mov     es:[_edi],ah ; copy byte in al to es:di
            inc     _edi
            mov     ah,bh
            sub     _ebp,8       ; number of pixels/byte
          _until  le
        _endif
        pop     _esi
        pop     _ebp
        ret

;=========================================================================
;
;   Zap routines
;
;   Input       ES:_EDI,DH  screen memory pixel position
;            V1 AX          colour (unmasked)
;            V2 SI:AX/EAX   colour (unmasked)
;               BX          not used
;               CX          number of pixels to fill
;               DL          not used
;
;=========================================================================

_EGAZapEO_:
_EGAZapMono_:
        mov     _ebx,offset MonoTab
        xlat    cs:MonoTab

_EGAZap_:
        mov     bh,dh           ; save dest bit #
        mov     _edx,GRADDR
        or      bh,bh
        _if     ne              ; if doesn't start on byte boundary
          xchg    bh,cl         ; - save low portion of count, and get bit #
          mov     bl,80h
          shr     bl,cl         ; - shift into start bit position
          mov     cl,bh         ; - restore count
          xor     ah,ah         ; - initial mask
bitlup:                         ; - loop
            or     ah,bl        ; - - put bit in
            shr    bl,1         ; - - move to next bit
          loopnz   bitlup       ; - until count = 0 or last bit shifted out
          mov     bh,al         ; save colour
          mov     al,EGAMASK
          out     dx,ax         ; - set mask
          mov     al,bh         ; - replace colour
          xchg    bh,es:[_edi]   ; - read to latches, write first byte of colour
          inc     _edi           ; - next byte
        _endif

        mov     bh,al           ; save colour
        mov     bl,cl           ; save low portion of count
        shr     _ecx,1           ; convert count to byte count
        shr     _ecx,1
        shr     _ecx,1
        _if     ne              ; not 0 byte?
          mov     ax,0ff00h+EGAMASK  ; unmask all bytes
          out     dx,ax
          cmp     word ptr ss:__PlotAct,0
          _if     e             ; if replace
            mov     al,bh       ; get colour
            cld                 ; autoinc di
            rep     stosb       ; fill buffer until(--count=0)
          _else                 ; else xor,or,and
zap_bytes:
            mov     al,bh
            xchg    al,es:[_edi] ; here the latches must be read in
            inc     _edi
            loop    zap_bytes   ; until --cx=0
          _endif
        _endif

        and     bl,0111B        ; convert to bit count
        _if     ne              ; more bits to do
          mov     ax,0ff00h+EGAMASK
          mov     cl,bl
          shr     ah,cl
          not     ah            ; ah now contains last byte mask
          out     dx,ax         ; set it
          xchg    bh,es:[_edi]   ; - latch data, write out new data
        _endif
        mov     dh,bl           ; bit #
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

_EGAFillEO_:
_EGAFillMono_:
        push    _ebx
        mov     _ebx,offset MonoTab   ; translate colour
        xlat    cs:MonoTab
        pop     _ebx

_EGAFill_:
        push    _ebp
        mov     ah,dh           ; put the bit position in ah to free dx
        mov     bh,al           ; save colour in bh
        mov     _ebp,_ecx         ; place count in bp
        mov     _edx,GRADDR      ; load graphics register address
        or      ah,ah           ; check for byte boundary
        _if     ne              ; if doesn't start on byte boundary
          mov     cl,ah         ; - prepare to shift
          mov     al,80h        ; - initial mask
          shr     al,cl         ; - shift bit mask into place
          xor     ah,ah         ; - build mask of wanted bits in ah
          _loop                 ; - loop
            or      ah,al       ; - - put bit in
            dec     _ebp         ; - - count--
            _quif   e           ; - - quif count == 0
            shr     al,1        ; - - move to next bit
          _until  c             ; - until last bit shifted out
          mov     ch,ah         ; - save boundary bit mask in ah
          mov     al,EGAMASK
          and     ah,bl         ; - and bit mask with the fill mask
          out     dx,ax         ; - set mask
          mov     al,bh         ; - get colour
          xchg    al,es:[_edi]   ; - read to latches, write first byte of colour
          cmp     word ptr ss:__Transparent,0     ; - check for transparency
          _if     e             ; transparent mode is on
            mov     al,EGAMASK  ;
            mov     ah,ch       ; - restore mask
            not     ah          ; - - reverse the bit mask
            or      ah,bl       ; - - or it with the fill mask
            not     ah          ; - - reverse it again
            out     dx,ax       ; - - set mask
            xor     al,al       ; - - background colour
            xchg    al,es:[_edi] ; - - set the 0 bits in the fill mask to 0
          _endif
          inc   _edi             ; - next byte
        _endif

        mov     ah,bl           ; get the fill mask
        mov     _ecx,_ebp         ; get count
        shr     _ecx,1           ; convert pixel count to byte count
        shr     _ecx,1
        shr     _ecx,1
        _if     ne              ; not 0 byte?
          cmp     word ptr ss:__Transparent,0
          _if e
            push    _edi         ; save screen ptr
            push    _ecx         ; save count
            push    _eax         ; save fill mask in ah
            not     ah          ; set the 0 bits to colour 0 first
            mov     al,EGAMASK
            out     dx,ax       ; set mask for the 0 bits
fill_tbytes:
            xor     al,al       ; background color
            xchg    al,es:[_edi] ; here the latches must be read in
            inc     _edi
            loop    fill_tbytes ; until --cx=0
            pop     _eax         ; restore fill mask
            pop     _ecx         ; restore count
            pop     _edi         ; restore screen ptr
          _endif
          mov     al,EGAMASK
          out     dx,ax         ; set mask for the 1 bits
fill_bytes:
          mov     al,bh         ; get colour
          xchg    al,es:[_edi]   ; here the latches must be read in
          inc     _edi
          loop    fill_bytes    ; until --cx=0
        _endif

        and     _ebp,0111B       ; convert to bit count
        _if     ne              ; more bits to do
          mov     al,EGAMASK
          mov     _ecx,_ebp
          mov     ch,0ffh       ; build a new mask
          shr     ch,cl         ; shift the fill mask by the count
          not     ch            ; last byte mask
          and     ah,ch         ; ah now contains last fill style mask
          out     dx,ax         ; set it
          xchg    bh,es:[_edi]    ; - latch data, write out new data
          cmp     word ptr ss:__Transparent,0
          _if     e
            not     ah
            and     ah,ch
            out     dx,ax
            xor     al,al
            xchg    al,es:[_edi]
          _endif
        _endif
        pop     _ebp
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

_EGAScanLeftEO_:
ifdef VERSION2
ifndef __386__
        push    bp
        mov     bp,sp
        mov     si,[bp+4]         ; get starting byte
endif
endif
        mov     cl,dl           ; save border flag in ah into cl
        mov     _edx,GRADDR
        mov     ah,al
        mov     al,CMPREG
        out     dx,ax           ; set colour compare register
        mov     ax,(EVEN_ODD_MODE+READ_MODE_1+WRITE_MODE_2)*256+MODEREG
        out     dx,ax           ; select read mode
        mov     al,DONTCARE
        out     dx,al           ; set colour don't care reg
        inc     _edx

        mov     al,0101B        ; set don't care reg for even
        test    _edi,1           ; or odd address
        _if     ne
          shl     al,1
        _endif
        out     dx,al

        inc     _ebx
        mov     ah,es:[_edi]     ; get starting byte
        _loop
          test    ah,ch         ; test if pixel is set
          _if     e             ; bit is not set
            or      cl,cl       ; quit loop if paint while
            jne     short done_EGAEOscanleft
          _else
            or      cl,cl       ; quit loop if paint until
            je      short done_EGAEOscanleft
          _endif
          dec     _ebx           ; move to next pixel
          cmp     _esi,_ebx       ; check for viewport boundary
          _quif   ge
          rol     ch,1          ; rotate mask for new pixel
          _if     c
            dec     _edi         ; look at next byte
            not     al          ; reset register for even/odd address
            and     al,0Fh
            out     dx,al
            mov     ah,es:[_edi]
          _endif
        _endloop
done_EGAEOscanleft:
ifdef VERSION2
ifndef __386__
        pop bp
endif
endif
        ret

_EGAScanLeftMono_:
        mov     cl,0101B        ; mask for color don't care reg
        jmp     short EGAScanLeft

_EGAScanLeft_:
        mov     cl,1111B        ; want all planes in compare

EGAScanLeft:
ifdef VERSION2
ifndef __386__
        push    bp
        mov     bp,sp
        mov     si,[bp+4]         ; get starting byte
endif
endif
        push    _edx             ; save border flag in dl
        mov     _edx,GRADDR
        mov     ah,al
        mov     al,CMPREG
        out     dx,ax           ; set colour compare register
        mov     ax,(READ_MODE_1+WRITE_MODE_2)*256+MODEREG
        out     dx,ax           ; select read mode
        mov     ah,cl           ; set d_esired planes
        mov     al,DONTCARE
        out     dx,ax           ; set colour don't care reg
        pop     _eax             ; restor border flag in al
        inc     _ebx
        mov     ah,es:[_edi]     ; get starting byte
        _loop
          test    ah,ch         ; test if pixel is set
          _if     e             ; bit is not set
            or      al,al       ; quit loop if paint while
            jne     short done_EGAscanleft
          _else
            or      al,al       ; quit loop if paint until
            je      short done_EGAscanleft
          _endif
          dec     _ebx           ; move to next pixel
          cmp     _esi,_ebx       ; check for viewport boundary
          _quif   ge
          rol     ch,1          ; rotate mask for new pixel
          _if     c
            dec     _edi         ; look at next byte
            mov     ah,es:[_edi]
          _endif
        _endloop
done_EGAscanleft:
ifdef VERSION2
ifndef __386__
        pop bp
endif
endif
        ret

_EGAScanRightEO_:
ifdef VERSION2
ifndef __386__
        push    bp
        mov     bp,sp
        mov     si,[bp+4]         ; get starting byte
endif
endif
        mov     cl,dl           ; save border flag in ah into cl
        mov     _edx,GRADDR
        mov     ah,al
        mov     al,CMPREG
        out     dx,ax           ; set colour compare register
        mov     ax,(EVEN_ODD_MODE+READ_MODE_1+WRITE_MODE_2)*256+MODEREG
        out     dx,ax           ; select read mode
        mov     al,DONTCARE
        out     dx,al           ; set colour don't care reg
        inc     _edx
        mov     al,0101B        ; set don't care reg for even
        test    _edi,1           ; or odd address
        _if     ne
          shl     al,1
        _endif
        out     dx,al

        mov     al,ch           ; build an extended right mask for 1st byte
        shl     ch,1
        neg     ch              ; example : 00000100 -> 00000111
        not     ch
        _loop                   ; line up with byte boundary
          dec   _ebx
          shl   al,1
        _until  c

        xor     al,al               ; build a mask for the
        or      cl,cl               ; border flag condition
        _if     ne
          not     al
        _endif
        mov     ah,es:[_edi]         ; get starting byte
        xor     ah,al
        and     ah,ch               ; border condition true in first byte
        jne     short done_EGAEOscanright
        mov     ch,al               ; border flag condition now in ch
        xor     ah,ch               ; do this in case clipping region cuts out
        sub     _esi,8               ; we're done the first byte
        mov     cl,01010101B        ; set up the Even/Odd flag once
        test    _edi,1
        _if     ne
          shl     cl,1
        _endif
        _loop
          cmp     _esi,_ebx           ; check for clip region
          _quif   l
          add     _ebx,8             ; we're done another byte (8 pixels)
          inc     _edi               ; go to next byte on screen
          rol     cl,1              ; address goes Even/Odd/Even/Odd/...
          mov     al,cl
          and     al,00Fh           ; keep only the 4 bottom bits
          out     dx,al             ; set don't care register
          mov     ah,es:[_edi]
          cmp     ch,ah
        _until  ne
        add     _esi,8
        xor     ah,ch               ; reverse result if scan while colour
done_EGAEOscanright:
        jmp     short done_EGAscanright   ; check the last byte and return

_EGAScanRightMono_:
        mov     cl,0101B        ; mask for color don't care reg
        jmp     short EGAScanRight

_EGAScanRight_:
        mov     cl,1111B        ; want all planes in compare

EGAScanRight:
ifdef VERSION2
ifndef __386__
        push    bp
        mov     bp,sp
        mov     si,[bp+4]         ; get starting byte
endif
endif
        push    _edx              ; save border flag in dl
        mov     _edx,GRADDR
        mov     ah,al
        mov     al,CMPREG
        out     dx,ax           ; set colour compare register
        mov     ax,(READ_MODE_1+WRITE_MODE_2)*256+MODEREG
        out     dx,ax           ; select read mode
        mov     ah,cl           ; set d_esired planes
        mov     al,DONTCARE
        out     dx,ax           ; set colour don't care reg
        pop     _edx             ; restor border flag in dl
        mov     dh,ch           ; build an extended right mask for 1st byte
        shl     dh,1
        neg     dh              ; example : 00000100 -> 00000111
        not     dh
        _loop                   ; line up with byte boundary
          dec   _ebx
          shl   ch,1
        _until  c
        mov     ah,es:[_edi]     ; load first byte
        xor     al,al           ; build scan byte mask
        or      dl,dl           ; if( border_flag != 0 )
        _if     ne
          not     ax
        _endif
        and     ah,dh               ; border condition true in first byte
        jne     short done_EGAscanright
        mov     _ecx,_esi             ; convert the pixel count inside
        sub     _ecx,_ebx             ; the viewport to a byte count
        dec     _ecx                 ; in order to scan full bytes
        and     cl,0F8h
        add     _ebx,_ecx
        shr     _ecx,1
        shr     _ecx,1
        shr     _ecx,1
        jle     short done_EGAscanright   ; less than 8 pixels to scan
        inc     _edi
        repe    scasb
        shl     _ecx,1
        shl     _ecx,1
        shl     _ecx,1
        sub     _ebx,_ecx
        dec     _edi
        mov     ah,es:[_edi]
        or      dl,dl
        _if     ne
          not     ah
        _endif

done_EGAscanright:
        _loop
          shl     ah,1
          _quif   c
          cmp     _esi,_ebx
          _quif   le
          inc     _ebx
        _endloop

ifdef VERSION2
ifndef __386__
        pop bp
endif
endif
        ret

;       extrn   __LineXInc : word
;       extrn   __LineYInc : word
;       extrn   __LineYMove : word
;
;
; draw a line ( x increases each step )
; input:
;    ES:DI      pointer to screen memory
;    AL         colour mask
;    CH         bit mask
;    BX         line style
;    DX         number of pixels to draw
;    SI         decision variable

;        xdefp   _EGA_XLine_
;_EGA_XLine_:
;        push    bp
;        mov     bp,dx                   ; place count in bp
;        mov     dx,GRADDR               ; load graphics address
;        mov     cl,al                   ; save colour in cl
;        mov     al,EGAMASK              ; load mask register address
;        out     dx,al                   ; . . .
;        inc     dx                      ; point to value
;
;topx:   _loop                           ; loop ( for each point )
;          rol     bx,1                  ; if dot is to be drawn
;          _if     c                     ; then
;            mov     al,ch               ; - output the colour mask
;            out     dx,al               ; - set the set/reset register
;            mov     al,cl               ; - get colour back
;            xchg    al,es:[di]          ; - read to latch data, output data
;          _endif                        ; endif
;          dec     bp                    ; count--
;          _quif   l                     ; quif count < 0
;          ror     ch,1                  ; move right
;          adc     di,0                  ; . . .
;          sub     si,ss:__LineYInc      ; d -= dy
;          jg      topx                  ; if d <= 0
;          add     si,ss:__LineXInc      ; - d += dx
;          add     di,ss:__LineYMove     ; - y += y_inc
;        _endloop                        ; endloop
;
;        pop     bp
;        ret
;
;
;        xdefp   _EGA_YLine_
;_EGA_YLine_:
;        push    bp
;        mov     bp,dx                   ; place count in bp
;        mov     dx,GRADDR               ; load graphics address
;        mov     cl,al                   ; save colour in cl
;        mov     al,EGAMASK              ; load mask register address
;        out     dx,al                   ; . . .
;        inc     dx                      ; point to value
;
;topy:   _loop                           ; loop ( for each point )
;          rol     bx,1                  ; if dot is to be drawn
;          _if     c                     ; then
;            mov     al,ch               ; - output the colour mask
;            out     dx,al               ; - set the set/reset register
;            mov     al,cl               ; - get colour back
;            xchg    al,es:[di]          ; - read to latch data, output data
;          _endif                        ; endif
;          dec     bp                    ; --count
;          _quif   l                     ; quif count < 0
;          add     di,ss:__LineYMove     ; y += y_inc
;          sub     si,ss:__LineXInc      ; d -= dx
;          jg      topy                  ; if d <= 0
;          add     si,ss:__LineYInc      ; - d += dy
;          ror     ch,1                  ; - move right
;          adc     di,0                  ; - . . .
;        _endloop                        ; endloop
;
;        pop     bp
;        ret

        endmod egautils
        end
