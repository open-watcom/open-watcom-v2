name = compress
objs = compress.obj compapi.obj compusi.obj
extra_cflags=-dNOSIGNAL

!include $(posix_dir)\mif\targos.mif
!ifeq targos pharlap
model=f
!else ifeq targos os2v2
model=f
!else ifeq targos nt
model=f
!else ifeq targos qnx
model=s
!else ifeq targos alpha
model=f
!else
model=c
!endif

!include $(posix_dir)\mif\makeone.mif
