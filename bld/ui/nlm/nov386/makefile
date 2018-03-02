#pmake: nobuild cpu_386 os_nov
host_cpu = 386
host_os  = nov

!include ../../ui.mif

sample2.exe : sample.obj sampdial.obj ui.lib
    %write sample.lnk  sys novell 'Sample2 NLM'
    %append sample.lnk  name sample2
    %append sample.lnk  debug all debug novell
    %append sample.lnk  option map, caseexact, stack = 10k, quiet

    %append sample.lnk  module clib
    %append sample.lnk  import @$(nwoss_dir)/common/imp/ownwos.imp

    %append sample.lnk  file sample
    %append sample.lnk  file sampdial

    %append sample.lnk  library ../$(LIBNAME)

    wlink @sample.lnk
