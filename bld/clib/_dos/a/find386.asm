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


;
;
include mdef.inc
include struct.inc
include extender.inc
include int21.inc

        xref    __doserror_
        modstart dosfind

        datasegment
        extrn   "C",_Extender : byte
        enddata

        defp    _dos_findfirst
        if __WASM__ ge 100
            xdefp       "C",_dos_findfirst
        else
            xdefp       <"C",_dos_findfirst>
        endif
;
;       unsigned _dos_findfirst( char *path, unsigned attr, struct find_t *buf)
;       struct find_t {
;               char reserved[21];              reserved for use by DOS
;               char attribute;                 attribute byte for matched path
;               unsigned short wr_time;         time of last write to file
;               unsigned short wr_date;         date of last write to file
;               unsigned long  size;            length of file in bytes
;               char name[13];                  null-terminated name of file
;       };
DTA_SIZE equ    43
;
ifdef __STACK__
        push    EBX
        push    EDX
        mov     EAX,12[ESP]     ; get path
        mov     EDX,16[ESP]     ; attr
        mov     EBX,20[ESP]     ; buf
endif
        push    ECX             ; save ECX
        push    EAX             ; save pointer to path
        push    EDX             ; save attribute
ifndef __OSI__
        mov     EDX,EBX         ; get buffer address
        mov     AH,1Ah          ; set Disk Transfer Address
        int21h                  ; ...
endif
        pop     ECX             ; get attribute for search
        pop     EDX             ; get pointer to path
        mov     AH,4Eh          ; findfirst file
        int21h                  ; ...
        call    __doserror_     ; set return code
ifndef __OSI__
        mov     EDX,EBX         ; get buffer address
        call    __CopyDTA       ; copy DTA to EDX under Intel CodeBuilder
endif
        pop     ECX             ; restore ECX
ifdef __STACK__
        pop     EDX
        pop     EBX
endif
        ret                     ; return to caller
        endproc _dos_findfirst
        defp    _dos_findnext
        if __WASM__ ge 100
            xdefp       "C",_dos_findnext
        else
            xdefp       <"C",_dos_findnext>
        endif
;
;       unsigned _dos_findnext( struct find_t *buf)
;
ifdef __STACK__
        mov     EAX,4[ESP]      ; get parm
endif
        push    EDX             ; save EDX
        mov     EDX,EAX         ; get buffer address
ifndef __OSI__
        mov     AH,1Ah          ; set Disk Transfer Address
        int21h                  ; ...
        call    __SetDTA        ; set DTA for Intel CodeBuilder
endif
        mov     AH,4Fh          ; findnext file
ifdef __OSI__
        mov     AL,0            ; indicate findnext vs findclose
endif
        int21h                  ; ...
        call    __doserror_     ; set return code
ifndef __OSI__
        call    __CopyDTA       ; copy DTA to EDX under Intel CodeBuilder
endif
        pop     EDX             ; restore EDX
        ret                     ; return to caller
        endproc _dos_findnext
        defp    _dos_findclose
        if __WASM__ ge 100
            xdefp       "C",_dos_findclose
        else
            xdefp       <"C",_dos_findclose>
        endif
;
;       unsigned _dos_findclose( struct find_t *buf)
;
        ;       stub program under MS-DOS, doesn't do anything
        ;       under OSI, makes call to loader
ifdef __OSI__
        push    EDX             ; save edx
        mov     EDX,EAX         ; pass in buffer addr
        mov     AH,4Fh          ; find next
        mov     AL,1            ; indicate CLOSE function
        int21h                  ; ...
        pop     EDX             ; restore edx
endif
        xor     EAX,EAX         ; indicate no error
        ret                     ; return to caller
        endproc _dos_findclose

ifndef __OSI__
__CopyDTA proc  near
        cmp byte ptr _Extender,X_INTEL  ; if Intel CodeBuilder
        _if     e                       ; then
          push  EAX                     ; - save EAX (return code)
          push  EBX                     ; - save EBX
          push  ECX                     ; - save ECX
          push  ESI                     ; - save ESI
          push  EDI                     ; - save EDI
          mov   AH,2Fh                  ; - get Disk Transfer Address
          int21h                        ; - ... comes back in EBX
          mov   ESI,EBX                 ; - get Intel CB's DTA
          mov   EDI,EDX                 ; - get user's find_t struct
          mov   ECX,DTA_SIZE            ; - get size of struct
          rep   movsb                   ; - copy the struct
          pop   EDI                     ; - restore EDI
          pop   ESI                     ; - restore ESI
          pop   ECX                     ; - restore ECX
          pop   EBX                     ; - restore EBX
          pop   EAX                     ; - restore EAX
        _endif                          ; endif
        ret                             ; return
__CopyDTA endp

__SetDTA proc   near
        cmp byte ptr _Extender,X_INTEL  ; if Intel CodeBuilder
        _if     e                       ; then
          push  EAX                     ; - save EAX (return code)
          push  EBX                     ; - save EBX
          push  ECX                     ; - save ECX
          push  ESI                     ; - save ESI
          push  EDI                     ; - save EDI
          mov   AH,2Fh                  ; - get Disk Transfer Address
          int21h                        ; - ... comes back in EBX
          mov   EDI,EBX                 ; - get Intel CB's DTA
          mov   ESI,EDX                 ; - get user's find_t struct
          mov   ECX,DTA_SIZE            ; - get size of struct
          rep   movsb                   ; - copy the struct
          pop   EDI                     ; - restore EDI
          pop   ESI                     ; - restore ESI
          pop   ECX                     ; - restore ECX
          pop   EBX                     ; - restore EBX
          pop   EAX                     ; - restore EAX
        _endif                          ; endif
        ret                             ; return
__SetDTA endp
endif

        endmod
        end
