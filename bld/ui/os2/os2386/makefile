#pmake: build cpu_386 os_os2
host_cpu = 386
host_os  = os2

!include ../os2.mif

sample2.exe : sample.obj sampdial.obj ui.lib $(__MAKEFILES__)
    $(noecho)%create $^&.lnk
    @%append $^&.lnk  system os2v2
    @%append $^&.lnk  option map
    @%append $^&.lnk  name sample2.exe
    @%append $^&.lnk  debug all
    @%append $^&.lnk  file sample
    @%append $^&.lnk  file sampdial
    @%append $^&.lnk  library ui
    @%make echo_link
    $(linker) @$^&.lnk
