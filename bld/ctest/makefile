#pmake: test

ctest_name = result

test_grp_list = positive diagnose callconv inline codegen

!inject test0   all gr1 gr2 gr3
!inject test1   all     gr2 gr3
!inject test2   all     gr2
!inject test3   all     gr2
!inject test4   all     gr2
!inject test5   all     gr2
!inject test6   all     gr2
!inject test7   all     gr2
!inject test8   all     gr2
!inject test9   all     gr2
# next two should be used for testing of codegen with fpc option
#!inject test10  all
#!inject test11  all

positive_tests = $(gr2)
codegen_tests  = $(all)
callconv_tests = $(gr2)
diagnose_tests = $(gr1)
inline_tests   = $(gr3)

!include regress/environ.mif

test : .symbolic global start_test $(test_grp_list) finish_test global

start_test: .symbolic
    @if exist *.sav rm -f *.sav
    @if exist *.log rm -f *.log
    set CTEST_NAME=$(ctest_name)
    cd regress

cd_back : .symbolic
    cd ..

./chktest.exe : $(watcom_dir)/c/chktest.c
    $(bld_cl) $< $(wcl_util_opts)

finish_test : .symbolic cd_back ./chktest.exe
    @cdsay >> $(log_file)
    $]@ $(log_file)
    @%append $(log_file) TEST
    %append $(log_file) PASS $(%__CWD__)

$(test_grp_list) : .symbolic
    cd $@
    @for %%a in ($($@_tests)) do -@%make %%a
    cd ..

$(all) : .procedure
    -@wmake -h global
!ifdef %TEST_ARCH
    -wmake -h arch=$(%TEST_ARCH) $@=
    -@wmake -h arch=$(%TEST_ARCH) $@= save
!else
    -wmake -h $@=
    -@wmake -h $@= save
!ifdef %EXTRA_ARCH
    -@wmake -h global
    -wmake -h arch=$(%EXTRA_ARCH) $@=
    -@wmake -h arch=$(%EXTRA_ARCH) $@= save
!endif
!endif
    -@wmake -h global

global : .procedure
    @if exist *.obj rm -f *.obj
    @if exist *.exe rm -f *.exe
