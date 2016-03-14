ctest_name = regress

!include testenv1.mif

test : .symbolic ./chktest.exe
    @set ERROR_FILE=error.out
    @set ERROR_MSG=
    @%append $(log_file) ==== $(%__CTIME__) $(%__CWD__) ====
    $]@ $(log_file)
    %make clean
    %append $(log_file) TEST
    %append $(log_file) PASS $(%__CWD__)

del_log : .symbolic
    rm -f $(log_file)

clean : .symbolic
    %make common_clean
