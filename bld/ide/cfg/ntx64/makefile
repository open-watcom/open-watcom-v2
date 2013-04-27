#pmake: build os_nt cpu_x64

idex.cfg : ../idex.mfg
    wsplice -k FOR_NT $[@ $^@

clean : .symbolic
    rm -f *.cfg
