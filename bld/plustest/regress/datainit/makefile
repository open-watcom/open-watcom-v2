plustest_name = datainit
!include ..\environ.mif

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

cflags += -ox-d1-zq-w0

.cpp.obj:
    $(wpp_$(arch)) $[@ $(cflags)
    
.obj.$(exe):
    wlink @idata FILE $[@

test : .symbolic idata.lnk $(exes)
    %create idata.out
    @for %i in ( $(exes) ) do $(run) %i >>idata.out
    diff idata.out idata.chk
    %make global
    %append $(log_file) PASS $(%__CWD__)
    
idata.lnk : $(__MAKEFILES__)
    %create $^@
    @%append $^@ DEBUG line
    @%append $^@ $(lnk_$(arch))
    @%append $^@ OPTION map, caseexact, stack=16k, quiet

global : .SYMBOLIC
    @%make common_clean
