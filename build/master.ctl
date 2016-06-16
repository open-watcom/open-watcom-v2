# Master Builder Control File
# ===========================

# Default to build.log
set LOGFNAME=build
set LOGEXT=log
set CPCMD=acopy
set CCCMD=accopy

# Set default output directory
[ BLOCK .<OWRELROOT> . ]
#=======================
    set OWRELROOT=<OWROOT>\rel
    [ IFDEF <BLD_HOST> UNIX ]
        set OWRELROOT=<OWROOT>/rel
    [ ENDIF ]

# Configure executable extension: DOS, OS/2 and Windows use .exe, others nothing
[ BLOCK <BLD_HOST> NT OS2 DOS ]
#==============================
    set CMDEXT=.exe
    set DYEXT=.dll

[ BLOCK <BLD_HOST> UNIX ]
#========================
    set CMDEXT=
    set DYEXT=.so

[ BLOCK . . ]
#============

[ BLOCK .<1> . ]
#===============
    # default rule
    set 1=build

[ BLOCK .<1> .. ]
#================
    # default rule
    set 1=build

[ BLOCK <1> build ]
#==================
    set LOGFNAME=build
    echo **** BUILD rule

[ BLOCK <1> rel pass ]
#=====================
    set LOGFNAME=<1>
    echo **** REL rule
    set 1=rel

[ BLOCK <1> clean ]
#==================
    set LOGFNAME=<1>
    echo **** CLEAN rule

[ BLOCK <1> cprel ]
#==================
    set LOGFNAME=cprel
    echo **** CPREL rule
    # with 'cprel', we want to force the copy to occur
    set CPCMD=copy
    set CCCMD=ccopy

[ BLOCK <1> install missing ]
#============================
    set LOGFNAME=<1>

[ BLOCK <1> boot ]
#=================
    set LOGFNAME=<1>
    echo **** Building the <PROJNAME> bootstrap

[ BLOCK <1> bootclean ]
#======================
    set LOGFNAME=btclean
    echo **** BOOTCLEAN rule

[ BLOCK <1> test ]
#=================
    set LOGFNAME=<1>
    echo **** TEST rule

[ BLOCK <1> buildtest ]
#======================
    set BUILDTEST=1
    set LOGFNAME=bldtest
    echo **** TEST rule
    set 1=test

[ BLOCK <1> testclean ]
#======================
    set LOGFNAME=tstclean
    echo **** TESTCLEAN rule

[ BLOCK <1> docs ]
#=================
    set LOGFNAME=doc
    echo **** DOC rule

[ BLOCK <1> docsclean ]
#======================
    set LOGFNAME=docclean
    echo **** DOCCLEAN rule

[ BLOCK .<PROJDIR> . ]
    error PROJDIR must be set!

[ BLOCK . . ]

[ LOG <PROJDIR>/<LOGFNAME>.<LOGEXT> ]
