# pp11 has incorrect output
plustest_name = preproc
!include ..\environ.mif

comp = $(wpp_$(arch))
!ifdef test_cc
comp = $(cc)
!endif

.extensions:
.extensions: .pl0 .pl .plc .c

.c.pl:
    %append exec.out @@@ $^@ @@@
    $(comp) $[@ -zq-w0 -pl -fo=.pl
    ren exec.out tmp_exec.out
    copy /b tmp_exec.out+$^@ exec.out >nul
    del tmp_exec.out >nul

.c.plc:
    %append exec.out @@@ $^@ @@@
    $(comp) $[@ -zq-w0 -plc -fo=.plc
    ren exec.out tmp_exec.out
    copy /b tmp_exec.out+$^@ exec.out >nul
    del tmp_exec.out >nul

.c.pl0:
    %append exec.out @@@ $^@ @@@
    $(comp) $[@ -zq-w0 -pl -pw=0 -fo=.pl0
    ren exec.out tmp_exec.out
    copy /b tmp_exec.out+$^@ exec.out >nul
    del tmp_exec.out >nul
    
all_tests = &
pp01.pl &
pp02.pl &
pp03.pl &
pp04.pl &
pp04.pl0 &
pp05.plc &
pp06.plc &
pp07.plc &
pp08.plc &
pp09.plc &
pp10.plc &
pp11.plc &
pp12.plc &
pp13.pl &
pp14.plc &
pp15.pl0 &
pp16.plc &
pp17.plc &


test : .symbolic zap_output gen_input $(all_tests)
    diff exec.out exec.chk
    %make global
    %append $(log_file) PASS $(%__CWD__)
    
zap_output : .procedure
    %create exec.out
    if exist *.pl del *.pl
    if exist *.pl0 del *.pl0
    if exist *.plc del *.plc
    if exist *.gh del *.gh
    
gen_input: .procedure
    $(bld_cl) geninput.c -ox-d1 -cc++
    geninput
    del *.exe
    del *.obj
    
global : .symbolic
    %make zap_output
    %make common_clean
