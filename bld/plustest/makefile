#pmake: test

plustest_name = result

!include testenv1.mif

test : .symbolic start clean start_test ./chktest.exe
    @set ERROR_FILE=error.out
    @set ERROR_MSG=
    @%append $(log_file) ==== $(%__CTIME__) $(%__CWD__) ====
    $]@ $(log_file)
    %append $(log_file) TEST
    %append $(log_file) PASS $(%__CWD__)
    %make clean
    
start_test: .symbolic
#    @if exist *.sav rm -f *.sav
#    @if exist $(log_file) rm -f $(log_file)
    set PLUSTEST_LOG_FILE=$(log_file)

clean : .procedure
    %make common_clean
