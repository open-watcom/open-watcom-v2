// floating point conversion buffer length definition
// used by various floating point conversion routines
// used in clib/startup/c/cvtbuf.c, lib_misc/h/thread.h
// and mathlib/c/efcvt.c
// it must be equal maximum FP precision ( LDBL_DIG )
// hold it in sync with LDBL_DIG in watcom/h/xfloat.h

#define __FPCVT_BUFFERLEN  19

_WCRTLINK extern void *__CVTBuffer( void );
