#pmake: build os_dos os_nt os_win os_os2 os_linux cpu_i86 cpu_386 cpu_axp

ide.cfg : ide.mfg
    wsplice $[@ $^@

clean : .symbolic
    rm -f ide.cfg
