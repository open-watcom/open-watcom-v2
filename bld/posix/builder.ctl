# POSIX Builder Control file
# ==========================

set PROJNAME=posixutl

set BINTOOL=0

set PROJDIR=<CWD>

[ INCLUDE "<OWROOT>/build/prolog.ctl" ]

[ INCLUDE "<OWROOT>/build/defrule.ctl" ]

[ BLOCK <BINTOOL> build ]
#========================
[ IFDEF <BLD_HOST> OS2 NT DOS ]
    cdsay "<PROJDIR>"
    <CPCMD> <OWOBJDIR>/cat.exe   "<OWROOT>/build/<OWOBJDIR>/cat<CMDEXT>"
    <CPCMD> <OWOBJDIR>/egrep.exe "<OWROOT>/build/<OWOBJDIR>/egrep<CMDEXT>"
    <CPCMD> <OWOBJDIR>/false.exe "<OWROOT>/build/<OWOBJDIR>/false<CMDEXT>"
    <CPCMD> <OWOBJDIR>/head.exe  "<OWROOT>/build/<OWOBJDIR>/head<CMDEXT>"
    <CPCMD> <OWOBJDIR>/chmod.exe "<OWROOT>/build/<OWOBJDIR>/chmod<CMDEXT>"
    <CPCMD> <OWOBJDIR>/sed.exe   "<OWROOT>/build/<OWOBJDIR>/sed<CMDEXT>"
    <CPCMD> <OWOBJDIR>/sleep.exe "<OWROOT>/build/<OWOBJDIR>/sleep<CMDEXT>"
    <CPCMD> <OWOBJDIR>/sort.exe  "<OWROOT>/build/<OWOBJDIR>/sort<CMDEXT>"
    <CPCMD> <OWOBJDIR>/tee.exe   "<OWROOT>/build/<OWOBJDIR>/tee<CMDEXT>"
    <CPCMD> <OWOBJDIR>/tr.exe    "<OWROOT>/build/<OWOBJDIR>/tr<CMDEXT>"
    <CPCMD> <OWOBJDIR>/uniq.exe  "<OWROOT>/build/<OWOBJDIR>/uniq<CMDEXT>"
    <CPCMD> <OWOBJDIR>/wc.exe    "<OWROOT>/build/<OWOBJDIR>/wc<CMDEXT>"
    <CPCMD> <OWOBJDIR>/which.exe "<OWROOT>/build/<OWOBJDIR>/which<CMDEXT>"
[ ENDIF ]

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/cat<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/cat<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/egrep<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/egrep<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/false<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/false<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/head<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/head<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/chmod<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/chmod<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/sed<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/sed<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/sleep<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/sleep<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/sort<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/sort<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/tee<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/tee<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/tr<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/tr<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/uniq<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/uniq<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/wc<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/wc<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/which<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/which<CMDEXT>"

[ BLOCK <BLDRULE> rel cprel ]
#============================
    # Currently not built by default

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
