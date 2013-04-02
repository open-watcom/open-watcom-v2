#pmake: test

ftest_name = result

all_tests = test0 test1 test2 test3 test4 test5 test6 test7 test8 test9

!include testenv.mif

test : .symbolic clean start_test cd_nist $(all_tests) cd_back ./chktest.exe
    @%append $(log_file) ==== $(%__CTIME__) $(%__CWD__) ====
    $]@ $(log_file)
    %append $(log_file) TEST
    %append $(log_file) PASS $(%__CWD__)
    %make clean

start_test: .symbolic
    @if exist *.sav rm -f *.sav
    @if exist $(log_file) rm -f $(log_file)
    @set FTEST_LOG_FILE=$(log_file)

cd_nist : .symbolic
    cd nist

cd_back : .symbolic
    cd ..

$(all_tests) : .procedure
!ifdef %TEST_ARCH
    -wmake -h arch=$(%TEST_ARCH) $@=
    -@wmake -h arch=$(%TEST_ARCH) $@= save
!else
    -wmake -h $@=
    -@wmake -h $@= save
!ifdef %EXTRA_ARCH
    -wmake -h arch=$(%EXTRA_ARCH) $@=
    -@wmake -h arch=$(%EXTRA_ARCH) $@= save
!endif
!endif

clean : .procedure
    @%make common_clean
