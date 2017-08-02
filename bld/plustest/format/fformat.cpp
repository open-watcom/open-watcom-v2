// Copyright (C) 1992 Scott Kirkwood (Queen's University, Ontario, Canada)
// Voice phone : (613) 531-2674, internet: kirkwood@qucis.queensu.ca
// This program is given as freeware with no warantees.  Not liable for
// any loss or damage resulting from this code.  Please acknowledge authorship.
// Note: Tabs in are size 4

#include "fformat.h"
#include "assert.h"
#include <iostream.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
//#include <values.h>
#include <stdlib.h>

#ifndef USE_LONG_DOUBLE
#define fabsl fabs
#define modfl modf
#endif

// Define statics.
// Mega, Giga, Tera, Penta?, Exa
char FFormat::ext[] = {' ','K', 'M', 'G',  'T',  'P',  'E'};
ld FFormat::exp[]    = {1, 1E3, 1E6, 1E9, 1E12, 1E15, 1E18};
// Milli, Micro, Nano, Pico, Femto, Atto
#ifdef NICE_MU
#   ifdef __MSDOS__
        char FFormat::txe[] = {' ','m', 230, 'n',  'p',  'f',  'a'};
#   else
        char FFormat::txe[] = {' ','m', 181, 'n',  'p',  'f',  'a'};
#   endif
#else
    char FFormat::txe[] = {' ','m', 'u', 'n',  'p',  'f',  'a'};
#endif
ld FFormat::pxe[]    = {1,1E-3,1E-6,1E-9,1E-12,1E-15,1E-18};

// ============================================================
// Author      : Scott Kirkwood
// Date        : Mon Nov  2 1992
//
// Prototype   : FFormat.h
// Description :
// Constructor, sets the variables to the defaults.

FFormat::FFormat()
{
    putseps      = 1; // Should we use a separator
    width        = 0; // Width to output from 0, 2-  0=infinity
    use_si       = 1;
    right_adjust = 1;
    sep          = ',';
    sep_width    = 3;
    deci_sep     = ' ';
    deci_width   = 4;
    precision    = 9;
    work         = 0;
    worklen      = 0;
    allowexp     = 0;
    nodeci       = 0;
}

// ============================================================
// Author      : Scott Kirkwood
// Date        : Mon Nov  2 1992
//
// Prototype   : format.h
// Description :
// Main routine, handles output of doubles.
// Note: setting width to anything but 0 causes a big speed penalty.
const char *FFormat::Str(ld num) {
    int as = 0;
    int sa = 0;
    int intlen; // Length of integer part
    int decilen; // Length of decimal part
    int otherlen; // everthing else (except \0)
    int total_len; // Length with commas dots and \0
    char *str;
    ld tnum = num;
    int tprecision   = precision;
    int tnodeci      = nodeci;

    do {
        str = itoa(num);
        getinfo(str, intlen, decilen, otherlen);
        total_len = estimate_length(intlen, decilen, otherlen);
        if (as || sa)
            total_len++;
        if (width && total_len - 1 > width) {
            if (fabsl(num) >= 1) { // big number
                if (!nodeci) {
                    delete str;
                    int xtralen = decilen;
                    if (deci_width)
                        xtralen += (xtralen - 1) / deci_width;

                    if (width >= total_len - xtralen) {
                        precision = width - intlen - otherlen;
                        if (sep_width)
                            precision -= (intlen - 1) / sep_width;
                        if (deci_width)
                            precision -= (precision - 1) / deci_width;
                        if (precision < 0) {
                            precision = 0;
                            nodeci = 1;
                        }
                    }
                    else {
                        precision = 0;
                        nodeci = 1;
                    }
                }
                else if (use_si && !allowexp && as < 6) {
                    delete str;
                    num = tnum / exp[++as];
                }
                else
                    break;
            }
            else { // number is small
                // because of the way "precision" works we need to know
                // number of zeros between the point and the 1st num
                int leadingzeroes = 0;
                char *p = str;
                while (*p && *p != '.')
                    p++;
                p++;
                while (*p && *p++ == '0')
                    leadingzeroes++;

                if (deci_width)
                    decilen += (decilen - 1) / deci_width;
                
                if (width - leadingzeroes > total_len - decilen) {
                    precision = decilen - leadingzeroes -
                        (total_len - width);
                    delete str;
                }
                else if (use_si && !allowexp && sa < 6) {
                    delete str;
                    num = tnum / pxe[++sa];
                }
                else
                    break;
            }
        }
    } while (width && total_len - 1 > width);
    if (width >= total_len - 1) {
        total_len = width + 1;
    }
    if (!work || total_len > worklen) {
        if (work)
            delete work;
        work = new char [total_len];
        worklen = total_len;
    }
    PutCommas(str, intlen, work);
    if (as || sa) {
        int len = strlen(work);

        work[len]     = as?ext[as]:txe[sa];
        work[len + 1] = '\0';
    }

    if (width && right_adjust) {
        RightAdjust(work, width);
    }
    nodeci = tnodeci;
    precision = tprecision;
    assert(strlen(work) < worklen);

    return work;
}

// ============================================================
// Author      : Scott Kirkwood
// Date        : Tue Dec  8 1992
// 
// Prototype   : fformat.h
// Description : 
// I'm using ostrstream to handle the itoa() function.  You 
// could use you own (faster) routine if you'd like.

char *FFormat::itoa(ld num) {
    ostrstream mys; // Dynamically allocated string.
    char *str;

    mys.rdbuf()->setbuf(NULL,16);
    if (!allowexp) {
        mys.setf(ios::fixed);
    }
    else {
        mys.setf(ios::scientific);
    }
    mys.precision(precision);
    if (nodeci) { // compensate for lack of rounding
        if (modfl(num, &num) >= .5)
            num += 1;
    }
    mys.setf(0, ios::showpoint); // no trailing zeroes or unnecessary points
    mys << num;
    mys << ends;

    str = mys.str();
    if (!allowexp && str && strchr(str, '.')) {
        // Fix bug in iostream!
        char *p = str + strlen(str) - 1;
        while (*p == '0') {
            *p-- = '\0'; // kill off trailing 000.
        }
        if (*p == '.')
            *p = '\0';
    }
    return str;
}


// ============================================================
// Author      : Scott Kirkwood
// Date        : Tue Dec  8 1992
// 
// Prototype   : fformat.h
// Description : 
// Gets information about the string.  
// PS. Also zaps any trailing 00000 after the dot.
// Examples
// Num     -123456.789
//         ^
//         p      intlen=6,decilen=3,prelen=1,otherlen=2
//
// Num     123456
//         ^
//         p      intlen=6,decilen=0,prelen=0,otherlen=0;
//
// Num     123456.789E23
//         ^
//         p      intlen=6,decilen=3,prelen=0,otherlen=4
void FFormat::getinfo(char *p, int &intlen, int &decilen,
    int &otherlen)
{

    intlen = decilen = otherlen = 0;
    // Skip over - or + or spaces?

    while (*p && !isdigit(*p)) {
        p++;
        otherlen++;
    }

    while (*p && isdigit(*p)) {
        intlen++;
        p++;
    }
    if (*p == '.' && *(p + 1)) {
        otherlen++; // Count dot
        p++;
        while (*p && isdigit(*p)) {
            decilen++;
            p++;
        }
    }
    while (*p) {
        p++;
        otherlen++;
    }
}

// ============================================================
// Author      : Scott Kirkwood
// Date        : Mon Nov  2 1992
//
// Prototype   : FFormat.h
// Description :
// Right adjust the string to the width specified.  Fill with spaces
// on the left.
void FFormat::RightAdjust(char *str, int width)
{
    int len = strlen(str);
    if (len >= width)
        return;

    register char *p = str + len;
    register char *g = p + (width - len);
    int spaces = width - len;
    len++;
    while (len--) {
        *g-- = *p--;
    }
    while (spaces--) {
        *g-- = ' ';
    }
}

// ============================================================
// Author      : Scott Kirkwood
// Date        : Tue Dec  8 1992
// 
// Prototype   : fformat.h
// Description : 
// Make an exact estimate of the total length required to fit the
// string including the trailing '\0'.

int FFormat::estimate_length(int intlen, int decilen, int otherlen) {
    int len;

    len = intlen + otherlen + 1;
    if (sep_width && putseps)
        len += (intlen - 1) / sep_width;

    if (!nodeci) {
        len += decilen;
        if (deci_width && putseps)
            len += (decilen - 1) / deci_width;
    }
    else if (decilen)
        len--; // chop off '.'

    return len;
}

// ============================================================
// Author      : Scott Kirkwood
// Date        : Mon Nov  2 1992
//
// Prototype   : FFormat.h
// Description :
// Put commas etc. in the proper places.
// Work points to some location inside a string that is large
// enough to hold the final output.
// Example
//    123456.890123E123    ????????????????????
//         ^ ^             ^
//       end afterdot     work
//   After call
//                         123,456.8901 23E123\0
// Update: Made PutCommas() delete trailing zeroes that some libraries
// put in (even when you specify unsetf(showpoint)).

void FFormat::PutCommas(char *from, int intlen, char *to)
{
    int count = 0;

    if (!from || !*from || !to)
        return; // abort, abort!

    while (*from && !isdigit(*from))
        *to++ = *from++;

    if (putseps)
        count = sep_width - (intlen % sep_width) + 1;
    else
        count = sep_width + 1;

    if (count > sep_width)
        count = 1;
    while (*from && intlen--) {
        *to++ = *from++;
        if (sep_width && count == sep_width && intlen && putseps) {
            count = 0;
            *to++ = sep;
        }
        count++;
    }
    if (*from == '.') {
        if (nodeci) {
            from++;
            while (*from && isdigit(*from))
                from++;
        }
        else {
            *to++ = *from++;
            count = 1;

            while (*from && isdigit(*from)) {
                *to++ = *from++;
                if (deci_width && putseps && 
                    count == deci_width && *from &&
                    !strchr("EeGg", *from)) {
                    count = 0;
                    *to++ = deci_sep;
                }
                count++;
            }
        }
    }
    while (*from) {
        *to++ = *from++;
    }
    *to = '\0';
}


// ============================================================
// Author      : Scott Kirkwood
// Date        : Tue Dec  8 1992
// 
// Prototype   : fformat.h
// Description : 
// Set the exponent character to c.  Ignored if i is out of range
// Warning: These values are static and will affect all output by this
// class! 
void FFormat::SetSI(int i, char c) {
    if (i > 0 && i < 7) 
        ext[i] = c;
}

// ============================================================
// Author      : Scott Kirkwood
// Date        : Tue Dec  8 1992
// 
// Prototype   : fformat.h
// Description : 
// Set the exponent value to val.  Ignored if i is out of range.
// Warning: These values are static and will affect all output by this
// class! 
void FFormat::SetSI(int i, ld val) {
    if (i > 0 && i < 7)
        exp[i] = val;
}

// ============================================================
// Author      : Scott Kirkwood
// Date        : Tue Dec  8 1992
// 
// Prototype   : fformat.h
// Description : 
// Set the exponent character to c.  Ignored if i is out of range.
// Warning: These values are static and will affect all output by this
// class! 
void FFormat::SetIS(int i, char c) {
    if (i > 0 && i < 7)
        txe[i] = c;
}

// ============================================================
// Author      : Scott Kirkwood
// Date        : Tue Dec  8 1992
// 
// Prototype   : fformat.h
// Description : 
// Set the exponent value to val.  Ignored if i is out of range. 
// Warning: These values are static and will affect all output by this
// class! 

void FFormat::SetIS(int i, ld val) {
    if (i > 0 && i < 7)
        pxe[i] = val;
}

#ifdef SAMPLE_FFORMAT
// The following is a rather an example of some of the ways you
// can use this class.  
// Inside main() is the simplest way of using FFormat.
// Inside try_template() is an example using:
//    Integer class from GNU's C++ library.
//    Dollar  class defined below.
#include <iostream.h>

void try_template();

int main() {
    FFormat FForm;
    double dbl = 123456.7890;

    cout << "Standard format     : " << FForm.Str(dbl) << endl;
    try_template();
    return 0;
}

#ifdef HAVE_TEMPLATES
#ifdef __GNUC__
#include <Integer.h>
void try_template() {
    const num_width = 33;
    Integer test;
    TFormat<Integer> IForm; // Hey man, were using the Integer class!
    
    test = 1;
    for (int i = 0; i < num_width; i++) {
        test *= 10;
        test += (i + 2) % 10;
    }
    cout << "Using Template class<Integer>: " << IForm.Str(test) << endl;
}
#else
class Dollar { // MINIMAL money class, use as an example only.
    long money;// 12345 => 123.45
public: 
    Dollar() { money = 0L; }
    Dollar(long v) { money = v; }
    operator long() const { return money; }
    friend ostream& operator <<(ostream &o, Dollar &d);
    friend Dollar operator /(const Dollar &a, const Dollar &b);
    friend void mod(const Dollar &a, const Dollar &b, Dollar &c);
    friend long  abs(const Dollar &a);
    friend void operator +=(Dollar &a, const int i);
};
Dollar operator /(const Dollar &a, const Dollar &b) {
    Dollar tmp(a); tmp.money /= b.money; return tmp;
}
void mod(const Dollar &a, const Dollar &b, Dollar &c) {
    c.money = a.money % b.money;
}
long  abs(const Dollar &a) {
    return (a.money > 0)?a.money:-a.money;
}
void operator +=(Dollar &a, const int i) {
    a.money += i; 
}
ostream& operator <<(ostream &o, Dollar &d) {
    return o << '$' << d.money / 100 << '.' << d.money % 100;
}

void try_template() {
    TFormat<Dollar> DForm;
    Dollar money(12345678L);

    cout << "Using Template class: " << DForm.Str(money) << endl;
}
#endif
#else
void try_template() { } // Nothing....
#endif /* HAVE_TEMPLATE */
#endif
