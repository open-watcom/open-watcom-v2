# Master Open Watcom Builder Control file
# =======================================

set PROJDIR=<OWROOT>/build/<OWOBJDIR>

[ INCLUDE "<OWROOT>/build/master.ctl" ]

echo Build host: <BLD_HOST>

cdsay .

[ BLOCK <BLDRULE> bootclean boot ]
#=================================
# Build Open Watcom tools using the host platform's native compiler.
#
# NB: The ordering of the following inclusions is significant!
# At the beginning, we only have wmake and builder/pmake
# If necessary, build clibext library
[ INCLUDE "<OWROOT>/bld/watcom/builder.ctl" ]
# GIT related stuff
[ INCLUDE "<OWROOT>/bld/git/builder.ctl" ]
# If necessary, build POSIX tools (awk, sed, cp, ...)
[ INCLUDE "<OWROOT>/bld/posix/builder.ctl" ]
# Build wsplice and genverrc
[ INCLUDE "<OWROOT>/bld/builder/builder.ctl" ]
# Build our version of yacc
[ INCLUDE "<OWROOT>/bld/yacc/builder.ctl" ]
[ IFDEF <BLD_HOST> OS2 NT DOS ]
[ INCLUDE "<OWROOT>/bld/awk/builder.ctl" ]
[ ENDIF ]
# Build C preprocessing utility
[ INCLUDE "<OWROOT>/bld/cpp/builder.ctl" ]
# Build optencod, msgencod and findhash utilities
[ INCLUDE "<OWROOT>/bld/fe_misc/builder.ctl" ]
# Start with the ORL and librarian - boot build has no other dependencies
[ INCLUDE "<OWROOT>/bld/orl/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/nwlib/builder.ctl" ]
# Next build wres and the resource compiler
[ INCLUDE "<OWROOT>/bld/wres/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/rc/rc/builder.ctl" ]
# Build wstrip
[ INCLUDE "<OWROOT>/bld/wstrip/builder.ctl" ]
# Continue with the linker
[ INCLUDE "<OWROOT>/bld/dwarf/dw/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/wl/builder.ctl" ]
#[ INCLUDE "<OWROOT>/bld/wl/exe2bin/builder.ctl" ]
# Build the OWL for code generator and assemblers
[ INCLUDE "<OWROOT>/bld/owl/builder.ctl" ]
# Now we also have everything to build assemblers
# Build RISC assemblers
[ INCLUDE "<OWROOT>/bld/re2c/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/as/builder.ctl" ]
# Build WASM assembler
[ INCLUDE "<OWROOT>/bld/wasm/builder.ctl" ]
# Now we also have everything to build wcl, owcc
[ INCLUDE "<OWROOT>/bld/wcl/builder.ctl" ]
#[ INCLUDE "<OWROOT>/bld/wcl/owcc/builder.ctl" ]
# Build the 16-bit and 32-bit x86 compilers
[ INCLUDE "<OWROOT>/bld/cfloat/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/cg/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/cc/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/plusplus/builder.ctl" ]
# copy DOS4GW extender on build PATH for DOS build host
[ IFDEF <BLD_HOST> DOS ]
[ INCLUDE "<OWROOT>/bld/redist/builder.ctl" ]
[ ENDIF ]
# Now miscellaneous utilities required during build
[ INCLUDE "<OWROOT>/bld/lib_misc/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/ssl/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/causeway/cwc/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/parsedlg/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/vi/bind/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/vi/ctl/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/vi/vicomp/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/omftools/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/whpcvt/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/win386/wbind/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/browser/dlgprs/builder.ctl" ]
# Build help compilers and tools
[ INCLUDE "<OWROOT>/bld/wgml/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/hcdos/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/hcwin/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/wipfc/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/bmp2eps/builder.ctl" ]
# Build ide2make tool for idedemo
[ INCLUDE "<OWROOT>/bld/wclass/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/ide/lib/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/ide/ide2make/builder.ctl" ]
# Miscellaneous libraries/utilities required during installers build
[ INCLUDE "<OWROOT>/bld/setupgui/ziplib/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/setupgui/zlib/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/setupgui/mkdisk/builder.ctl" ]
# The following tools are required by F77
[ INCLUDE "<OWROOT>/bld/f77/utils/builder.ctl" ]

## The following tools are required by tests
[ INCLUDE "<OWROOT>/bld/ndisasm/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/dmpobj/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/testutil/builder.ctl" ]

## The following tools are not required during build
#[ INCLUDE "<OWROOT>/bld/exedump/builder.ctl" ]
#[ INCLUDE "<OWROOT>/bld/orl/test/builder.ctl" ]
#[ INCLUDE "<OWROOT>/bld/dwarf/util/builder.ctl" ]
## Debugger/profiler libraries
#[ INCLUDE "<OWROOT>/bld/dip/builder.ctl" ]
#[ INCLUDE "<OWROOT>/bld/mad/builder.ctl" ]

#[ IFDEF <BLD_HOST> UNIX ]
##========================
## Build UNIX specific utilities using native tools.
##
## Build our vi editor and the libs it requires
#[ INCLUDE "<OWROOT>/bld/ncurses/builder.ctl" ]
#[ INCLUDE "<OWROOT>/bld/ui/builder.ctl" ]
#[ INCLUDE "<OWROOT>/bld/vi/builder.ctl" ]
#[ INCLUDE "<OWROOT>/bld/hlpview/builder.ctl" ]
## Build gui/aui lib and the profiler
#[ INCLUDE "<OWROOT>/bld/gui/builder.ctl" ]
#[ INCLUDE "<OWROOT>/bld/aui/builder.ctl" ]
#[ INCLUDE "<OWROOT>/bld/wprof/builder.ctl" ]
#[ ENDIF ]

[ BLOCK <1> clean build rel cprel passclean pass ]
#=================================================
# Build all of Open Watcom using freshly built tools.
#
# NB: Again, the order is significant.
# At the beginning, assume to have compilers/assemblers/librarian/linker
# running on the host platform, but not necessarily anything else.
# If necessary, build clibext library (64-bit)
[ INCLUDE "<OWROOT>/bld/watcom/builder.ctl" ]
# Start with language and API headers
[ INCLUDE "<OWROOT>/bld/hdr/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/os2api/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/w16api/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/w32api/builder.ctl" ]
# Continue with runtime libraries.
[ INCLUDE "<OWROOT>/bld/clib/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/mathlib/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/cpplib/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/rtdll/builder.ctl" ]
#
[ INCLUDE "<OWROOT>/bld/f77/f77lib/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/fpuemu/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/omftools/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/graphlib/builder.ctl" ]
# Start with DOS4GW DOS extender stub.
[ INCLUDE "<OWROOT>/bld/wstuba/builder.ctl" ]
# Continue with Causeway DOS extender.
[ INCLUDE "<OWROOT>/bld/causeway/builder.ctl" ]
# Continue with WIN386 extender.
[ INCLUDE "<OWROOT>/bld/win386/builder.ctl" ]
# Now we have enough to start cross building everything else
# Start with the libs used by various tools
[ INCLUDE "<OWROOT>/bld/wres/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/orl/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/owl/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/dwarf/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/cfloat/builder.ctl" ]
# Continue with the assemblers/librarian/linker/make
[ INCLUDE "<OWROOT>/bld/wasm/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/as/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/nwlib/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/wl/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/wmake/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/wtouch/builder.ctl" ]
# On to the compilers
[ INCLUDE "<OWROOT>/bld/cg/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/cc/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/plusplus/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/f77/wfc/builder.ctl" ]
# Resource tools, first Resource compiler
[ INCLUDE "<OWROOT>/bld/rc/builder.ctl" ]
# Continue with SDK tools
[ INCLUDE "<OWROOT>/bld/wpi/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/commonui/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/sdk/builder.ctl" ]
# Now miscellaneous command line tools
[ INCLUDE "<OWROOT>/bld/ndisasm/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/exedump/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/dmpobj/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/wcl/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/f77/wfl/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/wstrip/builder.ctl" ]
# User interface libs
[ INCLUDE "<OWROOT>/bld/ncurses/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/ui/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/gui/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/aui/builder.ctl" ]
# The vi(w) editor
[ INCLUDE "<OWROOT>/bld/rcsdll/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/vi/builder.ctl" ]
# Build the debugger and sampler
[ INCLUDE "<OWROOT>/bld/dip/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/mad/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/rsilib/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/wattcp/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/trap/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/wv/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/wsample/builder.ctl" ]
# GUI tools libraries
[ INCLUDE "<OWROOT>/bld/wclass/builder.ctl" ]
# Other GUI tools
[ INCLUDE "<OWROOT>/bld/wprof/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/browser/builder.ctl" ]
# The IDE tools
[ INCLUDE "<OWROOT>/bld/editdll/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/idebatch/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/ide/builder.ctl" ]
# The miscelaneous tools
[ INCLUDE "<OWROOT>/bld/cmdedit/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/cvpack/builder.ctl" ]
# OW clones for MS tools
[ INCLUDE "<OWROOT>/bld/mstools/builder.ctl" ]
# Miscelaneous files
[ INCLUDE "<OWROOT>/bld/misc/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/bdiff/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/techinfo/builder.ctl" ]
# Source code samples
[ INCLUDE "<OWROOT>/bld/src/builder.ctl" ]
# IDE samples
[ INCLUDE "<OWROOT>/bld/idedemo/builder.ctl" ]
# Build help viewer
[ INCLUDE "<OWROOT>/bld/hlpview/builder.ctl" ]
# Build help compilers
[ INCLUDE "<OWROOT>/bld/wgml/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/hcdos/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/hcwin/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/wipfc/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/bmp2eps/builder.ctl" ]
# Copy the redistributable components
[ INCLUDE "<OWROOT>/bld/redist/builder.ctl" ]
# Build installer tools
[ INCLUDE "<OWROOT>/bld/uninstal/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/setupgui/builder.ctl" ]
# Build text documents
[ INCLUDE "<OWROOT>/bld/docstxt/builder.ctl" ]

# Build documentation
[ IFDEF <OWDOCBUILD> 1 ]
[ INCLUDE "<OWROOT>/docs/builder.ctl" ]
[ ENDIF ]
# Build installers
[ IFDEF <OWDISTRBUILD> 1 ]
[ INCLUDE "<OWROOT>/distrib/ow/builder.ctl" ]
[ ENDIF ]

# test - build POSIX tools (awk, sed, cp, ...)
[ INCLUDE "<OWROOT>/bld/posix/builder.ctl" ]

[ BLOCK <BLDRULE> docsclean docs ]
#=================================
[ INCLUDE "<OWROOT>/docs/builder.ctl" ]

[ BLOCK <BLDRULE> install missing instclean ]
#============================================
[ INCLUDE "<OWROOT>/distrib/ow/builder.ctl" ]

[ BLOCK <BLDRULE> testclean test cleanlog ]
#==========================================
[ INCLUDE "<OWROOT>/bld/wasmtest/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/ctest/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/f77test/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/plustest/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/clibtest/builder.ctl" ]
[ INCLUDE "<OWROOT>/bld/mathtest/builder.ctl" ]

[ BLOCK <1> relclean passclean ]
#===============================
    echo rm -rf <OWRELROOT>
    rm -rf <OWRELROOT>

[ BLOCK . . ]

cdsay .
