#pmake: build os_nt cpu_axp

idex.cfg : ../idex.mfg
    wsplice -k FOR_NT -k FOR_AXP $[@ $^@

clean : .symbolic
    rm -f *.cfg
