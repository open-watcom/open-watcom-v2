wasmtest_name = result

!include testenv1.mif

test : .symbolic ./chktest.exe
    set WASMTEST_LOG_FILE=$(log_file)
    $(noecho)%make set_fail
    @%append $(log_file) ==== $(%__CTIME__) $(%__CWD__) ====
    $]@ $(log_file)
    %append $(log_file) TEST
    $(noecho)%make set_pass

clean: .symbolic
    @%make echo_clean
    @if exist chktest.exe $(noecho)rm chktest.exe
    @if exist chktest.obj $(noecho)rm chktest.obj
    @if exist chktest.map $(noecho)rm chktest.map
