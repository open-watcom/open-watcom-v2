#pmake: build lcl os2 16 trap x86
#
# Create the OS/2 STD16.DLL
#

os      = os2
srv     = lcl
cpu     = x86
name    = std16

extra_objs = &
        accmisc.obj &
        os2v1acc.obj &
        os2rfx.obj &
        loadstd.obj &
        npxtype.obj &
        squish87.obj &
        x86cpu.obj &
        supp.obj &
        core.obj

!include $(trap_dir)\trap.mif
