@rem VERASM -- disassemble object file and compare
@rem
@rem 91/07/05 -- J.W.Welch      -- defined
@rem
@clog __DIFF__.LOG
@wdisasm %TEST_OUT%\%TEST_FIL%.obj -m-s%TEST_IN%\%TEST_FIL%%TEST_EXT% >__ASM__.OUT
@sed -f ..\sed\stripasm.sed __ASM__.OUT >__ASM__.LOG
@if not exist %TEST_IN%\%TEST_FIL%.VAS goto DIFF_COP
@diff __ASM__.LOG %TEST_IN%\%TEST_FIL%.VAS >__DIFF__.LST
@if errorlevel 1 goto DIFF_IND
    @echo ** - no difference in disassembled file
    @echo **
    @goto DIFF_END
:DIFF_IND
    @echo ** - DISASSEMBLY DIFFERENCE DETECTED
    @echo **
    @type __DIFF__.LST
    @set TEST_DIF=TRUE
:DIFF_COP
    @echo ** - DISASSEMBLY FILE SAVED AS %TEST_OUT%\%TEST_FIL%.VAS
    @echo **
    @copy __ASM__.LOG %TEST_OUT%\%TEST_FIL%.VAS >NUL
:DIFF_END
@clog
@call logsave __DIFF__.LOG
@if exist __ASM__.OUT  del __ASM__.OUT  >NUL
@if exist __ASM__.LOG  del __ASM__.LOG  >NUL
@if exist __DIFF__.LOG del __DIFF__.LOG >NUL
@if exist __DIFF__.LST del __DIFF__.LST >NUL
