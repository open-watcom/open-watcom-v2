#ifndef _FORMAT_H
#define _FORMAT_H
// Copyright (C) 1992 Scott Kirkwood (Queen's University, Ontario, Canada)
// Voice phone : (613) 531-2674, internet: kirkwood@qucis.queensu.ca
// This program is given as freeware with no warantees.  Not liable for
// any loss or damage resulting from this code.  Please acknowledge authorship.
// Note: Tabs in are size 4


//Description
//===========
//
// IFormat is a C++ class that is designed to output formatted
// integers (int, long, etc.).  By formatted I mean:
//  1,234,567
//
// One of the main concerns of IFormat was to make it FAST.
// On some Unix machines using gcc and CC IFormat was between
// 50% to 200% faster than sprintf() -- and sprintf won't put in commas!
// With Borlands C++ v3 IFormat was 25% to 50% SLOWER.
// With gcc 2.2.2 on the PC IFormat about even with sprintf().
//
// OTHER FEATURES
// ==============
//
// IFormat can format to a specified total character width, if the
// number doesn't fit it will be divided down until it does and then
// the appropriate SI notation will be affixed.  For instance, if
// you gave IFormat the number 1234567 and set the width to 6 you would
// get:
//  1,234K           (note, it was truncated not rounded)
//
// By default the postfixes are K=1,000,  M=1,000,000,  G=1,000,000,000
// but you can change these values if you prefer the computer science
// K = 1,024, for instance.
//
// You can also have output in hexadecimal or octal.
//
// IFormat will, by default, right adjust number if the ouput is smaller
// than the width passed.  Where a width of 0 signifies variable width.
// For instance, here's 123 with a width of 5 (vertical bars for clarity)
//   |  123|
//
// You can toggle whether to use SI postfixes, right adjusting, and
// whether separators are used.
//
// The width must be less than or equal to 14 otherwise it will
// be ignored (unless you have BIG_NUMBERS set).
// Note: I have only tested this program with 4 byte longs.  If you
// have large longs #define BIG_NUMBERS and try it, but I'm not sure
// if it will work with larger longs (haven't tested it).

#ifndef BIG_NUMBERS
int const IFormatMAXSI = 4;
int const IFormatMaxCommas = 3;
int const IFormatMaxLen = 16; // including -, comma, K and null
#else
int const IFormatMAXSI = 5;
int const IFormatMaxCommas = 6;
int const IFormatMaxLen = 29;
#endif

class IFormat {
    char   sep;                // Usually a comma
    short  sep_width;          // Usually three
    static char ext[IFormatMAXSI];
    static unsigned long divby[IFormatMAXSI];
    char   str[IFormatMaxLen]; // string where I number is stored.
    int    putseps;            // Should we use a separator?
    int    right_adjust;       // Should we right adjust?
    int    use_si;             // Should we use SI postfix notations?
    int    width;              // Width to output from 0-(MaxLen - 2)
    int    mode;               // oct, hex or (default) dec.

    void Show();
    void SetVars();
    int  Size(register unsigned long num, int neg);
    void PutCommas(char *&end, int &len);
    void KillDot(char *str);
    void RightAdjust(register char *&last, int len, int width);
public:
    IFormat(); // Constructor
    
    const char *Str(int num)          { return Str((long)num); }
    const char *Str(long num);
    const char *Str(unsigned int num) { return Str((long)num); }
    const char *Str(unsigned long num, int neg = 0);
    const char *Str(char num)         { return Str((long)num); }
    void  SetSepWidth(int i);
    void  SetWidth(int i);
    void  SetSep(char ch)             { sep = ch; }
    void  SetUseSeparators(int i)     { putseps = i; }
    void  SetRightAdjust(int i)       { right_adjust = i; }
    void  SetUseSI(int i)             { use_si = i; }
    // use SetMode(ios::hex or ios::dec or ios::oct)
    void  SetMode(int i)              { mode = i; }
    void  SetSI(int i, char c);            // set SI character ex. i=1 ->'K'
    void  SetSI(int i, unsigned long val); // Set SI value     ex. i=1->1000
    int   SepWidth()                  { return sep_width; }
    int   Width()                     { return width; }
    char  Sep()                       { return sep; }
    int   UseSeparators()             { return putseps; }
    int   RightAdjust()               { return right_adjust; }
    int   UseSI()                     { return use_si; }
};

#endif
