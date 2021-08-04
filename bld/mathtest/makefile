mathtest_name = result

!include testenv.mif

test : ./chktest.exe .symbolic
    set MATHTEST_LOG_FILE=$(log_file)
    $(noecho)%make full_set_result

clean: .symbolic
    @%make echo_clean
    @if exist chktest.exe $(noecho)rm chktest.exe
    @if exist chktest.obj $(noecho)rm chktest.obj
    @if exist chktest.map $(noecho)rm chktest.map
