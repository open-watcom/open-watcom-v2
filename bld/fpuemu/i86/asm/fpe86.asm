PAGE ,120
.286
        name    fpe86

ifdef QNX16
__QNX__         equ     1
endif
ifdef QNX32
__QNX__         equ     1
endif

_TEXT    segment word public 'CODE'

modstart        macro   modname
                endm

xdefp           macro   xsym
ifdef _DEBUG
                public  xsym
endif
                endm

defp            macro   dsym
         dsym   proc    near
                endm

xrefp           macro   dsym
                extrn   dsym:near
                endm

endproc         macro   dsym
         dsym   endp
                endm

endmod          macro
_TEXT           ends
                endm

.xlist
include struct.inc
.list
.xall
include fpe86.inc

_TEXT   ends
        end
