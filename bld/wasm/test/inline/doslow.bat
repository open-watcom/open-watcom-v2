@echo off
set cmplr=wcc386p
if not [%2]==[] set cmplr=%2
echo %1 ================================================================
if exist %1.c del %1.c
keystuff :r\b%1.asm\n:so\btoc.vi\n:0r\bcinit.vi\n:w\b%1.c\n:q!\n vi.exe -d -q
%cmplr% %1 /s /ot /zm /zq
wdisasm %1 /l /a
rem keystuff :so\bfixlst.vi\n:so\brev.vi\nZZ vi.exe %1.lst -d -q
keystuff :so\bfixlst.vi\nZZ vi.exe %1.lst -d -q
rev %1.lst
diff -b %1.asm %1.lst > %1.%4
if [%3] == [clean] del %1.obj
if [%3] == [clean] del %1.lst
if [%3] == [clean] del %1.c
set cmplr=
