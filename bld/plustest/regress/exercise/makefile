plustest_name = exercise
!include ..\environ.mif

exes = &
chkcl.$(exe) &
ctordtor.$(exe) &
mp.$(exe) &
ovldall.$(exe) &
switcher.$(exe) &

!ifdef test1
cflags += -d2-xds
!else ifdef test2
cflags += -d2-xss
!else ifdef test3
cflags += -d2-xdt
!else ifdef test4
cflags += -d2-xst
!else ifdef test5
cflags += -ox-d1-xds
!else ifdef test6
cflags += -ox-d1-xss
!else ifdef test7
cflags += -ox-d1-xdt
!else ifdef test8
cflags += -d2-xs
add_br_ = -br
!else ifdef test9
cflags += -ox-d1-xs
add_br_ = -br
!else
cflags += -ox-d1-xst
!endif

cflags += -zq-w0

!ifdef %plustest_bad_br
no_br_ctordtor = no
no_br_switcher = no
!endif

!ifeq arch 386
cflags += $(add_br_$(no_br_$^*))
!endif

.cpp.obj:
    $(wpp_$(arch)) $[@ $(cflags)
    
.obj.$(exe):
    wlink @test FILE $[@

test : .symbolic test.lnk $(exes)
    %create exercise.out
    @for %i in ( $(exes) ) do $(run) %i >>exercise.out
    diff exercise.out exercise.chk
    %make global
    %append $(log_file) PASS $(%__CWD__)
    
test.lnk : $(__MAKEFILES__)
    %create $^@
    @%append $^@ DEBUG line
    @%append $^@ $(lnk_$(arch))
    @%append $^@ OPTION map, caseexact, stack=16k, quiet

global : .SYMBOLIC
    @%make common_clean
