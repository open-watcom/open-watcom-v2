#pmake: build os_os2 cpu_386

idex.cfg : ../idex.mfg
    wsplice -k FOR_OS2 $[@ $^@

clean : .symbolic
    rm -f *.cfg
