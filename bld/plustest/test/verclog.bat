@rem VERCLOG -- compare console log
@rem
@rem 91/07/05 -- J.W.Welch      -- defined
@rem
@sed -f ..\sed\stripvcl.sed __TEST__.LOG >__TEMP__.VCL
@clog __DIFF__.LOG
@if not exist %TEST_IN%\%TEST_FIL%.VCL goto DIFF_COP
:DIFF_IT
@diff -b __TEMP__.VCL %TEST_IN%\%TEST_FIL%.VCL >__DIFF__.LST
@if errorlevel 1 goto DIFF_IND
    @echo ** - no difference in console-log output
    @echo **
    @goto DIFF_END
:DIFF_IND
    @echo ** - CONSOLE-LOG DIFFERENCE DETECTED
    @echo **
    @type __DIFF__.LST
    @set TEST_DIF=TRUE
:DIFF_COP
    @echo ** - CONSOLE LOG FILE SAVED AS %TEST_OUT%\%TEST_FIL%.VCL
    @echo **
    @copy __TEMP__.VCL %TEST_OUT%\%TEST_FIL%.VCL >NUL
:DIFF_END
@clog
@call logsave __DIFF__.LOG
@if exist __DIFF__.LOG del __DIFF__.LOG >NUL
@if exist __DIFF__.LST del __DIFF__.LST >NUL
@if exist __TEMP__.VCL del __TEMP__.VCL >NUL
