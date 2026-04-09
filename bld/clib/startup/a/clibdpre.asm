;
; clibdpre.asm - CLIB DLL prelude for NetWare NLM shared libraries
;
; Like clibpre.obj but without main() and _StartNLM (no console).
; CRT init via __init_environment; keep-alive thread on a semaphore.
;
; wasm -3s -bt=netware clibdpre.asm
;

.387
.386p

        PUBLIC  _Prelude
        PUBLIC  _Stop
        PUBLIC  _argc
        PUBLIC  kNLMInfo

        EXTRN   __init_environment:BYTE
        EXTRN   __deinit_environment:BYTE
        EXTRN   _end:BYTE
        EXTRN   _edata:BYTE
        EXTRN   memset:BYTE
        ; Semaphore + thread functions to keep the NLM resident
        EXTRN   OpenLocalSemaphore:BYTE
        EXTRN   WaitOnLocalSemaphore:BYTE
        EXTRN   SignalLocalSemaphore:BYTE
        EXTRN   CloseLocalSemaphore:BYTE
        EXTRN   BeginThread:BYTE

DGROUP  GROUP   CONST,CONST2,_DATA,_BSS

_TEXT   SEGMENT PARA PUBLIC USE32 'CODE'
        ASSUME  CS:_TEXT, DS:DGROUP, SS:DGROUP

;
; __keepalive_thread -- thread proc that blocks on the semaphore.
; Runs until _Stop signals _stop_sema.
; void __keepalive_thread(void *arg)
;
__keepalive_thread:
        push    dword ptr _stop_sema
        call    near ptr WaitOnLocalSemaphore
        add     esp,4
        ret

;
; _Prelude -- NLM entry point called by the NetWare loader.
; Clears BSS, initialises CRT, starts a keep-alive thread, returns 0.
; No _StartNLM call, so no console screen is created.
;
_Prelude:
        push    ebx
        push    esi
        push    edi
        push    ebp
        ; --- clear BSS ---
        mov     eax,offset _end
        sub     eax,offset _edata
        push    eax
        push    0
        push    offset _edata
        call    near ptr memset
        add     esp,0cH
        ; --- init CRT ---
        push    0
        call    near ptr __init_environment
        add     esp,4
        test    eax,eax
        jnz     L$fail
        ; --- open semaphore (count=0, will block immediately) ---
        push    0
        call    near ptr OpenLocalSemaphore
        add     esp,4
        mov     dword ptr _stop_sema,eax
        ; --- start keep-alive thread ---
        ; BeginThread(proc, stack, stackSize, arg)
        push    0                               ; arg  (void*)
        push    4000H                           ; stackSize (16 KB)
        push    0                               ; stack (NULL = allocate)
        push    offset __keepalive_thread       ; proc
        call    near ptr BeginThread
        add     esp,10H
        ; --- success ---
        xor     eax,eax
        jmp     L$done
L$fail:
        or      eax,-1
L$done:
        pop     ebp
        pop     edi
        pop     esi
        pop     ebx
        ret

;
; _Stop -- NLM exit procedure.
; Signal the semaphore (unblocks keep-alive thread), clean up CRT.
;
_Stop:
        ; wake up the keep-alive thread
        mov     eax,dword ptr _stop_sema
        test    eax,eax
        jz      L$stop_deinit
        push    eax
        call    near ptr SignalLocalSemaphore
        add     esp,4
        push    dword ptr _stop_sema
        call    near ptr CloseLocalSemaphore
        add     esp,4
L$stop_deinit:
        push    0
        call    near ptr __deinit_environment
        add     esp,4
        ret

_TEXT   ENDS

CONST   SEGMENT DWORD PUBLIC USE32 'DATA'
CONST   ENDS

CONST2  SEGMENT DWORD PUBLIC USE32 'DATA'
CONST2  ENDS

_DATA   SEGMENT DWORD PUBLIC USE32 'DATA'
_argc:
        DB      0, 0, 0, 0
kNLMInfo:
        DB      4eH, 4cH, 4dH, 49H, 0, 0, 0, 0
        DB      3, 0, 0, 0, 8, 0, 0, 0
        DB      2, 0, 0, 0
_stop_sema:
        DB      0, 0, 0, 0
_DATA   ENDS

_BSS    SEGMENT PARA PUBLIC USE32 'BSS'
_BSS    ENDS

        END
