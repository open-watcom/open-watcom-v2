plustest_name = dcmat

!include testenv.mif

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
    diff $(%ERROR_FILE) test.chk
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

start_test : .symbolic
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)

matrix.lib : $(objs) matrix.lbc
    $(librarian) $@ @matrix.lbc

matrix.lbc : makefile
    %create $@
    @for %i in ( $(objs) ) do @%append $@ -+%i

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

clean : .procedure
    @%make common_clean
