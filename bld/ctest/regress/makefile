ctest_name = regress

all : .symbolic test

!include testenv.mif

test : .symbolic ./chktest.exe
    @cdsay >> $(log_file)
    $]@ $(log_file)
    %make global
    %append $(log_file) TEST
    %append $(log_file) PASS $(%__CWD__)

del_log : .symbolic
    rm -f $(log_file)

global : .symbolic
    %make common_clean
