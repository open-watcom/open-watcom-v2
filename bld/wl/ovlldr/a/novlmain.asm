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
;* Description:  Dynamic Overlay Manager
;*
;*****************************************************************************


;        page    64,110

;   NOTE!  A few of the routines that walk up the stack here assume that
;       BP != 0 on entry.  This seems reasonable, but means that _cstart_
;       should do:
;               xor bp,bp
;               push bp
;               mov bp,sp
;       As soon as it creates the stack.  Just doing a xor bp,bp is not good
;       enough - if somehow an init routine ends up in an overlay (i.e.,
;       someone puts it there) then bp could be zero on entry.
;

;       For multithreaded database, there will be several stack chains to chase
;       This involves:
;               Chasing through Context_list, a linked list of task contexts
;                       0       saved bp (0 if active task, use BPChain instead)
;                       2       saved sp
;                       4       link to next context
;       There may be multiple return traps per overlay (up to one per stack)
;       Since we don't want to have more than one segment per overlay, all
;       the return traps point to the same trap; there is a list of
;       (bp,list,offset) sets at the end of the trap followed by a zero bp
;       value.  This increases the size of a return trap to:
;               14      original size (one thread)
;               24      6 bytes times 4 additional threads
;                2      0 at end of list
;               --
;               40

        include struct.inc
        include novlldr.inc

        name    novlmain

        comm    __get_ovl_stack:dword
        comm    __restore_ovl_stack:dword
        comm    __close_ovl_file:dword
        comm    __pcode_ret_trap:dword

OVL_DBG_INFO STRUC
    location    dw 0
    section     dw 0
    bitsize     dw 0
OVL_DBG_INFO ENDS

DGROUP  group   _DATA

_DATA   segment byte 'DATA' PUBLIC

ifdef OVL_MULTITHREAD
        extrn   "C",Context_list:word
endif

_DATA   ends

_TEXT   segment para '_OVLCODE' PUBLIC

ifdef OVL_MULTITHREAD
        assume  SS:DGROUP
endif

        extrn   "C",__NOVLTINIT__:near
        extrn   "C",__WOVLLDR__:near
        extrn   "C",__OvlExit__:near
        extrn   "C",__OVLMAXSECT__:near
        extrn   "C",__FINDOVLADDR__:far
        extrn   "C",__OVLLONGJMP__:near
        extrn   "C",__NDBG_HANDLER__:far
        extrn   __OVLTAB__:near
        extrn   __OVLTABEND__:near
        extrn   "C",__OVLPSP__:word
        extrn   "C",__OVLCAUSE__:word
        extrn   "C",__LoadNewOverlay__:near
        extrn   "C",__OVLINITAREA__:near
        extrn   "C",__OVLFLAGS__:word
        extrn   "C",__CloseOvl__:far

        public  "C",__NDBG_HOOK__
        public  "C",__OVLAREALIST__
        public  "C",__OVLROVER__
        public  "C",__OVLSTARTPARA__
        public  "C",__OVLDBGINFO__

;
;   void __NOVLINIT__( DOS EXE startup parms )
;
        public  __NOVLINIT__
__NOVLINIT__ proc far
;
; the program entry point. The original stack cannot be used before the call
; to NOVLTINIT since NOVLTINIT loads the program's initial data area and
; can potentially clobber the stack.  In this case, it is assumed that
; the first overlay section in the table points to an area at least 256
; bytes long that we can use.
;

        jmp     short around

                dw OVL_SIGNATURE; the overlay manager's signature.
__NDBG_HOOK__   dd NullHook     ; the debugger's "hook" into the ovl mgr
                dw __NDBG_HANDLER__
BPChain         dw      0       ; set by __NOVLLDR__ and __OVLRETTRAP__
SaveWord        dw      0       ; used by __NOVLLDR__ and __OVLRETTRAP__
__OVLAREALIST__ dw      0       ; head of the list of areas
__OVLROVER__    dw      0       ; used for 2nd chance cyclic algorithm
__OVLSTARTPARA__ dw     0       ; start_para for non-PRELOAD sections

__OVLDBGINFO__  OVL_DBG_INFO <?>

around:
        mov     __OVLPSP__,ES   ; save segment address of PSP
        mov     BPChain,SS      ; save actual SS:SP
        mov     SaveWord,SP     ; . . .
        mov     AX,__OVLTAB__.ov_prolog.ovp_delta
        add     AX,__OVLTAB__.ov_entries.ove_start_para
        cli                     ; set SS:SP to temporary stack
        mov     SS,AX           ; . . .
        mov     SP,1024         ; linker guarantees at least 1024
        sti                     ; . . .
                                ; initialize __OVLFLAGS__
        _guess                  ; determine which CPU we are using
          mov   AX,0FFFFh       ; do the shift trick
          mov   CL,33           ; shift right 33 times
          shr   AX,CL           ; AX will be 0 for 8088/8086 types
          _quif e               ; quit if 808x processor
          xor   AX,AX           ; set up return value.
          push  SP              ; do the stack trick
          pop   CX              ; . . .
          cmp   CX,SP           ; quit if stack value was post dec value
          _quif ne              ; quit if 8018x processor
          pushf                 ; save flags
          mov   CX,0f000H       ; try to turn on top bits in flags
          push  CX              ; . . .
          popf                  ; . . .
          pushf                 ; see if any turned on
          pop   CX              ; . . .
          popf                  ; restore flags
          test  CX,0f000H       ; are any bits on?
          _quif e               ; if not, it's a 286
          inc   AX              ; it's a 386!
        _endguess
        mov     __OVLFLAGS__,AX ; store result.
        mov     ax,3000h        ; get dos version number
        int     21h             ; ...
        cmp     al,3            ; check if version 3 or greater
        _if     ae              ; ...
          or    __OVLFLAGS__,2  ; set OVL_DOS3 flag
        _endif
                                ; initialize overlay loader
        call    __NOVLTINIT__   ; initialize overlays
        cli                     ; set SS:SP to actual stack
        mov     SS,BPChain      ; . . .
        mov     SP,SaveWord     ; . . .
        sti                     ; . . .
        push    DX              ; push actual start segment
        push    AX              ; push actual start offset
                ; We can't setup the initial area before the temporary stack
                ; has been moved out of the overlay area.
        mov     AX,__OVLAREALIST__
        mov     DX,__OVLTAB__.ov_prolog.ovp_ovl_size
        call    __OVLINITAREA__ ; . . .

assume  DS:DGROUP

        mov     AX,seg DGROUP   ; get dgroup
        mov     DS,AX           ; now set up setjmp/longjmp vbls
        mov     word ptr __get_ovl_stack,offset __FINDOVLADDR__
        mov     word ptr __restore_ovl_stack,offset longjmp_wrap
        mov     word ptr __close_ovl_file,offset __CloseOvl__
        mov     word ptr __pcode_ret_trap,offset pcode_ret_trap
        mov     word ptr __get_ovl_stack+2,CS
        mov     word ptr __restore_ovl_stack+2,CS
        mov     word ptr __close_ovl_file+2,CS
        mov     word ptr __pcode_ret_trap+2,CS

assume  DS:nothing

        mov     DS,__OVLPSP__   ; restore DS
        _guess  death
          mov   AX,word ptr __NDBG_HOOK__+2; get segment of hook
          mov   DX,CS           ; get CS value
          cmp   AX,DX           ; is it in our code segment?
          _if   ne              ; . . .
            call __OVLMAXSECT__ ; get size of largest section.
            mov DX,AX           ; save this for later.
            mov BX,AX           ; allocate memory block
            mov AH,48H          ; . . .
            int 21H             ; . . .
            _if c               ; check if we got it
              mov ES,__OVLPSP__ ; get PSP segment again
              mov BX,ES:[2]     ; get end of allocation
              mov AX,ES         ; get psp value (start of alloc.)
              sub BX,AX         ; subtract start..
              dec BX            ; fix "off by 1"
              dec BX            ; give DOS an extra paragraph
              sub BX,DX         ; remove size of largest overlay
              mov AH,4AH        ; resize memory block
              int 21H           ; . . .
              _quif c,death     ; die if there was a problem
              mov BX,DX         ; get max section size.
              mov AH,48H        ; allocate memory block
              int 21H           ; . . .
              _quif c,death     ; die if there was a problem
            _endif
            mov __OVLDBGINFO__.location,AX; save debugger area
          _endif
          jmp   __NDBG_HOOK__   ; hook into debugger if it's there
                                ; otherwise, start program
        _endguess
        mov     AX,3            ; out of memory message
        jmp     __OvlExit__     ; write message and end program.
__NOVLINIT__ endp

;
; unsigned near __WhichArea__( unsigned seg )
;
        public  "C",__WhichArea__
__WhichArea__ proc near
;
;       Determine the overlay area that seg belongs to.  Return 0 if it
;       doesn't belong to any area.  Assumes that AX != 0 and that there
;       is at least one overlay area.
;
;       We also return carry=1 if it is in an area, carry=0 otherwise.
;
        push    DS              ; save registers
        push    DX              ; . . .
        mov     DX,__OVLAREALIST__; get head of area list
        _loop
          mov   DS,DX           ; get segment of area
          sub   AX,DX           ; check if ax in bounds
          cmp   AX,DS:[al_size] ; . . .
          _quif c               ; carry set if in bounds
          add   AX,DX           ; restore ax
          mov   DX,DS:[al_next] ;get next segment
          test  DX,DX           ; check if end of list (carry set to 0)
        _until  e
        mov     AX,DX           ; DON'T MODIFY CARRY FROM HERE . . .
        pop     DX              ; . . .
        pop     DS              ; . . .
        ret                     ; . . . TO HERE!!
__WhichArea__ endp


;
; unsigned near __OVLSCANCALLCHAIN__( void )
;
        public "C",__OVLSCANCALLCHAIN__
__OVLSCANCALLCHAIN__ proc near
;
;   This function modifies all start_paras in the ovltab so that they are
;   either:  zero ==> indicates the overlay is not in call chain
;       or:  non-zero ==> part of a linked list of sections which are in
;               the call chain
;   returns the head of the linked list.  0xffff terminates the linked list.
;   The list is in the order highest-on-stack to lowest-on-stack.  The
;   list entries are actually (size OVLTAB_ENTRY) * overlay number.
;
        push    BX
        push    CX
        push    DX
        push    SI
        push    BP
        push    ES
        push    DS
ifdef OVL_MULTITHREAD
        mov     BP,Context_list ; get list before switching DS
endif
        mov     AX,CS           ; set ds == cs for speed (& size)
        mov     BX,offset __OVLTAB__.ov_entries.ove_start_para - size OVLTAB_ENTRY
        mov     DS,AX           ; (pipelined) set ds == cs

assume  DS:_TEXT

        mov     DX,0ffffh       ; DX is head of linked list
ifdef OVL_MULTITHREAD
      _loop
        push    BP              ; save context
        mov     BP,[bp+tl_saved_bp]; get bp
        test    BP,BP           ; is it valid?
        jne     sc_beg          ; yes
        mov     BP,BPChain      ; active task - use start of bp chain
else
        mov     BP,BPChain      ; start of bp chain
endif
        jmp     short sc_beg
        EVEN
        _loop
          shl   BP,1            ; restore bp and test if non-zero
          _quif z               ; if zero then done
sc_beg:   mov   SI,[BP+4]       ; get possible segment
          mov   BP,[BP]         ; get bp link
          shr   BP,1            ; test if near call
          _loopif nc            ; continue if it's a near call
          mov   AX,SI           ; check if in an overlay area
          call  __WhichArea__   ; . . .
          _loopif nc            ; continue if not in overlay area
          dec   SI              ; get overlay number from descriptor
          mov   ES,SI           ; . . .
          mov   CL,4            ;               (pipelined) for shifting
          mov   SI,ES:[0eH]     ;. . .
          shl   SI,CL           ; multiply by size OVLTAB_ENTRY
          mov   CX,[BX+SI]      ; get start_para for this overlay
          test  CX,CX           ; if non-zero then already in call-chain
          _loopif ne            ; continue if already in call-chain
          mov   [BX+SI],DX      ; store the linked list head
          mov   DX,SI           ; change the head
        _endloop                ; continue looping
ifdef OVL_MULTITHREAD
        pop     BP              ; get context
        mov     BP,[BP+tl_next]
        test    BP,BP           ; at end of list?
      _until e                  ; loop if not
endif
        mov     AX,DX           ; get return value
        pop     DS

assume  DS:nothing

        pop     ES
        pop     BP
        pop     SI
        pop     DX
        pop     CX
        pop     BX
        ret
__OVLSCANCALLCHAIN__ endp


;
; void near __OVLBUILDRETTRAP__( unsigned old_handle, unsigned rt_seg );
;
        public  "C",__OVLBUILDRETTRAP__
__OVLBUILDRETTRAP__ proc near
;
;   This function constructs a return trap in rt_seg.  It only initializes
;   the ret_offset, ret_list, and stack_trap fields.  See novlldr.c for
;   more info on what a return trap looks like.
;
;   We assume that old_handle is actually in the call chain at least once...
;
        push    BP
        push    DS
        push    CX
ifdef OVL_MULTITHREAD
        push    BX
        mov     BP,Context_list ; get list of contexts
endif
        mov     DS,DX           ; set ds to rt_seg
ifdef OVL_MULTITHREAD
        mov     BX,rt_traps     ; initialize to point to first trap
        mov     byte ptr DS:[rt_call_far],CALL_FAR
        mov     word ptr DS:[rt_entry],offset __OVLRETTRAP__
        mov     word ptr DS:[rt_entry+2],seg __OVLRETTRAP__
        mov     word ptr DS:[rt_old_code_handle],AX
      _loop                     ; extra loop for each context
        mov     [BX+te_context],BP; assume this thread needs trap
        push    BP              ; save context
        mov     BP,[BP+tl_saved_bp]; get saved bp
        test    BP,BP           ; is it the active task?
        jne     bd_1a           ; yes, get started
        mov     BP,BPChain      ; get BP of active task
else
        mov     BP,BPChain      ; get BP
endif
        jmp     short bd_1a
        _loop
          shl   CX,1            ; restore bp
          mov   BP,CX
bd_1a:    mov   CX,[BP]         ; get next bp
          mov   DX,[BP+4]       ; get (possible) segment
          shr   CX,1            ; check if near or far ret
ifdef OVL_MULTITHREAD
          je    skip_context    ; NULL => not on this thread...
endif
          _loopif nc            ; was near so try again
          cmp   DX,AX           ; is it our handle?
        _until e                ; loop until our handle
                                ; fill in the return trap & modify stack
        mov     DX,[BP+2]       ; get original ret addr
ifdef OVL_MULTITHREAD
        mov     [BX+te_stack_trap],BP
        mov     [BX+te_ret_offset],DX
else
        mov     byte ptr DS:[rt_call_far],CALL_FAR
        mov     word ptr DS:[rt_entry],offset __OVLRETTRAP__
        mov     word ptr DS:[rt_entry+2],seg __OVLRETTRAP__
        mov     word ptr DS:[rt_stack_trap],BP
        mov     word ptr DS:[rt_old_code_handle],AX
        mov     word ptr DS:[rt_ret_offset],DX
endif
        xor     DX,DX           ; zero head of list
        mov     [BP+4],DS       ;set far ret to the ret trap
        mov     [BP+2],DX       ;. . .
        _loop                   ; loop to link any other returns on stack
          shl   CX,1            ; restore bp
          _quif e               ; finished loop if bp 0
          mov   BP,CX           ; advance BP
          mov   CX,[BP]         ; check if far or near ret
          shr   CX,1            ; . . .
          _loopif nc            ; continue loop if near ret
          cmp   [BP+4],AX       ; check if it's our code_handle
          _loopif ne            ; continue loop if not one of ours
          dec   byte ptr [BP]   ; unset the far bit
          mov   [BP+4],DX       ; add it to the list
          mov   DX,BP           ; . . .
        _endloop                ; continue loop
ifdef OVL_MULTITHREAD
        mov     [BX+te_ret_list],DX
        add     BX,size TRAP_ENTRY
skip_context:
        pop     BP
        mov     BP,[BP+tl_next] ; get next context
        test    BP,BP
      _until    e               ; loop until end of list
        mov     word ptr [BX+te_stack_trap],0 ; mark end of list
        pop     BX
else
        mov     DS:[rt_ret_list],DX
endif
        pop     CX
        pop     DS
        pop     BP
        ret
__OVLBUILDRETTRAP__ endp


;ifdef OVL_MULTITHREAD
;
; unsigned near __OVLUNDORETTRAP__( unsigned rt_seg, unsigned new_handle );
;
;else
;
; void near __OVLUNDORETTRAP__( unsigned stack_trap, unsigned ret_offset,
;       unsigned ret_list, unsigned new_handle );
;
;endif

        public  "C",__OVLUNDORETTRAP__
__OVLUNDORETTRAP__ proc near
;
; Walk up the linked list created by __OVLBUILDRETTRAP__ and set the segments
; to their new value.  Also set the far bit in the BP chain.  Remove the
; actual trap if necessary (i.e., the section has been loaded before the trap
; was invoked).
;
        push    BP
ifdef OVL_MULTITHREAD

; Return != 0 if the return trap is still used, 0 if no longer needed

        push    BX
        push    CX
        push    SI
        push    DS
        mov     DS,AX           ; get segment of trap
        mov     BX,rt_traps     ; start at first trap
        xor     SI,SI           ; used = FALSE
        _loop                   ; loop for each context
          mov   CX,[BX+te_stack_trap]
          test  CX,CX
          _quif e               ; done when stack_trap == 0
          mov   BP,[BX+te_context]
          mov   BP,[BP+tl_saved_bp]
          test  BP,BP
          _if   e
            mov BP,BPChain      ; get BP of active task
          _endif
          cmp   CX,BP           ; if stack_trap >= BP
          _if   ae
            mov BP,CX
            mov [BP+4],DX       ;   set proper return segment
            mov AX,[BX+te_ret_offset];   and offset
            mov [BP+2],AX
            inc SI              ;   used = TRUE
          _endif
          mov   AX,[BX+te_ret_list]
          _loop
            cmp AX,CX           ; are we done yet? i.e. BP == 0 or BP is
            _quif b             ; . . . below chain
            mov BP,AX
            inc byte ptr [BP]   ; set the far bit
            mov AX,[BP+4]       ; get the link
            mov [BP+4],DX       ; store new_handle as segment of return
            inc SI              ; used = TRUE
          _endloop
          add   BX,size TRAP_ENTRY
        _endloop
        mov     AX,SI           ; set return code
        pop     DS
        pop     SI
        pop     CX
        pop     BX
else
        cmp     AX,BPChain      ; check if we can remove trap from stack
        _if     ae              ; we can if the stack is below us
          mov   BP,AX           ; get offset of stack_trap
          mov   [BP+4],CX       ; new return seg
          mov   [BP+2],DX       ; restore return offset
        _endif
        mov     AX,BPChain
        _loop
          cmp   BX,AX           ; are we done yet? i.e., BX == 0 or BX is
          _quif b               ; . . . below chain
          mov   BP,BX           ; get the next link
          inc   byte ptr [BP]   ; set the far bit
          mov   BX,CX           ; get the new_handle
          xchg  BX,[BP+4]       ; store handle and get next link
        _endloop
endif
        pop     BP
        ret
__OVLUNDORETTRAP__ endp


;
; void near __OVLFIXCALLCHAIN__( unsigned old, unsigned new )
;
        public  "C",__OVLFIXCALLCHAIN__
__OVLFIXCALLCHAIN__ proc near
;
; Scan through the call chain and change any far rets that point to seg
; in AX to point to the seg in DX.
;
        push    BP
        push    CX
ifdef OVL_MULTITHREAD
        mov     BP,Context_list
        _loop
          push  BP
          mov   BP,[BP+tl_saved_bp]
          test  BP,BP
          jne   fix_start       ; used saved bp if not current task
          mov   BP,BPChain      ; otherwise use BPChain
else
        mov     BP,BPChain      ; get the BP on entry
endif
        jmp     short fix_start
        _loop
          shl   CX,1            ; fix the BP.
          _quif z               ; done if BP zero
          mov   BP,CX           ; restore BP
fix_start:
          mov   CX,[BP]         ; get next BP
          shr   CX,1            ; test if odd
          _loopif nc            ; nc if BP was even (near rtn)
          cmp   AX,[BP+4]       ; compare segment
          _loopif ne            ; continue loop if not right seg
          mov   [BP+4],DX       ; set new value
        _endloop
ifdef OVL_MULTITHREAD
          pop   BP              ; restore context
          mov   BP,[BP+tl_next] ; get next context
          test  BP,BP
        _until e                ; until NULL
endif
        pop     CX
        pop     BP
        ret
__OVLFIXCALLCHAIN__  endp


;
; int near __NCheckRetAddr__( ovl_addr far *data )
;
        public "C",__NCheckRetAddr__
__NCheckRetAddr__ proc near
;
; This routine is for debugging support.  It checks if the return address
; pointed to by data is a return trap.
;
        push    BX              ; save registers
        push    DI              ; . . .
        push    DS              ; . . .
        push    ES              ; . . .
        _guess
          mov   DS,DX           ; segment of data
          mov   BX,AX           ; offset of data
          xor   DX,DX           ; set up return code
          cmp   [BX],DX         ; all ret traps have offset 0
          _quif ne
          mov   AX,[BX+2]       ; get seg of (possible) ret trap
          call  __WhichArea__   ; check if in an overlay area
          _quif nc              ; . . .
          mov   AX,[BX+2]       ; get seg of ret trap
          dec   AX              ; . . .
          mov   ES,AX           ; . . .
          mov   DI,ES:[0eH]     ; get overlay number
          push  CX              ; save CX
          mov   CL,4            ; multiply by size OVLTAB_ENTRY
          mov   AX,DI           ; save overlay number in ax
          shl   DI,CL           ; . . . multiply
          pop   CX              ; restore CX
          test  byte ptr [DI+__OVLTAB__.ov_entries.ove_flags_anc+1- size OVLTAB_ENTRY],10H
                                ; check if FLAG_RET_TRAP set
          _quif e               ; . . .
          inc   DX              ; this is a return trap
          mov   [BX+4],AX       ; set the overlay number
ifdef OVL_MULTITHREAD
          mov   AX,10H + rt_traps; find the active context
          _loop                 ; (assume return trap is in active stack)
            mov DI,AX           ; save pointer
            mov DI,ES:[DI+te_context]
            cmp word ptr SS:[DI+tl_saved_bp],0
            _quif e
            add AX,size TRAP_ENTRY
          _endloop              ; done when found active context
          mov   DI,AX
          mov   AX,ES:[DI+te_ret_offset]
else
          mov   AX,ES:[10H + rt_ret_offset]
endif
          mov   DI,ES:[10H + rt_old_code_handle]
          mov   [BX+2],DI       ; give seg to debugger
          mov   [BX],AX         ; give offset to debugger
        _endguess
        mov     AX,DX           ; get return code
        pop     ES              ; restore registers
        pop     DS              ; . . .
        pop     DI              ; . . .
        pop     BX              ; . . .
        ret
__NCheckRetAddr__ endp

;
; void far __OVLRETTRAP__( void )
;
        align   16
        public  "C",__OVLRETTRAP__
__OVLRETTRAP__  proc far
;
;   This is the entry point that all return traps use.
;
        mov     SaveWord,AX             ; save AX
        pop     AX                      ; pop offset of ret trap (not used)
        pop     AX                      ; pop segment of ret trap
        pushf                           ; save flags
        call    do_ret_trap             ; unload the trap & load the section
        popf                            ; restore flags
        mov     AX,SaveWord             ; restore AX
        jmp     dword ptr __OVLCAUSE__  ; return to overlay
__OVLRETTRAP__ endp

        align 4

do_ret_trap proc near
;
; do a return trap. this expects the segment of the return trap in AX
;
        mov     BPChain,BP              ; save call chain
        push    DX                      ; save registers
        push    CX                      ; . . .
        push    BX                      ; . . .
        push    DS                      ; . . .
        dec     AX                      ; get the overlay number and
        mov     DS,AX                   ; get return offset
ifdef OVL_MULTITHREAD
        mov     AX,10H + rt_traps
        _loop
          mov   BX,AX
          add   AX,size TRAP_ENTRY      ; (note that loop must end)
          mov   CX,[BX+te_ret_offset]   ; get return offset
          mov   BX,[BX+te_context]      ; get context
          cmp   word ptr SS:[BX+tl_saved_bp],0
        _until  e                       ; until active task found
        mov     BX,CX                   ; save return offset
else
        mov     BX,DS:[010H + rt_ret_offset]; save return offset
endif
        mov     AX,DS:[0EH]             ; get the overlay number
        mov     CX,AX                   ; save overlay number
        call    __LoadNewOverlay__      ; load overlay
        mov     __OVLCAUSE__,BX         ; save offset of return
        mov     __OVLCAUSE__+2,AX       ; save segment of return
        xchg    CX,AX                   ; ovl num in AX, seg in CX
        mov     DL,1                    ; overlay load cause by return
        call    __NDBG_HOOK__           ; tell debugger
        pop     DS                      ; restore registers
        pop     BX                      ; . . .
        pop     CX                      ; . . .
        pop     DX                      ; . . .
        mov     BP,BPChain              ; . . .
        ret
do_ret_trap endp

pcode_ret_trap proc far
;
; this is the return trap routine called by the pcode interpreter
; this expects the segment of the trap in AX, and returns the new segment in AX
;
        call    do_ret_trap             ; do the return trap;
        mov     AX,__OVLCAUSE__+2       ; load the segment of section
        ret
pcode_ret_trap  endp

;
; void __NOVLLDR__( void )
;
        align   16
        public  __NOVLLDR__
__NOVLLDR__ proc near
;
; Notice by the time we get here, the bp chain calling convention has
; been violated (two calls in a row).  We must farkle the stack to
; restore the bp chain linkage.
;
        mov     SaveWord,AX             ; save original AX
        pop     AX                      ; get vector ret addr from stack
        sub     AX,3                    ; make point to beginning of vector
        inc     BP                      ; restore BP chain
        push    BP                      ; . . .
        mov     BP,SP                   ; . . .
        mov     BPChain,SP              ; store BP for scanning later
        push    SaveWord                ; save original AX on stack
        mov     SaveWord,AX             ; save vector in SaveWord
        push    DX                      ; save original DX
        mov     DX,[BP+2]               ; get original return offset
        mov     __OVLCAUSE__,DX         ; stash it for the debugger
        mov     DX,[BP+4]               ; get original return segment
        mov     __OVLCAUSE__+2,DX       ; stash it for the debugger
        mov     DX,CS                   ; move CS to where the fn. can get it
        call    __WOVLLDR__             ; load overlay
        pop     DX                      ; restore registers
        pop     AX                      ; . . .
        pop     BP                      ; . . .
        dec     BP                      ; . . .
        jmp     word ptr SaveWord       ; exit through vector
__NOVLLDR__ endp


longjmp_wrap proc far
ifdef OVL_MULTITHREAD
        mov     BPChain,BP
        call    __OVLLONGJMP__
else
        push    BX
        mov     BPChain,BP
        mov     BX,BP
        call    __OVLLONGJMP__
        pop     BX
endif
NullHook proc far
        ret
NullHook endp
longjmp_wrap endp


_TEXT   ends

        end
