plustest_name = torture

!include testenv1.mif

objs = &
    assert.obj &
    jamstr.obj &
    newlog.obj &
    ordkeyma.obj &
    pool.obj

!ifdef test1
test_flags = -d2-j
alt_error=01
!else
test_flags = -ox-d1-j
alt_error=00
!endif

test_flags += -w0-I. -fi=__ialias.h

test_flags_386 = -mf
test_flags_i86 = -ml -fi=noinline.h
test_flags_axp =

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

opt_link = LIB tort.lib

exes = dim2arr.$(exe) mdarrtst.$(exe)

test : .symbolic start clean start_test test.lnk tort.lib $(exes)
    cat output.out >>$(%ERROR_FILE)
    @set ERROR_FILE=$(log_file)
    @set ERROR_MSG=FAILED
    $(noecho)%make save_sav
    diff $(sav_file) exec.chk
    $(noecho)%make clean_sav
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

start_test : .symbolic
    $(noecho)%make clean_sav
    @set ERROR_FILE=exec.out
    $(noecho)%create $(%ERROR_FILE)

tort.lib : tort.lbc $(objs)
    @%make set_path_wlib
    @%make echo_lib
    $(noecho)$(librarian) @$[@
    @%make set_old_path

tort.lbc : $(__MAKEFILES__)
    $(noecho)%create $^@
    @%append $^@ $^*.lib
    @for %i in ( $(objs) ) do @%append $^@ -+%i

save_sav : .procedure
    @if exist exec.out @cp exec.out $(sav_file) >$(nulldevice)
    @if exist exec.out @rm exec.out

clean_sav : .procedure
    @if exist $(sav_file) @rm $(sav_file)

clean : .procedure
    $(noecho)%make common_clean
