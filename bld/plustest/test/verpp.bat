@rem VERPP -- compare preprocessor output
@rem
@rem 93/04/16 -- J.W.Welch      -- defined
@rem
@clog __DIFF__.LOG
@if not exist %TEST_IN%\%TEST_FIL%.PPO goto DIFF_COP
:DIFF_IT
@diff -b %TEST_OUT%\%TEST_FIL%.PPO %TEST_IN%\%TEST_FIL%.PPO >__DIFF__.LST
@if errorlevel 1 goto DIFF_IND
    @echo ** - no difference in preprocessor output
    @echo **
    @goto DIFF_END
:DIFF_IND
    @echo ** - PREPROCESSOR DIFFERENCE DETECTED
    @echo **
    @type __DIFF__.LST
    @set TEST_DIF=TRUE
:DIFF_COP
    @echo ** - NEW PREPROCESSOR FILE: %TEST_OUT%\%TEST_FIL%.PPO
    @echo **
:DIFF_END
@clog
@call logsave __DIFF__.LOG
@if exist __DIFF__.LOG del __DIFF__.LOG >NUL
@if exist __DIFF__.LST del __DIFF__.LST >NUL
