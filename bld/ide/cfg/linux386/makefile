#pmake: build os_linux cpu_386

idex.cfg : ../idex.mfg
    wsplice -k FOR_LINUX $[@ $^@

clean : .symbolic
    rm -f *.cfg
