;/=================================================================\
;|                                                                 |
;|      OS/2 Physical Device Driver Segment Ordering               |
;|                  for Open Watcom C/C++                          |
;|                                                                 |
;\=================================================================/


;; DEVSEGS.ASM
;;
;; Segment declarations for OS/2 PDDs.
;;

;; Declare segments in the correct order

              .386p
              .seq

_HEADER     segment word public use16 'DATA'
_HEADER     ends

CONST       segment word public use16 'DATA'
CONST       ends

CONST2      segment word public use16 'DATA'
CONST2      ends

_DATA       segment word public use16 'DATA'
_DATA       ends

_BSS        segment word public use16 'BSS'
_BSS        ends

_INITDATA   segment word public use16 'INITDATA'
_INITDATA   ends


_TEXT       segment word public use16 'CODE'
_TEXT       ends

_INITCODE   segment word public use16 'CODE'
_INITCODE   ends


DGROUP      group _HEADER, CONST, CONST2, _DATA, _BSS,_INITDATA


_DATA       segment word public use16 'DATA'
_DATA       ends


_TEXT       segment word public use16 'CODE'
            assume cs:_TEXT, ds:DGROUP, es:NOTHING, ss:NOTHING
_TEXT       ends


_INITDATA    segment word public use16 'INITDATA'
             public _OffFinalDS

_OffFinalDS  db 0
_INITDATA    ends


_INITCODE    segment word public use16 'CODE'
             public _OffFinalCS

_OffFinalCS  proc near
             int 3
_OffFinalCS  endp

_INITCODE    ends

             end
