echo %1 inline assembler test

set outfile=%1.out
call do 486 %1 %outfile%
call do bug %1 %outfile%
call do imul %1 %outfile%
call do lea %1 %outfile%
call do misc %1 %outfile%
call do misc2 %1 %outfile%
call do modes6 %1 %outfile%
call do pop %1 %outfile%
call do xor %1 %outfile%
call do fcom %1 %outfile%
%1 kernel.c /i=c:\lang\qh
wdisasm kernel /l
cat kernel.lst >> %outfile%
