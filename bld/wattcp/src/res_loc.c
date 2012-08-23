/*
 * ++Copyright++ 1985, 1989, 1993
 * -
 * Copyright (c) 1985, 1989, 1993
 *    The Regents of the University of California.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *   This product includes software developed by the University of
 *   California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * -
 * Portions Copyright (c) 1993 by Digital Equipment Corporation.
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies, and that
 * the name of Digital Equipment Corporation not be used in advertising or
 * publicity pertaining to distribution of the document or software without
 * specific, written prior permission.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND DIGITAL EQUIPMENT CORP. DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL DIGITAL EQUIPMENT
 * CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 * -
 * --Copyright--
 *
 * Code taken from RFC-1876
 *
 * Routines to convert between on-the-wire RR format and zone file
 * format.  Does not contain conversion to/from decimal degrees;
 * divide or multiply by 60*60*1000 for that.
 *
 * Adapted for WatTCP by Gisle Vanem 1997
 */

#include "resolver.h"

#if defined(USE_BIND)

static unsigned long poweroften[10] = {
                     1, 10, 100, 1000, 10000, 100000,
                     1000000,10000000,100000000,1000000000
                   };

/*
 * takes an XeY precision/size value, returns a string representation.
 */
static const char *precsize_ntoa (BYTE prec)
{
  static char retbuf[sizeof("90000000.00")];

  int   mantissa = (int)((prec >> 4) & 0x0f) % 10;
  int   exponent = (int)((prec >> 0) & 0x0f) % 10;
  DWORD value    = mantissa * poweroften[exponent];

  sprintf (retbuf,"%lu.%.2lu", value / 100, value % 100);
  return (retbuf);
}

/*
 * converts ascii size/precision X * 10**Y(cm) to 0xXY. moves pointer.
 */
static BYTE precsize_aton (char **strptr)
{
  UINT mval   = 0;
  UINT cmval  = 0;
  BYTE retval = 0;
  char *cp    = *strptr;
  int  exponent;
  int  mantissa;

  while (isdigit(*cp))
     mval = mval * 10 + (*cp++ - '0');

  if (*cp == '.')             /* centimeters */
  {
    cp++;
    if (isdigit(*cp))
    {
      cmval = (*cp++ - '0') * 10;
      if (isdigit(*cp))
         cmval += (*cp++ - '0');
    }
  }
  cmval = (mval * 100) + cmval;

  for (exponent = 0; exponent < 9; exponent++)
      if (cmval < poweroften[exponent+1])
         break;

  mantissa = cmval / poweroften[exponent];
  if (mantissa > 9)
      mantissa = 9;

  retval = (mantissa << 4) | exponent;
  *strptr = cp;
  return (retval);
}

/*
 * Converts ascii lat/lon to unsigned encoded 32-bit number
 */
static DWORD latlon2ul (char **latlonstrptr, int *which)
{
  DWORD retval;
  char *cp   = *latlonstrptr;
  int   deg  = 0;
  int   min  = 0;
  int   secs = 0;
  int   secsfrac = 0;

  while (isdigit(*cp))
     deg = deg * 10 + (*cp++ - '0');

  while (isspace(*cp))
     cp++;

  if (!(isdigit(*cp)))
     goto fndhemi;

  while (isdigit(*cp))
     min = min * 10 + (*cp++ - '0');

  while (isspace(*cp))
     cp++;

  if (!(isdigit(*cp)))
     goto fndhemi;

  while (isdigit(*cp))
     secs = secs * 10 + (*cp++ - '0');

  if (*cp == '.')            /* decimal seconds */
  {
    cp++;
    if (isdigit(*cp))
    {
      secsfrac = (*cp++ - '0') * 100;
      if (isdigit(*cp))
      {
        secsfrac += (*cp++ - '0') * 10;
        if (isdigit(*cp))
           secsfrac += (*cp++ - '0');
      }
    }
  }

  while (!isspace(*cp))   /* if any trailing garbage */
     cp++;

  while (isspace(*cp))
     cp++;

fndhemi:

  switch (toupper(*cp))
  {
    case 'N':
    case 'E':
         retval = (1UL << 31UL)
                + (((((deg * 60UL) + min) * 60UL) + secs) * 1000UL)
                + secsfrac;
         break;
    case 'S':
    case 'W':
         retval = (1UL << 31UL)
                - (((((deg * 60UL) + min) * 60UL) + secs) * 1000UL)
                - secsfrac;
         break;
    default:
         retval = 0;     /* invalid value -- indicates error */
         break;
  }

  switch (toupper(*cp))
  {
    case 'N':
    case 'S':
         *which = 1;      /* latitude */
         break;
    case 'E':
    case 'W':
         *which = 2;      /* longitude */
         break;
    default:
         *which = 0;      /* error */
         break;
  }

  cp++;                   /* skip the hemisphere */

  while (!isspace(*cp))   /* if any trailing garbage */
     cp++;

  while (isspace(*cp))    /* move to next field */
     cp++;

  *latlonstrptr = cp;
  return (retval);
}

/*
 * Converts a zone file representation in a string to an RDATA
 * on-the-wire representation.
 */
int loc_aton (const char *ascii, u_char *binary)
{
  char *cp, *maxcp;
  BYTE *bcp;
  DWORD latit     = 0, longit  = 0, alt = 0;
  DWORD lltemp1   = 0, lltemp2 = 0;
  int   altmeters = 0;
  int   altfrac   = 0;
  int   altsign   = 1;
  BYTE  hp        = 0x16;   /* default = 1e6 cm = 10000.00m = 10km */
  BYTE  vp        = 0x13;   /* default = 1e3 cm = 10.00m           */
  BYTE  siz       = 0x12;   /* default = 1e2 cm = 1.00m            */
  int   which1    = 0;
  int   which2    = 0;

  cp    = (char*)ascii;
  maxcp = cp + strlen (ascii);

  lltemp1 = latlon2ul (&cp, &which1);
  lltemp2 = latlon2ul (&cp, &which2);

  switch (which1 + which2)
  {
    case 3:                 /* 1 + 2, the only valid combination */
         if (which1 == 1 && which2 == 2)  /* normal case */
         {
           latit  = lltemp1;
           longit = lltemp2;
         }
         else if (which1 == 2 && which2 == 1) /* reversed */
         {
           longit = lltemp1;
           latit  = lltemp2;
         }
         else               /* some kind of brokenness */
           return (0);
         break;
    default:                /* we didn't get one of each */
         return (0);
  }

  /* altitude */
  if (*cp == '-')
  {
    altsign = -1;
    cp++;
  }

  if (*cp == '+')
     ++cp;

  while (isdigit(*cp))
     altmeters = altmeters * 10 + (*cp++ - '0');

  if (*cp == '.')                /* decimal meters */
  {
    cp++;
    if (isdigit(*cp))
    {
      altfrac = (*cp++ - '0') * 10;
      if (isdigit(*cp))
         altfrac += (*cp++ - '0');
    }
  }

  alt = (10000000 + (altsign * (altmeters * 100 + altfrac)));

  while (!isspace(*cp) && (cp < maxcp))
     cp++;                /* if trailing garbage or m */

  while (isspace(*cp) && (cp < maxcp))
     cp++;

  if (cp >= maxcp)
     goto defaults;

  siz = precsize_aton (&cp);

  while (!isspace(*cp) && (cp < maxcp)) /* if trailing garbage or m */
     cp++;

  while (isspace(*cp) && (cp < maxcp))
     cp++;

  if (cp >= maxcp)
     goto defaults;

  hp = precsize_aton (&cp);

  while (!isspace(*cp) && (cp < maxcp)) /* if trailing garbage or m */
     cp++;

  while (isspace(*cp) && (cp < maxcp))
     cp++;

  if (cp >= maxcp)
     goto defaults;

  vp = precsize_aton(&cp);

 defaults:

  bcp = binary;
  *bcp++ = (BYTE) 0;  /* version byte */
  *bcp++ = siz;
  *bcp++ = hp;
  *bcp++ = vp;
  PUTLONG (latit,bcp);
  PUTLONG (longit,bcp);
  PUTLONG (alt,bcp);

  return (16);            /* size of RR in octets */
}

/*
 * Takes an on-the-wire LOC RR and prints it in zone file
 * (human readable) format.
 */
char * loc_ntoa (const u_char *binary, char *ascii)
{
  static char  tmpbuf[255*3];
  const  DWORD referencealt = 100000UL * 100UL;
  const  BYTE *rcp;
  char  *cp, *sizestr, *hpstr, *vpstr;

  int   latdeg,  latmin,  latsec,  latsecfrac;
  int   longdeg, longmin, longsec, longsecfrac;
  char  northsouth, eastwest;
  int   altmeters,  altfrac, altsign;

  long  latval, longval, altval;
  DWORD templ;
  BYTE  sizeval, hpval, vpval, versionval;

  rcp = binary;
  if (ascii)
       cp = ascii;
  else cp = tmpbuf;

  versionval = *rcp++;

  if (versionval)
  {
    sprintf (cp, "; error: unknown LOC RR version");
    return (cp);
  }

  sizeval = *rcp++;
  hpval   = *rcp++;
  vpval   = *rcp++;

  GETLONG (templ, rcp);
  latval = (templ - (1UL << 31));

  GETLONG (templ, rcp);
  longval = (templ - (1UL << 31));

  GETLONG (templ, rcp);
  if (templ < referencealt)  /* below WGS 84 spheroid */
  {
    altval  = referencealt - templ;
    altsign = -1;
  }
  else
  {
    altval  = templ - referencealt;
    altsign = 1;
  }

  if (latval < 0)
  {
    northsouth = 'S';
    latval = -latval;
  }
  else
    northsouth = 'N';

  latsecfrac = latval % 1000;
  latval     = latval / 1000;
  latsec     = latval % 60;
  latval     = latval / 60;
  latmin     = latval % 60;
  latval     = latval / 60;
  latdeg     = latval;

  if (longval < 0)
  {
    eastwest = 'W';
    longval  = -longval;
  }
  else
    eastwest = 'E';

  longsecfrac = longval % 1000;
  longval     = longval / 1000;
  longsec     = longval % 60;
  longval     = longval / 60;
  longmin     = longval % 60;
  longval     = longval / 60;
  longdeg     = longval;

  altfrac   = altval % 100;
  altmeters = (altval / 100) * altsign;

  sizestr = strdup (precsize_ntoa(sizeval));
  hpstr   = strdup (precsize_ntoa(hpval));
  vpstr   = strdup (precsize_ntoa(vpval));

  sprintf (cp,
           "%d %.2d %.2d.%.3d %c %d %.2d %.2d.%.3d %c %d.%.2dm %sm %sm %sm",
           latdeg, latmin, latsec, latsecfrac, northsouth,
           longdeg, longmin, longsec, longsecfrac, eastwest,
           altmeters, altfrac, sizestr, hpstr, vpstr);
  free (sizestr);
  free (hpstr);
  free (vpstr);

  return (cp);
}
#endif /* USE_BIND */

