# pp11 has incorrect output
plustest_name = preproc

!include testenv.mif

alt_error=00

testcomp =  $(wpp_$(arch))

.c.pl:
    %append exec.out @@@ $^@ @@@
    $(testcomp) $[@ -w0 -pl -fo=.pl -na
    cat exec.out $^@ >tmp_exec.out
    cp tmp_exec.out exec.out

.c.plc:
    %append exec.out @@@ $^@ @@@
    $(testcomp) $[@ -w0 -plc -fo=.plc -na
    cat exec.out $^@ >tmp_exec.out
    cp tmp_exec.out exec.out

.c.pl0:
    %append exec.out @@@ $^@ @@@
    $(testcomp) $[@ -w0 -pl -pw=0 -fo=.pl0 -na
    cat exec.out $^@ >tmp_exec.out
    cp tmp_exec.out exec.out

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


test : .symbolic clean start_test $(all_tests)
    diff exec.out exec.chk
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

start_test : .symbolic
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)
    %make gen_input

zap_output : .procedure
    @if exist *.pl rm *.pl
    @if exist *.pl0 rm *.pl0
    @if exist *.plc rm *.plc
    @if exist *.gh rm *.gh

gen_input: .procedure ./geninput.exe
    $[@
    @rm *.exe
    @rm *.obj

./geninput.exe : geninput.c
    $(bld_cl) $(bcl_cl_opts) -cc++ $<

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

clean : .procedure
    @%make zap_output
    @%make common_clean
