clibtest_name = result

!include testenv1.mif

test : ./chktest.exe .symbolic
    set CLIBTEST_LOG_FILE=$(log_file)
    $(noecho)%make set_fail
    @%append $(log_file) ==== $(%__CTIME__) $(%__CWD__) ====
    $]@ $(log_file)
    %append $(log_file) TEST
    $(noecho)%make set_pass

clean: .symbolic
    @if exist chktest.exe rm chktest.exe
    @if exist chktest.obj rm chktest.obj
    @if exist chktest.map rm chktest.map
