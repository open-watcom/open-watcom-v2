plustest_name = extref
!include ../environ.mif

!ifdef test1
test_cflags = -d2
extref_eh = -xss
alt_error=01
!else ifdef test2
test_cflags = -ot-d1
extref_eh = -xst
alt_error=02
!else ifdef test3
test_cflags = -d2
extref_eh = -xst
alt_error=03
!else
test_cflags = -ot-d1
extref_eh = -xss
alt_error=00
!endif

!ifneq arch axp
# until we implement it under AXP
test_cflags += -zv
!endif
test_cflags += -zq -I"../positive/source"

stack_386=opt stack=8k
stack_i86=opt stack=4k
stack_axp=opt stack=8k

lib = $(librarian_$(arch)) -q-n-b

#
# extra flags for a module
#
extra_er01a = $(extref_eh)
extra_er01b = $(extref_eh)
extra_er01c = $(extref_eh)
extra_er01d = $(extref_eh)
extra_er03a = $(extref_eh)
extra_er03b = $(extref_eh)
extra_er03c = $(extref_eh)

.c.obj:
    $(wpp_$(arch)) $[@ $(test_cflags) $(extra_$^*) -fo=.obj

exes = er01.$(exe) er02.$(exe) er03.$(exe)

test : .symbolic start_test $(exes)
    diff test.out test.chk
    @%make global
    %append $(log_file) PASS $(%__CWD__)

start_test : .symbolic
    %create test.out
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

er01.$(exe) : er01a.obj er01.lib er.lnk
    $(linker) @er.lnk NAME $^@ FILE er01a.obj LIBRARY er01
    $(run) $(exec_prefix)$@ >>test.out

er01.lib : er01b.obj er01c.obj er01d.obj
    $(lib) $^@ $?

er02.$(exe) : er02a.obj er02b.obj er.lnk
    $(linker) @er.lnk NAME $^@ FILE er02a.obj, er02b.obj
    $(run) $(exec_prefix)$@ >>test.out

er03.$(exe) : er03c.obj er03.lib er.lnk
    $(linker) @er.lnk NAME $^@ FILE er03c.obj LIB er03.lib
    $(run) $(exec_prefix)$@ >>test.out

er03.lib : er03a.obj er03b.obj
    $(lib) $^@ +er03a.obj +er03b.obj

er.lnk : makefile
    %create $^@
    @%append $^@ $(stack_$(arch))
!ifneq arch axp
# until we implement it under AXP
    @%append $^@ OPTION eliminate, vfremoval
!endif
    @%append $^@ $(lnk_$(arch))
    @%append $^@ $(ldebug_$(arch))

save : .symbolic
    @if exist test.out cp test.out s$(arch)_$(alt_error).sav

global : .symbolic
    @%make common_clean
