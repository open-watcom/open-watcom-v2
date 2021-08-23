#ifndef __IOPORT_H
#define __IOPORT_H

/*
 * Make sure <conio.h> (or <pc.h> for djgpp) gets included
 */
#ifndef __WATT_TARGET_H
#error You must include IOPORT.H after TARGET.H
#endif

/*
 * Macros necessary to bypass the limitation of Borland's BCC32 not
 * to allow inline port functions. Assignements should be volatile,
 * check the .asm output.
 * NB! macro-args must be TASM/MASM compatible
 */

#if defined(__BORLANDC__) && defined(__FLAT__)
#  define VOLATILE        volatile
#  define __in(p,t,z)     (_DX=(unsigned short)(p),__emit__(0xEC+z),(unsigned t)_AX)
#  define __out(p,x,z)    {_AX=(unsigned short)(x);_DX=(unsigned short)(p);__emit__(0xEE+z);}
#  define _inportb(p)     __in(p,char,0)
#  define _inportw(p)     __in(p,short,1)
#  define _outportb(p,x)  __out(p,x,0)
#  define _outportw(p,x)  __out(p,x,1)

#elif defined (__HIGHC__)
#  define _inportb(p)     _inb(p)
#  define _inportw(p)      _inw(p)
#  define _outportb(p,x)  _outb(p,x)
#  define _outportw(p,x)  _outpw(p,x)
#  define VOLATILE

#elif defined (__DJGPP__)
#  define _inportb(p)     inportb(p)
#  define _inport(p)      inportw(p)
#  define _outportb(p,x)  outportb(p,x)
#  define _outportw(p,x)  outportw(p,x)
#  define VOLATILE 

#else
#  define _inportb(p)     inp(p)
#  define _inportw(p)     inpw(p)
#  define _outportb(p,x)  outp(p,x)
#  define _outportw(p,x)  outpw(p,x)
#  define VOLATILE
#endif

#endif
