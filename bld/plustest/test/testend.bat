@rem TESTEND.BAT -- cleanup after test
@rem
@rem 91/07/05 -- J.W.Welch      -- defined
@rem
@if exist diff.lst goto errs
@   echo *******************
@   echo All tests succeeded
@   echo *******************
@   goto done
:errs
@   type diff.lst
:done
@set TEST_EXT=
@set TEST_LOG=
@set TEST_IN=
@set TEST_OUT=
@set TEST_CMD=
