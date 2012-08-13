# C++ Reference Compilers Prerequisite Tool Build Control File
# ============================================================

    echo Building C++ reference compilers
    cdsay <DEVDIR>/plusplus/i86/rpp
    wmake -h -k
    cdsay <DEVDIR>/plusplus/386/rpp
    wmake -h -k
    cdsay <DEVDIR>/plusplus/axp/rpp
    wmake -h -k
    cdsay <PROJDIR>
