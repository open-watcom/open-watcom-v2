plustest_name = regress
!include environ.mif

test : .symbolic
    $(bld_cl) $(plustest_dir)\c\chktest.c -ox-d1
    chktest $(log_file)
    %make global
    %append $(log_file) PASS $(%__CWD__)

del_log : .symbolic
    del $(log_file)

global : .symbolic
    %make common_clean
