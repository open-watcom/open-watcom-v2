# Master Builder Control File
# ===========================

# Set default output directory
[ BLOCK .<OWRELROOT> . ]
#=======================
    set OWRELROOT=<OWROOT>/rel

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
#===================
    set LOGFNAME=cprel
    echo **** CPREL rule
    # with 'cprel', we want to force the copy to occur
    set CPCMD=copy
    set CCCMD=ccopy

[ BLOCK <1> build ]
#==================
    set LOGFNAME=build
    echo **** BUILD rule

[ BLOCK <1> rel ]
#=================
    set LOGFNAME=build
    echo **** REL rule

[ BLOCK <1> clean ]
#==================
    set LOGFNAME=clean
    echo **** CLEAN rule

[ BLOCK <1> install ]
#====================
    set LOGFNAME=install

[ BLOCK <1> boot ]
#==================
    set LOGFNAME=boot
    echo **** Building the <PROJNAME> bootstrap

[ BLOCK <1> bootclean ]
#======================
    set LOGFNAME=btclean
    echo **** BOOTCLEAN rule

[ BLOCK <1> pass1 ]
#==================
    set LOGFNAME=pass1
    echo **** REL rule
    set 1=rel

[ BLOCK <1> pass2 ]
#==================
    set LOGFNAME=pass2
    echo **** REL rule
    set 1=rel

[ BLOCK <1> test ]
#==================
    set LOGFNAME=test
    echo **** TEST rule

[ BLOCK <1> testclean ]
#======================
    set LOGFNAME=tstclean
    echo **** TESTCLEAN rule

[ BLOCK <1> doc ]
#==================
    set LOGFNAME=doc
    echo **** DOC rule
    set 1=build

[ BLOCK <1> docclean ]
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
