# C++ Reference Compilers Prerequisite Tool Build Control File
# ============================================================

    echo Building C++ reference compilers
    cdsay <SRCDIR>/plusplus/i86/rpp
    wmake -h -k
    cdsay <SRCDIR>/plusplus/386/rpp
    wmake -h -k
    cdsay <SRCDIR>/plusplus/axp/rpp
    wmake -h -k
    cdsay <PROJDIR>
