#pmake: build os_nt cpu_386

all : pbidex.cfg pbide.cfg .symbolic

pbidex.cfg : ../../../viper/cfg/idex.mfg
    wsplice -k C_FOR_PB -k FOR_NT $[@ $^@

pbide.cfg : ../../../viper/cfg/ide.mfg
    wsplice -k C_FOR_PB -k FOR_NT $[@ $^@

clean : .symbolic
    rm -f *.cfg
