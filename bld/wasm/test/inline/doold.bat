@echo off
echo %1 ================================================================
if [cmplr] == [] set cmplr = %2
%cmplr% %1 /s /ot /zm /zq
wdisasm %1 /l /a
keystuff :so\bfixlst.vi\nZZ vi.exe %1.lst -d -q
rev %1.lst
diff -b %1.asm %1.lst >> %cmplr%.dif
