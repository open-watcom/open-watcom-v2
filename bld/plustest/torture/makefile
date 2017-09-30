plustest_name = torture

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

!include testenv1.mif

objs = &
    assert.obj &
    jamstr.obj &
    newlog.obj &
    ordkeyma.obj &
    pool.obj

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

opt_link = LIB tort.lib

exes = dim2arr.$(exe) mdarrtst.$(exe)

test : .symbolic start clean start_test test.lnk tort.lib $(exes) test_set_result
    @%make clean

test_set_result : .symbolic
    cat output.out >>$(%ERROR_FILE)
    $(noecho)%make common_set_result

start_test : .symbolic
    @%make common_start_test

tort.lib : tort.lbc $(objs)
    @%make set_path_wlib
    @%make echo_lib
    $(noecho)$(librarian) @$[@
    @%make set_old_path

tort.lbc : $(__MAKEFILES__)
    $(noecho)%create $^@
    @%append $^@ $^*.lib
    @for %i in ( $(objs) ) do @%append $^@ -+%i

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
