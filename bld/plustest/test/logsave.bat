@rem LOGSAVE -- save a partial console log
@rem
@rem 91/07/06 -- J.W.Welch      -- defined
@rem
@if [%TEST_LOG%] == [] goto DFLT_LOG
    @if not exist %TEST_LOG% goto INIT_LOG
        @copy %TEST_LOG%+%1 ___NEW__.LOG >nul
        @del %TEST_LOG%
        @rename ___NEW__.LOG %TEST_LOG%
        @goto DONE_LOG
    :INIT_LOG
        @copy %1 %TEST_LOG%
        @goto DONE_LOG
:DFLT_LOG
    @if not exist TEST.LOG goto INIT_DFLT
        @copy TEST.LOG+%1 ___NEW__.LOG >nul
        @del TEST.LOG >NUL
        @rename ___NEW__.LOG TEST.LOG
        @goto DONE_LOG
    :INIT_DFLT
        @copy %1 TEST.LOG
:DONE_LOG
