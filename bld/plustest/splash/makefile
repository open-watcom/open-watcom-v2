plustest_name = splash

!include testenv1.mif

objs = &
assoc.obj &
chgfnt.obj &
cislog.obj &
justify.obj &
regex.obj &
splash.obj &
spltest.obj &
tracer.obj &
tstio.obj &
xcl.obj

!ifdef test1
test_flags = -d2
alt_error=01
!else ifdef test2
test_flags = -ox-d1
alt_error=02
!else
test_flags = -ox-d1 -oe1000
alt_error=00
!endif
test_flags += -w0-dTEST

test_flags_386 = $(test_flags) -3s
test_flags_i86 = $(test_flags) -mm
test_flags_axp = $(test_flags)

stack_386=opt stack=64k
stack_i86=opt stack=8k
stack_axp=opt stack=64k

extra_run_justify = 13 <justify.cpp

exes = &
    splassoc.$(exe) &
    justify.$(exe) &
    spltest.$(exe)

test : .symbolic start clean start_test test.lnk $(exes)
    @set ERROR_FILE=$(log_file)
    @set ERROR_MSG=FAILED
    $(noecho)%make save_sav
    diff $(sav_file) exec.chk
    $(noecho)%make clean_sav
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

spltest.$(exe) : spltest.obj splash.obj regex.obj
splassoc.$(exe) : splassoc.obj splash.obj regex.obj
justify.$(exe) : justify.obj splash.obj regex.obj

start_test : .symbolic
    $(noecho)%make clean_sav
    @set ERROR_FILE=exec.out
    $(noecho)%create $(%ERROR_FILE)

regex.obj : regex.c
    @%make set_path_wcc
    $(wcc_$(arch)) $[* $(test_flags_$(arch)) -fo=.obj
    @%make set_old_path

save_sav : .procedure
    @if exist exec.out @cp exec.out $(sav_file) >$(nulldevice)
    @if exist exec.out @rm exec.out

clean_sav : .procedure
    @if exist $(sav_file) @rm $(sav_file)

clean : .procedure
    $(noecho)%make common_clean
