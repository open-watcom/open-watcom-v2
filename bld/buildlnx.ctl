# Builder control file for Linux. Using a separate file
# because of bootstrapping issues and the fact that only
# a small subset can be built
set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]

set OWLINUXBUILD=bootstrap
set WLINK_LNK=bootstrp.lnk

# compiled using GNU make + gcc + ar
[ INCLUDE <SRCDIR>/wmake/prereq.ctl ]
[ INCLUDE <SRCDIR>/builder/builder.ctl ]
[ INCLUDE <SRCDIR>/cpp/prereq.ctl ]
[ INCLUDE <SRCDIR>/nwlib/prereq.ctl ]

# compiled using wmake + gcc + ar/wlib
[ INCLUDE <SRCDIR>/yacc/prereq.ctl ]
[ INCLUDE <SRCDIR>/wres/prereq.ctl ]
[ INCLUDE <SRCDIR>/rc/rc/prereq.ctl ]
[ INCLUDE <SRCDIR>/wstrip/prereq.ctl ]
[ INCLUDE <SRCDIR>/orl/prereq.ctl ]
[ INCLUDE <SRCDIR>/cfloat/prereq.ctl ]
[ INCLUDE <SRCDIR>/owl/prereq.ctl ]
[ INCLUDE <SRCDIR>/dwarf/prereq.ctl ]
[ INCLUDE <SRCDIR>/wl/prereq.ctl ]
[ INCLUDE <SRCDIR>/wasm/prereq.ctl ]
[ INCLUDE <SRCDIR>/cg/prereq.ctl ]
[ INCLUDE <SRCDIR>/cc/prereq.ctl ]

# compiled using watcom utilities
[ INCLUDE <SRCDIR>/hdr/builder.ctl ]
[ INCLUDE <SRCDIR>/clib/builder.ctl ]
[ INCLUDE <SRCDIR>/mathlib/builder.ctl ]
[ INCLUDE <SRCDIR>/emu/builder.ctl ]
[ INCLUDE <SRCDIR>/wcl/prereq.ctl ]
[ INCLUDE <SRCDIR>/cfloat/builder.ctl ]
[ INCLUDE <SRCDIR>/owl/builder.ctl ]
[ INCLUDE <SRCDIR>/dwarf/builder.ctl ]
[ INCLUDE <SRCDIR>/re2c/prereq.ctl ]
[ INCLUDE <SRCDIR>/wres/builder.ctl ]
[ INCLUDE <SRCDIR>/as/builder.ctl ]
[ INCLUDE <SRCDIR>/cg/builder.ctl ]
[ INCLUDE <SRCDIR>/cc/builder.ctl ]
[ INCLUDE <SRCDIR>/plusplus/builder.ctl ]
[ INCLUDE <SRCDIR>/cpplib/builder.ctl ]
[ INCLUDE <SRCDIR>/wcl/builder.ctl ]

[ BLOCK <1> rel2 cprel2 ]
    <CPCMD> <SRCDIR>/wcl/386/linux386/wcl386.exe       <OWBINDIR>/wcl386
    <CPCMD> <SRCDIR>/wcl/i86/linux386/wcl.exe          <OWBINDIR>/wcl
    <CPCMD> <SRCDIR>/cc/386/linux386/wcc386c.exe       <OWBINDIR>/wcc386
    <CPCMD> <SRCDIR>/plusplus/386/linux386/wcpp386.exe <OWBINDIR>/wpp386
    <CPCMD> <SRCDIR>/plusplus/i86/linux386/wcppi86.exe <OWBINDIR>/wpp

[ BLOCK . . ]
set OWLINUXBUILD=normal
set WLINK_LNK=

# create full-featured wmake...

[ INCLUDE <SRCDIR>/wres/prereq.ctl ]
[ INCLUDE <SRCDIR>/rc/rc/prereq.ctl ]
[ INCLUDE <SRCDIR>/orl/builder.ctl ]
[ INCLUDE <SRCDIR>/wmake/prereq.ctl ]

# all is ready to do everything else now

[ BLOCK <1> clean ]
    rm -rf <SRCDIR>/builder/<OWOBJDIR>
    rm -rf <SRCDIR>/pmake/<OWOBJDIR>
    rm -rf <SRCDIR>/cpp/<OWOBJDIR>
    rm -rf <SRCDIR>/yacc/<OWOBJDIR>
    rm -rf <SRCDIR>/wres/<OWOBJDIR>
    rm -rf <SRCDIR>/rc/rc/<OWOBJDIR>
    rm -rf <SRCDIR>/wstrip/<OWOBJDIR>
    rm -rf <SRCDIR>/wmake/<OWOBJDIR>

    rm -rf <SRCDIR>/orl/<OWOBJDIR>
    rm -rf <SRCDIR>/nwlib/<OWOBJDIR>
    rm -rf <SRCDIR>/cfloat/<OWOBJDIR>
    rm -rf <SRCDIR>/owl/<OWOBJDIR>
    rm -rf <SRCDIR>/dwarf/dw/<OWOBJDIR>
    rm -rf <SRCDIR>/cg/intel/386/<OWOBJDIR>
    rm -rf <SRCDIR>/cc/<OWOBJDIR>
    rm -rf <SRCDIR>/wcl/<OWOBJDIR>
    rm -rf <SRCDIR>/wasm/<OWOBJDIR>
    rm -rf <SRCDIR>/wl/<OWOBJDIR>

    rm -f <OWBINDIR>/wcl386
    rm -f <OWBINDIR>/wcl
    rm -f <OWBINDIR>/wcc386
    rm -f <OWBINDIR>/wcc
    rm -f <OWBINDIR>/wpp386
    rm -f <OWBINDIR>/wpp
    rm -f <OWBINDIR>/wgml
    rm -f <OWBINDIR>/diff
    rm -f <OWBINDIR>/builder
