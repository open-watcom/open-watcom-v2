#pmake: build lcl os2 32 trap x86
#
# Create the 32-bit OS/2 STD.DLL
#

os      = os2v2
srv     = lcl
cpu     = x86
name    = std

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
        pmlock.obj &
        x86cpu.obj &
        supp.obj &
        core.obj

extra_linker = &
        exp TellHandles_.4,TellHardMode_.5 &
        lib os2386,os2

!include $(trap_dir)\trap.mif
