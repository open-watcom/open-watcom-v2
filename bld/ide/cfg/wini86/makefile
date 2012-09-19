#pmake: build os_win cpu_i86

idex.cfg : ../idex.mfg
    wsplice -k FOR_WIN $[@ $^@

clean : .symbolic
    rm -f *.cfg
