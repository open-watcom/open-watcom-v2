# Builder control file for "bootstrap" style build. Starts out
# with GNU make but later switches to wmake. Intended to be OS
# and compiler agnostic as much as possible. Usable on platforms
# not supported by Open Watcom compilers.
#
# NB: Environment variables BOOTSTRAP_OS and BOOTSTRAP_CPU must
# be set. Currently valid combinations are:
#
#  OS        CPU   comment
#-----------------------------------------
#  linux     386   Linux on IA-32
#  linux     ppc   Linux on 32-bit PowerPC
#  linux     mips  Linux on 32-bit MIPS
# 
set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
[ LOG <DEVDIR>/<LOGFNAME>.<LOGEXT> ]

set OWLINUXBUILD=bootstrap
# compiled using GNU make + gcc + ar
[ INCLUDE <DEVDIR>/wmake/boot.ctl ]

# compiled using wmake + gcc
[ INCLUDE <DEVDIR>/builder/boot.ctl ]
[ INCLUDE <DEVDIR>/pmake/boot.ctl ]
[ INCLUDE <DEVDIR>/yacc/boot.ctl ]
[ INCLUDE <DEVDIR>/sdk/rc/boot.ctl ]
[ INCLUDE <DEVDIR>/orl/boot.ctl ]
[ INCLUDE <DEVDIR>/wstrip/boot.ctl ]
[ INCLUDE <DEVDIR>/re2c/boot.ctl ]
[ INCLUDE <DEVDIR>/nwlib/boot.ctl ]

# compiled using wmake + gcc + wlib
[ INCLUDE <DEVDIR>/ssl/boot.ctl ]
[ INCLUDE <DEVDIR>/dmpobj/boot.ctl ]
[ INCLUDE <DEVDIR>/ndisasm/boot.ctl ]
[ INCLUDE <DEVDIR>/ncurses/boot.ctl ]
[ INCLUDE <DEVDIR>/ui/boot.ctl ]
[ INCLUDE <DEVDIR>/help/boot.ctl ]
[ INCLUDE <DEVDIR>/wsample/boot.ctl ]
[ INCLUDE <DEVDIR>/mad/boot.ctl ]
[ INCLUDE <DEVDIR>/dip/boot.ctl ]
[ INCLUDE <DEVDIR>/aui/boot.ctl ]
[ INCLUDE <DEVDIR>/gui/boot.ctl ]
[ INCLUDE <DEVDIR>/wprof/boot.ctl ]
[ INCLUDE <DEVDIR>/trap/lcl/linux/boot.ctl ]
[ INCLUDE <DEVDIR>/wv/boot.ctl ]
[ INCLUDE <DEVDIR>/owl/boot.ctl ]
[ INCLUDE <DEVDIR>/orl/test/boot.ctl ]
[ INCLUDE <DEVDIR>/as/ppc/boot.ctl ]
[ INCLUDE <DEVDIR>/dwarf/dw/boot.ctl ]
[ INCLUDE <DEVDIR>/wl/boot.ctl ]
