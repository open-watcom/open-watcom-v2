ftest_name = positive
!include ../environ.mif

positive_dir = source

!include $(positive_dir)/positive.mif

!ifdef test1
fflags_386 = -D2 -MF
fflags_i86 = -D2 -MM -2
fflags_axp = -d2 -si
alt_error = 01
!else ifdef test2
fflags_386 = -D2 -MF -DEBug
fflags_i86 = -D2 -MM -DEBug
fflags_axp = -d2-si
alt_error = 02
!else ifdef test3
fflags_386 = -D2 -MS -FPC
fflags_i86 = -D2 -ML -FPC
fflags_axp = -d2
alt_error = 03
!else ifdef test4
fflags_386 = -D2 -MF -6
fflags_i86 = -D2 -ML -6
fflags_axp = -d2-si
alt_error = 04
!else ifdef test5
fflags_386 = -D1 -MF
fflags_i86 = -D1 -MM -2
fflags_axp = -d1
alt_error = 05
!else ifdef test6
fflags_386 = -D1 -MF -FPI87
fflags_i86 = -D1 -MM -FPI87
fflags_axp = -d1
alt_error = 06
!else ifdef test7
fflags_386 = -D1 -MS -DEBug -4
fflags_i86 = -D1 -ML -DEBug -4
fflags_axp = -d1
alt_error = 07
!else ifdef test8
fflags_386 = -D1 -MF
fflags_i86 = -D1 -ML -SSfloats
fflags_axp = -d1
alt_error = 08
!else ifdef test9
fflags_386 = -D1 -OI -MF
fflags_i86 = -D1 -OI -MM
fflags_axp = -d1-oe
alt_error = 09
!else
fflags_386 = -OX -D1 -MS
fflags_i86 = -OX -D1 -ML
fflags_axp = -ox-d1
alt_error = 00
!endif

!ifdef __LINUX__
exec_prefix = ./
!else
exec_prefix =
!endif

!ifndef noisy
run_quietly = -Quiet
!endif

fflags_386 += -INCPath=$(positive_dir) $(run_quietly) #-we-w0
fflags_i86 += -INCPath=$(positive_dir) $(run_quietly) #-we-w0
fflags_axp += -INCPath=$(positive_dir) $(run_quietly) #-we-w0

.error
    @%append $(%error_file) $(%error_msg)

.for: $(positive_dir)
.for.obj:
    @set error_msg=failure to compile $^@
    $(wfc_$(arch)) $[@ $(fflags_$(arch)) $(opt_$^*) -FO=.obj

.obj.$(exe):
    @set error_msg=failure to link $^@
    $(linker) @test.lnk $(opt_link_$^*) FILE $[@ NAME $^@
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $(exec_prefix)$^@ >> $*.out
    diff $*.out expected/$*.exp >> exec.out
    rm $[@
    rm $*.out
#    wstrip -q $^@

test : .symbolic start_test test.lnk $(positive_exes) finish_test
    
./chk_exec.exe : ../c/chk_exec.c
    $(bld_cl) $< $(wcl_util_opts)

finish_test : .symbolic ./chk_exec.exe
    @set error_file=error.out
    @set error_msg=failure to complete test stream
    echo OK > error.out
    $[@ exec.out >> error.out
    diff -b error.out error.chk
    %make global
    %append $(log_file) PASS $(%__CWD__)

fnstat1.$(exe) : fnstat1a.obj fnstat1b.obj
    @set error_msg=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE $[@, $]@
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $(exec_prefix)$^@ >>exec.out

start_test : .symbolic
    rm -f s$(arch)_$(alt_error).sav
    %create exec.out
    @set error_file=exec.out

test.lnk : makefile
    %create $^@
    @%append $^@ $(ldebug_$(arch))
    @%append $^@ $(lnk_$(arch))
    @%append $^@ OPTION noredef

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

global : .symbolic
    @if exist for007 rm for007
    @if exist for009 rm for009
    @if exist for024 rm for024
    @if exist FM411.DAT rm FM411.DAT
    @%make common_clean
