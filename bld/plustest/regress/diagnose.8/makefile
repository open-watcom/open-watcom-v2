#
# 8.0.1 branch diagnostic test stream
#
plustest_name = diag8
!include ..\environ.mif

diagnose_dir = ..\diagnose\source

!include $(diagnose_dir)\diagnose.mif
!ifeq arch 386
opt_diag0253  += -ei	# make Intel match the AXP compiler for now
!endif

diagnose_opts += -ew -we -wx -e1000 -eq
archdep_opts = -dARCH=$(arch) -fr=.ser 

.c: $(diagnose_dir)
.c.err:
    -$(wpp_$(arch)) $[@ $(diagnose_opts) -zq $(opt_$^&)
    @if not exist $^&.err echo I did not generate errors for $^& > $^&.err

.c.ser:
    -$(wpp_$(arch)) $[@ $(diagnose_opts) -zq $(opt_$^&) $(archdep_opts)
    @if not exist $^&.ser echo I did not generate errors for $^& > $^&.ser

test : .symbolic test0000 test0100 test0200 testsys
    %make global
    %append $(log_file) PASS $(%__CWD__)
    
test0000 : .symbolic diag0000.out
    -diff diag0000.out diag0000.chk >diff0000.out
    diff diff0000.out diff0000.chk
    
test0100 : .symbolic diag0100.out
    -diff diag0100.out diag0100.chk >diff0100.out
    diff diff0100.out diff0100.chk
    
test0200 : .symbolic diag0200.out
    -diff diag0200.out diag0200.chk >diff0200.out
    diff diff0200.out diff0200.chk
    
testsys : .symbolic diag$(arch).out
    -diff diag$(arch).out diag$(arch).chk >diff$(arch).out
    diff diff$(arch).out diff$(arch).chk
    
diag0000.out : $(diags_0099)
    ..\..\bin\catfile /a *.err >diag0000.out
    del *.err
    
diag0100.out : $(diags_0199)
    ..\..\bin\catfile /a *.err >diag0100.out
    del *.err
    
diag0200.out : $(diags_0299)
    ..\..\bin\catfile /a *.err >diag0200.out
    del *.err
    
diag386.out : $(diags_sys)
    ..\..\bin\catfile /a *.ser >diag386.out
    del *.ser
    
diagaxp.out : $(diags_sys)
    ..\..\bin\catfile /a *.ser >diagaxp.out
    del *.ser
    
diag_fc : .symbolic
    %create diagnose.fc
    for %i in ($(diags_0099_c)) do %append diagnose.fc source\%i $(opt_%i)
    -$(wpp_$(arch)) $(diagnose_opts) -fc=diagnose.fc -k >nul
    ..\..\bin\catfile /a *.err >diag0000.out
    del *.err
    %create diagnose.fc
    for %i in ($(diags_0199_c)) do %append diagnose.fc source\%i $(opt_%i)
    copy diagnose.fc jww.fc
    -$(wpp_$(arch)) $(diagnose_opts) -fc=diagnose.fc -k >nul
    ..\..\bin\catfile /a *.err >diag0100.out
    del *.err
    %create diagnose.fc
    for %i in ($(diags_0299_c)) do %append diagnose.fc source\%i $(opt_%i)
    -$(wpp_$(arch)) $(diagnose_opts) -fc=diagnose.fc -k >nul
    ..\..\bin\catfile /a *.err >diag0200.out
    del *.err
    %create diagnose.fc
    for %i in ($(diags_sys_c)) do %append diagnose.fc source\%i $(opt_%i)
    -$(wpp_$(arch)) $(diagnose_opts) -fc=diagnose.fc $(archdep_opts) -k >nul
    ..\..\bin\catfile /a *.ser >diag$(arch).out
    del *.ser
    
save : .symbolic
    if exist diff0000.out copy diff0000.out s$(arch)_0000.sav
    if exist diff0100.out copy diff0100.out s$(arch)_0100.sav
    if exist diff0200.out copy diff0200.out s$(arch)_0200.sav
    if exist diff$(arch).out copy diff$(arch).out s$(arch)_$(arch).sav
    
global : .symbolic
    if exist *.ser del *.ser
    %make common_clean
