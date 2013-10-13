#pmake: test

wasmtest_name = result

all_tests = test1 test2 test3 ideal diag

!include testenv.mif

test : .symbolic start clean start_test $(all_tests) ./chktest.exe
    @set ERROR_FILE=error.out
    @set ERROR_MSG=
    @%append $(log_file) ==== $(%__CTIME__) $(%__CWD__) ====
    $]@ $(log_file)
    %append $(log_file) TEST
    %append $(log_file) PASS $(%__CWD__)
    %make clean

start_test: .symbolic
    @if exist *.sav rm -f *.sav
    @if exist $(log_file) rm -f $(log_file)
    @set WASMTEST_LOG_FILE=$(log_file)

$(all_tests) : .symbolic
    cd $@
!ifdef %TEST_ARCH
    -wmake -h arch=$(%TEST_ARCH)
!else
    -wmake -h
!ifdef %EXTRA_ARCH
    -wmake -h arch=$(%EXTRA_ARCH)
!endif
!endif
    cd ..

clean : .procedure
    @%make common_clean
