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


/*@H************************ < COMPRESS API    > ****************************
*                                                                           *
*   compress : compapi.c  <current version of compress algorithm>           *
*                                                                           *
*   port by  : Donald J. Gloistein                                          *
*                                                                           *
*   Source, Documentation, Object Code:                                     *
*   released to Public Domain.  This code is based on code as documented    *
*   below in release notes.                                                 *
*                                                                           *
*---------------------------  Module Description  --------------------------*
*   Contains source code for modified Lempel-Ziv method (LZW) compression   *
*   and decompression.                                                      *
*                                                                           *
*   This code module can be maintained to keep current on releases on the   *
*   Unix system. The command shell and dos modules can remain the same.     *
*                                                                           *
*--------------------------- Implementation Notes --------------------------*
*                                                                           *
*   compiled with : compress.h compress.fns compress.c                      *
*   linked with   : compress.obj compusi.obj                                *
*                                                                           *
*   problems:                                                               *
*                                                                           *
*                                                                           *
*   CAUTION: Uses a number of defines for access and speed. If you change   *
*            anything, make sure about side effects.                        *
*                                                                           *
* Compression:                                                              *
* Algorithm:  use open addressing double hashing (no chaining) on the       *
* prefix code / next character combination.  We do a variant of Knuth's     *
* algorithm D (vol. 3, sec. 6.4) along with G. Knott's relatively-prime     *
* secondary probe.  Here, the modular division first probe is gives way     *
* to a faster exclusive-or manipulation.                                    *
* Also block compression with an adaptive reset was used in original code,  *
* whereby the code table is cleared when the compression ration decreases   *
* but after the table fills.  This was removed from this edition. The table *
* is re-sized at this point when it is filled , and a special CLEAR code is *
* generated for the decompressor. This results in some size difference from *
* straight version 4.0 joe Release. But it is fully compatible in both v4.0 *
* and v4.01                                                                 *
*                                                                           *
* Decompression:                                                            *
* This routine adapts to the codes in the file building the "string" table  *
* on-the-fly; requiring no table to be stored in the compressed file.  The  *
* tables used herein are shared with those of the compress() routine.       *
*                                                                           *
*     Initials ---- Name ---------------------------------                  *
*      DjG          Donald J. Gloistein, current port to MsDos 16 bit       *
*                   Plus many others, see rev.hst file for full list        *
*      LvR          Lyle V. Rains, many thanks for improved implementation  *
*                   of the compression and decompression routines.          *
*************************************************************************@H*/

#include <stdio.h>

#include "compress.h" /* contains the rest of the include file declarations */

static int offset;
static long int in_count ;         /* length of input */
static long int bytes_out;         /* length of compressed output */

static CODE prefxcode, nextfree;
static CODE highcode;
static CODE maxcode;
static HASH hashsize;
static int  bits;


/*
 * The following two parameter tables are the hash table sizes and
 * maximum code values for various code bit-lengths.  The requirements
 * are that Hashsize[n] must be a prime number and Maxcode[n] must be less
 * than Maxhash[n].  Table occupancy factor is (Maxcode - 256)/Maxhash.
 * Note:  I am using a lower Maxcode for 16-bit codes in order to
 * keep the hash table size less than 64k entries.
 */
CONST HASH hs[] = {
  0x13FF,       /* 12-bit codes, 75% occupancy */
  0x26C3,       /* 13-bit codes, 80% occupancy */
  0x4A1D,       /* 14-bit codes, 85% occupancy */
  0x8D0D,       /* 15-bit codes, 90% occupancy */
  0xFFD9        /* 16-bit codes, 94% occupancy, 6% of code values unused */
};
#define Hashsize(maxb) (hs[(maxb) -MINBITS])

CONST CODE mc[] = {
  0x0FFF,       /* 12-bit codes */
  0x1FFF,       /* 13-bit codes */
  0x3FFF,       /* 14-bit codes */
  0x7FFF,       /* 15-bit codes */
  0xEFFF        /* 16-bit codes, 6% of code values unused */
};
#define Maxcode(maxb) (mc[(maxb) -MINBITS])

#ifdef __STDC__
#ifndef NDEBUG
#define allocx(type, ptr, size) \
    (((ptr) = (type FAR *) emalloc((unsigned int)(size),sizeof(type))) == NULLPTR(type) \
    ?   (fprintf(stderr,"%s: "#ptr" -- ", prog_name), NOMEM) : OK \
    )
#else
#define allocx(type,ptr,size) \
    (((ptr) = (type FAR *) emalloc((unsigned int)(size),sizeof(type))) == NULLPTR(type) \
    ? NOMEM : OK \
    )
#endif
#else
#define allocx(type,ptr,size) \
    (((ptr) = (type FAR *) emalloc((unsigned int)(size),sizeof(type))) == NULLPTR(type) \
    ? NOMEM : OK \
    )
#endif

#define free_array(type,ptr,offset) \
    if (ptr != NULLPTR(type)) { \
        efree((ALLOCTYPE FAR *)((ptr) + (offset))); \
        (ptr) = NULLPTR(type); \
    }

  /*
   * Macro to allocate new memory to a pointer with an offset value.
   */
#define alloc_array(type, ptr, size, offset) \
    ( allocx(type, ptr, (size) - (offset)) != OK \
      ? NOMEM \
      : (((ptr) -= (offset)), OK) \
    )

static char FAR *sfx = NULLPTR(char) ;
#define suffix(code)     sfx[code]


#if (SPLIT_PFX)
  static CODE FAR *pfx[2] = {NULLPTR(CODE), NULLPTR(CODE)};
#else
  static CODE FAR *pfx = NULLPTR(CODE);
#endif


#if (SPLIT_HT)
  static CODE FAR *ht[2] = {NULLPTR(CODE),NULLPTR(CODE)};
#else
  static CODE FAR *ht = NULLPTR(CODE);
#endif


int alloc_tables(maxcode, hashsize)
  CODE maxcode;
  HASH hashsize;
{
  static CODE oldmaxcode = 0;
  static HASH oldhashsize = 0;

  if (hashsize > oldhashsize) {
#if (SPLIT_HT)
      free_array(CODE,ht[1], 0);
      free_array(CODE,ht[0], 0);
#else
      free_array(CODE,ht, 0);
#endif
    oldhashsize = 0;
  }

    if (maxcode > oldmaxcode) {
#if (SPLIT_PFX)
        free_array(CODE,pfx[1], 128);
        free_array(CODE,pfx[0], 128);
#else
        free_array(CODE,pfx, 256);
#endif
        free_array(char,sfx, 256);

        if (   alloc_array(char, sfx, maxcode + 1, 256)
#if (SPLIT_PFX)
            || alloc_array(CODE, pfx[0], (maxcode + 1) / 2, 128)
            || alloc_array(CODE, pfx[1], (maxcode + 1) / 2, 128)
#else
            || alloc_array(CODE, pfx, (maxcode + 1), 256)
#endif
        ) {
            oldmaxcode = 0;
            exit_stat = NOMEM;
            return(NOMEM);
        }
        oldmaxcode = maxcode;
    }
    if (hashsize > oldhashsize) {
        if (
#if (SPLIT_HT)
            alloc_array(CODE, ht[0], (hashsize / 2) + 1, 0)
            || alloc_array(CODE, ht[1], hashsize / 2, 0)
#else
            alloc_array(CODE, ht, hashsize, 0)
#endif
        ) {
            oldhashsize = 0;
            exit_stat = NOMEM;
            return(NOMEM);
        }
        oldhashsize = hashsize;
    }
    return (OK);
}

# if (SPLIT_PFX)
    /*
     * We have to split pfx[] table in half,
     * because it's potentially larger than 64k bytes.
     */
#   define prefix(code)   (pfx[(code) & 1][(code) >> 1])
# else
    /*
     * Then pfx[] can't be larger than 64k bytes,
     * or we don't care if it is, so we don't split.
     */
#   define prefix(code) (pfx[code])
# endif


/* The initializing of the tables can be done quicker with memset() */
/* but this way is portable through out the memory models.          */
/* If you use Microsoft halloc() to allocate the arrays, then       */
/* include the pragma #pragma function(memset)  and make sure that  */
/* the length of the memory block is not greater than 64K.          */
/* This also means that you MUST compile in a model that makes the  */
/* default pointers to be far pointers (compact or large models).   */
/* See the file COMPUSI.DOS to modify function emalloc().           */

# if (SPLIT_HT)
    /*
     * We have to split ht[] hash table in half,
     * because it's potentially larger than 64k bytes.
     */
#   define probe(hash)    (ht[(hash) & 1][(hash) >> 1])
#   define init_tables() \
    { \
      hash = hashsize >> 1; \
      ht[0][hash] = 0; \
      while (hash--) ht[0][hash] = ht[1][hash] = 0; \
      highcode = ~(~(CODE)0 << (bits = INITBITS)); \
      nextfree = (block_compress ? FIRSTFREE : 256); \
    }

# else

    /*
     * Then ht[] can't be larger than 64k bytes,
     * or we don't care if it is, so we don't split.
     */
#   define probe(hash) (ht[hash])
#   define init_tables() \
    { \
      hash = hashsize; \
      while (hash--) ht[hash] = 0; \
      highcode = ~(~(CODE)0 << (bits = INITBITS)); \
      nextfree = (block_compress ? FIRSTFREE : 256); \
    }

# endif

#ifdef COMP40
/* table clear for block compress */
/* this is for adaptive reset present in version 4.0 joe release */
/* DjG, sets it up and returns TRUE to compress and FALSE to not compress */
int cl_block ()
{
    register long int rat;

    checkpoint = in_count + CHECK_GAP;
#ifndef NDEBUG
        if ( debug ) {
        fprintf ( stderr, "count: %ld, ratio: ", in_count );
        prratio ( stderr, in_count, bytes_out );
                fprintf ( stderr, "\n");
        }
#endif

    if(in_count > 0x007fffff) { /* shift will overflow */
        rat = bytes_out >> 8;
        if(rat == 0)       /* Don't divide by zero */
            rat = 0x7fffffff;
        else
            rat = in_count / rat;
    }
    else
        rat = (in_count << 8) / bytes_out;  /* 8 fractional bits */

    if ( rat > ratio ){
        ratio = rat;
        return FALSE;
    }
    else {
        ratio = 0;
#ifndef NDEBUG
        if(debug)
                fprintf ( stderr, "clear\n" );
#endif
        return TRUE;    /* clear the table */
    }
    return FALSE; /* don't clear the table */
}
#endif

/*
 * compress stdin to stdout
 *
 */
void compress()
{
    int c,adjbits;
    register HASH hash;
    register CODE code;
    HASH hashf[256];

    maxcode = Maxcode(maxbits);
    hashsize = Hashsize(maxbits);

#ifdef COMP40
/* Only needed for adaptive reset */
    checkpoint = CHECK_GAP;
    ratio = 0;
#endif

    adjbits = maxbits -10;
    for (c = 256; --c >= 0; ){
        hashf[c] = ((( c &0x7) << 7) ^ c) << adjbits;
    }
    exit_stat = OK;
    if (alloc_tables(maxcode, hashsize))  /* exit_stat already set */
        return;
    init_tables();
    /* if not zcat or filter */
    if(is_list && !zcat_flg) {  /* Open output file */
        if (freopen(ofname, WRITE_FILE_TYPE, stdout) == NULL) {
            exit_stat = NOTOPENED;
            return;
        }
        if (!quiet)
            fprintf(stderr, "%s: ",ifname);
        setvbuf(stdout,zbuf,_IOFBF,ZBUFSIZE);
    }
 /*
   * Check the input stream for previously seen strings.  We keep
   * adding characters to the previously seen prefix string until we
   * get a character which forms a new (unseen) string.  We then send
   * the code for the previously seen prefix string, and add the new
   * string to our tables.  The check for previous strings is done by
   * hashing.  If the code for the hash value is unused, then we have
   * a new string.  If the code is used, we check to see if the prefix
   * and suffix values match the current input; if so, we have found
   * a previously seen string.  Otherwise, we have a hash collision,
   * and we try secondary hash probes until we either find the current
   * string, or we find an unused entry (which indicates a new string).
   */
    if (!nomagic) {
        putchar(magic_header[0]); putchar(magic_header[1]);
        putchar((char)(maxbits | block_compress));
        if(ferror(stdout)){  /* check it on entry */
            exit_stat = WRITEERR;
            return;
        }
        bytes_out = 3L;     /* includes 3-byte header mojo */
    }
    else
        bytes_out = 0L;      /* no 3-byte header mojo */
    in_count = 1L;
    offset = 0;

    if ((c = getchar()) == EOF) {
        exit_stat = ferror(stdin) ? READERR : OK;
        return;
    }
    prefxcode = (CODE)c;

    while ((c = getchar()) != EOF) {
        in_count++;
        hash = prefxcode ^ hashf[c];
        /* I need to check that my hash value is within range
        * because my 16-bit hash table is smaller than 64k.
        */
        if (hash >= hashsize)
            hash -= hashsize;
        if ((code = probe(hash)) != UNUSED) {
            if (suffix(code) != (char)c || prefix(code) != prefxcode) {
            /* hashdelta is subtracted from hash on each iteration of
            * the following hash table search loop.  I compute it once
            * here to remove it from the loop.
            */
                HASH hashdelta = (0x120 - c) << (adjbits);
                do  {
                    /* rehash and keep looking */
                    assert(code >= FIRSTFREE && code <= maxcode);
                    if (hash >= hashdelta) hash -= hashdelta;
                        else hash += (hashsize - hashdelta);
                    assert(hash < hashsize);
                    if ((code = probe(hash)) == UNUSED)
                        goto newcode;
                } while (suffix(code) != (char)c || prefix(code) != prefxcode);
            }
            prefxcode = code;
        }
        else {
            newcode: {
                putcode(prefxcode, bits);
                code = nextfree;
                assert(hash < hashsize);
                assert(code >= FIRSTFREE);
                assert(code <= maxcode + 1);
                if (code <= maxcode) {
                    probe(hash) = code;
                    prefix(code) = prefxcode;
                    suffix(code) = (char)c;
                    if (code > highcode) {
                        highcode += code;
                        ++bits;
                    }
                    nextfree = code + 1;
                }
#ifdef COMP40
                else if (in_count >= checkpoint && block_compress ) {
                    if (cl_block()){
#else
                else if (block_compress){
#endif
                        putcode((CODE)c, bits);
                        putcode((CODE)CLEAR,bits);
                        init_tables();
                        if ((c = getchar()) == EOF)
                            break;
                        in_count++;
#ifdef COMP40
                    }
#endif
                }
                prefxcode = (CODE)c;
            }
        }
    }
    putcode(prefxcode, bits);
    putcode((CODE)CLEAR, 0);
    if (ferror(stdout)){ /* check it on exit */
        exit_stat = WRITEERR;
        return;
    }
    /*
     * Print out stats on stderr
     */
    if(zcat_flg == 0 && !quiet) {
#ifndef NDEBUG
        fprintf( stderr,
            "%ld chars in, (%ld bytes) out, compression factor: ",
            in_count, bytes_out );
        prratio( stderr, in_count, bytes_out );
        fprintf( stderr, "\n");
        fprintf( stderr, "\tCompression as in compact: " );
        prratio( stderr, in_count-bytes_out, in_count );
        fprintf( stderr, "\n");
        fprintf( stderr, "\tLargest code (of last block) was %d (%d bits)\n",
            prefxcode - 1, bits );
#else
        fprintf( stderr, "Compression: " );
        prratio( stderr, in_count-bytes_out, in_count );
#endif /* NDEBUG */
    }
    if(bytes_out > in_count)      /*  if no savings */
        exit_stat = NOSAVING;
    return ;
}

CONST UCHAR rmask[9] = {0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};

void putcode(code,bits)
CODE code;
register int bits;
{
  static int oldbits = 0;
  static UCHAR outbuf[MAXBITS];
  register UCHAR *buf;
  register int shift;

  if (bits != oldbits) {
    if (bits == 0) {
      /* bits == 0 means EOF, write the rest of the buffer. */
      if (offset > 0) {
          fwrite(outbuf,1,(offset +7) >> 3, stdout);
          bytes_out += ((offset +7) >> 3);
      }
      offset = 0;
      oldbits = 0;
      fflush(stdout);
      return;
    }
    else {
      /* Change the code size.  We must write the whole buffer,
       * because the expand side won't discover the size change
       * until after it has read a buffer full.
       */
      if (offset > 0) {
        fwrite(outbuf, 1, oldbits, stdout);
        bytes_out += oldbits;
        offset = 0;
      }
      oldbits = bits;
#ifndef NDEBUG
      if ( debug )
        fprintf( stderr, "\nChange to %d bits\n", bits );
#endif /* !NDEBUG */
    }
  }
  /*  Get to the first byte. */
  buf = outbuf + ((shift = offset) >> 3);
  if ((shift &= 7) != 0) {
    *(buf) |= (*buf & rmask[shift]) | (UCHAR)(code << shift);
    *(++buf) = (UCHAR)(code >> (8 - shift));
    if (bits + shift > 16)
        *(++buf) = (UCHAR)(code >> (16 - shift));
  }
  else {
    /* Special case for fast execution */
    *(buf) = (UCHAR)code;
    *(++buf) = (UCHAR)(code >> 8);
  }
  if ((offset += bits) == (bits << 3)) {
    bytes_out += bits;
    fwrite(outbuf,1,bits,stdout);
    offset = 0;
  }
  return;
}

int nextcode(codeptr)
CODE *codeptr;
/* Get the next code from input and put it in *codeptr.
 * Return (TRUE) on success, or return (FALSE) on end-of-file.
 * Adapted from COMPRESS V4.0.
 */
{
  static int prevbits = 0;
  register CODE code;
  static int size;
  static UCHAR inbuf[MAXBITS];
  register int shift;
  UCHAR *bp;

  /* If the next entry is a different bit-size than the preceeding one
   * then we must adjust the size and scrap the old buffer.
   */
  if (prevbits != bits) {
    prevbits = bits;
    size = 0;
  }
  /* If we can't read another code from the buffer, then refill it.
   */
  if (size - (shift = offset) < bits) {
    /* Read more input and convert size from # of bytes to # of bits */
    if ((size = fread(inbuf, 1, bits, stdin) << 3) <= 0 || ferror(stdin))
      return(NO);
    offset = shift = 0;
  }
  /* Get to the first byte. */
  bp = inbuf + (shift >> 3);
  /* Get first part (low order bits) */
  code = (*bp++ >> (shift &= 7));
  /* high order bits. */
  code |= *bp++ << (shift = 8 - shift);
  if ((shift += 8) < bits) code |= *bp << shift;
  *codeptr = code & highcode;
  offset += bits;
  return (TRUE);
}

void decompress()
{
  register int i;
  register CODE code;
  char sufxchar;
  CODE savecode;
  FLAG fulltable, cleartable;
  static char token[MAXTOKLEN];         /* String buffer to build token */

  exit_stat = OK;

  if (alloc_tables(maxcode = ~(~(CODE)0 << maxbits),0)) /* exit_stat already set */
     return;

    /* if not zcat or filter */
    if(is_list && !zcat_flg) {  /* Open output file */
        if (freopen(ofname, WRITE_FILE_TYPE, stdout) == NULL) {
            exit_stat = NOTOPENED;
            return;
        }
        if (!quiet)
            fprintf(stderr, "%s: ",ifname);
        setvbuf(stdout,xbuf,_IOFBF,XBUFSIZE);
    }
  cleartable = TRUE;
  savecode = CLEAR;
  offset = 0;
  do {
    if ((code = savecode) == CLEAR && cleartable) {
      highcode = ~(~(CODE)0 << (bits = INITBITS));
      fulltable = FALSE;
      nextfree = (cleartable = block_compress) == FALSE ? 256 : FIRSTFREE;
      if (!nextcode(&prefxcode))
        break;
      putc((sufxchar = (char)prefxcode), stdout);
      continue;
    }
    i = 0;
    if (code >= nextfree && !fulltable) {
      if (code != nextfree){
        exit_stat = CODEBAD;
        return ;     /* Non-existant code */
    }
      /* Special case for sequence KwKwK (see text of article)         */
      code = prefxcode;
      token[i++] = sufxchar;
    }
    /* Build the token string in reverse order by chasing down through
     * successive prefix tokens of the current token.  Then output it.
     */
    while (code >= 256) {
#     if !defined(NDEBUG)
        /* These are checks to ease paranoia. Prefix codes must decrease
         * monotonically, otherwise we must have corrupt tables.  We can
         * also check that we haven't overrun the token buffer.
         */
        if (code <= prefix(code)){
            exit_stat= TABLEBAD;
            return;
        }
        if (i >= MAXTOKLEN){
            exit_stat = TOKTOOBIG;
            return;
        }
#     endif
      token[i++] = suffix(code);
      code = prefix(code);
    }
    putc(sufxchar = (char)code, stdout);
    while (--i >= 0)
        putc(token[i], stdout);
    if (ferror(stdout)) {
        exit_stat = WRITEERR;
        return;
    }
    /* If table isn't full, add new token code to the table with
     * codeprefix and codesuffix, and remember current code.
     */
    if (!fulltable) {
      code = nextfree;
      assert(256 <= code && code <= maxcode);
      prefix(code) = prefxcode;
      suffix(code) = sufxchar;
      prefxcode = savecode;
      if (code++ == highcode) {
        if (highcode >= maxcode) {
          fulltable = TRUE;
          --code;
        }
        else {
          ++bits;
          highcode += code;           /* nextfree == highcode + 1 */
        }
      }
      nextfree = code;
    }
  } while (nextcode(&savecode));
  exit_stat = (ferror(stdin))? READERR : OK;
  return ;
}

