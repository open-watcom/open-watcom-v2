.286
        name    emu8087

ifdef QNX16
__QNX__         equ     1
endif
ifdef QNX32
__QNX__         equ     1
endif

_TEXT   segment word public 'CODE'
        assume  cs:_TEXT

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

xref            macro   dsym
                endm

endproc         macro   dsym
         dsym   endp
                endm

endmod          macro
_TEXT           ends
                endm

include struct.inc
include xception.inc
include fstatus.inc
include cmpconst.inc
include fpe86.inc
include e86flda.inc
include e86fldc.inc
include e86fldd.inc
include e86fldm.inc
include e86ldi4.inc
include e86ldi8.inc
include e86i4ld.inc
include e86fdld.inc
include e86ldfs.inc
include e86ldfd.inc
include e86fsld.inc
include e86i8ld.inc
include e86round.inc
include e86atan.inc
include e86fprem.inc
include e86fxam.inc
include e86log.inc
include sindata.inc
include e86sin.inc
include e86f2xm1.inc
include e86sqrt.inc
include e86poly.inc

        endmod
        end
