#pmake: build lcl pmd qnx 16 32 trap x86
#
# Create the QNX PMD.TRP
#

os      = qnx
srv     = lcl
cpu     = x86
name    = pmd

extra_objs = &
        qnxpmd.obj &
        qnxcomm.obj &
        qnxfile.obj &
        squish87.obj &
        supp.obj &
        core.obj

extra_incs = $(trap_dir)\lcl\h

!include $(trap_dir)\trap.mif
