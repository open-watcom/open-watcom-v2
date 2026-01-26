    cdsay "<PROJDIR>"
    echo deftesth 1
    wmake -h <TESTARG>
[ IFDEF <EXTRA_ARCH> i86 386 axp ppc mps ]
    cdsay "<PROJDIR>"
    echo deftesth 2
    wmake -h arch=<EXTRA_ARCH> <TESTARG>
[ ENDIF ]
