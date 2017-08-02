// Copyright (C) 1992 Scott Kirkwood (Queen's University, Ontario, Canada)
// Voice phone : (613) 531-2674, internet: kirkwood@qucis.queensu.ca
// This program is given as freeware with no warantees.  Not liable for
// any loss or damage resulting from this code.  Please acknowledge authorship.
// Note: Tabs in are size 4

// IFormat.C : routines for the IFormat class.
// Please read IFormat.h for more information.

#include "iformat.h"
#include <iostream.h>
#include <string.h>
#include <math.h>

// Note: may get wrong results for negative number equal to LONGMIN
// I also haven't been able to test REALLY_BIG
// Notes to myself...
// 18,446,744,073,709,551,616  (64 bits) 20+6(commas)+3 = 29
//              2,147,483,647  (32 bits) 10+3+3(-,K,\0) = 16

#ifndef REALLY_BIG
    char IFormat::ext[IFormatMAXSI] = {0, 'K', 'M', 'G'};
    unsigned long IFormat::divby[IFormatMAXSI] = {1, 1000, 1000000,
                                          1000000000};
#else
    const MAXSI = 5;
    char IFormat::ext[IFormatMAXSI] = {0, 'K', 'M', 'G','T'};
    unsigned long IFormat::divby[IFormatMAXSI] = {1, 1000, 1000000,
              1000000000, 1000000000000};
#endif

// Function prototypes.
char* uconv10(unsigned long i, char* bufend, int &len);
char* conv8(register unsigned long i, register char* p, register int &len);
char* conv16(register unsigned long i, register char* p, register int &len);

// ============================================================
// Author      : Scott Kirkwood
// Date        : Mon Nov  2 1992
//
// Prototype   : iformat.h
// Description :
// Constructor, sets the variables to the defaults.

IFormat::IFormat()
{
    putseps = 1; // Should we use a separator
    width   = 0; // Width to output from 0, 2-  0=infinity
    use_si  = 1;
    right_adjust = 1;
    sep     = ',';
    sep_width = 3;
    mode    = ios::dec;
}

// ============================================================
// Author      : Scott Kirkwood
// Date        : Mon Nov  2 1992
//
// Prototype   : iformat.h
// Description :

const char *IFormat::Str(long num) {
    int neg = 0;

    if (num < 0) {
        neg = 1;
        num = -num;
    }
    return Str((unsigned long)num, neg);
}

// ============================================================
// Author      : Scott Kirkwood
// Date        : Tue Nov 10 1992
//
// Prototype   : iformat.h
// Description :
// Main routine, handles output of unsigned longs.
// Note: setting width to anything but 0 causes a big speed penalty.
//
const char *IFormat::Str(unsigned long num, int neg) {
    int as = 0;
    int old_putseps = putseps;
    // Commas will move the string to the right need 2 bytes for
    // the potential SI postfix and the null.
    char *last = str + sizeof(str) - IFormatMaxCommas - 2;
    char *first;
    int len = 0;

    if (width) {
        if (use_si) {
            int tempwidth = width;
            register unsigned long tempnum = num;
            while (as < IFormatMAXSI - 1 && Size(tempnum, neg) > tempwidth) {
                if (as == 0)
                    tempwidth--; // because of the postfix character
                as++;
                tempnum = num / divby[as];
            }
            num = tempnum;
        }
        else {
            if (putseps && Size(num, neg) > width) {
                putseps = 0;
                if (Size(num, neg) > width)
                    putseps = old_putseps;
            }
        }
    }
    // The converter routines grows the string backwards from the least sig.
    // digit to the m.s. digit.  No null is placed.
    switch (mode) {
    case ios::hex:
        first = conv16(num, last, len);
        break;
    case ios::oct:
        first = conv8(num, last, len);
        break;
    default:
        first = uconv10(num, last, len);
        break;
    }
    last++;
    *last = 0;
    if (putseps)
        PutCommas(last, len);

    if (neg) {
        first--;
        *first = '-';
        len++;
    }

    if (width) {
        if (use_si && as) {
            *last++ = ext[as];
            *last   = 0;
            len++;
        }
        if (right_adjust)
            RightAdjust(first, len, width);
    }
    putseps = old_putseps;
    return first;
}

// ============================================================
// Author      : Scott Kirkwood
// Date        : Mon Nov  2 1992
//
// Prototype   : iformat.h
// Description :
// Right adjust the string to the width specified.  Fill with spaces
// on the left.

void IFormat::RightAdjust(register char *&first, int len, int width)
{
    register int spaces = width - len;
    if (spaces < 1)
        return;

    while (spaces--) {
        *--first = ' ';
    }
}

// ============================================================
// Author      : Scott Kirkwood
// Date        : Mon Nov  2 1992
//
// Prototype   : iformat.h
// Description :
// Return the expected number of characters the number will take.
// Calculates the number of separators (commas) and the minus sign,
// but does not assume any postfix characters.

int IFormat::Size(register unsigned long num, int neg)
{
#ifdef REALLY_BIG
    static long arr[] = {1,10,100,1000,10000,100000,1000000,
      10000000,100000000,1000000000,10000000000,100000000000,
      1000000000000,10000000000000,100000000000000,
      1000000000000000,10000000000000000,100000000000000000,
      1000000000000000000,10000000000000000000};
#else
    static long arr[] = {1,10,100,1000,10000,100000,1000000,
    10000000,100000000,1000000000};
#endif

    register int len = 0;
    while (len < 10 && num > arr[len]) {
        len++;
    }
    if (len == 0)
        len = 1; // 0 = size 1;

    // Ok, how many commas will be used for a string
    // of length len.
    if (len == 1)
        len = neg ? 2 : 1;
    else {
        // Count commas and possibly the minus sign
        if (putseps)
            len += ((len - 1) / sep_width) + (neg ? 1 : 0);
        else
            len += neg ? 1: 0;
    }
    return len;
}

// ============================================================
// Author      : Scott Kirkwood
// Date        : Mon Nov  2 1992
//
// Prototype   : iformat.h
// Description :
// Put commas in the proper places.  Does this in place and therefore
// assumes that you have enough room in the string for the operation.
// Goes from right to left.
// P points to the null character.

void IFormat::PutCommas(char *&last, int &length)
{
    register int len = length;
    int count = 0;

    // If length is <= sep_width then we don't need any stinking commas.
    if (len <= sep_width)
        return;

    register char *p = last;
    int diff = (len - 1) / sep_width;
    length += diff;
    last   += diff;
    
    register char *g = p + diff;

    count = sep_width;
    while (len--) {
        *g-- = *p--;
        if (count-- == 0 && *p != '-') {
            count = sep_width - 1;
            *g-- = sep;
        }
    }
}

// ============================================================
// Author      : Scott Kirkwood
// Date        : Tue Nov 10 1992
//
// Prototype   : iformat.h
// Description :
// Sets the total format width, making sure it's not too big.
void IFormat::SetWidth(int w) {
    if (w < IFormatMaxLen - 2) // null, and K not included
        width = w;
    else
        width = IFormatMaxLen - 2;
}


// ============================================================
// Author      : Scott Kirkwood
// Date        : Tue Nov 10 1992
//
// Prototype   : iformat.h
// Description :
// Sets the separation width making sure it's greater than or
// equal to 3.

void IFormat::SetSepWidth(int sw)
{
    if (sw < 3)
        sep_width = 3;
    else
        sep_width = sw;
}

// ============================================================
// Author      : Scott Kirkwood
// Date        : Tue Nov 10 1992
//
// Prototype   : iformat.h
// Description :
// Set the SI postfix character (checks bounds)

void IFormat::SetSI(int i, char c)
{
    if (i > 0 && i < IFormatMAXSI)
        IFormat::ext[i] = c;
}

// ============================================================
// Author      : Scott Kirkwood
// Date        : Tue Nov 10 1992
//
// Prototype   : iformat.h
// Description :
// Set the SI representation (checks bounds)

void IFormat::SetSI(int i, unsigned long val)
{

    if (i > 0 && i < IFormatMAXSI)
        IFormat::divby[i] = val;
}

static const char digit1[] = {
    '0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9',
    };

static const char digit2[] = {
    '0','0','0','0','0','0','0','0','0','0',
    '1','1','1','1','1','1','1','1','1','1',
    '2','2','2','2','2','2','2','2','2','2',
    '3','3','3','3','3','3','3','3','3','3',
    '4','4','4','4','4','4','4','4','4','4',
    '5','5','5','5','5','5','5','5','5','5',
    '6','6','6','6','6','6','6','6','6','6',
    '7','7','7','7','7','7','7','7','7','7',
    '8','8','8','8','8','8','8','8','8','8',
    '9','9','9','9','9','9','9','9','9','9',
    };


// ============================================================
// Description :
// This routine converts the long unsigned number to decimal.
// The variable p points to RIGHT end of a buffer. Function uconv10 returns
// pointer to left end of converted number.  The buffer is
// is not 0 terminated.

char* uconv10(unsigned long i, char* bufend, int &len)
{
    register unsigned long j = i;
    register char* p = bufend;
    register int diff;

    len = 0;
    do {
        long register by100 = j/100;
        diff = (int)(j-100*by100);
        *p-- = digit1[diff];
        *p-- = digit2[diff];
        len += 2;
        j = by100;
    } while ( j > 0 );
    if ( diff<10 ) {
        ++p; //compensate for extra 0
        len--;
    }
    return p + 1;
}


// ============================================================
// Description :
// This one converts to octal.

char* conv8(register unsigned long i, register char* p, register int &len)
{
    len = 0;
    do {
        *p-- = (char)('0' + i%8);
        len++;
    } while ( (i >>= 3) > 0 );

    return p + 1;
}


// ============================================================
// Description :
// This routine converts to hexadecimal.

char* conv16(register unsigned long i, register char* p, register int &len)
{
    len = 0;
    do {
        register int dig = (int)(i%16);

        if (dig < 10)
            *p-- = (char)('0' + i%16);
        else
            *p-- = (char)('A'-10 + dig);
        len++;
    } while ( (i >>= 4) > 0 );

    return p + 1;
}
