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


;================================================================
;       Bit operations                                          =
;       Register conventions                                    =
;               source          DS:SI, bit # in DL              =
;               destination     ES:DI, bit # in DH              =
;               bit count       CX                              =
;               bits/element    source AH, dest AL              =
;               small source    pattern in AL                   =
;               translate table BX                              =
;               small source    pattern in AL                   =
;               translate table BX                              =
;================================================================
;       All bit movement routines leave source and/or           =
;       destination registers prepared for another movement     =
;================================================================
;       Bit numbering is from left to right                     =
;       i.e. bit 0 is on the left, bit 7 is on the right        =
;       It is assumed that addresses increase to the right      =
;       and that bit value 128 is on the right                  =
;================================================================
include graph.inc
include bitmac.inc

;================================================================
;       Bit Tables                                              =
;================================================================

        modstart bit,WORD

        xdefp   BitReplace
        xdefp   BitAnd
        xdefp   BitXor
        xdefp   BitOr
        xdefp   BitCopy

if _ICON

RightTable      db 0ffh,0feh,0fch,0f8h,0f0h,0e0h,0c0h,080h
LeftTable       db 001h,003h,007h,00fh,01fh,03fh,07fh,0ffh
MaskTable       db 080h,0c0h,0e0h,0f0h,0f8h,0fch,0feh,0ffh

else

RightTable      db 0ffh,07fh,03fh,01fh,00fh,007h,003h,001h
LeftTable       db 080h,0c0h,0e0h,0f0h,0f8h,0fch,0feh,0ffh
MaskTable       db 001h,003h,007h,00fh,01fh,03fh,07fh,0ffh

endif

ifdef __386__

    DGROUP  group _DATA
    assume  ds:DGROUP,ss:DGROUP
    _DATA   segment word public 'DATA'
    BitSingle       dd BitRepSingle
    BitString       dd BitRepString
    _DATA   ends

    bit_seg     equ ss
    size_ptr    equ dword ptr

else

    BitSingle       dw BitRepSingle
    BitString       dw BitRepString

    bit_seg     equ cs
    size_ptr    equ word ptr

endif


;================================================================
;       Bit action routines                                     =
;================================================================
;       BitXXXSingle                                            =
;       al      bit pattern                                     =
;       ah      free to use                                     =
;       es:di   pointer to byte to modify                       =
;       ch      mask of bits to change                          =
;       Returns                                                 =
;       al      other bits may have been set on                 =
;       all other registers intact
;================================================================
;       BitXXXString                                            =
;       src     DS:SI,DL                                        =
;       dst     ES:DI,DH (DH = 0)                               =
;       shift   CL                                              =
;       count   BX                                              =
;================================================================
;       BitXXXZap                                               =
;       src     AL                                              =
;       dst     ES:DI,DH (DH = 0)                               =
;       count   BX                                              =
;================================================================

;       Replacement routines

BitRepSingle:
        and     al,ch           ; new_bits &= right_mask
        mov     ah,es:[_edi]     ; target &= ~ right_mask
        or      ah,ch           ; ...
        xor     ah,ch           ; ...
        or      al,ah           ; target |= new_bits
        mov     es:[_edi],al     ; dst@ := target
        ret

BitRepString:
        cmp     cl,8            ; if right_shift = 8
        _if     e
          xchg   _ecx,_ebx          ; Copy( src, dst, count )
          rep    movsb            ; ...
          xchg   _ecx,_ebx          ; ...
        _else                   ; else
          _loop                   ; loop
            lodsw                   ; new_bit := Fetchword( src++ )
            dec     _esi
            bit_shift               ; special macro
            stosb                   ; dst++@ := new_bit
            dec     _ebx             ; -- count
          _until e                ; until count = 0
        _endif                  ; endif
        ret

;       And routines

BitAndSingle:
        and     al,ch           ; new_bits &= right_mask
        mov     ah,ch           ; new_bits |= ~ mask
        not     ah              ; ...
        or      al,ah           ; ...
        and     es:[_edi],al     ; dst@ &= new_bits
        ret

BitAndString:
        _loop                   ; loop
          lodsw                   ; new_bit := Fetchword( src++ )
          dec     _esi
          bit_shift               ; special macro
          and     es:[_edi],al     ; dst++@ &= new_bit
          inc     _edi             ; ...
          dec     _ebx             ; -- count
        _until e                ; until count = 0
        ret

;       Or routines

BitOrSingle:
        and     al,ch           ; new_bits &= right_mask
        or      es:[_edi],al     ; dst@ |= new_bits
        ret

BitOrString:
        _loop                   ; loop
          lodsw                   ; new_bit := Fetchword( src++ )
          dec     _esi
          bit_shift               ; special macro
          or      es:[_edi],al     ; dst++@ |= new_bit
          inc     _edi             ; ...
          dec     _ebx             ; -- count
        _until e                ; until count = 0
        ret

;       Xor routines

BitXorSingle:
        and     al,ch           ; new_bits &= right_mask
        xor     es:[_edi],al     ; dst@ &= new_bits
        ret

BitXorString:
        _loop                   ; loop
          lodsw                   ; new_bit := Fetchword( src++ )
          dec     _esi
          bit_shift               ; special macro
          xor     es:[_edi],al     ; dst++@ xor= new_bit
          inc     _edi             ; ...
          dec     _ebx             ; -- count
        _until e                ; until count = 0
        ret

;================================================================
;       Setting up actions                                      =
;================================================================

BitReplace:
        mov     size_ptr bit_seg:BitSingle,offset BitRepSingle
        mov     size_ptr bit_seg:BitString,offset BitRepString
        ret

BitAnd:
        mov     size_ptr bit_seg:BitSingle,offset BitAndSingle
        mov     size_ptr bit_seg:BitString,offset BitAndString
        ret

BitOr:
        mov     size_ptr bit_seg:BitSingle,offset BitOrSingle
        mov     size_ptr bit_seg:BitString,offset BitOrString
        ret

BitXor:
        mov     size_ptr bit_seg:BitSingle,offset BitXorSingle
        mov     size_ptr bit_seg:BitString,offset BitXorString
        ret


;================================================================
;       BitCopy( src, dst, count )                              =
;       input : source          ES:DI, bit # in DL             =
;               destination     DS:SI, bit # in DH             =
;               bit count       CX                              =
;       uses  : translate table BX                              =
;================================================================

BitCopy:
        xor     _ebx,_ebx
        mov     bl,dh           ; right_mask := RightTable[dst.bit#]
        mov     bh,cs:RightTable[_ebx]
        sub     bl,dl           ; right_shift := (8 - src.bit#) + dst.bit#
        add     bl,8            ; (8 - src.bit#) will left align bits in al
                                ; dst.bit# will move bits to proper pos'n in al
        xchg    _ebx,_ecx         ; cl = right_shift, ch mask, bx count
        test    dh,dh           ; if dst.bit# <> 0
        _if     ne                ; copy first (8-dst.bit#) bits
          mov     ax,[_esi]        ; new_bits := Fetchword( src ) >> right_shift
          bit_shift               ; special macro
          add     bl,dh           ; count -= 8 - dst.bit#
          adc     bh,0            ; ...
          sub     _ebx,8           ; ...
          _if     b               ; if count < 0
            add     _ebx,8           ; count += 8 (count := original + dst.bit#)
            add     dl,bl           ; src.bit# += original count
            sub     dl,dh           ; i.e. src.bit# += count - dst.bit#
            cmp     dl,8            ; if src.bit# >= 8
            _if     ae              ; then
              inc     _esi             ; src ++
              sub     dl,8            ; src.bit# -= 8
            _endif
            mov     dh,bl           ; dst.bit# := dst.bit# + original count
            and     ch,cs:(LeftTable-1)[_ebx]; right_mask &= LeftTable[dst.bit#-1]
            jmp     size_ptr bit_seg:BitSingle   ; do the byte & return
          _endif                  ; endif
          add     dl,8            ; src.bit += 8 - dst.bit#
          sub     dl,dh           ; ...
          cmp     dl,8            ; if src.bit# >= 8
          _if     ae              ; then
            inc     _esi             ; src ++
            sub     dl,8            ; src.bit# -= 8
          _endif                  ; endif
          call    size_ptr bit_seg:BitSingle     ; call single byte routine
          inc     _edi             ; dst++
        _endif                  ; endif
        mov     cl,8            ; right_shift := 8 - src.bit#
        sub     cl,dl           ; ...
        mov     dh,bl           ; dst.bit# := count
        shr     _ebx,1           ; count /= 8
        shr     _ebx,1           ; ...
        shr     _ebx,1           ; ...
        _if     ne              ; if count <> 0
          call  size_ptr bit_seg:BitString   ; call string routine
        _endif                  ; endif
        and     dh,7            ; dst.bit# &= 7
        _if     ne              ; if dst.bit# <> 0
          mov     bl,dh           ; left_mask := LeftTable[dst.bit#-1]
;;;;      mov     bh,0            ; NB bh was 0 from BitString
          mov     ch,cs:(LeftTable-1)[_ebx]
          mov     ax,[_esi]        ; new_bit := FetchWord( src )
          bit_shift
          call    size_ptr bit_seg:BitSingle ; store new bits according to action
          or      dl,0f8h         ; to get carry set if need be
          add     dl,dh           ; src.bit# += dst.bit#
          adc     _esi,0           ; if src.bit# >=8 then src++
          and     dl,7            ; src.bit# &= 7
          inc     _edi             ; move to next byte
        _endif                  ; endif
        ret                     ; return

        endmod bit
        end
