#pmake: test

wasmtest_name = result

all_tests = test1 test2 test3 ideal diag

!include environ.mif

test : .symbolic start_test $(all_tests) finish_test

start_test: .symbolic
    set WASMTEST_NAME=$(wasmtest_name)
    rm -f *.sav
    @%make global

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

./chktest.exe : $(watcom_dir)/c/chktest.c
    $(bld_cl) $< $(wcl_util_opts)

finish_test : .symbolic ./chktest.exe
    @cdsay >> $(log_file)
    $]@ $(log_file)
    %append $(log_file) TEST
    %append $(log_file) PASS $(%__CWD__)

global : .symbolic
    %make common_clean
