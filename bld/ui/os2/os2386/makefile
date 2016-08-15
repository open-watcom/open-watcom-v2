#pmake: build cpu_386 os_os2
host_cpu = 386
host_os  = os2

!include ../os2.mif

sample2.exe : sample.obj sampdial.obj ui.lib $(__MAKEFILES__)
    @%create sample.lnk
    @%append sample.lnk  system os2v2
    @%append sample.lnk  option map
    @%append sample.lnk  name sample2.exe
    @%append sample.lnk  debug all
    @%append sample.lnk  file sample
    @%append sample.lnk  file sampdial
    @%append sample.lnk  library ui
    @%make echo_link
    $(linker) @sample.lnk
