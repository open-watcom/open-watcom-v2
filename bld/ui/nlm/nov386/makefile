#pmake: cpu_386 os_nov
host_CPU = 386
host_OS = nov
extra_files = $(nlm)

!include ../nov.mif

sample2.$(EXE) : sample.obj sampdial.obj ui.lib
    %create sample.lnk
    %append sample.lnk  name sample2
    %append sample.lnk  format novell 'sample2 nlm'
    %append sample.lnk  debug all debug novell
    %append sample.lnk  module clib
    %append sample.lnk  option map, caseexact, stack = 10k, quiet
    %append sample.lnk  libpath %WATCOM%/lib386
    %append sample.lnk  libpath %WATCOM%/lib386/netware

    %append sample.lnk  import @%WATCOM%/novi/clib.imp
    %append sample.lnk  import @../uinlm.imp

    %append sample.lnk  file sample
    %append sample.lnk  file sampdial

    %append sample.lnk  library ../$(LIBNAME)

    set lib=$(watcomlib);$(%lib)

    wlink @sample.lnk

