#pmake: build os_win cpu_i86

all : pbidex.cfg pbide.cfg .symbolic

pbidex.cfg : $(idecfg_dir)/idex.mfg
    wsplice -k C_FOR_PB -k FOR_WIN $[@ $^@

pbide.cfg : $(idecfg_dir)/ide.mfg
    wsplice -k C_FOR_PB -k FOR_WIN $[@ $^@

clean : .symbolic
    rm -f *.cfg
