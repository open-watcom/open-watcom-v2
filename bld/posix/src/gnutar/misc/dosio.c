/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


/*
 * Dos direct-disk I/O routines for PDTAR
 * By E. Roskos 2/88
 * For Minix-compatible multivolume tar implementation under DOS
 *
 * These routines are based on my Minix "build" I/O code, although
 * changed a lot...
 */

#ifdef MSDOS

#include <stdio.h>
#include <fcntl.h>
#include <dos.h>

extern int physdrv;
extern int devsize;
extern int ftty;
static long curblk = 0;
static int inited = 0;

static void diskerr(char *, int, int, int );

/*
 * Local I/O buffers.  We do the actual disk I/O to one of these two.
 * The reason is that we select which one we'll use, and set iobuf to
 * point to it, in order to get a block of memory that doesn't cross
 * a 64K boundary -- because the DMA controller can't do I/O across
 * a 64K boundary.
 */
static char buf1[512], buf2[512];
static char *iobuf;

/*
 * DMAoverrun checks whether buff1 crosses a 64K boundary.
 */
static int
DMAoverrun(buff1)
char *buff1;
{
int i;

        i = (int)buff1;
        return(i > i + 512);
}

/*
 * absio does absolute disk I/O, using the ROM BIOS
 * It performs BIOS operation "fn", on "drive", specifying
 * block number "blocknr" and buffer "buff" (which must
 * be in the single (small-model) data segment).
 * Note that the block numbering scheme corresponds to the
 * Minix, PC/IX, and DOS 2.x block numbering, not the DOS
 * 3.x block numbering.
 */
static int
absio(fn, drive, blocknr, buff)
int fn;
int drive;
long blocknr;
char *buff;
{
union REGS iregs;
union REGS oregs;
int track;

        iregs.h.ah = fn;
        iregs.h.dl = drive;
        track = blocknr / 9;
        iregs.h.dh = track & 1;
        iregs.h.ch = track >> 1;
        iregs.h.cl = (blocknr % 9) + 1;
        iregs.h.al = 1;
        iregs.x.bx = (int)buff;

        int86(0x13, &iregs, &oregs);

        if (oregs.x.cflag)
        {
#ifdef DEBUGIO
                fprintf(stderr, "absio: error %sing drv %c block %d address %x: bios code %x\n",
                        fn==2? "read" : "writ", 'A'+drive, blocknr, buff, oregs.h.ah&0xff);
#endif /* DEBUGIO */
                return(oregs.h.ah&0xff);
        }
        else
        {
                return(0);
        }
}

/*
 * initdiskio initializes the floppy disk subsystem and selects
 * a local buffer for us to use, if this is the first time it is
 * called.  Otherwise, it does nothing.
 */
static void
initdskio()
{
        if (!inited)
        {
                absio(0, 0, 0, 0);
                if (DMAoverrun(buf1))
                        iobuf = buf2;
                else
                        iobuf = buf1;
                inited++;
        }
}

/*
 * absread performs an absolute disk read of "drive"'s block
 * "blocknr", reading into the buffer at "buff".
 */
static int
absread(drive, blocknr, buff)
int drive;
long blocknr;
char *buff;
{
int err;

        initdskio();
        err = absio(2, drive, blocknr, iobuf);
        if (!err)
                memcpy(buff, iobuf, 512);
        return(err);
}

/*
 * abswrite performs an absolute disk write of "drive"'s block
 * "blocknr", reading into the buffer at "buff".
 */
static int
abswrite(drive, blocknr, buff)
int drive;
long blocknr;
char *buff;
{

        initdskio();
        memcpy(iobuf, buff, 512);
        return(absio(3, drive, blocknr, iobuf));
}

/*
 * physrw reads or writes the data at "buf" for length "len", which
 * must be a multiple of 512 bytes; it reads if fread is 1, or writes
 * if fread is 0.  The data is read/written on the next consecutive
 * block of the floppy disk; if the end of the disk has been reached
 * (as determined by the disk size block count in the global variable
 * devsize) it asks the user to change disks before it performs the I/O,
 * then performs the I/O to block 0 of the new disk.
 */
static int
physrw(buf, len, fread)
char *buf;
int len;
int fread;
{
int err;
int errct = 0;
int nbytes;

        nbytes = len; /* save for return value */

        /* be sure size of xfer is a multiple of DOS physical block size */
        if (len & 0x1ff)
        {
                fprintf(stderr, "tar: fatal error: phys disk I/O must be ");
                fprintf(stderr, "multiple of 512 bytes\n");
                exit(1);
        }

        /* convert byte count to DOS block count */
        len >>= 9;

        /* now read or write a block at a time into the buffer */
        while (len > 0)
        {
                /* check for time to change disks */
                if (curblk >= devsize)
                {
                        uprintf(ftty, "\ntar: Change disks and press [Enter]: ");
                        while (ugetc(ftty)!='\n') ;
                        curblk = 0;
                }

                /* read or write the next block */
                if (fread)
                        err = absread(physdrv, curblk, buf);
                else
                        err = abswrite(physdrv, curblk, buf);

                /* check for an error */
                if (err)
                {
                        diskerr(fread? "reading" : "writing",
                                 physdrv, curblk, err);
                        errct++;
                }
                /* increment block number & buf addr, decrement count */
                curblk++;
                buf += 512;
                len--;
        }
        if (errct)
                return(-1);
        else
                return(nbytes);
}

/*
 * physwrite is the "write" version of physrw, and is what is called
 * from outside this package.  It writes the data at "buf" for
 * length "len", which must be a multiple of 512 bytes, as described
 * above under "physrw".
 */
int
physwrite(buf, len)
char *buf;
int len;
{
        return(physrw(buf, len, 0));
}

/*
 * see comments for physwrite
 */
int
physread(buf, len)
char *buf;
int len;
{
        return(physrw(buf, len, 1));
}

/*
 * This routine prints an error message for disk I/O: the operation
 * ("reading", "writing") is in s, the drive number in "drive",
 * the sector number in "sectnum", and the BIOS AH return code
 * is in "err".
 */
static void
diskerr(s,drive,sectnum,err)
int sectnum, err,drive;
char *s;
{
  extern char *derrtab[];
  char *mp;
  fprintf(stderr, "Error %s drive %c, sector: %d, code: %d = '",
           s, drive+'A',sectnum, err);
  switch (err)
  {
        case 0:
                mp = "No error";
                break;
        case 1:
                mp = "Bad command passed to BIOS";
                break;
        case 2:
                mp = "Address mark not found";
                break;
        case 3:
                mp = "Disk is write protected";
                break;
        case 4:
                mp = "Sector not found";
                break;
        case 8:
                mp = "DMA overrun";
                break;
        case 9:
                mp = "DMA crosses 64K boundary (internal error)";
                break;
        case 0x10:
                mp = "Bad CRC on disk read";
                break;
        case 0x20:
                mp = "Disk controller has failed";
                break;
        case 0x40:
                mp = "Seek failed";
                break;
        case 0x80:
                mp = "No response from controller";
                break;
        default:
                mp = "Unknown error";
                break;
        }
        fprintf(stderr,"%s'\n", mp);
}

#endif /* MSDOS */
