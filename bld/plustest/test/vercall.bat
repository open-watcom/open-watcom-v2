@rem VERCALL -- disassemble object file and compare call statements
@rem
@rem 91/11/30 -- G.R.Bentz      -- modified from verasm.bat
@rem
@clog __DIFF__.LOG
@wdisasm %TEST_OUT%\%TEST_FIL%.obj -s%TEST_IN%\%TEST_FIL%%TEST_EXT% >__ASM__.TMP
@wgrep -f -l call __ASM__.TMP | sed -f ..\sed\stripvac.sed >__ASM__.LOG
@if not exist %TEST_IN%\%TEST_FIL%.VAC goto DIFF_COP
@diff __ASM__.LOG %TEST_IN%\%TEST_FIL%.VAC >__DIFF__.LST
@if errorlevel 1 goto DIFF_IND
    @echo ** - no difference in disassembled call file
    @echo **
    @goto DIFF_END
:DIFF_IND
    @echo ** - DISASSEMBLY CALL DIFFERENCE DETECTED
    @echo **
    @type __DIFF__.LST
    @set TEST_DIF=TRUE
:DIFF_COP
    @echo ** - DISASSEMBLY CALL FILE SAVED AS %TEST_OUT%\%TEST_FIL%.VAC
    @echo **
    @copy __ASM__.LOG %TEST_OUT%\%TEST_FIL%.VAC >NUL
:DIFF_END
@clog
@call logsave __DIFF__.LOG
@if exist __ASM__.TMP  del __ASM__.TMP  >NUL
@if exist __ASM__.LOG  del __ASM__.LOG  >NUL
@if exist __DIFF__.LOG del __DIFF__.LOG >NUL
@if exist __DIFF__.LST del __DIFF__.LST >NUL
