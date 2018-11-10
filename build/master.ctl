# Master Builder Control File
# ===========================

# Default to build.log
set LOGFNAME=build
set LOGEXT=log
set CPCMD=acopy
set CCCMD=accopy
set BLDRULE=

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

[ BLOCK <1> build build1 build2 ]
#================================
    set BLDRULE=build
    set LOGFNAME=<1>

[ BLOCK <1> rel pass ]
#=====================
    set BLDRULE=rel
    set LOGFNAME=<1>

[ BLOCK <1> clean clean1 clean2 ]
#================================
    set BLDRULE=clean
    set LOGFNAME=<1>

[ BLOCK <1> cprel cprel1 cprel2 ]
#================================
    set BLDRULE=cprel
    set LOGFNAME=<1>
    # with 'cprel', we want to force the copy to occur
    set CPCMD=copy
    set CCCMD=ccopy

[ BLOCK <1> install missing ]
#============================
    set LOGFNAME=<1>

[ BLOCK <1> boot ]
#=================
    set BLDRULE=boot
    set LOGFNAME=<1>

[ BLOCK <1> bootclean ]
#======================
    set BLDRULE=bootclean
    set LOGFNAME=btclean

[ BLOCK <1> test ]
#=================
    set BLDRULE=test
    set LOGFNAME=<1>

[ BLOCK <1> buildtest ]
#======================
    set BLDRULE=test
    set BUILDTEST=1
    set LOGFNAME=bldtest

[ BLOCK <1> testclean ]
#======================
    set BLDRULE=testclean
    set LOGFNAME=tstclean

[ BLOCK <1> cleanlog ]
#======================
    set BLDRULE=cleanlog
    set LOGFNAME=cleanlog

[ BLOCK <1> docs ]
#=================
    set BLDRULE=docs
    set LOGFNAME=doc

[ BLOCK <1> docpdf ]
#===================
    set BLDRULE=docpdf
    set LOGFNAME=docpdf

[ BLOCK <1> cpdocpdf ]
#=====================
    set BLDRULE=cpdocref
    set LOGFNAME=cpdocref
    # with 'cpdocref', we want to force the copy to occur
    set CPCMD=copy
    set CCCMD=ccopy

[ BLOCK <1> docsclean ]
#======================
    set BLDRULE=docsclean
    set LOGFNAME=docclean

[ BLOCK <BLDRULE> boot ]
#=======================
    echo **** Building the <PROJNAME> bootstrap

[ BLOCK <BLDRULE> bootclean ]
#============================
    echo **** BOOTCLEAN rule

[ BLOCK <BLDRULE> build ]
#========================
    echo **** BUILD rule

[ BLOCK <BLDRULE> rel ]
#======================
    echo **** REL rule

[ BLOCK <BLDRULE> clean ]
#========================
    echo **** CLEAN rule

[ BLOCK <BLDRULE> cprel ]
#========================
    echo **** CPREL rule

[ BLOCK <BLDRULE> test ]
#=======================
    echo **** TEST rule

[ BLOCK <BLDRULE> testclean ]
#============================
    echo **** TESTCLEAN rule

[ BLOCK <BLDRULE> cleanlog ]
#===========================
    echo **** CLEANLOG rule

[ BLOCK <BLDRULE> docs ]
#=======================
    echo **** DOC rule

[ BLOCK <BLDRULE> docpdf ]
#=========================
    echo **** DOCPDF rule

[ BLOCK <BLDRULE> cpdocpdf ]
#===========================
    echo **** CPDOCPDF rule

[ BLOCK <BLDRULE> docsclean ]
#============================
    echo **** DOCCLEAN rule

[ BLOCK .<PROJDIR> . ]
    error PROJDIR must be set!

[ BLOCK . . ]

[ LOG <PROJDIR>/<LOGFNAME>.<LOGEXT> ]
