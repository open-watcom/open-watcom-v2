name = cp
objs = cp.obj cpmem.obj cpfile.obj cpglob.obj cpio.obj

!include $(posix_dir)\mif\targos.mif
!ifeq targos dos
model=s
!else ifeq targos dosos2
model=s
!else
model=f
!endif
extra_lnk = xlnk

!include $(posix_dir)\mif\makeone.mif

xlnk: .PROCEDURE
    @%append $(LNK) op stack=20k
