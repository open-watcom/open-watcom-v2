#pmake: build os_nt os_win os_os2 cpu_386 cpu_axp
all : .symbolic IDE PBIDE
IDE : win\idex.cfg nt\idex.cfg ide.cfg .SYMBOLIC
PBIDE : win\pbidex.cfg nt\pbidex.cfg win\pbide.cfg nt\pbide.cfg .SYMBOLIC

win\idex.cfg : win\idex.mfg
        wsplice -r C_FOR_PB win\idex.mfg win\idex.cfg

nt\idex.cfg : nt\idex.mfg
        wsplice -r C_FOR_PB nt\idex.mfg nt\idex.cfg

win\pbidex.cfg : win\idex.mfg
        wsplice -k C_FOR_PB -r C_FOR_PB_NT win\idex.mfg win\pbidex.cfg

nt\pbidex.cfg : nt\idex.mfg
        wsplice -k C_FOR_PB -k C_FOR_PB_NT nt\idex.mfg nt\pbidex.cfg

ide.cfg : ide.mfg
        wsplice -r C_FOR_PB ide.mfg ide.cfg

win\pbide.cfg : ide.mfg
        wsplice -k C_FOR_PB -r C_FOR_PB_NT ide.mfg win\pbide.cfg

nt\pbide.cfg : ide.mfg
        wsplice -k C_FOR_PB -k C_FOR_PB_NT ide.mfg nt\pbide.cfg

