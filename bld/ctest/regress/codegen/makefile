ctest_name = codegen

!include testenv1.mif
!include source/codegen.mif

test_flags = -we-w0 -Isource

!ifdef test1
test_flags_386 = -d2-mf
test_flags_i86 = -d2-mc-2
test_flags_axp = -d2-si
add_br_ = $(br_opt)
alt_error = 01
!else ifdef test2
test_flags_386 = -d1-oxtan-mf
test_flags_i86 = -d1-oxtan-mm
test_flags_axp = -d1-oxtan-si
alt_error = 02
!else ifdef test3
test_flags_386 = -d2-ms
test_flags_i86 = -d2-ml
test_flags_axp = -d2
alt_error = 03
!else ifdef test4
test_flags_386 = -d1-oxs-mf
test_flags_i86 = -d1-oxs-ms
test_flags_axp = -d1-oxs-si
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
!else ifdef test10
test_flags_386 = -d2-mf-fpc
test_flags_i86 = -d2-mc-2-fpc
test_flags_axp = -d2-si
add_br_ = $(br_opt)
alt_error = 10
!else ifdef test11
test_flags_386 = -d1-oxtan-ms-fpc
test_flags_i86 = -d1-oxtan-ms-fpc
test_flags_axp = -d1-oxtan-si
alt_error = 11
!else
test_flags_386 = -ox-d1-ms
test_flags_i86 = -ox-d1-ms
test_flags_axp = -ox-d1
alt_error = 00
add_br_ = $(br_opt)
!endif

test_flags_386 += -we-w0 -I"$(codegen_dir)" $(run_quietly)
test_flags_i86 += -we-w0 -I"$(codegen_dir)" $(run_quietly)
test_flags_axp += -we-w0 -I"$(codegen_dir)" $(run_quietly)

test_flags_386 += $(add_br_$(no_br_$^*))

.c: source

test : .symbolic start clean start_test $(codegen_exes) ./chk_exec.exe
    @set ERROR_FILE=error.out
    @set ERROR_MSG=failure to complete test stream
    %write $(%ERROR_FILE) OK
    $]@ exec.out >> $(%ERROR_FILE)
    diff -b $(%ERROR_FILE) error.chk
    %append $(log_file) PASS $(%__CWD__)
    %make clean

start_test : .symbolic test.lnk
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

clean : .procedure
    @if exist failure rm -f failure
    @%make common_clean
