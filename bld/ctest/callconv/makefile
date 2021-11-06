ctest_name = callconv

test_flags = -we-w0 -Isource

!ifdef test1
test_flags_386 = -d2-mf
test_flags_i86 = -d2-mc-2
test_flags_axp = -d2-si
alt_error = 01
!else ifdef test2
test_flags_386 = -d1-otan-mf
test_flags_i86 = -d1-otan-mm
test_flags_axp = -d1-otan-si
alt_error = 02
!else ifdef test3
test_flags_386 = -d2-ms
test_flags_i86 = -d2-ml
test_flags_axp = -d2
alt_error = 03
!else ifdef test4
test_flags_386 = -d1-os-mf
test_flags_i86 = -d1-os-ms
test_flags_axp = -d1-os-si
alt_error = 04
!else ifdef test5
test_flags_386 = -d1-ei-mf
test_flags_i86 = -d1-ei-mc-2
test_flags_axp = -d1-ei
alt_error = 05
!else ifdef test6
test_flags_386 = -d2-mf
test_flags_i86 = -d2-mm
test_flags_axp = -d2
alt_error = 06
!else ifdef test7
test_flags_386 = -d1-ms-6s-fp6
test_flags_i86 = -d1-ml-3-fp3
test_flags_axp = -d1
alt_error = 07
!else ifdef test8
test_flags_386 = -d2-mf-zro
test_flags_i86 = -d2-ms-zro
test_flags_axp = -d2
alt_error = 08
!else ifdef test9
test_flags_386 = -d1-oe-zri-mf
test_flags_i86 = -d1-oe-zri-ms
test_flags_axp = -d1-oe
alt_error = 09
!else
test_flags_386 = -d1-ms
test_flags_i86 = -d1-ms
test_flags_axp = -d1
alt_error = 00
!endif

sav_file_2 = x$(arch)_$(alt_error).sav

!include testenv.mif
!include ../callconv.mif

.c: source

test : .symbolic start start_test test.lnk $(callconv_exes) test_set_result
    $(noecho)%make common_set_result_check

check_result : .procedure
    -diff -b error.out error.chk

test_set_result : .symbolic ./chk_exec.exe
    -diff -b $(%RUN_FILE) sym$(arch)$(alt_error).chk >error.out
    -%append error.out OK
    -$]@ exec.out >>error.out

start_test : .symbolic
    @set RUN_FILE=exec2.out
    %create $(%RUN_FILE)

$(callconv_all_objs) : source/obj.c
    @set ERROR_MSG=failure to compile $@
    @%make set_path_wcc
    @%make echo_cc
    $(noecho)$(wcc_$(arch)) $[@ $(test_flags) $(test_flags_$(arch)) $(opt_$^*) $(opt_EH_$(EH_$^*)) -fo=$@
    @%make set_old_path
    %append $(%RUN_FILE) **** $@ pubdef ****
    objxdef $@ >>$(%RUN_FILE)
    %append $(%RUN_FILE) **** $@ extdef ****
    objxref $@ >>$(%RUN_FILE)

clean : .procedure
    @%make echo_clean
    @%make common_clean
