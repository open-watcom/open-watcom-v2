CFLAGS = -d2 -ml
mkmk.exe: mkmk.obj preproc.obj ppexpr.obj ppmacro.obj ppmem.obj
        wlink name mkmk debug all file mkmk,preproc,ppexpr,ppmacro,ppmem
testpp.exe: testpp.obj preproc.obj ppexpr.obj ppmacro.obj ppmem.obj
        wlink name testpp debug all file testpp,preproc,ppexpr,ppmacro,ppmem
dumpmac.exe: dumpmac.obj preproc.obj ppexpr.obj ppmacro.obj ppmem.obj
        wlink name dumpmac debug all file dumpmac,preproc,ppexpr,ppmacro,ppmem

mkmk.obj: mkmk.c
        wcc mkmk $(CFLAGS)
dumpmac.obj: dumpmac.c
        wcc dumpmac $(CFLAGS)
testpp.obj: testpp.c
        wcc testpp $(CFLAGS)
preproc.obj: preproc.c
        wcc preproc $(CFLAGS)
ppexpr.obj: ppexpr.c
        wcc ppexpr $(CFLAGS)
ppmacro.obj: ppmacro.c
        wcc ppmacro $(CFLAGS)
ppmem.obj: ppmem.c
        wcc ppmem $(CFLAGS)
