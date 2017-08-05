plustest_name = getlong

!include testenv1.mif

!ifdef test1
test_flags = -d2
alt_error=01
!else
test_flags = -ox-d1
alt_error=00
!endif

stack_386=opt stack=8k
stack_i86=opt stack=4k
stack_axp=opt stack=8k

objs = &
    getlongo.obj &
    readme.obj

test : .symbolic start clean start_test ./opt.$(exe)
    %make save_sav
    diff $(sav_file) opt.chk
    %make clean_sav
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

./opt.$(exe) : $(objs) test.lnk
    @set ERROR_MSG=failure to link $^*
    @%make set_path_wlink
    @%make echo_link
    $(noecho)$(linker) @$]@ NAME $@ file {$(objs)}
    @set ERROR_MSG=failure to run $^*
    @%make set_path_rtdll
    $(run) $@ >>$(%ERROR_FILE)
    $(run) $@ -h >>$(%ERROR_FILE)
    -$(run) $@ -hopeless >>$(%ERROR_FILE)
    $(run) $@ -vers >>$(%ERROR_FILE)
    $(run) $@ -v >>$(%ERROR_FILE)
    -$(run) $@ -o >>$(%ERROR_FILE)
    -$(run) $@ -v=1 >>$(%ERROR_FILE)
    @%make set_old_path

start_test : .symbolic
    %make clean_sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)

save_sav : .procedure
    @if exist exec.out @cp exec.out $(sav_file) >$(nulldevice)
    @if exist exec.out @rm exec.out

clean_sav : .procedure
    @if exist $(sav_file) @rm $(sav_file)

clean : .procedure
    %make common_clean
