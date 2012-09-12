#pmake/50: build os_dos os_os2 os_win os_nt os_linux os_nov os_osi os_qnx

all : .symbolic wlsystem.lnk qnx.lnk linux.lnk

wlsystem.lnk : specs.sp
    wsplice -k Pwlsystem $< $@

qnx.lnk : wlqnx.lnk wlsystem.lnk
    cat $< >$@

linux.lnk : wlbinl.lnk wlsystem.lnk
    cat $< >$@

clean : .symbolic
    rm -f wlsystem.lnk qnx.lnk linux.lnk
