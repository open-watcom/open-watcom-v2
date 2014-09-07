        name    timer

extrn "C",_Extender : byte

_text segment byte public 'code'

_data segment byte public 'data'
        meg1    dw      0
_data ends

dgroup group _data

assume cs:_text,ds:_data

;
;       GrabTimer() - grab the 18.2/sec timer interrupt
;

        public  GrabTimer_
GrabTimer_ proc near
        ret                             ; - return to caller
GrabTimer_ endp

;
;       ReleTimer_
;       - give back timer
;

        public  ReleTimer_
ReleTimer_ proc near
        ret                             ; - return to caller
        ret
ReleTimer_ endp

FIRST_MEG_SEL   equ     60H
BIOS_CLOCK      equ     46CH            ; 40:6C

;
;       uint    GetTickCnt(void)
;       - return the BIOS time of day counter
;

public  GetTickCnt_
GetTickCnt_ proc near
        cmp     ds:_Extender,1         ; rational?
        jne     pharlap
        mov     eax,BIOS_CLOCK
        mov     eax,[eax]               ; ds maps first meg directly
        ret
pharlap:
        mov     ax,meg1
        test    ax,ax
        jne     havesel
        push    ecx
        push    ebx
        sub     esp,512
        mov     ebx,esp
        mov     ecx,esp
        mov     ax,2526h                ; ask pharlap for selector for 1st meg
        int     21h
        mov     ax,108h[ebx]
        mov     meg1,ax
        add     esp,512
        pop     ebx
        pop     ecx
havesel:
        push    ds
        mov     ds,ax
        mov     eax,BIOS_CLOCK
        mov     eax,[eax]
        pop     ds
        ret
GetTickCnt_ endp

_text ends

        end
