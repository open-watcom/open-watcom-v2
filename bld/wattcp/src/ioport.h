#ifndef __IOPORT_H
#define __IOPORT_H

#ifndef __WATT_TARGET_H
#error You must include IOPORT.H after TARGET.H
#endif

/*
 * Macros necessary to bypass the limitation of Borland's BCC32 not
 * to allow inline port functions. Assignements should be volatile,
 * check the .asm output.
 * NB! macro-args must be TASM/MASM compatible
 */

#  define _inportb(p)     inp(p)
#  define _inportw(p)     inpw(p)
#  define _outportb(p,x)  outp(p,x)
#  define _outportw(p,x)  outpw(p,x)
#  define VOLATILE

#endif
