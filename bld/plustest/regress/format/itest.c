// Copyright (C) 1992 Scott Kirkwood (Queen's University, Ontario, Canada)
// Voice phone : (613) 531-2674, internet: kirkwood@qucis.queensu.ca
// This program is given as freeware with no warantees.  Not liable for
// any loss or damage resulting from this code.  Please acknowledge authorship.
// Note: Tabs in are size 4

// This program is a test suite for the IFormat C++ class.

#include <iostream.h>
#include "iformat.h"
#include <time.h>
#include <stdio.h>
#include <limits.h>

// Function prototypes

void test_switches(int mode);
void show_width(int width);
void speed_test();
void type_test();
void other_test();

int main(int argc, char **argv)
{
    if (argc == 2) {
        speed_test();
    }
    else {
        test_switches(ios::dec);
              test_switches(ios::hex);
              test_switches(ios::oct);

        other_test();
        type_test();
    }

    return 0;
}

inline long CLOCK() { return clock(); }

void speed_test() {
    const long factor = 5;
    const char *mes = " clock ticks.";
    unsigned long iter;
    unsigned long time_start;
    register unsigned long int i;
    char work[81];

    cout << "Performing speed check please wait..." << endl;
    // Let's do a quick and dirty speed check
    long speed = 0;

    time_start = time(0);
    while (time(0) == time_start) // sychronize on clock edge
        ;
    time_start = time(0) + 1;
    while (time(0) < time_start)
        speed++;

    if (factor < 0)
        iter = speed / (-factor);
    else
        iter = speed * factor;


    IFormat a, b;
    a.SetWidth(2);

    cout << "Performing tests (loops of " << b.Str(iter) << 
        " iterations) ..." << endl;
    time_start = CLOCK();

    for (i = 0; i < iter; i++) {
        a.Str(LONG_MAX);
    }

    long time_worst = CLOCK() - time_start;
    cout << "IFormat (worst case)      : " << b.Str(time_worst) <<
        mes << endl;
    cout << "Sample:" << a.Str(LONG_MAX) << endl;
    a.SetWidth(0);

    time_start = CLOCK();

    for (i = 0; i < iter; i++) {
        a.Str(123456);
    }

    long time_avg = CLOCK() - time_start;
    cout << "IFormat (avg case)        : " << b.Str(time_avg) <<
        mes << endl;
    cout << "Sample:" << a.Str(123456) << endl;

    a.SetUseSeparators(0);
    a.SetRightAdjust(0);
    a.SetUseSI(0);

    time_start = CLOCK();

    for (i = 0; i < iter; i++) {
        a.Str(123456);
    }
    long time_best = CLOCK() - time_start;
    cout << "IFormat (best case)       : " << b.Str(time_best) <<
        mes << endl;
    cout << "Sample:" << a.Str(123456) << endl;

    time_start = CLOCK();

    for (i = 0; i < iter; i++) {
        sprintf(work,"%li", 123456L);
    }

    long time_sprintf = CLOCK() - time_start;
    cout << "Sprintf (fast)            : " << b.Str(time_sprintf) <<
        mes << endl;
    cout << "Sample:" << work << endl;

    time_start = CLOCK();

    for (i = 0; i < iter; i++) {
        sprintf(work, "%7li", 123456L);
    }
    long time_sprintf_slow = CLOCK() - time_start;
    cout << "Sprintf (width specified) : " << b.Str(time_sprintf_slow) <<
        mes << endl;
    cout << "Sample:" << work << endl;


    cout << "IFormat is between ";
    if (time_worst)
        cout << (100 * time_sprintf / time_worst) - 100;
    else
        cout << "more than " << (time_sprintf * 100) - 100;
    cout << "% and ";
    if (time_best)
        cout << (100 * time_sprintf_slow / time_best) - 100;
    else
        cout << "more than " << (time_sprintf_slow * 100) - 100;

    cout << "% faster than sprintf." << endl;
    cout << "Negative numbers mean it's slower." << endl;
}

void test_switches(int mode)
{
    long l;

    for (int j = 0; j <= 6; j++) {
        IFormat a;

        a.SetMode(mode);
        switch (j) {
        case 0:
            cout << "Default." << endl;
            break;
        case 1:
            cout << "Don't use the separators." << endl;
            a.SetUseSeparators(0);
            break;
        case 2: {
            int const width2 = 7;
            cout << "Set width to " << width2 <<
                " and don't right adjust." << endl;
            a.SetWidth(width2);
            a.SetRightAdjust(0);
        }       break;
        case 3: {
            int const width3 = 7;
            cout << "Set width to " << width3 <<
                " and don't use SI." << endl;
            a.SetWidth(width3);
            a.SetUseSI(0);
        }       break;
        case 4: {
            int const sepw4 = 4;
            cout << "Set separator width to " << sepw4 <<
                " and Sep is a space." << endl;

            a.SetSep(' ');
            a.SetSepWidth(sepw4);
        }       break;
        case 5: {
            int const width5 = 5;
            cout << "Normal SepWidth, but Width set to " << width5 << endl;
            a.SetWidth(width5);
        }       break;
        case 6: {
            int const width6 = 5;
            cout << "Width set to " << width6 << " and don't use seps." << endl;
            a.SetWidth(width6);
            a.SetUseSeparators(0);
        }       break;
        }
        l = 1;
        show_width(a.Width());
        int i;
        for (i = 0; i < 10; i++) {
            cout << "|" << a.Str(l) << "|" << endl;
            switch (mode) {
            default: // ios::dec
                l *= 10;
                l += (i + 2) % 10;
                break;
            case ios::hex:
                l <<= 4;
                l += (i + 2) % 16;
                break;
            case ios::oct:
                l <<= 3;
                l += (i + 2) % 8;
                break;
            }
        }
        l = -1;
        cout << "|" << a.Str(LONG_MAX) << "| (LONG_MAX)" << endl;
        cout << "|" << a.Str(0) << "| (ZERO)" << endl;

        for (i = 0; i < 10; i++) {
            cout << "|" <<  a.Str(l) << "|" << endl;
            switch (mode) {
            default: // ios::dec
                l *= 10;
                l -= (i + 2) % 10;
                break;
            case ios::hex:
                l <<= 4;
                l -= (i + 2) % 16;
                break;
            case ios::oct:
                l <<= 3;
                l -= (i + 2) % 8;
                break;
            }
        }
        cout << "|" << a.Str(LONG_MIN) << "| (LONG_MIN)" << endl;
    }
}

void show_width(int width)
{
    int i;

    cout << '|';
    if (width) {
        for (i = 0; i < width; i++)
            cout << '-';
    }
    else {
        cout << "Unlimited";
    }
    cout << '|' << endl;;
}

void type_test() {
    IFormat a;
    cout << "Testing type long." << endl;
    cout << a.Str(12345L) << endl;
    cout << "Testing type unsigned long."  << endl;
    cout << a.Str(12345UL) << endl;
    cout << "Testing type int." << endl;
    cout << a.Str(12345) << endl;
    cout << "Testing type unsigned int." << endl;
    cout << a.Str(12345U) << endl;
    cout << "Testing type char." << endl;
    cout << a.Str('A') << endl;
}

void other_test() {
    IFormat a;
    cout << "Different widths" << endl;
    cout << "================" << endl;
    for (int i = 0; i < IFormatMaxLen - 1; i++) {
        a.SetWidth(i);
        cout << '|' << a.Str(LONG_MIN) << "| width = " << i << endl;
    }
    a.SetWidth(999);
    cout << '|' << a.Str(LONG_MIN) << "| max width" << endl;
}
