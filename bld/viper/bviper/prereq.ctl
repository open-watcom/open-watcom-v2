# IDE2MAKE (aka BVIPER) Prerequisite Tool Build Control File
# ===========================================================

set TMP_BUILD_PLATFORM=<BUILD_PLATFORM>

[ BLOCK <1> clean ]
#==================
    rm -f <OWBINDIR>/ide2make
    rm -f <OWBINDIR>/ide2make.exe
    set BUILD_PLATFORM=

[ BLOCK <BUILD_PLATFORM> dos386 ]
#================================
    <CPCMD> dos386/ide2make.exe <OWBINDIR>/ide2make.exe

[ BLOCK <BUILD_PLATFORM> os2386 ]
#================================
    <CPCMD> os2386/ide2make.exe <OWBINDIR>/ide2make.exe

[ BLOCK <BUILD_PLATFORM> nt386 ]
#===============================
    <CPCMD> nt386/ide2make.exe <OWBINDIR>/ide2make.exe

[ BLOCK <BUILD_PLATFORM> linux386 ]
#==================================
    <CPCMD> linux386/ide2make.exe <OWBINDIR>/ide2make

[ BLOCK . . ]
#============
set BUILD_PLATFORM=<TMP_BUILD_PLATFORM>
set TMP_BUILD_PLATFORM=
