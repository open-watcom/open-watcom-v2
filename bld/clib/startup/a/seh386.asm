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



.386p
include struct.inc

        name    seh


_TEXT   segment use32 dword public 'CODE'
        assume  cs:_TEXT

        public  __TryInit
        public  __TryFini
        public  __TryUnwind
        public  __TryHandler
        public  __Except

ifdef __OS2__
        extrn   DosUnwindException : near
else
  ifdef __NT__
        extrn   _RtlUnwind@16   : near
  else
        error __OS2__ or __NT__ must be defined
  endif
endif

;
;       layout of exception registration record
;       (Compiler reserves space for the registration record on the
;       stack and passes the address of this struct to __TryInit & __TryFini
;       The first 2 fields are the only ones required by NT and OS/2.
;       The rest of the fields are compiler specific.
;
prev_rr         equ     0       ; pointer to previous registration record
handler_addr    equ     4       ; address of exception handler
saved_ebp       equ     8       ; EBP save area
scope_table     equ     12      ; address of _try scope table
scope_index     equ     16      ; current _try scope index
unwindflag      equ     17      ; unwindflag 0 => not unwinding
unwind_index    equ     18      ; scope index to unwind to
unused_byte     equ     19      ; ***unused***
exception_info  equ     20      ; pointer to pointer to exeception info
exception_infop equ     24      ; pointer to exeception info
;;;
;;; exeception info consists of exception_record and context_record pointers
;;;
;;;context_record       equ     24      ; pointer to context record

;
;       layout of _try scope table
;
parent_scope    equ     0       ; BYTE - scope index of parent scope
try_type        equ     1       ; BYTE - 0 => _except, 1 => _finally
func_ptr        equ     2       ; DWORD - addr of _except filter or _finally

ExceptionContinueExecution      equ     0
ExceptionContinueSearch         equ     1
ExceptionNestedException        equ     2

UNWINDING       equ     6
;
;       __TryInit is called at the top of a function that contains a _try
;       statement.  EAX contains address of exception registration record.
;
__TryInit proc  near export
        push    fs:0                    ; get current head of exception chain
        pop     prev_rr[eax]            ; save it in previous pointer
        mov     dword ptr handler_addr[eax],offset __TryHandler
        mov     saved_ebp[eax],ebp      ; save user's EBP
        mov     byte ptr scope_index[eax],0ffh  ; indicate not inside try blk
        mov     byte ptr unwindflag[eax],0      ; indicate not unwinding
        mov     fs:0,eax                ; point fs:0 to new registration rec.
        ret                             ; return
__TryInit endp

;
;       __TryFini is called at the end of a function that contains a _try
;       statement.  EAX contains address of exception registration record.
;
__TryFini proc  near export
        push    prev_rr[eax]            ; get previous exception reg. rec.
        pop     fs:0                    ; set fs:0 to point to it
        mov     fs:0,eax
        ret                             ; return
__TryFini endp

;
;       This routine gets called when an exception occurs.
;       There are four parameters to this routine.
;
;       The following are offsets past EBP
ExceptionRecord equ     8
EstablisherFrame equ    12
ContextRecord   equ     16
DispatcherContext equ   20

__TryHandler proc near export
        push    ebp                     ; save registers
        mov     ebp,esp                 ; get access to parms
        push    edi                     ; ...
        push    esi                     ; ...
        push    edx                     ; ...
        push    ecx                     ; ...
        push    ebx                     ; ...
        mov     edi,EstablisherFrame[ebp]; get address of registration record
        mov     eax,ContextRecord[ebp]  ; copy context record address to
        push    eax                     ; ... stack
        mov     eax,ExceptionRecord[ebp]; get exception record address
        push    eax                     ; ...
        test    dword ptr 4[eax],UNWINDING; check to see if we are unwinding
        _if     ne                      ; if we are unwinding, then
          mov   ebp,edi                 ; - get address of establisher Frame
          mov   byte ptr unwind_index[ebp],0ffh ; - do full local unwind
          call  __local_unwind          ; - do local unwind
        _else                           ; else
;;;       mov   exception_rec[edi],eax  ; - save exception record address
;;;
;;; The compiler is generating too many levels of indirection
;;; so we have to create a pointer to a pointer to the exception_info
;;;
          mov   exception_infop[edi],esp; - save ptr to exception info
          lea   eax,exception_infop[edi]; - get address of ptr
          mov   exception_info[edi],eax ; - save ptr to ptr to exception_info
;;;       mov   eax,ContextRecord[ebp]  ; - copy context record address to
;;;       mov   context_record[edi],eax ; - ... registration record
          mov   ebp,edi                 ; - save address of establisher frame
          mov   edi,scope_table[ebp]    ; - get address of scope table
          sub   ebx,ebx                 ; - zero ebx
          mov   bl,scope_index[ebp]     ; - get current scope index
          _loop                         ; - loop (find _except filter => 1)
            cmp   bl,0ffh               ; - - quit if out of scope
            _quif e                     ; - - ...
            mov   unwind_index[ebp],bl  ; - - set unwind index
            lea   ebx,[ebx+ebx*2]       ; - - get scope index times 3
            cmp   byte ptr 1[edi+ebx*2],0; - - if this _try contains _except
            _if   e                     ; - - then
              push ebp                  ; - - - save address of establisher frame
              push edi                  ; - - - save address of scope table
              push ebx                  ; - - - save current _try scope index
              mov  ebp,saved_ebp[ebp]   ; - - - reload EBP of _try routine
              jmp  dword ptr 2[edi+ebx*2]; - - - jump to _except filter code
              ; _except filter code ends with a call to __Except. See below.
              ; filter may have destroyed some registers on us.
__Except proc near export
              pop  edx                  ; - - - get return address
              pop  ebx                  ; - - - restore _try scope index
              pop  edi                  ; - - - restore address of scope table
              pop  ebp                  ; - - - restore establisher frame addr
              inc  eax                  ; - - - adjust return value
              je   short exit_handler   ; - - - exit if EXCEPTION_CONTINUE_EXECUTION
              dec  eax                  ; - - - readjust
              jne  short HandleException; - - - exit if we can handle the exception
              ; otherwise continue search
            _endif                      ; - - endif
            mov   bl,[edi+ebx*2]        ; - - get parent scope index
          _endloop                      ; - endloop
        _endif                          ; endif
        mov     eax,ExceptionContinueSearch ; indicate CONTINUE_SEARCH
exit_handler:
        pop     ebx                     ; remove exception record and
        pop     ebx                     ; .. context record pointers
        pop     ebx                     ; restore registers
        pop     ecx                     ; ...
        pop     edx                     ; ...
        pop     esi                     ; ...
        pop     edi                     ; ...
        pop     ebp                     ; ...
        ret                             ; return
__Except endp
__TryHandler endp

;
;       __Except is called from the fragment "} _except( expr ) {"
;       EAX contains one of the following values:
;       -1 => EXCEPTION_CONTINUE_EXECUTION
;        0 => EXCEPTION_CONTINUE_SEARCH
;       +1 => EXCEPTION_EXECUTE_HANDLER
;             The return address is the address of the exception handler.
;             Before "calling" the exception handler, we must do a global
;             unwind to get fs:0 pointing to the current exception record.
;             Once in the current exception context, we must do a local
;             unwind (i.e. call the _finally blocks of all active nested
;             _try blocks).
;
HandleException proc    near
        push    edx                     ; save address of exception handler
        call    __global_unwind         ; perform global unwind
        call    __local_unwind          ; perform local unwind
        mov     ebp,saved_ebp[ebp]      ; reload EBP of _try routine
        ret                             ; return to exception handler
HandleException endp

__global_unwind proc    near
        push    ebp                     ; save registers
        push    edi                     ; ...
        push    esi                     ; ...
        push    edx                     ; ...
        push    ecx                     ; ...
        push    ebx                     ; ...
        push    0                       ; 0
        push    0                       ; 0
        push    offset done_unwind      ; address of where to continue
        push    ebp                     ; push address of establisher frame
ifdef __OS2__
        call    DosUnwindException      ; do global unwind
else
        call    _RtlUnwind@16           ; do global unwind
endif
done_unwind:
        pop     ebx                     ; restore registers
        pop     ecx                     ; ...
        pop     edx                     ; ...
        pop     esi                     ; ...
        pop     edi                     ; ...
        pop     ebp                     ; ...
        ret                             ; return
__global_unwind endp

__local_unwind proc near
        push    ebp                     ; save registers
        push    edi                     ; ...
        push    esi                     ; ...
        push    edx                     ; ...
        push    ecx                     ; ...
        push    ebx                     ; ...
        mov     edi,scope_table[ebp]    ; get address of scope table
        sub     ebx,ebx                 ; zero ebx
        mov     bl,scope_index[ebp]     ; get current scope index
        mov     byte ptr unwindflag[ebp],1; indicate unwinding
        _loop                           ; loop (call _finally blocks)
          cmp   bl,0ffh                 ; - quit if out of scope
          _quif e                       ; - ...
          cmp   bl,unwind_index[ebp]    ; - quit if at desired scope
          _quif e                       ; - ...
          lea   ebx,[ebx+ebx*2]         ; - get scope index times 3
          cmp   byte ptr 1[edi+ebx*2],0 ; - if this _try contains _finally
          _if   ne                      ; - then
            push  ebp                   ; - - save address of establisher frame
            push  edi                   ; - - save address of scope table
            push  ebx                   ; - - save current _try scope index
            mov   ebp,saved_ebp[ebp]    ; - - reload EBP of _try routine
            call  dword ptr 2[edi+ebx*2]; - - call _finally block
            pop   ebx                   ; - - restore _try scope index
            pop   edi                   ; - - restore address of scope table
            pop   ebp                   ; - - restore establisher frame addr
          _endif                        ; - endif
          mov   bl,[edi+ebx*2]          ; - get parent scope index
          mov   scope_index[ebp],bl     ; - update current scope index
        _endloop                        ; endloop
        mov     byte ptr unwindflag[ebp],0 ; indicate not unwinding
        pop     ebx                     ; restore registers
        pop     ecx                     ; ...
        pop     edx                     ; ...
        pop     esi                     ; ...
        pop     edi                     ; ...
        pop     ebp                     ; ...
        ret                             ; return
__local_unwind endp

;
;  input:
;       AL - scope index of block to unwind to
;
__TryUnwind proc near export
        push    ebp                     ; save ebp
        mov     ebp,FS:0                ; get address of current reg record
        mov     unwind_index[ebp],al    ; save index to unwind to
        call    __local_unwind          ; do local unwind
        pop     ebp                     ; restore ebp
        ret                             ; return
__TryUnwind endp

_TEXT   ends
        end
