@fastdel testout >nul
@md testout >nul
@copy testmsg.mdl testout >nul
@copy testmsgf.bat testout >nul
@..\bin\msgstrm ..\gml\messages.gml testout\testmsg.mdl /b=%1 /e=%2 /d=testout
@call testbeg.bat
@del testout\error.ref >nul
@call testout\testmsg.bat
@if exist testin\error.ref goto DODIFF
    @echo **** No ERROR.REF in TESTIN directory
    @goto DONE
:DODIFF
@diff -b testin\error.ref testout\error.ref >testout\diagnose.out
@if errorlevel 1 goto DIFFS
    @echo *** NO DIFFERENCES ***
    @goto DONE
:DIFFS
    @echo *** DIFFERENCES DETECTED ***
    type testout\diagnose.out
    @echo *** DIFFERENCES DETECTED ***
:DONE
