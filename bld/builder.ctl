# Master Open Watcom Builder Control file
# =======================================

set PROJDIR=<OWBINDIR>

[ INCLUDE <OWROOT>/build/master.ctl ]

echo Build host: <BLD_HOST>

cdsay .

[ BLOCK <1> boot bootclean ]
#===========================
# Build Open Watcom tools using the host platform's native compiler.
#
# NB: The ordering of the following inclusions is significant!
# At the beginning, we only have wmake and builder/pmake
# If necessary, build clibext library
[ INCLUDE <OWSRCDIR>/watcom/builder.ctl ]
# If necessary, build POSIX tools (awk, sed, cp, ...)
[ IFDEF <BLD_HOST> OS2 NT DOS ]
[ INCLUDE <OWSRCDIR>/posix/builder.ctl ]
[ ENDIF ]
# Build our version of yacc
[ INCLUDE <OWSRCDIR>/yacc/builder.ctl ]
[ IFDEF <BLD_HOST> OS2 NT DOS ]
[ INCLUDE <OWSRCDIR>/awk/builder.ctl ]
[ ENDIF ]
# Build C preprocessing utility
[ INCLUDE <OWSRCDIR>/cpp/builder.ctl ]
# Build optencod, msgencod and findhash utilities
[ INCLUDE <OWSRCDIR>/fe_misc/builder.ctl ]
# Start with the ORL and librarian - boot build has no other dependencies
[ INCLUDE <OWSRCDIR>/orl/builder.ctl ]
[ INCLUDE <OWSRCDIR>/nwlib/builder.ctl ]
# Build wsplice and genverrc
[ INCLUDE <OWSRCDIR>/builder/builder.ctl ]
# Next build wres and the resource compiler
[ INCLUDE <OWSRCDIR>/wres/builder.ctl ]
[ INCLUDE <OWSRCDIR>/rc/rc/builder.ctl ]
# Build wstrip
[ INCLUDE <OWSRCDIR>/wstrip/builder.ctl ]
# Continue with the linker
[ INCLUDE <OWSRCDIR>/dwarf/dw/builder.ctl ]
[ INCLUDE <OWSRCDIR>/wl/builder.ctl ]
#[ INCLUDE <OWSRCDIR>/wl/exe2bin/builder.ctl ]
# Build the OWL for code generator and assemblers
[ INCLUDE <OWSRCDIR>/owl/builder.ctl ]
# Now we also have everything to build assemblers
# Build RISC assemblers
[ INCLUDE <OWSRCDIR>/re2c/builder.ctl ]
[ INCLUDE <OWSRCDIR>/as/builder.ctl ]
# Build WASM assembler
[ INCLUDE <OWSRCDIR>/wasm/builder.ctl ]
# Now we also have everything to build wcl, owcc
[ INCLUDE <OWSRCDIR>/wcl/386/builder.ctl ]
[ INCLUDE <OWSRCDIR>/wcl/i86/builder.ctl ]
#[ INCLUDE <OWSRCDIR>/wcl/owcc/builder.ctl ]
# Build the 16-bit and 32-bit x86 compilers
[ INCLUDE <OWSRCDIR>/cfloat/builder.ctl ]
[ INCLUDE <OWSRCDIR>/cg/intel/386/builder.ctl ]
[ INCLUDE <OWSRCDIR>/cg/intel/i86/builder.ctl ]
[ INCLUDE <OWSRCDIR>/cc/386/builder.ctl ]
[ INCLUDE <OWSRCDIR>/cc/i86/builder.ctl ]
[ INCLUDE <OWSRCDIR>/plusplus/386/builder.ctl ]
[ INCLUDE <OWSRCDIR>/plusplus/i86/builder.ctl ]
# copy DOS4GW extender on build PATH for DOS build host
[ IFDEF <BLD_HOST> DOS ]
[ INCLUDE <OWSRCDIR>/redist/builder.ctl ]
[ ENDIF ]
# Now miscellaneous utilities required during build
[ INCLUDE <OWSRCDIR>/lib_misc/builder.ctl ]
[ INCLUDE <OWSRCDIR>/ssl/builder.ctl ]
[ INCLUDE <OWSRCDIR>/causeway/cwc/builder.ctl ]
[ INCLUDE <OWSRCDIR>/parsedlg/builder.ctl ]
[ INCLUDE <OWSRCDIR>/vi/bind/builder.ctl ]
[ INCLUDE <OWSRCDIR>/vi/ctl/builder.ctl ]
[ INCLUDE <OWSRCDIR>/vi/vicomp/builder.ctl ]
[ INCLUDE <OWSRCDIR>/omftools/builder.ctl ]
[ INCLUDE <OWSRCDIR>/whpcvt/builder.ctl ]
[ INCLUDE <OWSRCDIR>/win386/wbind/builder.ctl ]
#[ INCLUDE <OWSRCDIR>/w32loadr/builder.ctl ]
[ INCLUDE <OWSRCDIR>/browser/dlgprs/builder.ctl ]
# Build help compilers and tools
[ INCLUDE <OWSRCDIR>/hcdos/builder.ctl ]
[ INCLUDE <OWSRCDIR>/hcwin/builder.ctl ]
[ INCLUDE <OWSRCDIR>/wipfc/builder.ctl ]
[ INCLUDE <OWSRCDIR>/bmp2eps/builder.ctl ]
# Build ide2make tool for idedemo
[ INCLUDE <OWSRCDIR>/wclass/builder.ctl ]
[ INCLUDE <OWSRCDIR>/ide/lib/builder.ctl ]
[ INCLUDE <OWSRCDIR>/ide/ide2make/builder.ctl ]
# Miscellaneous libraries/utilities required during installers build
[ INCLUDE <OWSRCDIR>/setupgui/ziplib/builder.ctl ]
[ INCLUDE <OWSRCDIR>/setupgui/zlib/builder.ctl ]
[ INCLUDE <OWSRCDIR>/setupgui/mkdisk/builder.ctl ]
# The following tools are required by F77
[ INCLUDE <OWSRCDIR>/f77/utils/builder.ctl ]

## The following tools are required by tests
[ INCLUDE <OWSRCDIR>/ndisasm/builder.ctl ]
[ INCLUDE <OWSRCDIR>/dmpobj/builder.ctl ]

## The following tools are not required during build
#[ INCLUDE <OWSRCDIR>/exedump/builder.ctl ]
#[ INCLUDE <OWSRCDIR>/orl/test/builder.ctl ]
#[ INCLUDE <OWSRCDIR>/dwarf/util/builder.ctl ]
## Debugger/profiler libraries
#[ INCLUDE <OWSRCDIR>/dip/builder.ctl ]
#[ INCLUDE <OWSRCDIR>/mad/builder.ctl ]

#[ IFDEF <BLD_HOST> UNIX ]
##========================
## Build UNIX specific utilities using native tools.
##
## Build our vi editor and the libs it requires
#[ INCLUDE <OWSRCDIR>/ncurses/builder.ctl ]
#[ INCLUDE <OWSRCDIR>/ui/builder.ctl ]
#[ INCLUDE <OWSRCDIR>/vi/builder.ctl ]
#[ INCLUDE <OWSRCDIR>/hlpview/builder.ctl ]
## Build gui/aui lib and the profiler
#[ INCLUDE <OWSRCDIR>/gui/builder.ctl ]
#[ INCLUDE <OWSRCDIR>/aui/builder.ctl ]
#[ INCLUDE <OWSRCDIR>/wprof/builder.ctl ]
#[ ENDIF ]

[ BLOCK <1> build rel clean cprel build1 clean1 cprel1 ]
#=======================================================
# Build all of Open Watcom using freshly built tools.
# part 1
#
# NB: Again, the order is significant.
# At the beginning, assume to have compilers/assemblers/librarian/linker
# running on the host platform, but not necessarily anything else.
# Start with language and API headers
[ INCLUDE <OWSRCDIR>/hdr/builder.ctl ]
[ INCLUDE <OWSRCDIR>/os2api/builder.ctl ]
[ INCLUDE <OWSRCDIR>/w16api/builder.ctl ]
[ INCLUDE <OWSRCDIR>/w32api/builder.ctl ]
# Continue with runtime libraries.
[ INCLUDE <OWSRCDIR>/clib/builder.ctl ]
[ INCLUDE <OWSRCDIR>/mathlib/builder.ctl ]
[ INCLUDE <OWSRCDIR>/cpplib/builder.ctl ]
[ INCLUDE <OWSRCDIR>/f77/f77lib/builder.ctl ]
[ INCLUDE <OWSRCDIR>/rtdll/builder.ctl ]

[ BLOCK <1> build rel clean cprel build2 clean2 cprel2 ]
#=======================================================
# Build all of Open Watcom using freshly built tools.
# part 2
#
[ INCLUDE <OWSRCDIR>/fpuemu/builder.ctl ]
[ INCLUDE <OWSRCDIR>/omftools/builder.ctl ]
[ INCLUDE <OWSRCDIR>/graphlib/builder.ctl ]
# Start with DOS4GW DOS extender stub.
[ INCLUDE <OWSRCDIR>/wstuba/builder.ctl ]
# Continue with Causeway DOS extender.
[ INCLUDE <OWSRCDIR>/causeway/builder.ctl ]
# Continue with WIN386 extender.
[ INCLUDE <OWSRCDIR>/win386/builder.ctl ]
# Continue with OSI extenders stubs.
[ INCLUDE <OWSRCDIR>/w32loadr/builder.ctl ]
# Now we have enough to start cross building everything else

# Start with the libs used by various tools
[ INCLUDE <OWSRCDIR>/wres/builder.ctl ]
[ INCLUDE <OWSRCDIR>/orl/builder.ctl ]
[ INCLUDE <OWSRCDIR>/owl/builder.ctl ]
[ INCLUDE <OWSRCDIR>/dwarf/builder.ctl ]
[ INCLUDE <OWSRCDIR>/cfloat/builder.ctl ]
# Continue with the assemblers/librarian/linker/make
[ INCLUDE <OWSRCDIR>/wasm/builder.ctl ]
[ INCLUDE <OWSRCDIR>/as/builder.ctl ]
[ INCLUDE <OWSRCDIR>/nwlib/builder.ctl ]
[ INCLUDE <OWSRCDIR>/wl/builder.ctl ]
[ INCLUDE <OWSRCDIR>/wmake/builder.ctl ]
[ INCLUDE <OWSRCDIR>/wtouch/builder.ctl ]
# On to the compilers
[ INCLUDE <OWSRCDIR>/cg/builder.ctl ]
[ INCLUDE <OWSRCDIR>/cc/builder.ctl ]
[ INCLUDE <OWSRCDIR>/plusplus/builder.ctl ]
[ INCLUDE <OWSRCDIR>/f77/wfc/builder.ctl ]
# Resource tools, first Resource compiler
[ INCLUDE <OWSRCDIR>/rc/builder.ctl ]
# Continue with SDK tools
[ INCLUDE <OWSRCDIR>/wpi/builder.ctl ]
[ INCLUDE <OWSRCDIR>/commonui/builder.ctl ]
[ INCLUDE <OWSRCDIR>/sdk/builder.ctl ]
# Now miscellaneous command line tools
[ INCLUDE <OWSRCDIR>/ndisasm/builder.ctl ]
[ INCLUDE <OWSRCDIR>/exedump/builder.ctl ]
[ INCLUDE <OWSRCDIR>/dmpobj/builder.ctl ]
[ INCLUDE <OWSRCDIR>/wcl/builder.ctl ]
[ INCLUDE <OWSRCDIR>/f77/wfl/builder.ctl ]
[ INCLUDE <OWSRCDIR>/wstrip/builder.ctl ]
# User interface libs
[ INCLUDE <OWSRCDIR>/ncurses/builder.ctl ]
[ INCLUDE <OWSRCDIR>/ui/builder.ctl ]
[ INCLUDE <OWSRCDIR>/gui/builder.ctl ]
[ INCLUDE <OWSRCDIR>/aui/builder.ctl ]
# The vi(w) editor
[ INCLUDE <OWSRCDIR>/rcsdll/builder.ctl ]
[ INCLUDE <OWSRCDIR>/vi/builder.ctl ]
# Build the debugger and sampler
[ INCLUDE <OWSRCDIR>/dip/builder.ctl ]
[ INCLUDE <OWSRCDIR>/mad/builder.ctl ]
[ INCLUDE <OWSRCDIR>/rsilib/builder.ctl ]
[ INCLUDE <OWSRCDIR>/wattcp/builder.ctl ]
[ INCLUDE <OWSRCDIR>/trap/builder.ctl ]
[ INCLUDE <OWSRCDIR>/wv/builder.ctl ]
[ INCLUDE <OWSRCDIR>/wsample/builder.ctl ]
# GUI tools libraries
[ INCLUDE <OWSRCDIR>/wclass/builder.ctl ]
# Other GUI tools
[ INCLUDE <OWSRCDIR>/wprof/builder.ctl ]
[ INCLUDE <OWSRCDIR>/browser/builder.ctl ]
# The IDE tools
[ INCLUDE <OWSRCDIR>/editdll/builder.ctl ]
[ INCLUDE <OWSRCDIR>/idebatch/builder.ctl ]
[ INCLUDE <OWSRCDIR>/ide/builder.ctl ]
# The miscelaneous tools
[ INCLUDE <OWSRCDIR>/cmdedit/builder.ctl ]
[ INCLUDE <OWSRCDIR>/cvpack/builder.ctl ]
# OW clones for MS tools
[ INCLUDE <OWSRCDIR>/mstools/builder.ctl ]
# Miscelaneous files
[ INCLUDE <OWSRCDIR>/misc/builder.ctl ]
[ INCLUDE <OWSRCDIR>/bdiff/builder.ctl ]
[ INCLUDE <OWSRCDIR>/techinfo/builder.ctl ]
# Source code samples
[ INCLUDE <OWSRCDIR>/src/builder.ctl ]
# IDE samples
[ INCLUDE <OWSRCDIR>/idedemo/builder.ctl ]
# Build help viewer
[ INCLUDE <OWSRCDIR>/hlpview/builder.ctl ]
# Build help compilers
[ INCLUDE <OWSRCDIR>/hcdos/builder.ctl ]
[ INCLUDE <OWSRCDIR>/hcwin/builder.ctl ]
[ INCLUDE <OWSRCDIR>/wipfc/builder.ctl ]
[ INCLUDE <OWSRCDIR>/bmp2eps/builder.ctl ]
# Copy the redistributable components
[ INCLUDE <OWSRCDIR>/redist/builder.ctl ]
# Build installer tools
[ INCLUDE <OWSRCDIR>/uninstal/builder.ctl ]
[ INCLUDE <OWSRCDIR>/setupgui/builder.ctl ]

# only test build for now
[ INCLUDE <OWSRCDIR>/wgml/builder.ctl ]
# Build documentation
set OWTXTDOCBUILD=1
[ IFDEF <OWDOCBUILD> 1 ]
    set OWTXTDOCBUILD=
[ ENDIF ]
[ INCLUDE <OWDOCSDIR>/builder.ctl ]
set OWTXTDOCBUILD=

[ BLOCK <1> docs docsclean docpdf cpdocpdf ]
#===========================================
[ INCLUDE <OWDOCSDIR>/builder.ctl ]

[ BLOCK <1> test testclean cleanlog ]
#====================================
[ INCLUDE <OWSRCDIR>/wasmtest/builder.ctl ]
[ INCLUDE <OWSRCDIR>/ctest/builder.ctl ]
[ INCLUDE <OWSRCDIR>/f77test/builder.ctl ]
[ INCLUDE <OWSRCDIR>/plustest/builder.ctl ]
[ INCLUDE <OWSRCDIR>/clibtest/builder.ctl ]

[ BLOCK <1> test cleanrel ]
#==========================
    echo rm -rf <OWRELROOT>
    rm -rf <OWRELROOT>

[ BLOCK . . ]

cdsay .
