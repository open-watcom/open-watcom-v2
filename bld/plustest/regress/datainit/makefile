plustest_name = datainit

!include testenv.mif

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

test_flags += -ox-d1-w0
alt_error=00

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

test : .symbolic clean start_test test.lnk $(exes)
    diff exec.out idata.chk
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

start_test : .symbolic
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

clean : .procedure
    @%make common_clean
