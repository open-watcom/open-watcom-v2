plustest_name = datainit
!include ../environ.mif

exes = &
idata01.$(exe) &
idata02.$(exe) &
idata03.$(exe) &
idata04.$(exe) &
idata05.$(exe) &
idata06.$(exe) &
idata07.$(exe) &
idata08.$(exe) &
idata09.$(exe) &
idata10.$(exe) &
idata11.$(exe) &
idata12.$(exe) &
idata13.$(exe) &
idata14.$(exe) &
idata15.$(exe) &
idata16.$(exe) &
idata17.$(exe) &
idata18.$(exe) &
idata19.$(exe) &
idata20.$(exe) &
idata21.$(exe) &
idata22.$(exe) &
idata23.$(exe) &
idata24.$(exe) &
idata25.$(exe) &
idata26.$(exe) &
idata27.$(exe) &
idata28.$(exe) &
idata29.$(exe) &
idata30.$(exe) &
idata31.$(exe) &
idata32.$(exe) &
idata33.$(exe) &
idata34.$(exe) &
idata35.$(exe) &
idata36.$(exe) &
idata37.$(exe) &
idata38.$(exe) &
idata39.$(exe) &
idata40.$(exe) &
idata41.$(exe) &
idata42.$(exe)

test_cflags += -ox-d1-zq-w0
alt_error=00

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

.cpp.obj:
    $(wpp_$(arch)) $[@ $(test_cflags) -fo=.obj

.obj.$(exe):
    $(linker) @idata.lnk name $@ file $[@
    $(run) $(exec_prefix)$@ >>idata.out

test : .symbolic start_test idata.lnk $(exes)
    diff idata.out idata.chk
    @%make global
    %append $(log_file) PASS $(%__CWD__)

start_test : .symbolic
    %create idata.out
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

idata.lnk : $(__MAKEFILES__)
    %create $^@
    @%append $^@ DEBUG line
    @%append $^@ $(lnk_$(arch))
    @%append $^@ $(stack_$(arch))

save : .symbolic
    @if exist idata.out cp idata.out s$(arch)_$(alt_error).sav

global : .procedure
    @%make common_clean
