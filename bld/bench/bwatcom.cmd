@echo %verbose% off
call bone.cmd %devdir%\bench\watcom.0 -3r-fp3-oaxs
call bone.cmd %devdir%\bench\watcom.1 -6r-fp3-oaxs
call bone.cmd %devdir%\bench\watcom.2 -6r-fp6-oaxt
call bone.cmd %devdir%\bench\watcom.3 -6r-fp3-oaxt
