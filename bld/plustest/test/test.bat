@rem TEST.BAT -- test a file
@rem
@rem 91/07/05 -- J.W.Welch      -- defined
@rem
@set OLD_BLIP=%WCGBLIPON%
@set TEST_CMD=run386 ..\dosi86\wcppi86d.exp
@if [%DEF_CPP%] == [] goto DEFAULTCPP
  @set TEST_CMD=%DEF_CPP%
:DEFAULTCPP
@set TEST_LINKER=wlinkp
@if [%DEF_LINKER%] == [] goto DEFAULT_LNKER
  @set TEST_LINKER=%DEF_LINKER%
:DEFAULT_LNKER
@set TEST_LNK=@linkpp.lnk
@if [%DEF_LNK%] == [] goto DEFAULTLNK
  @set TEST_LNK=%DEF_LNK%
:DEFAULTLNK
@set TEST_RUN=
@set TEST_RUNEXT=.EXE
@if [%DEF_RUN%] == [] goto DEFAULTRUN
  @set TEST_RUN=%DEF_RUN%
  @set TEST_RUNEXT=%DEF_RUNEXT%
:DEFAULTRUN
@set TEST_CMD=%TEST_CMD% -d1 -ox -os -zq
@set TEST_CDIR=%TEST_IN%
@set TEST_CON=
@set TEST_EXE=
@set TEST_ASM=
@set TEST_CALL=
@set TEST_FIL=
@set TEST_INP=
@set TEST_DIF=
@set TEST_PRE=
@set TEST_FO=.obj
:ARG_LOOP
    @if [%1] == [CONSOLE]  goto ARG_CONSOLE
    @if [%1] == [DISASM]   goto ARG_DISASM
    @if [%1] == [DISCALL]  goto ARG_DISCALL
    @if [%1] == [EXECUTE]  goto ARG_EXECUTE
    @if [%1] == [TESTFILE] goto ARG_TESTFILE
    @if [%1] == [SRCDIR]   goto  ARG_SRCDIR
    @if [%1] == [PREPROC]  goto  ARG_PREPROC
  :ARG_FILE
    @if not [%TEST_INP%] == [] goto ARG_OPTION
    @set TEST_INP=%1
    @if not [%TEST_FIL%] == [] goto ARG_SHIFT
    @set TEST_FIL=%1
    @goto ARG_SHIFT
  :ARG_OPTION
    @set TEST_CMD=%TEST_CMD% %1
    @goto ARG_SHIFT
  :ARG_CONSOLE
    @set TEST_CON=y
    @goto ARG_SHIFT
  :ARG_EXECUTE
    @set TEST_EXE=y
    @goto ARG_SHIFT
  :ARG_DISASM
    @set TEST_ASM=y
    @goto ARG_SHIFT
  :ARG_DISCALL
    @set TEST_CALL=y
    @goto ARG_SHIFT
  :ARG_TESTFILE
    @shift
    @set TEST_FIL=%1
    @goto ARG_SHIFT
  :ARG_SRCDIR
    @set TEST_CDIR=TESTSRC\C
    @goto ARG_SHIFT
  :ARG_PREPROC
    @set TEST_CMD=%TEST_CMD% -p
    @set TEST_FO=.ppo
    @set TEST_PRE=y
  :ARG_SHIFT
    @shift
    @if not [%1] == [] goto :ARG_LOOP
@set TEST_CMD=%TEST_CMD% -fo%TEST_OUT%\%TEST_FIL%%TEST_FO%
@set TEST_CMD=%TEST_CMD% %TEST_CDIR%\%TEST_INP%%TEST_EXT%
@rem
@rem -- do test
@rem
@clog __test__.log
@echo ** TESTING: %TEST_FIL%%TEST_EXT%
@echo ** cmd: %TEST_CMD%
    @%TEST_CMD%
    @if [%TEST_EXE%] == [] goto EXE_DONE
    @if errorlevel 1 goto EXE_DONE
@echo ** LINKING: %TEST_FIL%
    @%TEST_LINKER% %TEST_LNK% name %TEST_OUT%\%TEST_FIL% file %TEST_OUT%\%TEST_FIL%
@echo ** EXECUTING: %TEST_FIL%%TEST_RUNEXT%
    @%TEST_RUN% %TEST_OUT%\%TEST_FIL%%TEST_RUNEXT%
:EXE_DONE
@clog
@call logsave __test__.log
@if [%TEST_PRE%] == [] goto PRE_DONE
    @call verpp
:PRE_DONE
@if [%TEST_CON%] == [] goto CON_DONE
    @call verclog
:CON_DONE
@if [%TEST_ASM%] == [] goto ASM_DONE
    @call verasm
:ASM_DONE
@if [%TEST_CALL%] == [] goto CALL_DONE
    @call vercall
:CALL_DONE
@if [%TEST_DIF%] == [] goto NO_DIFF
    @echo DIFFERENCE IN %TEST_FIL% >> DIFF.LST
:NO_DIFF
@if exist __TEST__.LOG del __TEST__.LOG >NUL
@set TEST_CDIR=
@set TEST_CON=
@set TEST_EXE=
@set TEST_ASM=
@set TEST_CALL=
@set TEST_FIL=
@set TEST_INP=
@set TEST_DIF=
@set TEST_EXT=
@set WCGBLIPON=%OLD_BLIP%
