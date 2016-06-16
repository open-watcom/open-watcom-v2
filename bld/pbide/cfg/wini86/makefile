#pmake: build guitool os_win cpu_i86

all : pbidex.cfg pbide.cfg .symbolic

pbidex.cfg : $(idecfg_dir)/idex.sp
    *wsplice -k C_FOR_PB -k FOR_WIN $[@ $^@

pbide.cfg : $(idecfg_dir)/ide.sp
    *wsplice -k C_FOR_PB -k FOR_WIN $[@ $^@

clean : .symbolic
    rm -f *.cfg
