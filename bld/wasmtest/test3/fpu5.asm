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
