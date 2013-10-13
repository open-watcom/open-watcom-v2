plustest_name = regress

!include testenv.mif

test : .symbolic ./chktest.exe
    @set ERROR_FILE=error.out
    @set ERROR_MSG=
    @%append $(log_file) ==== $(%__CTIME__) $(%__CWD__) ====
    $]@ $(log_file)
    @%make global
    %append $(log_file) TEST
    %append $(log_file) PASS $(%__CWD__)

del_log : .symbolic
    @if exist $(log_file) rm -f $(log_file)

global : .symbolic
    @%make common_clean
