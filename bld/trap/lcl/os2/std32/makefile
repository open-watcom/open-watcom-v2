#pmake: build lcl os2 32 trap x86
#
# Create the OS/2 STD32.DLL
#

os      = os2
srv     = lcl
cpu     = x86
name    = std32

extra_cflags =-i$(watcom_dir)\os220_h

extra_objs = &
        accmisc.obj &
        os2v2acc.obj &
        os2rfx.obj &
        pgmexec.obj &
        softmode.obj &
        dbgthrd.obj &
        os2conv.obj &
        os2call.obj &
        loadstd.obj &
        pmlock.obj &
        x86cpu.obj &
        supp.obj &
        core.obj

extra_linker = &
        imp DOS16SELTOFLAT DOSCALLS.424 &
        exp TellHandles_.4,TellHardMode_.5 &
        lib os2

!include $(trap_dir)\trap.mif
