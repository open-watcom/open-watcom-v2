@del __msgs__.log >nul
@call testmsgt.bat msg000.tst
@call testmsgt.bat msg100.tst
@call testmsgt.bat msg200.tst
@call testmsgt.bat msg300.tst
@call testmsgt.bat msg400.tst
@call testmsgt.bat msg500.tst
@echo **************** COMPLETED MESSAGES.GML TEST *****************
@call testdarc.bat messages.tst
@copy __msgs__.log testout\error.ref
@del __msgs__.log >nul
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
