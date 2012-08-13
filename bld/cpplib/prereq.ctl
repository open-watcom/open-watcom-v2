# C++ Reference Compilers Prerequisite Tool Build Control File
# ============================================================

    echo Building C++ reference compilers
    cdsay <PROJDIR>/../rpp.i86
    wmake -h -k
    cdsay <PROJDIR>/../rpp.386
    wmake -h -k
    cdsay <PROJDIR>/../rpp.axp
    wmake -h -k
    cdsay <PROJDIR>
