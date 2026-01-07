    cdsay "<PROJDIR>"
    wmake -h <TESTARG>
[ IFDEF <EXTRA_ARCH> i86 386 axp ppc mps ]
    cdsay "<PROJDIR>"
    wmake -h arch=<EXTRA_ARCH> <TESTARG>
[ ENDIF ]
