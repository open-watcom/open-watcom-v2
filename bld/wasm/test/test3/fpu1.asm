; Notes on the FPU tests:
;
; - By default, wasm generates code with support for 8087 software emulation
;   (-fpi). That is different from MASM/TASM which default to no emulation.
;   It's not clear why wasm is different in this respect.
; - When 8087 emulation is used, FWAIT is prefixed by a NOP. Additionally,
;   FRSTOR and FLDENV and all non-control FPU instructions get a WAIT prefix
;   even on 286 and later.
; - When emulation is not used (-fpi87) and 286 or higher CPU is selected,
;   FRSTOR, FLDENV and non-control FPU instructions get no WAIT prefix. Note
;   that this behavior depends on the CPU, not FPU selection, since the CPU
;   type determines how the FPU is connected (and combinations like .386 with
;   .8087 are technically impossible).

.8086
.8087

S1 segment public 'DATA'
   assume ds:S1
   assume cs:S1
status dw 0
S1 ends

S2 segment public 'CODE'
   assume ds:S1
   assume cs:S2

   FLD tbyte ptr ss:[bx + 1]
   FCLEX
   FNCLEX
   FINIT
   FNINIT
   FDISI
   FNDISI
   FENI
   FNENI
   FSAVE   ss:status
   FNSAVE  ss:status
   FRSTOR  ss:status
   FSTENV  ss:status
   FNSTENV ss:status
   FLDENV  ss:status
   FSTCW   ss:status
   FNSTCW  ss:status
   FSTSW   ss:status
   FNSTSW  ss:status
;   FSTSW   AX
;   FNSTSW  AX
   FWAIT

S2 ends

end
