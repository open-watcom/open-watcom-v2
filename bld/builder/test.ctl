#
# BUILDER test control file
# =========================
# call with 'builder -c test.ctl' followed by optional parameters

cdsay .

# prove ifdef works for single match words and isn't nestable within itself
# call with and without parameters
[ IFDEF os_linux os_os2 blub blab blob ]
    echo Ifdef: You shouldn't see this line
[ IFDEF os_linux blub blab blob os_linux ]
    echo Ifdef: os_linux

# prove block works for single match words
# call with and without parameters
[ BLOCK os_linux os_os2 blub blab blob ]
    echo Block: You shouldn't see this line
[ BLOCK os_linux blub blab blob os_linux ]
    echo Block: os_linux

# call with 4 parameters
echo Parameters 1+: <*>
echo Parameters 1+: <1*>
echo Parameters 2+: <2*>
echo Parameters 3+: <3*>


[ BLOCK <1> build rel2 ]
#=======================
    echo We're in block (build rel2)
    [ IFDEF os_os2 <2*> ]
        echo os_os2 detected in parms 2+
    [ IFDEF ("") <2*> ]
        echo no extra parms detected
    [ IFDEF (os_os2 os_linux) <2*> ]
        echo linux or os2 detected
    [ IFDEF (os_os2 os_linux "") <2*> ]
        echo linux or os2 or nothing detected
    [ IFDEF (os_nt) <2*> ]
        echo Oh no! Not Windows please

[ BLOCK <1> rel2 ]
#=======================
    echo We're in block (rel2)
    [ IFDEF os_os2 <2*> ]
        echo os_os2 detected in parms <2*>
    [ IFDEF ("") <2*> ]
        echo no extra parms detected
    [ IFDEF (os_os2 os_linux) <2*> ]
        echo linux or os2 detected
    [ IFDEF (os_os2 os_linux "") <2*> ]
        echo linux or os2 or nothing detected
    [ IFDEF (os_nt) <2*> ]
        echo Oh no! Not Windows please

[ BLOCK . .]
    echo We're in a generic block with parameters: <*>

# Line below should be active only when called without parameters
[ IFDEF ("") <*> ]
    echo You shouldn't see this line with any parameters: <*>


