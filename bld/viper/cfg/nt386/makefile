#pmake: build os_nt cpu_386

idex.cfg : ../idex.mfg
    wsplice -k FOR_NT $[@ $^@

clean : .symbolic
    rm -f *.cfg
