@echo off
set cmplr=wcc386p
echo %1 ================================================================
if exist %1.c del %1.c
keystuff :r\b%1.asm\n:so\btoc.vi\n:0r\bcinit.vi\n:w\b%1.c\n:q!\n vi.exe -d -q
