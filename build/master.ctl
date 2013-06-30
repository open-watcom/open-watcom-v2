# Master Builder Control File
# ===========================

# Set default output directory
[ BLOCK .<OWRELROOT> . ]
#=======================
    set OWRELROOT=<OWROOT>\rel
    [ IFDEF <BLD_HOST> UNIX ]
        set OWRELROOT=<OWROOT>/rel
    [ ENDIF ]

[ BLOCK . . ]
#============

# Default to build.log
set LOGFNAME=build
set LOGEXT=log
set CPCMD=acopy
set CCCMD=accopy

[ BLOCK .<1> . ]
#===============
    # default rule
    set 1=build

[ BLOCK .<1> .. ]
#================
    # default rule
    set 1=build

[ BLOCK <1> cprel ]
#==================
    set LOGFNAME=cprel
    echo **** CPREL rule
    # with 'cprel', we want to force the copy to occur
    set CPCMD=copy
    set CCCMD=ccopy

[ BLOCK <1> build ]
#==================
    set LOGFNAME=build
    echo **** BUILD rule

[ BLOCK <1> rel pass1 pass2 ]
#============================
    set LOGFNAME=<1>
    echo **** REL rule
    set 1=rel

[ BLOCK <1> clean clean1 clean2 ]
#================================
    set LOGFNAME=<1>
    echo **** CLEAN rule
    set 1=clean

[ BLOCK <1> install missing ]
#============================
    set LOGFNAME=<1>

[ BLOCK <1> boot boot1 boot2 ]
#=============================
    set LOGFNAME=<1>
    echo **** Building the <PROJNAME> bootstrap
    set 1=boot

[ BLOCK <1> bootclean ]
    set LOGFNAME=btclean
[ BLOCK <1> bootclean1 ]
    set LOGFNAME=btclean1
[ BLOCK <1> bootclean2 ]
    set LOGFNAME=btclean2
[ BLOCK <1> bootclean bootclean1 bootclean2 ]
#============================================
    echo **** BOOTCLEAN rule
    set 1=bootclean

[ BLOCK <1> test test1 test2 ]
#=============================
    set LOGFNAME=<1>
    echo **** TEST rule
    set 1=test

[ BLOCK <1> testclean ]
    set LOGFNAME=tstclean
[ BLOCK <1> testclean1 ]
    set LOGFNAME=tstclean1
[ BLOCK <1> testclean2 ]
    set LOGFNAME=tstclean2
[ BLOCK <1> testclean testclean1 testclean2 ]
#============================================
    echo **** TESTCLEAN rule
    set 1=testclean

[ BLOCK <1> docs ]
#=================
    set LOGFNAME=doc
    echo **** DOC rule
    set 1=rel

[ BLOCK <1> docsclean ]
#======================
    set LOGFNAME=docclean
    echo **** DOCCLEAN rule
    set 1=clean

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
