plustest_name = diagnose
!include ..\environ.mif

diagnose_dir = source

!include $(diagnose_dir)\diagnose.mif

diagnose_opts += -ew -we -wx -e1000 -zq -eq
archdep_opts = -dARCH=$(arch) -fr=.ser 

.c: $(diagnose_dir)
.c.err:
    -$(wpp_$(arch)) $[@ @diag $(opt_$^&)
    @if not exist $^&.err echo I did not generate errors for $^& > $^&.err
.c.ser:
    -$(wpp_$(arch)) $[@ @diag $(opt_$^&) $(archdep_opts)
    @if not exist $^&.ser echo I did not generate errors for $^& > $^&.ser

test : .symbolic initopts test0000 test0100 test0200 test0300 testsys test_special
    %make global
    %append $(log_file) PASS $(%__CWD__)
    
initopts : .symbolic
    set diag=$(diagnose_opts)
    
test0000 : .symbolic diag0000.out
    -diff -b diag0000.out diag0000.chk >diff0000.out
    diff -b diff0000.out diff0000.chk
    
test0100 : .symbolic diag0100.out
    -diff -b diag0100.out diag0100.chk >diff0100.out
    diff -b diff0100.out diff0100.chk
    
test0200 : .symbolic diag0200.out
    -diff -b diag0200.out diag0200.chk >diff0200.out
    diff -b diff0200.out diff0200.chk
    
test0300 : .symbolic diag0300.out
    -diff -b diag0300.out diag0300.chk >diff0300.out
    diff -b diff0300.out diff0300.chk
    
testsys : .symbolic diag$(arch).out
    -diff -b diag$(arch).out diag$(arch).chk >diff$(arch).out
    diff -b diff$(arch).out diff$(arch).chk
    
test_special : .symbolic
    -$(wpp_$(arch)) $(diagnose_dir)\diag0001.c @diag $(opt_diag0001) -eq >diagspec.out
    -diff -b diagspec.out diagspec.chk >diffspec.out
    %make diag_clean
    diff -b diffspec.out diffspec.chk
    
diag0000.out : $(diags_0099)
    ..\..\bin\catfile /a *.err >diag0000.out
    %make diag_clean
    
diag0100.out : $(diags_0199)
    ..\..\bin\catfile /a *.err >diag0100.out
    %make diag_clean
    
diag0200.out : $(diags_0299)
    ..\..\bin\catfile /a *.err >diag0200.out
    %make diag_clean
    
diag0300.out : $(diags_0399)
    ..\..\bin\catfile /a *.err >diag0300.out
    %make diag_clean
    
diag386.out : $(diags_sys)
    ..\..\bin\catfile /a *.ser >diag386.out
    %make diag_clean
    
diagaxp.out : $(diags_sys)
    ..\..\bin\catfile /a *.ser >diagaxp.out
    %make diag_clean
    
diag_clean : .procedure
    if exist *.err del *.err
    if exist *.ser del *.ser
    if exist *.pch del *.pch
    
save : .symbolic
    if exist diff0000.out copy diff0000.out s$(arch)_0000.sav
    if exist diff0100.out copy diff0100.out s$(arch)_0100.sav
    if exist diff0200.out copy diff0200.out s$(arch)_0200.sav
    if exist diff0300.out copy diff0300.out s$(arch)_0300.sav
    if exist diff$(arch).out copy diff$(arch).out s$(arch)_$(arch).sav
    
global : .symbolic
    %make diag_clean
    %make common_clean
