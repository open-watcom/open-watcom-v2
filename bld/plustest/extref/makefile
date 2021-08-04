plustest_name = extref

!ifdef test1
test_flags = -d2
extref_eh = -xss
alt_error=01
!else ifdef test2
test_flags = -ot-d1
extref_eh = -xst
alt_error=02
!else ifdef test3
test_flags = -d2
extref_eh = -xst
alt_error=03
!else
test_flags = -ot-d1
extref_eh = -xss
alt_error=00
!endif

!ifneq arch axp
# until we implement it under AXP
test_flags += -zv
!endif
test_flags += -I"../positive/source"

!include testenv.mif

!ifneq arch axp
# until we implement it under AXP
link_flags = OPTION eliminate, vfremoval
!endif

stack_386=opt stack=8k
stack_i86=opt stack=4k
stack_axp=opt stack=8k

#
# extra flags for a module
#
opt_er01 = $(extref_eh)
opt_er01b = $(extref_eh)
opt_er01c = $(extref_eh)
opt_er01d = $(extref_eh)
opt_er03 = $(extref_eh)
opt_er03b = $(extref_eh)
opt_er03c = $(extref_eh)

opt_link_er01 = LIB er01.lib
opt_link_er02 = FILE er02b.obj
opt_link_er03 = LIB er03.lib

exes = er01.$(exe) er02.$(exe) er03.$(exe)

test : .symbolic start test.lnk er01.lib er02b.obj er03.lib $(exes) test_set_result
    $(noecho)%make common_set_result_check

check_result : .procedure
    -diff -b error.out error.chk >$(nulldevice)

test_set_result : ./chk_exec.exe .symbolic
    -$(noecho)%write error.out OK
    -$]@ exec.out >>error.out

er01.lib : er01b.obj er01c.obj er01d.obj
    @%make set_path_wlib
    @%make echo_lib
    $(noecho)$(librarian) $@ $?
    @%make set_old_path

er03.lib : er03b.obj er03c.obj
    @%make set_path_wlib
    @%make echo_lib
    $(noecho)$(librarian) $@ +er03b.obj +er03c.obj
    @%make set_old_path

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
