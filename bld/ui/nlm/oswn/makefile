MODEL = S
!ifdef comp386
WCCEXE=$(comp386)
!else
WCCEXE=wcc386
!endif
WCPPEXE=wcpp386
INCLUDES=/I..\h
FORM=NLM
EXE=nlm

.before
    set include=$(%WATCOM)\novh;$(watcom_nw40_h);$(watcom_h);$(%WATCOM)\h;$(%include)

!include ..\makefile

sample2.$(EXE) : sample.obj sampdial.obj ..\$(LIBNAME).lib
    %create sample.lnk
    %append sample.lnk  name sample2
    %append sample.lnk  format novell 'sample2 nlm'
    %append sample.lnk  debug all debug novell
    %append sample.lnk  module clib
    %append sample.lnk  option map, caseexact, stack = 10k, quiet
    %append sample.lnk  libpath %WATCOM%\lib386
    %append sample.lnk  libpath %WATCOM%\lib386\netware

    %append sample.lnk  import @%WATCOM%\novi\clib.imp
    %append sample.lnk  import @..\uinlm.imp

    %append sample.lnk  file sample
    %append sample.lnk  file sampdial

    %append sample.lnk  library ..\$(LIBNAME)

    set lib=$(watcomlib);$(%lib)

    wlink @sample.lnk

