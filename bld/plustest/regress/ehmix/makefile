plustest_name = ehmix
!include ../environ.mif

!ifdef __OS2__
check_file = os2386.chk
!else ifdef __LINUX__
check_file = linux386.chk
!else
check_file = nt386.chk
!endif

!ifdef test1
comp_flags = -d2-xs-hd $(br_opt)
alt_error=01
!else ifdef test2
comp_flags = -d2-xss-hd
alt_error=02
!else ifdef test3
comp_flags = -d2-xst-hd
alt_error=03
!else ifdef test4
comp_flags = -ox-d1-xst
alt_error=04
!else ifdef test5
comp_flags = -ox-d1-xss
alt_error=05
!else
comp_flags = -ox-d1-xs $(br_opt)
alt_error=00
!endif

comp_flags += -zq -xr

stack_386=opt stack=64k
stack_i86=opt stack=16k
stack_axp=opt stack=64k

.cpp.obj:
    $(wpp_$(arch)) $[@ $(comp_flags) -fo=.obj

.obj.exe:
    $(linker) @ehmix.lnk name $@ file $[@ 
    $(run) $(exec_prefix)$@ >>exec.out

# not posible to compile it for 16-bit due to 
# ehmix.c requires more than 64kB memory for code
#
!ifeq arch i86
test : .symbolic start_test
!else
test : .symbolic start_test ehmix.lnk ehmix.exe
    diff exec.out $(check_file)
!endif
    @%make global
    %append $(log_file) PASS $(%__CWD__)

ehmix.lnk : $(__MAKEFILES__)
    %create $^@
    @%append $^@ name $^*
    @%append $^@ $(ldebug_$(arch))
    @%append $^@ $(lnk_$(arch))
    @%append $^@ $(stack_$(arch))

start_test : .symbolic
    %create exec.out
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

global : .SYMBOLIC
    @%make common_clean
