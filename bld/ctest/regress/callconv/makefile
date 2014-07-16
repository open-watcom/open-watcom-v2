ctest_name = callconv

!include testenv.mif
!include source/callconv.mif

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
test_flags_i86 = -d1-oe-ms
test_flags_axp = -d1-oe
alt_error = 09
!else
test_flags_386 = -d1-ms
test_flags_i86 = -d1-ms
test_flags_axp = -d1
alt_error = 00
!endif

test_flags_386 += $(add_br_$(no_br_$^*))

.c: source

test : .symbolic start clean start_test $(callconv_exes) ./chk_exec.exe
    @set ERROR_MSG=symbol name failure
    diff -b $(%RUN_FILE) sym$(arch)$(alt_error).chk
    @set ERROR_FILE=error.out
    @set ERROR_MSG=failure to complete test stream
    %write $(%ERROR_FILE) OK
    $]@ exec.out >>$(%ERROR_FILE)
    diff -b $(%ERROR_FILE) error.chk
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

start_test : .symbolic test.lnk
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav
    @if exist x$(arch)_$(alt_error).sav rm x$(arch)_$(alt_error).sav
    @set ERROR_FILE=exec.out
    @set RUN_FILE=symbols.out
    %create $(%ERROR_FILE)
    %create $(%RUN_FILE)

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav
    @if exist symbols.out cp symbols.out x$(arch)_$(alt_error).sav

$(callconv_all_objs) : source/obj.c
    @set ERROR_MSG=failure to compile $^@
    @%make set_path_wcc
    $(wcc_$(arch)) $[@ $(test_flags) $(test_flags_$(arch)) $(opt_$^*) $(opt_EH_$(EH_$^*)) -fo=$^@
    @%make set_old_path
    %append $(%RUN_FILE) **** $^@ pubdef ****
    objxdef $^@ >>$(%RUN_FILE)
    %append $(%RUN_FILE) **** $^@ extdef ****
    objxref $^@ >>$(%RUN_FILE)

clean : .procedure
    @%make common_clean
