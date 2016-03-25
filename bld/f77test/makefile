#pmake: test

ftest_name = result

test_grp_list = nist

nist_tests = test0 test1 test2 test3 test4 test5 test6 test7 test8 test9

all_tests = $(nist_tests)

!include testenv1.mif

test : .symbolic start clean start_test cd_regress $(test_grp_list) cd_back ./chktest.exe
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
    @set FTEST_LOG_FILE=$(log_file)

cd_regress : .symbolic
    cd regress

cd_back : .symbolic
    cd ..

$(test_grp_list) : .symbolic
    cd $@
    @for %%a in ($($@_tests)) do -@%make %%a
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
