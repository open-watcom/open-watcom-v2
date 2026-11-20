# DOS/32A DOS Extender Builder Control file
# =========================================

set PROJNAME=dos32a

set PROJDIR=<CWD>

[ INCLUDE "<OWROOT>/build/prolog.ctl" ]

[ INCLUDE "<OWROOT>/build/defrule.ctl" ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dos386/*.exe                                "<OWRELROOT>/binw/"
    <CCCMD> "<OWROOT>/contrib/extender/dos32a/license.txt" "<OWRELROOT>/binw/license.d32"
    <CCCMD> "<OWROOT>/contrib/extender/dos32a/docs/readme.txt" "<OWRELROOT>/binw/readme.d32"

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
