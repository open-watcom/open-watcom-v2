#ifndef _FFORMAT_H
#define _FFORMAT_H

// Copyright (C) 1992 Scott Kirkwood (Queen's University, Ontario, Canada)
// Voice phone : (613) 531-2674, internet: kirkwood@qucis.queensu.ca
// This program is given as freeware with no warantees.  Not liable for
// any loss or damage resulting from this code.  Please acknowledge authorship.
// Note: Tabs in are size 4

#include <strstream.h>

#ifdef SIGNED_IMPLEMENTED
#define SIGNED signed
#else
#define SIGNED
#endif

#ifdef USE_LONG_DOUBLE
type long double ld;
#else
typedef double ld;
#endif

// Description
// ===========
//
// FFormat is a C++ class that is designed to output formatted
// floating point numbers (float, double, etc.).  By formatted I mean:
//  1,234,567.8901 2345
//
// TFormat is a template version of FFormat (actually a subclass of
// FFormat) that is designed to be flexible enough to plug in your own
// extended numeric classes.  For instance, I have successfully used
// g++'s Integer class and Fix class.  I have also given a minimal
// example of a Money class where "Money" uses a long to store its
// value and divides the value by a 100 to get dollars and cents.  It
// also show's that FFormat can handle funny leading characters like
// the dollar sign with no probs. 
//
// OTHER FEATURES
// ==============
//
// FFormat can format to a specified total character width, if the
// number doesn't fit it will be divided down until it does and then
// the appropriate SI notation will be affixed.  For instance, if
// you gave IFormat the number 1234567 and set the width to 6 you would
// get:
//  1,235K
//
// You can change these values if you prefer the computer science
// K = 1,024, for instance.
//
// IFormat will, by default, right adjust number if the ouput is smaller
// than the width passed.  Where a width of 0 signifies variable width.
// For instance, here's 123 with a width of 5 (vertical bars for clarity)
//   |  123|
//
// You can toggle whether to use SI postfixes, right adjusting, and
// whether separators are used.
//
// DEFAULTS
// ========
// putseps      = true (use the separator characters)
// width        = 0    (unlimited length of output)
// use_si       = true (that is, if width set to something)
// right_adjust = true (numbers are right adjusted)
// sep          = ','  (comma is integer separator)
// sep_width    = 3    (123,456 etc.  in groups of 3)
// deci_sep     = ' '  (space is the fractional part separator)
// deci_width   = 4    (.1234 5678 etc. in groups of 4)
// precision    = 0    (unlimited)
// allowexp     = 0    (do not use exponent notation)
//
// By default the postfixes are:
// Kilo  'K' 1E3       Milli 'm' 1E-3 
// Mega  'M' 1E6       Micro 'u' 1E-6 (or the mu character)
// Giga  'G' 1E9       Nano  'n' 1E-9 
// Tera  'T' 1E12      Pico  'p' 1E-12
// Penta 'P' 1E15      Femto 'f' 1E-15
// Exa   'E' 1E18      Atto  'a' 1E-18
//
// WIDTH
// =====
// When specifying a width to shoot for, FFormat goes into a different
// mode.  In fact most of the code deals with the problems we get when
// a width is specified.  Here's a synopsis of what happens.
// - If the string fits, great.  Right adjust if necessary.
// else if it doesn't fit then
//   - If the number is not tiny then
//     - chop off as many decimal places as needed to make it fit.
//     - If that doesn't work,
//       - Divide the number by the minimum SI value to make it fit.
//   else if the number is tiny (like 1E-10) then
//     - chop off as many decimal places as needed to fit (but leave 1)
//     - if that doesn't work,
//       - Divide the number by the largest SI value (ex. micro) to make
//         it fit. 
// 
// DYNAMIC ALLOCATION
// ==================
// The string pointer "work" is dynamically allocated (with new and
// delete).  You can safely delete work if you need the space, just
// make sure that work points to NULL before you call Str() again.  
// If you haven't deleted "work" then the space will be either a)
// re-used if the new number will fit, or b) deleted and a new (larger)
// block of memory allocated.  It is better, therefore, to print your
// large numbers first and work down to your smaller numbers later to
// prevent heap fragmentation.  Alternatively, use Str(1E100), for instace,
// to get FFormat to make a lot of space available. 
//
// TO DO
// =====
// I guess some people would like even more formatting features. Like:
//   - Aligning numbers at the decimal point.
//   - Allowing a space between the SI postfix and the numbers.
//   - Allowing limits with the postfix notation.  i.e. allow the use of
//     K for 1,000 but not the use of M or higher.
// - It could be smarter about using exponents.
// - Could eliminate one or both separators in order to squeeze the
//   number in the width asked for.
// - Make it faster.
// - I wanted to use "locale.h" to figure out the standards for the
//   system, but I found the functions didn't work very well.  I think
//   that someone with more experience with these problems should make
//   a "locale" aware subclass of FFormat. (and send it to me!)

class FFormat {
    protected:
    int    putseps;      // Should we use a separator
    int    width;        // Total output width.
    int    use_si;       // Should we use SI postfix notations?
    int    right_adjust; // Should we right adjust
    char   sep;          // Usually a comma
    short  sep_width;    // Usually three
    short  deci_width;   // Usually four
    char   deci_sep;     // Usually a space
    int    precision;    // Number of digits after the decimal point
    char   *work;        // Output string, dynamically allocated.
    int    worklen;      // Size of allocated string
    int    allowexp;     // allow exponent notation.
    int    nodeci;       // Don't show past decimal point

    void Show();
    void SetVars();
    void getinfo(char *p, int &intlen, int &decilen, int &otherlen);
    void getinfo(char *p, char *&end, int &intlen,
        int &decilen, int &otherlen, char *&afterdot);
    int  estimate_length(int intlen, int decilen, int otherlen);
    void PutCommas(char *end, int len, char *work);
    void RightAdjust(char *, int);
protected:
    static char ext[];
    static char txe[];
    static ld exp[];
    static ld pxe[];
    char *itoa(ld);      // Make your own itoa...
public:
    FFormat();           // Constructor

    const char *Str(SIGNED int num)   { return Str((ld)num); }
    const char *Str(SIGNED long num)  { return Str((ld)num); }
    const char *Str(unsigned int num) { return Str((ld)num); }
    const char *Str(unsigned long num){ return Str((ld)num); }
    const char *Str(float num)        { return Str((ld)num); }
    const char *Str(char num)         { return Str((ld)num); }
#ifdef USE_LONG_DOUBLE
    const char *Str(double num)       { return Str((ld)num); }
#endif
    const char *Str(ld num);
    void  SetSepWidth(int i)          { sep_width = i; }
    void  SetSep(char ch)             { sep = ch; }
    void  SetDeciWidth(int i)         { deci_width = i; }
    void  SetDeciSep(char ch)         { deci_sep = ch; }
    void  SetWidth(int i)             { width = i; }
    void  SetUseSeparators(int i)     { putseps = i; }
    void  SetRightAdjust(int i)       { right_adjust = i; }
    void  SetUseSI(int i)             { use_si = i; }
    void  SetUseExp(int i)            { allowexp = i; }
    void  SetPrecision(int i)         { precision = i; }
    void  SetSI(int i, char c);       // set big SI character (i=1->'K')
    void  SetSI(int i, ld val);       // Set big SI value     (i=1->1E3)
    void  SetIS(int i, char c);       // set small SI character (i=1->'m')
    void  SetIS(int i, ld val);       // Set small SI value   (i=1->1E-3)

    int   SepWidth()                  { return sep_width; }
    char  Sep()                       { return sep; }
    int   DeciWidth()                 { return deci_width; }
    char  DeciSep()                   { return deci_sep; }
    int   Width()                     { return width; }
    int   UseSeparators()             { return putseps; }
    int   RightAdjust()               { return right_adjust; }
    int   Precision()                 { return precision; }
    int   UseSI()                     { return use_si; }
    int   UseExp()                    { return allowexp; }
};

#ifdef HAVE_TEMPLATES
// OK If we have templates, then let's make a template class.
// This template class allows you to ADD extra ability to Format.
// For instance, I have successfully used TFormat<Integer> from the
// GNU library of variable length integers.  The class you are adding
// should have (the equivalent of):
//
//     friend ostream& operator <<(ostream &, const YourClass &)
//     friend YourClass operator /(const YourClass, const YourClass)
//     friend void mod(const YourClass, const YourClass, YourClass &Result)
//     friend int  abs(const YourClass)
//     friend void operator +=(YourClass, const int)
//     operator int();
//
// defined and declared.  (Of course you know that it doesn't have to
// be declared exactly as above)
// My experience is that the entire template definition/declaration
// must be included in the header file to work correctly. So here it is...

template <class T>
class TFormat : public FFormat {
    protected:
    char *itoa(T);
public:
    const char *Str(T);
};

// This is a copy of the code contained in FFormat (pretty much).
template <class T>
const char *TFormat<T>::Str(T num) {
    int as = 0;
    int sa = 0;
    int intlen; // Length of integer part
    int decilen; // Length of decimal part
    int otherlen; // everthing else (except \0)
    int total_len; // Length with commas dots and \0
    char *str;
    T tnum = num;
    int tprecision   = precision;
    int tnodeci      = nodeci;

    do {
        str = itoa(num);
        getinfo(str, intlen, decilen, otherlen);
        total_len = estimate_length(intlen, decilen, otherlen);
        if (as || sa)
            total_len++;
        if (width && total_len - 1 > width) {
            if (abs(num) >= 1) { // big number
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
                else if (use_si && as < 6) {
                    delete str;
                    num = tnum / (T)(exp[++as]);
                }
                else
                    break;
            }
            else { // number is small
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
                else if (use_si && sa < 6) {
                    delete str;
                    num = tnum / (T)(pxe[++sa]);
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

        work[len]   = as?ext[as]:txe[sa];
        work[len + 1] = '\0';
    }

    if (width && right_adjust) {
        RightAdjust(work, width);
    }
    nodeci = tnodeci;
    precision = tprecision;
//  assert(strlen(work) < worklen);

    return work;
}

template <class T>
char *TFormat<T>::itoa(T num) {
    ostrstream mys; // Dynamically allocated string.
    if (!allowexp) {
        mys.setf(ios::fixed);
    }
    else {
        mys.setf(ios::scientific);
    }
    mys.precision(precision);
#ifdef TFORMAT_ROUND
    if (nodeci) { // compensate for lack of rounding
        T result;
        mod(num, num, result);
        if (10 * result >= 5)
            num += 1;
    }
#endif
    mys << num;
    mys << ends;
    return mys.str();
}

#endif
#endif
