plustest_name = dcmat

!include testenv1.mif

objs = &
    compmat.obj &
    cosft.obj &
    doubmat.obj &
    four1.obj &
    realft.obj &
    svdcmp.obj

!ifdef test1
test_flags = -d2
alt_error=01
!else
test_flags = -ox-d1
alt_error=00
!endif

test_flags += -e10 -w0 -D__ATT_complex__ -I.

test_flags_386 =
test_flags_i86 = -mm -zm
test_flags_axp =

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

opt_link = LIBRARY matrix.lib

exes = test.$(exe) tmatrix.$(exe)

test : .symbolic start clean start_test matrix.lib test.lnk $(exes)
    @set ERROR_FILE=$(log_file)
    @set ERROR_MSG=FAILED
    $(noecho)%make save_sav
    diff $(sav_file) test.chk
    $(noecho)%make clean_sav
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

start_test : .symbolic
    $(noecho)%make clean_sav
    @set ERROR_FILE=exec.out
    $(noecho)%create $(%ERROR_FILE)

matrix.lib : $(objs) matrix.lbc
    @%make set_path_wlib
    @%make echo_lib
    $(noecho)$(librarian) $@ @matrix.lbc
    @%make set_old_path

matrix.lbc : makefile
    $(noecho)%create $@
    @for %i in ( $(objs) ) do @%append $@ -+%i

save_sav : .procedure
    @if exist exec.out @cp exec.out $(sav_file) >$(nulldevice)
    @if exist exec.out @rm exec.out

clean_sav : .procedure
    @if exist $(sav_file) @rm $(sav_file)

clean : .procedure
    $(noecho)%make common_clean
