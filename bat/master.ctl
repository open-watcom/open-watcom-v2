# Master Languages Builder Control File
# =====================================

[ BLOCK . . ]

#default to build.log
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

[ BLOCK <1> whatsout ]
#=====================
    echo **** WHATSOUT rule
    set LOGFNAME=wout

[ BLOCK <1> qlocks ]
#===================
    echo **** QLOCKS rule
    set LOGFNAME=qlocks

[ BLOCK <1> refresh ]
#====================
    echo **** REFRESH rule
    set LOGFNAME=refresh

[ BLOCK <1> whatsnew ]
#====================
    echo **** WHATSNEW rule
    set LOGFNAME=wnew

[ BLOCK <1> cprel2 ]
#===================
    echo **** CPREL2 rule
    set LOGFNAME=cprel2
    # with 'cprel2', we want to force the copy to occur
    set CPCMD=copy
    set CCCMD=ccopy

[ BLOCK <1> build ]
#==================
    echo **** BUILD rule
    set LOGFNAME=build

[ BLOCK <1> rel2 ]
#==================
    echo **** REL2 rule
    set LOGFNAME=build

[ BLOCK <1> clean ]
#==================
    echo **** CLEAN rule
    set LOGFNAME=clean

[ BLOCK <1> install ]
#==================
    set LOGFNAME=install

[ BLOCK <1> boot ]
#==================
    set LOGFNAME=boot

[ BLOCK <1> pass1 ]
#==================
    echo **** REL2 rule
    set LOGFNAME=pass1
    set 1=rel2

[ BLOCK <1> pass2 ]
#==================
    echo **** REL2 rule
    set LOGFNAME=pass2
    set 1=rel2

# Configure executable extension: DOS, OS/2 and Windows use .exe, others nothing

[ BLOCK <BUILD_PLATFORM> nt386 ntaxp os2386 dos386 ]
#===================================================
    set CMDEXT=.exe

[ BLOCK . . ]
