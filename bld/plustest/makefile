#pmake: test

plustest_name = result

!include testenv1.mif

test : ./chktest.exe .symbolic
    set PLUSTEST_LOG_FILE=$(log_file)
    $(noecho)%make set_fail
    @%append $(log_file) ==== $(%__CTIME__) $(%__CWD__) ====
    $]@ $(log_file)
    %append $(log_file) TEST
    $(noecho)%make set_pass

clean: .symbolic
    @%null
