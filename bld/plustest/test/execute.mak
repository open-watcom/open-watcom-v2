# EXECUTION control
#
# 93/06/23 -- J.W.Welch         -- defined
# 94/02/24 -- J.W.Welch         -- use cpp_test_dir

!ifndef c_files
c_files     = testin\;$(cpp_test_dir)\testsrc\c\
!endif

!ifndef cpp_files
cpp_files   = testin\
!endif

!include $(cpp_test_dir)\tstream.mak

all: .symbolic $(testerrs)
        @if exist testout\*.obj @del testout\*.obj
        @if exist testout\*.err @catfile testout\*.err >testout\error.cat
        @if exist testout\*.err @del testout\*.err
        @if exist testout\*.lnk @del testout\*.lnk
        @wtouch testout\error.cat
        @-$(plusplus_dir)\bin\sed $(plusplus_dir)\sed\filt_err.sed testout\error.cat testout\vcl.ref
        @del testout\error.cat
        @wtouch testout\vcl.ref
        @if exist testout\*.vcl @catfile testout\*.vcl >>testout\vcl.ref
        @if exist testout\*.ppo @catfile testout\*.ppo >>testout\vcl.ref
        @-diff testout\vcl.ref testin\vcl.ref >testout\diagnose.out
        @echo **** EXERCISE Test Stream Compilation Differences:
        @-type testout\diagnose.out
        @echo **** EXERCISE Test Stream Completed

global: .symbolic
        @-if exist testout\*.err @del testout\*.err >nul
        @-if exist testout\*.obj @del testout\*.obj >nul
        @-if exist testout\*.ppo @del testout\*.ppo >nul
        @-if exist testout\*.ref @del testout\*.ref >nul
        @-if exist testout\*.cat @del testout\*.cat >nul
        @-if exist testout\*.out @del testout\*.out >nul
        @-if exist testout\*.exe @del testout\*.exe >nul
        @-if exist testout\*.exp @del testout\*.exp >nul
        @-if exist testout\*.vcl @del testout\*.vcl >nul
