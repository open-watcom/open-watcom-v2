# C++ Reference Compilers Prerequisite Tool Build Control File
# ============================================================

    echo Building C++ reference compilers
    cdsay <OWSRCDIR>/plusplus/i86/rpp
    wmake -h -k
    cdsay <OWSRCDIR>/plusplus/386/rpp
    wmake -h -k
    cdsay <OWSRCDIR>/plusplus/axp/rpp
    wmake -h -k
    cdsay <PROJDIR>
