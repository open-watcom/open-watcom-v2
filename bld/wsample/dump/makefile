.ERASE

!ifdef on_build_machine
debug_info =
!else
debug_info = debug line
!endif

.BEFORE:
    @set include=$(%include);$(watcom_dir)\h;$(watcom_dir)\os21x_h;$(dig_dir)\h

.c.obj:
    $(comp386) $[@ -mf -d1 -ox -w4

smpdump.exe : smpdump.obj
    wlink NAME $^@ $(debug_info) FILE $[@
