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



        NAME    APILIB
        TITLE   DESQview API Interface Library

; To change compilers simply change the definition of "compiler"
; to one of the following and reassemble:

        microsoft       equ     0
        turboc          equ     1
        lattice         equ     2
        metaware        equ     3
        watcom          equ     4

        compiler        equ     watcom

; To change memory models simply change the definition of "_model"
; to one of the following and reassemble:

        _small          equ     0
        _compact        equ     1
        _medium         equ     2
        _large          equ     3
        _huge           equ     4

        _model          equ     _compact

; The remaining control variables are computed based on the COMPILER
; and MODEL variables.  They may be set manually if the default values
; are inappropriate.  The meanings of these variables are:
;
;       farcode         - use FAR procedures
;       fardata         - use FAR data pointers
;       shortnames      - use 8-character names
;       underscore      - precede external names with an underscore

farcode     equ (_model eq _medium) or (_model eq _large) or (_model eq _huge)
fardata     equ (_model eq _compact) or (_model eq _large) or (_model eq _huge)
shortnames  equ (compiler eq lattice)
underscore  equ (compiler eq microsoft) or (compiler eq turboc) or (compiler eq watcom)


; The following macros replace the PROC, ENDP, PUBLIC, and EXTRN
; pseudo-ops with versions that take the control variables into account.

emit_proc macro name,x          ; emit a PROC pseudo-op
    public name
  if farcode
    name proc far
  else
    name proc near
  endif
  endm

emit_endp macro name,x          ; emit an ENDP pseudo-op
    name endp
  endm

emit_extrn macro name,distance  ; emit an EXTRN pseudo-op
    extrn &name:&distance
  endm

emit_public macro name,x        ; emit a PUBLIC pseudo-op
    public name
  endm

add_under macro name,x,mac      ; conditionally add an underscore to the name
  if underscore
    mac _&name,x
  else
    mac name,x
  endif
  endm

make_name macro short,rest,x,mac ; pass either the short or long name
  if shortnames
    add_under short,x,mac
  else
    add_under short&rest,x,mac
  endif
  endm

function macro short,rest       ; used instead of PROC
  make_name short,rest,,emit_proc
  endm

endfunc macro short,rest        ; used instead of ENDP
  make_name short,rest,,emit_endp
  endm

ext macro short,rest            ; used instead of EXTRN
  if farcode
    make_name short,rest,FAR,emit_extrn
  else
    make_name short,rest,NEAR,emit_extrn
  endif
  endm

pub macro short,rest            ; used instead of PUBLIC
  make_name short,rest,,emit_public
  endm


; The following macros are used to define the stack frame on entry to
; an API function.  The "frame" macro accounts for the return address
; and any registers that have been pushed since the call.  The "arg"
; macro assigns offset values for each argument name according to the
; COMPILER and MODEL being used.

frame macro pushed
    if farcode
        _off = 6
    else
        _off = 4
    endif
    ifnb <pushed>
        _off = _off + pushed * 2
    endif
    endm

arg macro aname,atype
        aname = _off
    ifidn <atype>,<word>
        _off = _off + 2
    else
    ifidn <atype>,<dword>
        _off = _off + 4
    else
    ifidn <atype>,<fptr>
        _off = _off + 4
    else
    ifidn <atype>,<ptr>
      if fardata
        _off = _off + 4
      else
        _off = _off + 2
      endif
    else
    ifidn <atype>,<cptr>
      if (compiler eq lattice)
        if fardata
          _off = _off + 4
        else
          _off = _off + 2
        endif
      else
      if farcode
        _off = _off + 4
      else
        _off = _off + 2
      endif
      endif
    endif
    endif
    endif
    endif
    endif
    endm


; The "fix_long" macro is a hook to support different conventions
; for how DWORD values are returned to C.  The API functions always
; setup such values in DX:AX (Microsoft convention).  The "fix_long"
; macro is called just before a function returns to allow these
; registers to be changed.

fix_long macro
    if (compiler eq lattice)
        mov     bx,ax
        mov     ax,dx
    endif
    endm


; The following macros are used to access code and data pointers.
; They take into account the COMPILER and MODEL being used.

push_dptr macro  addr           ; push data pointer
    if fardata
        push     addr+2
    else
        push     ds
    endif
        push     addr
    endm

push_fptr macro  addr           ; push data pointer
    push         addr+2
    push         addr
    endm

push_cptr macro  addr           ; push code pointer
    if (compiler eq lattice) and (farcode) and (not fardata)
        push    di
        mov     di,addr
        push    [di+2]
        push    [di]
        pop     di
    else
    if farcode
        push    addr+2
        push    addr
    else
        push    cs
        push    addr
    endif
    endif
    endm

load_dptr macro seg,off, addr   ; load data pointer into registers
    if fardata
        mov     seg, addr+2
    else
        push    ds
        pop     seg
    endif
        mov     off, addr
    endm

load_fptr macro seg,off, addr   ; load data pointer into registers
    mov seg, addr+2
    mov off, addr
    endm

load_cptr macro seg,off, addr   ; load code pointer into registers
    if (compiler eq lattice) and (farcode) and (not fardata)
      ifdif <off>,<di>
          push  di
      endif
          mov   di,addr
          mov   seg,[di+2]
          mov   off,[di]
      ifdif <off>,<di>
          pop   di
      endif
    else
    if farcode
        mov     seg,addr+2
        mov     off,addr
    else
        push    cs
        pop     seg
        mov     off,addr
    endif
    endif
    endm

; The following macros test code and data pointers for NULL.

null_dptr macro  addr           ; test data pointer for NULL
    local L1
        cmp     word ptr  addr,0
    if fardata
        jne     L1
        cmp     word ptr  addr+2,0
L1:
    endif
    endm

null_cptr macro  addr           ; test code pointer for NULL
    local L1
        cmp     word ptr  addr,0
    if farcode
        jne     L1
        cmp     word ptr  addr+2,0
L1:
    endif
    endm


; The following conditionals select appropriate segment declarations
; for each COMPILER and MODEL.  The results are four macros (dseg, dend
; cseg, cend) that are invoked later to do the actual declarations.

if (compiler eq microsoft) or (compiler eq turboc) or (compiler eq watcom)

        dseg    macro
        DGROUP  GROUP   _DATA
                ASSUME  DS:DGROUP
        _DATA   SEGMENT WORD PUBLIC 'DATA'
                public  _dvapibuf
        _dvapibuf db    260 dup (?)
                endm

        dend    macro
        _DATA    ENDS
                endm

  if (farcode eq 0)

        cseg    macro
        _TEXT   SEGMENT BYTE PUBLIC 'CODE'
                ASSUME  CS:_TEXT
                endm

        cend    macro
        _TEXT  ENDS
                endm

  else

        cseg    macro
        API1_TEXT SEGMENT BYTE PUBLIC 'CODE'
                ASSUME  CS:API1_TEXT
                endm

        cend    macro
        API1_TEXT ENDS
                endm

    endif

else
if (compiler eq metaware)

        dseg    macro
        DGROUP  GROUP   _DATA
                ASSUME  DS:DGROUP
        _DATA   SEGMENT WORD PUBLIC 'DATA'
                public   dvapibuf
        dvapibuf db    260 dup (?)
                endm

        dend    macro
        _DATA    ENDS
                endm

  if (farcode eq 0)

        cseg    macro
        CODE    SEGMENT BYTE PUBLIC 'CODE'
        CGROUP  GROUP   CODE
                ASSUME  CS:CGROUP
                endm

        cend    macro
         CODE  ENDS
                endm

  else

        cseg    macro
        API1_TEXT SEGMENT BYTE PUBLIC 'CODE'
                ASSUME  CS:API1_TEXT
                endm

        cend    macro
        API1_TEXT ENDS
                endm

    endif

else
if (compiler eq lattice)

        dseg    macro
        DGROUP  GROUP   DATA
        DATA    SEGMENT WORD PUBLIC 'DATA'
                public  dvapibuf
        dvapibuf db     260 dup (?)
                endm

        dend    macro
        DATA    ENDS
                endm

    if (farcode eq 0) and (fardata eq 0)

        cseg    macro
        PGROUP  GROUP   PROG
        PROG    SEGMENT BYTE PUBLIC 'PROG'
                ASSUME  CS:PGROUP
                endm
        cend    macro
        PROG    ENDS
                endm

    else
    if farcode and (fardata eq 0)

        cseg    macro
        _CODE   SEGMENT BYTE PUBLIC 'CODE'
                ASSUME  CS:_CODE
                endm
        cend    macro
        _CODE   ENDS
                endm

    else
    if (farcode eq 0) and fardata

        cseg    macro
        CGROUP  GROUP   CODE
        CODE    SEGMENT BYTE PUBLIC 'CODE'
                ASSUME  CS:CGROUP
                endm
        cend    macro
        CODE    ENDS
                endm

    else
    if farcode and fardata

        cseg    macro
        _PROG   SEGMENT BYTE PUBLIC 'PROG'
                ASSUME  CS:_PROG
                endm
        cend    macro
        _PROG   ENDS
                endm

    endif
    endif
    endif
    endif

endif
endif
endif

; All the control variables and associated macros have been defined.
; Its time to generate some code!


include dvapi.inc  ; define API assembly language interfaces

; if (compiler eq watcom)
;       if farcode
;         emit_extrn  sprintf_,FAR
;       else
;         emit_extrn  sprintf_,NEAR
;       endif
; else
;       ext     sprintf,        ; declare sprintf to be external
; endif

        dseg                    ; data segment starts here
        dend                    ;   and ends here

        cseg                    ; code segment starts here

        pub     win_stre,am     ; make win_stream public

CrunDS     dw   0               ; C's DS value
apiversion dw   0               ; API version number
lockobj    dw   0,0             ; dvapibuf access semaphore

; The following functions are used internally by the C Library
; to control access to the dvapibuf buffer.

; dvlockb
    function dvlockb,
        @send   lock,cs:lockobj
        ret
    endfunc dvlockb,

; dvfreeb
    function dvfreeb,
        @send   close,cs:lockobj
        ret
    endfunc dvfreeb,


; The remainder of this module contains the API interface functions.
; Functions having similar interfaces are grouped together to allow
; as much sharing of code as possible.  The result is that any given
; function is hard to find unless you search for it using an editor.

; api_beginc
    function api_begi,nc
        @call   beginc
        xor     ax,ax
        ret
    endfunc api_begi,nc

; win_disperor
    function win_disp,eror
    frame
    arg winhan,dword
    arg buffer,ptr
    arg lbuffer,word
    arg rows,word
    arg cols,word
    arg beep,word
    arg buttons,word
        push    bp
        mov     bp,sp
        push    bx
        push    cx
        push    dx
        push    di
        push    es
        mov     dx,winhan+2[bp]
        load_dptr es,di,buffer[bp]

        mov     ax,beep[bp]             ; get beep flag
        and     ax,1
        shl     ax,1                    ; make room for button info
        shl     ax,1
        mov     bx,buttons[bp]          ; get button info
        and     bx,3
        or      ax,bx                   ; OR in beep flag
        ror     ax,1                    ; rotate to top of word
        ror     ax,1
        ror     ax,1

        mov     bx,lbuffer[bp]
        and     bx,1fffh
        or      bx,ax
        mov     cl,rows[bp]
        mov     ch,cols[bp]
        @call   disperor
        mov     ax,bx
        pop     es
        pop     di
        pop     dx
        pop     cx
        pop     bx
        pop     bp
        ret
    endfunc win_disp,eror

; api_endc
    function api_endc,
        @call   endc
        xor     ax,ax
        ret
    endfunc api_endc,

; api_freebit
    function api_free,bit
    frame
    arg mask_arg,word
        push    bp
        mov     bp,sp
        push    bx
        mov     bx,mask_arg[bp]
        @call   freebit
        pop     bx
        pop     bp
        xor     ax,ax
        ret
    endfunc api_free,bit

; api_getbit
    function api_getb,it
    frame
    arg entryp,cptr
        push    bp
        mov     bp,sp
        push    bx
        push    di
        push    es
        load_cptr es,di,entryp[bp]
        @call   getbit
        mov     ax,bx
        pop     es
        pop     di
        pop     bx
        pop     bp
        ret
    endfunc api_getb,it

; win_buffer
    function win_buff,er
    frame
    arg winhan,dword
    arg bufptr,ptr
    arg lbuf,ptr
        push    bp
        mov     bp,sp
        push    bx
        push    cx
        push    dx
        push    di
        push    es
        mov     bx,winhan+2[bp]
        @call   getbuf
        mov     ax,es                   ; return pointer to buffer
        load_dptr es,bx,bufptr[bp]
        mov     es:[bx],di
        mov     es:[bx+2],ax
        load_dptr es,bx,lbuf[bp]        ; return length of buffer
        mov     es:[bx],cx
        mov     al,dl
        xor     ah,ah
        pop     es
        pop     di
        pop     dx
        pop     cx
        pop     bx
        pop     bp
        ret
    endfunc win_buff,er

; api_kmouse
    function api_kmou,se
    frame
    arg request,word
        push    bp
        mov     bp,sp
        push    bx
        mov     ax,request[bp]
        mov     bl,2
        test    ax,ax
        jz      akm1
        dec     bl
akm1:   @call   kmouse
        xor     ax,ax
        pop     bx
        pop     bp
        ret
    endfunc api_kmou,se

; qry_kmouse
    function qry_kmou,se
    frame
    arg request,word
        push    bp
        mov     bp,sp
        push    bx
        mov     ax,request[bp]
        mov     bl,1
        test    al,al
        jnz     qkm1
        inc     bl
qkm1:   @call   kmouse
        mov     al,bl
        xor     ah,ah
        pop     bx
        pop     bp
        ret
    endfunc qry_kmou,se

; win_locate
    function win_loca,te
    frame
    arg winhan,dword
    arg row,word
    arg col,word
        push    bp
        mov     bp,sp
        push    es
        mov     es,winhan+2[bp]
        mov     bl,byte ptr row[bp]
        mov     bh,byte ptr col[bp]
        @call   locate
        mov     dx,es
        xor     ax,ax
        pop     es
        pop     bp
        fix_long
        ret
    endfunc win_loca,te

; api_pause
    function api_paus,e
        @call   pause
        xor     ax,ax
        ret
    endfunc api_paus,e

; tsk_post
    function tsk_post,
    frame
    arg winhan,dword
        push    bp
        mov     bp,sp
        push    bx
        mov     bx,winhan+2[bp]
        @call   posttask
        pop     bx
        pop     bp
        ret
    endfunc tsk_post,

; win_poswin
    function win_posw,in
    frame
    arg winhan,dword
    arg relhan,dword
    arg rmode,word
    arg rows,word
    arg cmode,word
    arg cols,word
    arg draw,word
        push    bp
        mov     bp,sp
        push    bx
        push    cx
        push    dx
        push    es
        mov     es,winhan+2[bp]
        mov     bx,relhan+2[bp]
        mov     dx,draw[bp]
        and     dx,1
        mov     cl,4
        shl     dl,cl
        mov     cx,rmode[bp]
        and     cl,3
        shl     cl,1
        shl     cl,1
        or      dl,cl
        mov     cx,cmode[bp]
        and     cl,3
        or      dl,cl
        mov     cx,rows[bp]
        mov     ax,cols[bp]
        mov     ch,al
        @call   poswin
        pop     es
        pop     dx
        pop     cx
        pop     bx
        xor     ax,ax
        pop     bp
        ret
    endfunc win_posw,in

; win_putc
    function win_putc,
    frame
    arg winhan,dword
    arg chr,word
    arg atr,word
        push    bp
        mov     bp,sp
        push    bx
        push    dx
        mov     bl,byte ptr chr[bp]
        mov     bh,byte ptr atr[bp]
        mov     dx,winhan+2[bp]
        @call   printc
        xor     ax,ax
        pop     dx
        pop     bx
        pop     bp
        ret
    endfunc win_putc,

; win_hcur
    function win_hcur,
    frame
    arg winhan,dword
        push    bp
        mov     bp,sp
        push    bx
        push    dx
        xor     bx,bx
        mov     dx,winhan+2[bp]
        @call   printc
        xor     ax,ax
        pop     dx
        pop     bx
        pop     bp
        ret
    endfunc win_hcur,

; api_setbit
    function api_setb,it
    frame
    arg mask_arg,word
        push    bp
        mov     bp,sp
        push    bx
        mov     bx,mask_arg[bp]
        @call   setbit
        xor     ax,ax
        pop     bx
        pop     bp
        ret
    endfunc api_setb,it

; api_sound
    function api_soun,d
    frame
    arg freq,word
    arg dura,word
        push    bp
        mov     bp,sp
        push    bx
        push    cx
        mov     bx,freq[bp]
        mov     cx,dura[bp]
        @call   sound
        xor     ax,ax
        pop     cx
        pop     bx
        pop     bp
        ret
    endfunc api_soun,d

; tsk_start
    function tsk_star,t
    frame
    arg winhan,dword
        push    bp
        mov     bp,sp
        push    es
        mov     es,winhan+2[bp]
        @call   start
        xor     ax,ax
        pop     es
        pop     bp
        ret
    endfunc tsk_star,t

; app_start
    function app_star,t
    frame
    arg pifb,ptr
    arg lpifb,word
        push    bp
        mov     bp,sp
        push    di
        push    es
        mov     bx,lpifb[bp]
        load_dptr es,di,pifb[bp]
        @call   newproc
        mov     dx,bx
        xor     ax,ax
        pop     es
        pop     di
        pop     bp
        fix_long
        ret
    endfunc app_star,t

; tsk_stop
    function tsk_stop,
    frame
    arg winhan,dword
        push    bp
        mov     bp,sp
        push    es
        mov     es,winhan+2[bp]
        @call   stop
        xor     ax,ax
        pop     es
        pop     bp
        ret
    endfunc tsk_stop,

; win_open
    function win_open,
        push    bx
        mov     bx,dvm_open*256+dva_tos
        jmp     short ep_han_int
    endfunc win_open,

; ptr_subfrom
    function ptr_subf,rom
        jmp     short do_key_subfrom
    endfunc ptr_subf,rom

; key_subfrom
    function key_subf,rom
do_key_subfrom:
        push    bx
        mov     bx,dvm_subfrom*256+dva_tos
        jmp     short ep_han_int
    endfunc key_subf,rom

; ptr_addto
    function ptr_addt,o
        jmp     short do_key_addto
    endfunc ptr_addt,o

; key_addto
    function key_addt,o
    frame 1
    arg keyhan,dword
    arg flags,word
do_key_addto:
        push    bx
        mov     bx,dvm_addto*256+dva_tos
ep_han_int:
        push    bp
        mov     bp,sp
        xor     ax,ax
        push    ax
        push    flags[bp]
        push    keyhan+2[bp]
        push    keyhan[bp]
        mov     ah,12h
        int     15h
        xor     ax,ax
        pop     bp
        pop     bx
        ret
    endfunc key_addt,o

; mal_erase
    function mal_eras,e
        jmp     short do_key_erase
    endfunc mal_eras,e

; ptr_erase
    function ptr_eras,e
        jmp     short do_key_erase
    endfunc ptr_eras,e

; tim_erase
    function tim_eras,e
        jmp     short do_key_erase
    endfunc tim_eras,e

; win_erase
    function win_eras,e
        jmp     short do_key_erase
    endfunc win_eras,e

; key_erase
    function key_eras,e
do_key_erase:
        push    bx
        mov     bx,dvm_erase*256+dva_tos
        jmp     short ep_han
    endfunc key_eras,e

; mal_free
    function mal_free,
        jmp     short do_key_free
    endfunc mal_free,

; pan_free
    function pan_free,
        jmp     short do_key_free
    endfunc pan_free,

; ptr_free
    function ptr_free,
        jmp     short do_key_free
    endfunc ptr_free,

; tim_free
    function tim_free,
        jmp     short do_key_free
    endfunc tim_free,

; win_free
    function win_free,
        jmp     short do_key_free
    endfunc win_free,

; tsk_free
    function tsk_free,
        jmp     short do_key_free
    endfunc tsk_free,

; app_free
    function app_free,
        jmp     short do_key_free
    endfunc app_free,

; key_free
    function key_free,
do_key_free:
        push    bx
        mov     bx,dvm_free*256+dva_tos
        jmp     short ep_han
    endfunc key_free,

; mal_close
    function mal_clos,e
        jmp     short do_key_close
    endfunc mal_clos,e

; mal_unlock
    function mal_unlo,ck
        jmp     short do_key_close
    endfunc mal_unlo,ck

; pan_close
    function pan_clos,e
        jmp     short do_key_close
    endfunc pan_clos,e

; ptr_close
    function ptr_clos,e
        jmp     short do_key_close
    endfunc ptr_clos,e

; tim_close
    function tim_clos,e
        jmp     short do_key_close
    endfunc tim_clos,e

; key_close
    function key_clos,e
    frame 1
    arg keyhan,dword
do_key_close:
        push    bx
        mov     bx,dvm_close*256+dva_tos
ep_han:
        push    bp
        mov     bp,sp
        push    keyhan+2[bp]
        push    keyhan[bp]
        mov     ah,12h
        int     15h
        xor     ax,ax
        pop     bp
        mov     dx,bx
        pop     bx
        ret
    endfunc key_clos,e

; mallock
    function mallock,
        push    bx
        mov     bx,dvm_setesc*256+dva_tos
        jmp     short ep_han
    endfunc mallock,

; tim_open
    function tim_open,
        jmp     short do_mal_open
    endfunc tim_open,

; mal_open
    function mal_open,
do_mal_open:
        push    bx
        mov     bx,dvm_open*256+dva_tos
        jmp     short ep_han
    endfunc mal_open,

; mal_lock
    function mal_lock,
        push    bx
        mov     bx,dvm_lock*256+dva_tos
        jmp     short ep_han
    endfunc mal_lock,

; obq_subfrom
    function obq_subf,rom
        push    bx
        mov     bx,dvm_subfrom*256+dva_objqme
        jmp     short ep_han
    endfunc obq_subf,rom

; win_redraw
    function win_redr,aw
        push    bx
        mov     bx,dvm_redraw*256+dva_tos
        jmp     short ep_han
    endfunc win_redr,aw

; obq_read
    function obq_read,
        mov     bx,dvm_read*256+dva_objqme
        jmp     short ep_nul_ret_dword
    endfunc obq_read,

; pan_new
    function pan_new,
        mov     bx,dvm_new*256+dva_panel
        jmp     short ep_nul_ret_dword
    endfunc pan_new,

; ptr_new
    function ptr_new,
        mov     bx,dvm_new*256+dva_pointer
        jmp     short ep_nul_ret_dword
    endfunc ptr_new,

; tim_new
    function tim_new,
        mov     bx,dvm_new*256+dva_timer
        jmp     short ep_nul_ret_dword
    endfunc tim_new,

; mal_new
    function mal_new,
        mov     bx,dvm_new*256+dva_mailbox
        jmp     short ep_nul_ret_dword
    endfunc mal_new,

; mal_me
    function mal_me,
       mov     bx,dvm_handle*256+dva_mailme
        jmp     short ep_nul_ret_dword
    endfunc mal_me,

; win_me
    function win_me,
       mov     bx,dvm_handle*256+dva_me
        jmp     short ep_nul_ret_dword
    endfunc win_me,

; tsk_me
    function tsk_me,
       mov     bx,dvm_handle*256+dva_me
        jmp     short ep_nul_ret_dword
    endfunc tsk_me,

; key_new
    function key_new,
        mov     bx,dvm_new*256+dva_keyboard
        jmp     short ep_nul_ret_dword
    endfunc key_new,

; key_me
    function key_me,
       mov     bx,dvm_handle*256+dva_keyme
ep_nul_ret_dword:
        mov     ah,12h
        int     15h
        pop     ax
        pop     dx
        fix_long
        ret
    endfunc key_me ,

; key_of
    function key_of,
   frame
    arg winhan,dword
        mov     bx,dvm_handle*256+dva_keytos
ep_dword_ret_dword:
        push    bp
        mov     bp,sp
        push    winhan+2[bp]
        push    winhan[bp]
        mov     ah,12h
        int     15h
        pop     ax
        pop     dx
        pop     bp
        fix_long
        ret
    endfunc key_of ,

; mal_of
    function mal_of,
       mov     bx,dvm_handle*256+dva_mailtos
        jmp     short ep_dword_ret_dword
    endfunc mal_of,

; tim_len
    function tim_len,
    frame
    arg timhan,dword
        mov     bx,dvm_len*256+dva_tos
        jmp     short ep_dword_ret_dword
    endfunc tim_len,

; tim_read
    function tim_read,
        mov     bx,dvm_read*256+dva_tos
        jmp     short ep_dword_ret_dword
    endfunc tim_read,

; tim_sizeof
    function tim_size,of
        mov     bx,dvm_sizeof*256+dva_tos
        jmp     short ep_dword_ret_dword
    endfunc tim_size,of

; mal_addr
    function mal_addr,
        mov     bx,dvm_addr*256+dva_tos
        jmp     short ep_dword_ret_dword
    endfunc mal_addr,

; mal_sizeof
    function mal_size,of
        jmp     short do_key_sizeof
    endfunc mal_size,of

; pan_sizeof
    function pan_size,of
        jmp     short do_key_sizeof
    endfunc pan_size,of

; ptr_sizeof
    function ptr_size,of
        jmp     short do_key_sizeof
    endfunc ptr_size,of

; win_sizeof
    function win_size,of
        jmp     short do_key_sizeof
    endfunc win_size,of

; key_sizeof
    function key_size,of
    frame 1
    arg keyhan,dword
do_key_sizeof:
        push    bx
        mov     bx,dvm_sizeof*256+dva_tos
ep_dword_ret_int:
        push    bp
        mov     bp,sp
        push    keyhan+2[bp]
        push    keyhan[bp]
        mov     ah,12h
        int     15h
        pop     ax
        pop     bx
        pop     bp
        pop     bx
        ret
    endfunc key_size,of

; mal_status
    function mal_stat,us
        jmp     short do_key_status
    endfunc mal_stat,us

; pan_status
    function pan_stat,us
        jmp     short do_key_status
    endfunc pan_stat,us

; ptr_status
    function ptr_stat,us
        jmp     short do_key_status
    endfunc ptr_stat,us

; key_status
    function key_stat,us
do_key_status:
        push    bx
        mov     bx,dvm_status*256+dva_tos
        jmp     short ep_dword_ret_int
    endfunc key_stat,us

; win_len
    function win_len,
        push    bx
        mov     bx,dvm_len*256+dva_tos
        jmp     short ep_dword_ret_int
    endfunc win_len,

; ptr_open
    function ptr_open,
        jmp     short do_key_open
    endfunc ptr_open,

; key_open
    function key_open,
    frame
    arg keyhan,dword
    arg winhan,dword
do_key_open:
        push    bp
        mov     bp,sp
        push    bx
        push    winhan+2[bp]
        push    winhan[bp]
        @send   open,keyhan[bp]
        xor     ax,ax
        pop     bx
        pop     bp
        ret
    endfunc key_open,

; mal_read
    function mal_read,
    frame
    arg malhan,dword
    arg bufptr,fptr
    arg lbuf,fptr
        push    bp
        mov     bp,sp
        push    bx
        push    es
        @send   read,malhan[bp]
        load_fptr es,bx,lbuf[bp]
        pop     es:[bx]
        pop     ax
        load_fptr es,bx,bufptr[bp]
        pop     es:[bx]
        pop     es:[bx+2]
        @send   status,malhan[bp]
        pop     ax
        pop     bx
        pop     es
        pop     bx
        pop     bp
        ret
    endfunc mal_read,

; ptr_read
    function ptr_read,
        jmp     short do_key_read
    endfunc ptr_read,

; win_read
    function win_read,
        jmp     short do_key_read
    endfunc win_read,

; key_read
    function key_read,
    frame 1
    arg keyhan,dword
    arg bufptr,ptr
    arg lbuf,ptr
do_key_read:
        push    bx
        mov     bx,dvm_read*256+dva_tos
ep_dword_set_dword_int:
        push    bp
        mov     bp,sp
        push    es
        push    keyhan+2[bp]
        push    keyhan[bp]
        mov     ah,12h
        int     15h
        load_dptr es,bx,lbuf[bp]
        pop     es:[bx]
        pop     ax
        load_dptr es,bx,bufptr[bp]
        pop     es:[bx]
        pop     es:[bx+2]
        xor     ax,ax
        pop     es
        pop     bp
        pop     bx
        ret
    endfunc key_read,

; pan_dir
    function pan_dir,
        push    bx
        mov     bx,dvm_dir*256+dva_tos
        jmp     short ep_dword_set_dword_int
    endfunc pan_dir,

; key_getc
    function key_getc,
    frame
    arg keyhan,dword
        push    bp
        mov     bp,sp
        push    bx
        push    es
        @send   read,keyhan[bp]         ; wait for input
        pop     bx                      ; ignore length - should be 1
        pop     bx
        pop     bx                      ; get buffer address
        pop     es
        xor     ah,ah
        mov     al,es:[bx]              ; get character code
        test    al,al                   ; extended code ?
        jnz     kgc1                    ; jump if not
        @send   status,keyhan[bp]       ; get extended key code
        pop     ax
        pop     bx
        add     ax,256                  ; add 256 to extended code
kgc1:   pop     es                      ; restore registers
        pop     bx
        pop     bp
        ret                             ; return
    endfunc key_getc,

; key_setesc
    function key_sete,sc
    frame
    arg keyhan,dword
    arg entryp,cptr
        push    bp
        mov     bp,sp
        push    bx
        push_cptr entryp[bp]
        @send   setesc,keyhan[bp]
        xor     ax,ax
        pop     bx
        pop     bp
        ret
    endfunc key_sete,sc

; mal_addto
    function mal_addt,o
        push    bx
        mov     bx,dvm_addto*256+dva_tos
        jmp     short ep_dword_ptr_int_int
    endfunc mal_addt,o

; mal_subfrom
    function mal_subf,rom
        push    bx
        mov     bx,dvm_subfrom*256+dva_tos
        jmp     short ep_dword_ptr_int_int
    endfunc mal_subf,rom

; key_write
    function key_writ,e
    frame 1
    arg keyhan,dword
    arg bufptr,ptr
    arg lbuf,word
    arg stat,word
        push    bx
        mov     bx,dvm_write*256+dva_tos
ep_dword_ptr_int_int:
        push    bp
        mov     bp,sp
        push_dptr  bufptr[bp]
        xor     ax,ax
        push    ax
        push    lbuf[bp]
        push    ax
        push    stat[bp]
        push    keyhan+2[bp]
        push    keyhan[bp]
        mov     ah,12h
        int     15h
        xor     ax,ax
        pop     bp
        pop     bx
        ret
    endfunc key_writ,e

; win_subfrom
    function win_subf,rom
        push    bx
        mov     bx,dvm_subfrom*256+dva_tos
        jmp     short ep_dword_fptr_int
    endfunc win_subf,rom

; mal_name
    function mal_name,
        push    bx
        mov     bx,dvm_setscale*256+dva_tos
        jmp     short ep_dword_fptr_int
    endfunc mal_name,

; mal_write
    function mal_writ,e
    frame 1
    arg malhan,dword
    arg bufptr,fptr
    arg lbuf,word
        push    bx
        mov     bx,dvm_write*256+dva_tos
ep_dword_fptr_int:
        push    bp
        mov     bp,sp
        push_fptr  bufptr[bp]
        xor     ax,ax
        push    ax
        push    lbuf[bp]
        push    malhan+2[bp]
        push    malhan[bp]
        mov     ah,12h
        int     15h
        xor     ax,ax
        pop     bp
        pop     bx
        ret
    endfunc mal_writ,e

; pan_open
    function pan_open,
    frame 1
    arg panhan,dword
    arg bufptr,ptr
    arg lbuf,word
        push    bx
        push    bp
        mov     bp,sp
        push_dptr  bufptr[bp]
        xor     ax,ax
        push    ax
        push    lbuf[bp]
        @send   open,panhan[bp]
        @send   status,panhan[bp]
        pop     ax
        pop     bx
        pop     bp
        pop     bx
        ret
    endfunc pan_open,

; obq_close
    function obq_clos,e
        push    bx
        @send   close,objqme
        xor     ax,ax
        pop     bx
        ret
    endfunc obq_clos,e

; obq_erase
    function obq_eras,e
        push    bx
        @send   erase,objqme
        xor     ax,ax
        pop     bx
        ret
    endfunc obq_eras,e

; obq_open
    function obq_open,
        push    bx
        @send   open,objqme
        xor     ax,ax
        pop     bx
        ret
    endfunc obq_open,

; obq_sizeof
    function obq_size,of
        push    bx
        @send   sizeof,objqme
        pop     ax
        pop     bx
        pop     bx
        ret
    endfunc obq_size,of

; obq_status
    function obq_stat,us
        push    bx
        @send   status,objqme
        pop     bx
        pop     ax
        rol     ax,1
        xor     ax,1
        pop     bx
        ret
    endfunc obq_stat,us

; pan_apply
    function pan_appl,y
    frame
    arg panhan,dword
    arg winhan,dword
    arg namptr,ptr
    arg lname,word
    arg winptr,ptr
    arg keyptr,ptr
        push    bp
        mov     bp,sp
        push    bx
        push    es
        push_dptr  namptr[bp]
        xor     ax,ax
        push    ax
        push    lname[bp]
        push    winhan+2[bp]
        push    winhan[bp]
        @send   apply,panhan[bp]
        load_dptr es,bx,winptr[bp]
        pop     es:[bx]
        pop     es:[bx+2]
        load_dptr es,bx,keyptr[bp]
        pop     es:[bx]
        pop     es:[bx+2]
        @send   status,panhan[bp]
        pop     ax
        pop     bx
        pop     es
        pop     bx
        pop     bp
        ret
    endfunc pan_appl,y

; ptr_getscale
    function ptr_gets,cale
    frame
    arg ptrhan,dword
    arg rowptr,ptr
    arg colptr,ptr
        push    bp
        mov     bp,sp
        push    bx
        push    es
        @send   getscale,ptrhan[bp]
        load_dptr es,bx,colptr[bp]
        pop     es:[bx]
        pop     ax
        load_dptr es,bx,rowptr[bp]
        pop     es:[bx]
        pop     ax
        xor     ax,ax
        pop     es
        pop     bx
        pop     bp
        ret
    endfunc ptr_gets,cale

; ptr_setscale
    function ptr_sets,cale
    frame 1
    arg ptrhan,dword
    arg row,word
    arg col,word
        push    bx
        mov     bx,dvm_setscale*256+dva_tos
ep_dword_int_int:
        push    bp
        mov     bp,sp
        xor     ax,ax
        push    ax
        push    row[bp]
        push    ax
        push    col[bp]
        push    ptrhan+2[bp]
        push    ptrhan[bp]
        mov     ah,12h
        int     15h
        xor     ax,ax
        pop     bp
        pop     bx
        ret
    endfunc ptr_sets,cale

; ptr_write
    function ptr_writ,e
        push    bx
        mov     bx,dvm_write*256+dva_tos
        jmp     short ep_dword_int_int
    endfunc ptr_writ,e

; win_cursor
    function win_curs,or
        arg winhan,dword
        arg row,word
        arg col,word
        push    bx
        mov     bx,dvm_at*256+dva_tos
        jmp     short ep_dword_int_int
    endfunc win_curs,or

; tim_addto
    function tim_addt,o
    frame 1
    arg timhan,dword
    arg val,dword
        push    bx
        mov     bx,dvm_addto*256+dva_tos
ep_dword_dword:
        push    bp
        mov     bp,sp
        push    val+2[bp]
        push    val[bp]
        push    timhan+2[bp]
        push    timhan[bp]
        mov     ah,12h
        int     15h
        xor     ax,ax
        pop     bp
        pop     bx
        ret
    endfunc tim_addt,o

; tim_write
    function tim_writ,e
        arg winhan, dword
        arg time, dword
        push    bx
        mov     bx,dvm_write*256+dva_tos
        jmp     short ep_dword_dword
    endfunc tim_writ,e

; tim_status
    function tim_stat,us
    frame
    arg timhan,dword
        push    bp
        mov     bp,sp
        push    bx
        @send   status,timhan[bp]
        pop     bx
        pop     ax
        rol     ax,1
        rol     ax,1
        pop     bx
        pop     bp
        ret
    endfunc tim_stat,us

; win_addto
    function win_addt,o
    frame
    arg winhan,dword
    arg charptr,ptr
    arg nchars,word
    arg attrptr,ptr
    arg nattrs,word
        push    bp
        mov     bp,sp
        push    bx
        push_dptr  charptr[bp]
        xor     ax,ax
        push    ax
        push    nchars[bp]
        push_dptr  attrptr[bp]
        push    ax
        push    nattrs[bp]
        @send   addto,winhan[bp]
        pop     bx
        pop     bp
        ret
    endfunc win_addt,o

; win_eof
    function win_eof,
    frame
    arg winhan,dword
        push    bp
        mov     bp,sp
        push    bx
        @send   eof,winhan[bp]
        pop     ax
        pop     bx
        pop     bx
        pop     bp
        ret
    endfunc win_eof,

register_stream db      1bh,0,2,0,0d5h,0e4h,1bh,10h,0,0

register proc
        push    ax
        push    bx
        push    cx
        push    cs
        mov     cx,offset cs:register_stream
        push    cx
        xor     cx,cx
        push    cx
        mov     cx,10
        push    cx
        push    ax
        push    bx
        @send   write,tos
        pop     cx
        pop     bx
        pop     ax
        ret
register endp

; app_new
    function app_new,
        mov     bl,1
        jmp     short tsn1
    endfunc app_new,

; tsk_new
    function tsk_new,
    frame 1
    arg entryp,cptr
    arg stk,ptr
    arg lstack,word
    arg titlptr,ptr
    arg ltitle,word
    arg rows,word
    arg cols,word
        xor     bl,bl
tsn1:   push    bx
        push    bp
        mov     bp,sp
        push    cx
        push    di
        push    es
        push_dptr  titlptr[bp]           ; push pointer to title string
        xor     ax,ax                   ; push title length
        push    ax
        push    ltitle[bp]
        push    ax                      ; push # rows in window
        push    rows[bp]
        push    ax                      ; push # columns in window
        push    cols[bp]
        push    ax                      ; no input buffer needed
        push    ax

;
;  This is the path that spawns a subtask.  There is no way to spawn a task
;  with an initial set of registers or on a given stack.  We therefore spawn
;  the task to a intermediate routine called task_starter and send the
;  requested stack buffer to the new task as a mailbox message.  In the stack
;  buffer we place the initial DS and ES values and the requested entry
;  point.  Task_starter simply reads its mailbox to find the stack, sets
;  up things accordingly, and calls the users entry point.

wn1:    xor     ax,ax                   ; 128 byte system data stack
        push    ax
        mov     bx,128
        push    bx
        mov     bx,-1                   ; default return stack = -1
        push    bx
        push    bx
        push    cs                      ;  address of task_starter routine
        mov     bx,offset cs:task_starter
        push    bx
        @send   new,window              ; spawn subtask to execute task_starter
        cmp     byte ptr [bp+2],0       ; owner task ?
        jnz     wn2                     ; jump if so
        pop     bx
        pop     ax
        push    ax
        push    bx
        call    register                ; register subtask

wn2:    mov     ax,es                   ; save system ES
        load_dptr es,di,stk[bp]         ; get user stack buffer
        add     di,lstack[bp]
        mov     es:[di-2],cs            ; push task_ender onto user stack
        mov     es:[di-4],offset cs:task_ender
        load_cptr dx,cx,entryp[bp]      ; push task entry point
        mov     es:[di-6],dx
        mov     es:[di-8],cx
        mov     es:[di-10],ds           ; push initial ds
        mov     es:[di-12],ax           ; push initial es
        @send   handle,me               ; get parents handle
        pop     es:[di-16]              ; push parents handle
        pop     es:[di-14]
        mov     ax,[bp+2]               ; push owner task flag flag
        mov     es:[di-18],ax
        mov     bx,lstack[bp]           ; get stack length
        sub     di,bx
        pop     cx                      ; pop new task handle into dx:cx
        pop     dx
        push    es                      ; push pointer to stack
        push    di
        xor     ax,ax                   ; push length of stack
        push    ax
        push    bx
        push    ax                      ; status = 0
        push    ax
        push    dx                      ; push new task handle
        push    cx
        @send   subfrom,mailtos         ; send mail by reference
        mov     ax,cx                   ; return new tasks handle
        pop     es
        pop     di
        pop     cx
        pop     bp
        add     sp,2
        fix_long
        ret
    endfunc tsk_new,

task_starter proc far
        @send   read,mailme             ; read mailme
        pop     cx                      ; pop stack length
        pop     ax
        pop     bx                      ; pop stack base
        pop     ax
        add     bx,cx
        sub     bx,18
        cli
        mov     ss,ax
        mov     sp,bx
        sti
        @call   ostack                  ; trick DESQview into going off stack
        @call   ustack
        pop     bx                      ; pop owner task flag
        test    bl,bl
        jz      tst1                    ; jump if not owner task
        @send   handle,me               ; get task handle
        pop     bx
        pop     ax
        call    register                ; register self
tst1:   pop     cx                      ; pop parents handle
        pop     dx
        pop     es                      ; pop ES
        pop     ds                      ; pop DS
        ret                             ; return to subtask
task_starter endp

task_ender proc
        @send   free,me
task_ender endp

; win_new
    function win_new,
    frame
    arg titlptr,ptr
    arg ltitle,word
    arg rows,word
    arg cols,word
        push    bp
        mov     bp,sp
        push    cx
        push    di
        push    es
        push_dptr  titlptr[bp]           ; push pointer to title string
        xor     ax,ax                   ; push title length
        push    ax
        push    ltitle[bp]
        push    ax                      ; push # rows in window
        push    rows[bp]
        push    ax                      ; push # columns in window
        push    cols[bp]
        push    ax                      ; no input buffer needed
        push    ax
        push    ax                      ; no data stack
        push    ax
        push    ax                      ; no rturn stack
        push    ax
        push    ax                      ; no entry point
        push    ax
wn4:    @send   new,window              ; create it
        pop     bx                      ;get segment ptr
        pop     ax                      ;get ip ptr
        call    register                ; register it
        mov     dx,ax
        mov     ax,bx
        pop     es
        pop     di
        pop     cx
        pop     bp
        fix_long
        ret
    endfunc win_new,

; win_nread
   function win_nrea,d
   frame
   arg winhan,dword
   arg nchars,word
   arg bufptr,ptr
   arg lbuf,ptr
        push    bp
        mov     bp,sp
        push    bx
        push    es
        xor     ax,ax
        push    ax
        push    nchars[bp]
        @send   readn,winhan[bp]
        load_dptr es,bx,lbuf[bp]
        pop     es:[bx]
        pop     ax
        load_dptr es,bx,bufptr[bp]
        pop     es:[bx]
        pop     es:[bx+2]
        xor     ax,ax
        pop     es
        pop     bx
        pop     bp
        ret
    endfunc win_nrea,d

; win_stream
    function win_stre,am
    frame
    arg winhan,dword
    arg strptr,ptr
        push    bp
        mov     bp,sp
        push    es
        mov     dx,sp
        load_dptr es,bx,strptr[bp]
        push    es
        push    bx
        mov     bx,es:[bx+2]
        add     bx,4
        xor     ax,ax
        push    ax
        push    bx
        push    winhan+2[bp]
        push    winhan[bp]
        jmp     short do_win_write
    endfunc win_stre,am

; win_write
    function win_writ,e
    frame
    arg winhan,dword
    arg bufptr,ptr
    arg lbuf,word
        push    bp
        mov     bp,sp
        push    es
        mov     dx,sp
        push_dptr  bufptr[bp]
        xor     ax,ax
        push    ax
        push    lbuf[bp]
        push    winhan+2[bp]
        push    winhan[bp]
do_win_write:
        @send   write,tos
        mov     ax,sp
        cmp     dx,ax
        je      wst1
        pop     ax
        pop     dx
        jmp     short wst2
wst1:   xor     ax,ax
        xor     dx,dx
wst2:   pop     es
        pop     bp
        fix_long
        ret
    endfunc win_writ,e

; api_shadow
    function api_shad,ow
        push    es
        xor     ax,ax
        mov     es,ax
        @call   shadow
        mov     dx,es
        xor     ax,ax
        pop     es
        fix_long
        ret
    endfunc api_shad,ow

; api_update
    function api_upda,te
    frame
    arg vptr,dword
    arg count,word
        push    bp
        mov     bp,sp
        push    cx
        push    di
        push    es
        les     di,vptr[bp]
        mov     cx,count[bp]
        @call   update
        xor     ax,ax
        pop     es
        pop     di
        pop     cx
        pop     bp
        ret
    endfunc api_upda,te

; api_getmem
    function api_getm,em
    frame
    arg count,word
        push    bp
        mov     bp,sp
        push    di
        push    es
        mov     bx,count[bp]
        @call   getmem
        mov     dx,es
        mov     ax,di
        pop     es
        pop     di
        pop     bp
        fix_long
        ret
    endfunc api_getm,em

; api_putmem
    function api_putm,em
    frame
    arg memptr,dword
        push    bp
        mov     bp,sp
        push    es
        mov     es,word ptr memptr+2[bp]
        @call   putmem
        xor     ax,ax
        pop     es
        pop     bp
        ret
    endfunc api_putm,em

; api_isobj
    function api_isob,j
    frame
    arg obj,dword
        push    bp
        mov     bp,sp
        push    bx
        push    di
        push    es
        les     di,dword ptr obj[bp]
        @call   isobj
        mov     ax,bx
        neg     ax
        pop     es
        pop     di
        pop     bx
        pop     bp
        ret
    endfunc api_isob,j

; api_justify
    function api_just,ify
    frame
    arg val,word
        push    bp
        mov     bp,sp
        push    bx
        mov     bx,val[bp]
        @call   justify
        xor     ax,ax
        pop     bx
        pop     bp
        ret
    endfunc api_just,ify

; api_level
    function api_leve,l
    frame
    arg level,word
        push    bp
        mov     bp,sp
        push    bx
        mov     bx,level[bp]
        @call   apilevel
        pop     bx
        pop     bp
        ret
    endfunc api_leve,l

; api_poke
    function api_poke,
    frame
    arg val,word
        push    bp
        mov     bp,sp
        push    bx
        mov     bx,val[bp]
        @call   dbgpoke
        pop     bx
        pop     bp
        ret
    endfunc api_poke,

init_stream db  1bh,10h,2,0,0bdh,0bfh

; api_init
    function api_init,
        mov     cs:CrunDS,ds            ; save C runtime DS
        @call   dvpresent
        mov     cs:apiversion,ax        ; save API version number
        test    ax,ax
        jz      apr1
        cmp     ax,200h                 ; version 2 ?
        jb      apr1                    ; jump if not
        push    ax                      ; save version number
        push    bx
        cmp     ax,201h                 ; version 2.01 or later ?
        jb      apr2                    ; jump if not
        push    es
        @send   handle,me               ; get task handle
        pop     ax
        pop     es
        mov     bl,1
        @call   cstyle                  ; use C style control codes
        pop     es
apr2:   mov     bx,200h                 ; enable level 2 extensions
        @call   apilevel
        push    cs                      ; set alternate field mode
        mov     ax,offset cs:init_stream ;  and allow changing of
        push    ax                      ;   reverse attributes
        xor     ax,ax
        push    ax
        mov     ax,6
        push    ax
        @send   write,me
        @send   new,mailbox             ; allocate mailbox object
        @pop    cs:lockobj              ;   as dvapibuf semaphore
        pop     bx
        pop     ax                      ; return DESQview version
apr1:   ret
    endfunc api_init,

exit_stream db  1bh,10h,2,0,0bch,0beh

; api_exit
    function api_exit,
        push    bx
        cmp     cs:apiversion,201h
        jb      aex2
        push    es
        @send   handle,me               ; get task handle
        pop     ax
        pop     es
        xor     bl,bl
        @call   cstyle                  ; use normal control codes
        pop     es
aex2:   push    cs                      ; set normal field mode
        mov     ax,offset cs:exit_stream ;  and disallow changing of
        push    ax                      ;   reverse attributes
        xor     ax,ax
        push    ax
        mov     ax,6
        push    ax
        @send   write,me
        @mov    bxax,cs:lockobj
        or      ax,bx
        jz      aex1
        @send   free,cs:lockobj         ; free dvapibuf semaphore
        xor     ax,ax
        mov     bx,ax
        @mov    cs:lockobj,bxax
aex1:   pop     bx
        xor     ax,ax
        ret
    endfunc api_exit,

; ; api_pushkey
;     function api_push,key
;     frame
;     arg keyx,word
;       push    bp
;       mov     bp,sp
;       push    bx
;       push    di
;       push    es
;       mov     bx,keyx[bp]
;       @call   pushkey
;       pop     es
;       pop     di
;       pop     bx
;       pop     bp
;       ret
;     endfunc api_push,key


wpf_cs  dw      0
wpf_ip  dw      0
wpf_bp  dw      0
wpf_win dw      0,0

; ; win_printf
;     function win_prin,tf
;     frame
;     arg win,dword
;       @send   lock,cs:lockobj
;       pop     cs:wpf_ip
;     if farcode
;       pop     cs:wpf_cs
;     endif
;       mov     cs:wpf_bp,bp
;       mov     bp,sp
;       pop     cs:wpf_win
;       pop     cs:wpf_win+2
;
;     if underscore
;       if fardata
;       mov     ax, seg DGROUP:_dvapibuf
;       push    ax
;       endif
;       mov     ax,offset DGROUP:_dvapibuf
;       push    ax
;     else
;       if fardata
;       mov     ax, seg DGROUP:dvapibuf
;       push    ax
;       endif
;       mov     ax,offset DGROUP:dvapibuf
;       push    ax
;     endif
;
;     if farcode
;       push    cs
;     endif
;       mov     ax,offset cs:wpf_part2
;       push    ax
;
;     if (compiler eq metaware)
;       if farcode
;       jmp     sprintf
;       else
;       jmp     cgroup:sprintf
;       endif
;     else
;     if (compiler eq watcom)
;       jmp     sprintf_
;     else
;       if underscore
;       jmp     _sprintf
;       else
;       jmp     sprintf
;       endif
;     endif
;     endif
;
;
; wpf_part2:
;       mov     sp,bp
;     if farcode
;       push    cs:wpf_cs
;     endif
;       push    cs:wpf_ip
;       push    bx
;       push    cx
;       mov     cx,ax
;
;     if underscore
;       mov     ax,seg DGROUP:_dvapibuf
;       push    ax
;       mov     ax,offset DGROUP:_dvapibuf
;       push    ax
;     else
;       mov     ax,seg DGROUP:dvapibuf
;       push    ax
;       mov     ax,offset DGROUP:dvapibuf
;       push    ax
;     endif
;
;       xor     ax,ax
;       push    ax
;       push    cx
;       @send   write,cs:wpf_win
;       mov     bp,cs:wpf_bp
;       @send   close,cs:lockobj
;       pop     cx
;       pop     bx
;       ret
;     endfunc win_prin,tf

; app_number
    function app_numb,er
        @call   appnum
        ret
    endfunc app_numb,er

; mal_find
    function mal_find,
    frame
    arg name_ptr,fptr
    arg lname,word
        push    bp
        mov     bp,sp
        push    bx
        push    cx
        push    di
        push    es
        push    si
        push    ds
        load_fptr es,di,name_ptr[bp]
        mov     cx,lname[bp]
        @call   findmail
        test    bx,bx
        jnz     mf1
        xor     ax,ax
        xor     dx,dx
        jmp     short mf2
mf1:    @send   handle,dssi
        pop     ax
        pop     dx
mf2:    pop     ds
        pop     si
        pop     es
        pop     di
        pop     cx
        pop     bx
        pop     bp
        fix_long
        ret
    endfunc mal_find,

; win_async
    function win_asyn,c
    frame
    arg win,dword
    arg entryp,cptr
        push    bp
        mov     bp,sp
        sub     sp,14
        push    bx
        push    cx
        push    dx
        mov     word ptr [bp-14],101bh
        mov     word ptr [bp-12],10
        mov     byte ptr [bp-10],8ah
        null_cptr entryp[bp]
        jz      was1
        mov     ax,offset cs:async_entry
        mov     bx,cs
        load_cptr dx,cx,entryp[bp]      ; push task entry point
        jmp     short was2
was1:   xor     ax,ax
        xor     bx,bx
        xor     cx,cx
        xor     dx,dx
was2:   mov     word ptr [bp-9],ax
        mov     word ptr [bp-7],bx
        mov     byte ptr [bp-5],8bh
        mov     word ptr [bp-4],cx
        mov     word ptr [bp-2],dx
        push    ss
        lea     ax,[bp-14]
        push    ax
        xor     ax,ax
        push    ax
        mov     ax,14
        push    ax
        @send   write,win[bp]
        pop     dx
        pop     cx
        pop     bx
        mov     bx,ax
        mov     sp,bp
        pop     bp
        xor     ax,ax
        ret
    endfunc win_asyn,c

async_entry proc far
        @call   ustack                  ; back to user stack
    if farcode
        push    cs                      ; push return to here
    endif
        mov     ax,offset cs:async_ret
        push    ax
        push    ds                      ; push user entry point
        push    si
        mov     ds,cs:CrunDS            ; set user DS and ES
        mov     es,cs:CrunDS
        ret                             ; entry async routine
async_ret:
        @call   ostack                  ; back on system stack
        ret                             ; return
async_entry endp

; tsk_pgmint
    function tsk_pgmi,nt
    frame
    arg winhan,dword
    arg entryp,cptr
    arg wordparm,word
    arg ptrparm,ptr
        push    bp
        mov     bp,sp
        push    bx
        push    cx
        push    dx
        push    di
        push    si
        push    ds
        push    es
        mov     bx,winhan+2[bp]
        mov     dx,cs
        mov     cx,offset cs:pgmint_entry
        load_dptr es,di,ptrparm[bp]
        load_cptr ds,si,entryp[bp]
        mov     bp,wordparm[bp]
        @call   pgmint
        pop     es
        pop     ds
        pop     si
        pop     di
        pop     dx
        pop     cx
        pop     bx
        xor     ax,ax
        pop     bp
        ret
    endfunc tsk_pgmi,nt

pgmint_entry proc far
        @call   ustack                  ; back to user stack
    if fardata
        push    es                      ; push pointer parm
    endif
        push    di
        push    bp                      ; push integer parm
    if farcode
        push    cs                      ; push return to here
    endif
        mov     ax,offset cs:pgmint_ret
        push    ax
        push    ds                      ; push user entry point
        push    si
        mov     ds,cs:CrunDS            ; set user DS and ES
        mov     es,cs:CrunDS
        ret                             ; entry async routine
pgmint_ret:
    if fardata
        add     sp,6                    ; drop parms
    else
        add     sp,4
    endif
        @call   ostack                  ; back on system stack
        ret                             ; return
pgmint_entry endp

; ; win_reorder
;     function win_reor,der
;       push    bp
;       mov     bp,sp
;       push    bx
;       push    cx
;       push    di
;       push    es
;       @send   lock,cs:lockobj
;      if farcode                       ; adjust BP to point to parms
;       add     bp,6
;      else
;       add     bp,4
;      endif
;
;      if underscore
;       mov     ax,seg DGROUP:_dvapibuf         ; point ES:DI to _dvapibuf
;       mov     es,ax
;       mov     di,offset DGROUP:_dvapibuf
;      else
;       mov     ax,seg DGROUP:dvapibuf          ; point ES:DI to dvapibuf
;       mov     es,ax
;       mov     di,offset DGROUP:dvapibuf
;      endif
;
;       mov     cx,64                   ; limit of 64 parameters
;       add     di,cx
;       add     di,cx
;       std
;       xor     ax,ax                   ; terminate list
;       stosw
; wro2: mov     ax,[bp]                 ; get low order word of window handle
;       test    ax,ax                   ; terminator ?
;       jnz     wro1                    ; jump if so
;       mov     ax,[bp+2]               ; get high order word
;       stosw                           ; put in buffer
;       add     bp,4                    ; point to next parm
;       loop    wro2                    ; loop
;
; wro1: add     di,2                    ; point to start of list
;       push    di                      ; push pointer to list
;       sub     di,9                    ; reserve room for a stream
;       cld
;       mov     ax,101bh                ; manager stream header
;       stosw
;       mov     ax,5                    ; # of command bytes
;       stosw
;       mov     al,0ceh                 ; reorder command byte
;       stosb
;       pop     ax                      ; store window list offset
;       stosw
;       mov     ax,es                   ; store window list segment
;       stosw
;       sub     di,9
;       push    es                      ; push pointer to stream
;       push    di
;       xor     ax,ax                   ; push stream length = 9
;       push    ax
;       mov     ax,9
;       push    ax
;       @send   write,me                ; send reorder stream
;       @send   close,cs:lockobj
;       pop     es
;       pop     di
;       pop     cx
;       pop     bx
;       pop     bp
;       xor     ax,ax
;       ret
;     endfunc win_reor,der

        cend                            ; end of code segment

        END
