@rem TESTBEG.BAT -- initialization for a test stream
@rem
@rem 91/07/05 -- J.W.Welch      -- defined
@rem
@set TEST_IN=testin
@set TEST_OUT=testout
@if exist TEST.LOG erase TEST.LOG >NUL
@if exist DIFF.LST erase DIFF.LST >NUL
@if exist *.ERR erase *.ERR >NUL
@if not [%TEST_LOG%] == [] if exist %TEST_LOG% erase %TEST_LOG% >NUL
@if [%TEST_EXT%] == [] set TEST_EXT=.C
