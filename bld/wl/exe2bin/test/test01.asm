; test01.asm
; a very simple DOS executable with a "hand-made" header
;
; wasm    -wx test01.asm
; wlink   format dos com file test01.obj name test01.exe
;
; yes, this needs to be linked as a COM file!

hdr_size    equ   0030h
hdr_sizep   equ   0003h

_HEADER     segment  para  public   'data'

       db    'M', 'Z'             ; signature
       dw    hdr_size + exe_end   ; size mod 512
       dw    0001h                ; # of pages
       dw    0001h                ; # of relocs
       dw    hdr_sizep            ; size of header (paras)
       dw    1000h                ; minimum alloc
       dw    8000h                ; maximum alloc
       dw    0400h                ; initial ss
       dw    0000h                ; initial sp
       dw    0000h                ; checksum
       dw    0000h                ; initial ip
       dw    0000h                ; initial cs
       dw    0020h                ; offset to reltab
       dw    0000h                ; overlay number

       org   0020h
       dd    00000001h

_HEADER     ends


_TEXT       segment  para  public   'code'

    exe_start:
       mov   ax, 0
       mov   ds, ax
       mov   dx, offset msg
       mov   ah, 09h
       int   021h
       mov   ax, 04C00h
       int   021h

    msg:
       db    "Hello, World!$"

    exe_end:

_TEXT       ends

end
