echo off
echo %1 inline assembler test
set outfile=dif
if not [%2]==[] set outfile=%2

set cmplr=wcc386p
if not [%2]==[] set cmplr=%2
call do 486 %1 
call do bug %1 
call do imul %1 
call do lea %1 
call do misc %1 
call do misc2 %1 
call do modes6 %1 
call do pop %1 
call do xor %1 
%1 kernel.c /i=c:\dlang\qh

echo ================ differences from masm ========================
diff -b qtest.sam %cmplr%.dif
echo ================ differences from masm ========================
set cmplr=
