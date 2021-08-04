plustest_name = splash

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

test_flags_386 = -3s
test_flags_i86 = -mm
test_flags_axp =

opt_link_spltest = FILE { splash.obj regex.obj }
opt_link_splassoc = FILE { splash.obj regex.obj }
opt_link_justify = FILE { splash.obj regex.obj }

!include testenv.mif

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

stack_386=opt stack=64k
stack_i86=opt stack=8k
stack_axp=opt stack=64k

extra_run_justify = 13 <justify.cpp

exes = &
    splassoc.$(exe) &
    justify.$(exe) &
    spltest.$(exe)

test : .symbolic start test.lnk splash.obj regex.obj $(exes)
    $(noecho)%make common_set_result

regex.obj : regex.c
    @%make set_path_wcc
    $(wcc_$(arch)) $[* $(test_flags) $(test_flags_$(arch)) -fo=.obj
    @%make set_old_path

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
