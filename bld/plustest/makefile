#pmake: test

plustest_name = result

!include testenv1.mif

test : ./chktest.exe .symbolic
    set PLUSTEST_LOG_FILE=$(log_file)
    @set ERROR_FILE=$(log_file)
    @set ERROR_MSG=FAILED
    @%append $(log_file) ==== $(%__CTIME__) $(%__CWD__) ====
    $]@ $(log_file)
    %append $(log_file) TEST
    %append $(log_file) PASS $(%__CWD__)
    
clean: .symbolic
    @%null