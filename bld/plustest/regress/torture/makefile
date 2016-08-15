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
    diff $(%ERROR_FILE) exec.chk
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

start_test : .symbolic
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)

tort.lib : tort.lbc $(objs)
    @%make set_path_wlib
    @%make echo_lib
    $(librarian) @$[@
    @%make set_old_path

tort.lbc : $(__MAKEFILES__)
    %create $^@
    @%append $^@ $^*.lib
    @for %i in ( $(objs) ) do @%append $^@ -+%i

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

clean : .procedure
    @%make common_clean
