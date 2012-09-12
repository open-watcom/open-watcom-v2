@echo %verbose% off
call bone.cmd %owsrcdir%\bench\watcom.0 -3r-fp3-oaxs
call bone.cmd %owsrcdir%\bench\watcom.1 -6r-fp3-oaxs
call bone.cmd %owsrcdir%\bench\watcom.2 -6r-fp6-oaxt
call bone.cmd %owsrcdir%\bench\watcom.3 -6r-fp3-oaxt
