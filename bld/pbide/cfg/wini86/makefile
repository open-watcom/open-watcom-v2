#pmake: build os_win cpu_i86

all : pbidex.cfg pbide.cfg .symbolic

pbidex.cfg : ../../../viper/cfg/idex.mfg
    wsplice -k C_FOR_PB -k FOR_WIN $[@ $^@

pbide.cfg : ../../../viper/cfg/ide.mfg
    wsplice -k C_FOR_PB -k FOR_WIN $[@ $^@

clean : .symbolic
    rm -f *.cfg
