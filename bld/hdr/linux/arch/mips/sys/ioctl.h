/**** from <asm/ioctl.h> ****/

/*
 * The original linux ioctl numbering scheme was just a general
 * "anything goes" setup, where more or less random numbers were
 * assigned.  Sorry, I was clueless when I started out on this.
 *
 * On the alpha, we'll try to clean it up a bit, using a more sane
 * ioctl numbering, and also trying to be compatible with OSF/1 in
 * the process. I'd like to clean it up for the i386 as well, but
 * it's so painful recognizing both the new and the old numbers..
 *
 * The same applies for for the MIPS ABI; in fact even the macros
 * from Linux/Alpha fit almost perfectly.
 */

#define _IOC_NRBITS 8
#define _IOC_TYPEBITS   8
#define _IOC_SIZEBITS   13
#define _IOC_DIRBITS    3

#define _IOC_NRMASK ((1 << _IOC_NRBITS)-1)
#define _IOC_TYPEMASK   ((1 << _IOC_TYPEBITS)-1)
#define _IOC_SIZEMASK   ((1 << _IOC_SIZEBITS)-1)
#define _IOC_DIRMASK    ((1 << _IOC_DIRBITS)-1)

#define _IOC_NRSHIFT    0
#define _IOC_TYPESHIFT  (_IOC_NRSHIFT+_IOC_NRBITS)
#define _IOC_SIZESHIFT  (_IOC_TYPESHIFT+_IOC_TYPEBITS)
#define _IOC_DIRSHIFT   (_IOC_SIZESHIFT+_IOC_SIZEBITS)

/*
 * We to additionally limit parameters to a maximum 255 bytes.
 */
#define _IOC_SLMASK 0xff

/*
 * Direction bits _IOC_NONE could be 0, but OSF/1 gives it a bit.
 * And this turns out useful to catch old ioctl numbers in header
 * files for us.
 */
#define _IOC_NONE   1U
#define _IOC_READ   2U
#define _IOC_WRITE  4U

/*
 * The following are included for compatibility
 */
#define _IOC_VOID   0x20000000
#define _IOC_OUT    0x40000000
#define _IOC_IN     0x80000000
#define _IOC_INOUT  (IOC_IN|IOC_OUT)

#define _IOC(dir,type,nr,size) \
    (((dir)  << _IOC_DIRSHIFT) | \
     ((type) << _IOC_TYPESHIFT) | \
     ((nr)   << _IOC_NRSHIFT) | \
     ((size) << _IOC_SIZESHIFT))

/* provoke compile error for invalid uses of size argument */
extern unsigned int __invalid_size_argument_for_IOC;
#define _IOC_TYPECHECK(t) \
    ((sizeof(t) == sizeof(t[1]) && \
      sizeof(t) < (1 << _IOC_SIZEBITS)) ? \
      sizeof(t) : __invalid_size_argument_for_IOC)

/* used to create numbers */
#define _IO(type,nr)        _IOC(_IOC_NONE,(type),(nr),0)
#define _IOR(type,nr,size)  _IOC(_IOC_READ,(type),(nr),(_IOC_TYPECHECK(size)))
#define _IOW(type,nr,size)  _IOC(_IOC_WRITE,(type),(nr),(_IOC_TYPECHECK(size)))
#define _IOWR(type,nr,size) _IOC(_IOC_READ|_IOC_WRITE,(type),(nr),(_IOC_TYPECHECK(size)))
#define _IOR_BAD(type,nr,size)  _IOC(_IOC_READ,(type),(nr),sizeof(size))
#define _IOW_BAD(type,nr,size)  _IOC(_IOC_WRITE,(type),(nr),sizeof(size))
#define _IOWR_BAD(type,nr,size) _IOC(_IOC_READ|_IOC_WRITE,(type),(nr),sizeof(size))


/* used to decode them.. */
#define _IOC_DIR(nr)        (((nr) >> _IOC_DIRSHIFT) & _IOC_DIRMASK)
#define _IOC_TYPE(nr)       (((nr) >> _IOC_TYPESHIFT) & _IOC_TYPEMASK)
#define _IOC_NR(nr)     (((nr) >> _IOC_NRSHIFT) & _IOC_NRMASK)
#define _IOC_SIZE(nr)       (((nr) >> _IOC_SIZESHIFT) & _IOC_SIZEMASK)

/* ...and for the drivers/sound files... */

#define IOC_IN      (_IOC_WRITE << _IOC_DIRSHIFT)
#define IOC_OUT     (_IOC_READ << _IOC_DIRSHIFT)
#define IOC_INOUT   ((_IOC_WRITE|_IOC_READ) << _IOC_DIRSHIFT)
#define IOCSIZE_MASK    (_IOC_SIZEMASK << _IOC_SIZESHIFT)
#define IOCSIZE_SHIFT   (_IOC_SIZESHIFT)

/**** From <asm/ioctls.h> ****/

#define TCGETA      0x5401
#define TCSETA      0x5402  /* Clashes with SNDCTL_TMR_START sound ioctl */
#define TCSETAW     0x5403
#define TCSETAF     0x5404

#define TCSBRK      0x5405
#define TCXONC      0x5406
#define TCFLSH      0x5407

#define TCGETS      0x540d
#define TCSETS      0x540e
#define TCSETSW     0x540f
#define TCSETSF     0x5410

#define TIOCEXCL    0x740d      /* set exclusive use of tty */
#define TIOCNXCL    0x740e      /* reset exclusive use of tty */
#define TIOCOUTQ    0x7472      /* output queue size */
#define TIOCSTI     0x5472      /* simulate terminal input */
#define TIOCMGET    0x741d      /* get all modem bits */
#define TIOCMBIS    0x741b      /* bis modem bits */
#define TIOCMBIC    0x741c      /* bic modem bits */
#define TIOCMSET    0x741a      /* set all modem bits */
#define TIOCPKT     0x5470      /* pty: set/clear packet mode */
#define  TIOCPKT_DATA       0x00    /* data packet */
#define  TIOCPKT_FLUSHREAD  0x01    /* flush packet */
#define  TIOCPKT_FLUSHWRITE 0x02    /* flush packet */
#define  TIOCPKT_STOP       0x04    /* stop output */
#define  TIOCPKT_START      0x08    /* start output */
#define  TIOCPKT_NOSTOP     0x10    /* no more ^S, ^Q */
#define  TIOCPKT_DOSTOP     0x20    /* now do ^S ^Q */
/* #define  TIOCPKT_IOCTL       0x40    state change of pty driver */
#define TIOCSWINSZ  _IOW('t', 103, struct winsize)  /* set window size */
#define TIOCGWINSZ  _IOR('t', 104, struct winsize)  /* get window size */
#define TIOCNOTTY   0x5471      /* void tty association */
#define TIOCSETD    0x7401
#define TIOCGETD    0x7400

#define FIOCLEX     0x6601
#define FIONCLEX    0x6602
#define FIOASYNC    0x667d
#define FIONBIO     0x667e
#define FIOQSIZE    0x667f

#define TIOCGLTC    0x7474          /* get special local chars */
#define TIOCSLTC    0x7475          /* set special local chars */
#define TIOCSPGRP   _IOW('t', 118, int) /* set pgrp of tty */
#define TIOCGPGRP   _IOR('t', 119, int) /* get pgrp of tty */
#define TIOCCONS    _IOW('t', 120, int) /* become virtual console */

#define FIONREAD    0x467f
#define TIOCINQ     FIONREAD

#define TIOCGETP        0x7408
#define TIOCSETP        0x7409
#define TIOCSETN        0x740a          /* TIOCSETP wo flush */

/* #define TIOCSETA _IOW('t', 20, struct termios) set termios struct */
/* #define TIOCSETAW    _IOW('t', 21, struct termios) drain output, set */
/* #define TIOCSETAF    _IOW('t', 22, struct termios) drn out, fls in, set */
/* #define TIOCGETD _IOR('t', 26, int)  get line discipline */
/* #define TIOCSETD _IOW('t', 27, int)  set line discipline */
                        /* 127-124 compat */

#define TIOCSBRK    0x5427  /* BSD compatibility */
#define TIOCCBRK    0x5428  /* BSD compatibility */
#define TIOCGSID    0x7416  /* Return the session ID of FD */
#define TIOCGPTN    _IOR('T',0x30, unsigned int) /* Get Pty Number (of pty-mux device) */
#define TIOCSPTLCK  _IOW('T',0x31, int)  /* Lock/unlock Pty */

/* I hope the range from 0x5480 on is free ... */
#define TIOCSCTTY   0x5480      /* become controlling tty */
#define TIOCGSOFTCAR    0x5481
#define TIOCSSOFTCAR    0x5482
#define TIOCLINUX   0x5483
#define TIOCGSERIAL 0x5484
#define TIOCSSERIAL 0x5485
#define TCSBRKP     0x5486  /* Needed for POSIX tcsendbreak() */
#define TIOCSERCONFIG   0x5488
#define TIOCSERGWILD    0x5489
#define TIOCSERSWILD    0x548a
#define TIOCGLCKTRMIOS  0x548b
#define TIOCSLCKTRMIOS  0x548c
#define TIOCSERGSTRUCT  0x548d /* For debugging only */
#define TIOCSERGETLSR   0x548e /* Get line status register */
#define TIOCSERGETMULTI 0x548f /* Get multiport config  */
#define TIOCSERSETMULTI 0x5490 /* Set multiport config */
#define TIOCMIWAIT      0x5491 /* wait for a change on serial input line(s) */
#define TIOCGICOUNT     0x5492 /* read serial port inline interrupt counts */
#define TIOCGHAYESESP   0x5493 /* Get Hayes ESP configuration */
#define TIOCSHAYESESP   0x5494 /* Set Hayes ESP configuration */

