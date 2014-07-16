#include "fformat.h"
#include <math.h>

// Function prototypes

int const SAMPLEWIDTH = 25;

void test_switches();
void show_width(int width);
int main()
{
    test_switches();
    
    return 0;
}

void test_switches()
{
    int const width2 = 7;
    int const width3 = 7;
    int const sepw4 = 4;
    int const width5 = 5;
    int const width6 = 5;
    int const width7 = 15;

    int const max_sig = 7;
    ld l;
    FFormat sample;
    sample.SetWidth(SAMPLEWIDTH);

    for (int j = 0; j <= 7; j++) {
        FFormat a;

//      a.SetUseExp(1);
        switch (j) {
        case 0:
            cout << "Default." << endl;
            break;
        case 1:
            cout << "Don't use the separators." << endl;
            a.SetUseSeparators(0);
            break;
        case 2:
            cout << "Set width to " << width2 <<
                " and don't right adjust." << endl;
            a.SetWidth(width2);
            a.SetRightAdjust(0);
            break;
        case 3:
            cout << "Set width to " << width3 <<
                " and don't use SI." << endl;
            a.SetWidth(width3);
            a.SetUseSI(0);
            break;
        case 4:
            cout << "Set separator width to " << sepw4 <<
                " and Sep is a space." << endl;

            a.SetSep(' ');
            a.SetSepWidth(sepw4);
            break;
        case 5:
            cout << "Normal SepWidth, but Width set to " << width5 << endl;
            a.SetWidth(width5);
            break;
        case 6:
            cout << "Width set to " << width6 << " and don't use seps." << endl;
            a.SetWidth(width6);
            a.SetUseSeparators(0);
            break;
        case 7:
            cout << "Width set to " << width7 << endl;
            a.SetWidth(width7);
            break;
        }
        l = 1;
        show_width(a.Width());
        int i;
        for (i = 0; i < max_sig; i++) {
            cout << sample.Str(l) << flush;
            cout << " -> |";
            cout << a.Str(l);
            cout << "|" << endl;
            l *= 10;
            l += (i + 2) % 10;
        }
        for (i = 1; i < max_sig; i++) {
            cout << sample.Str(l) << flush;
            cout << " -> |";
            cout << a.Str(l);
            cout << '|' << endl;
            l += i / pow(10.0, i);
        }
        l = -1;
        for (i = 0; i < max_sig; i++) {
            cout << sample.Str(l) << flush;
            cout << " -> |";
            cout << a.Str(l);
            cout << "|" << endl;
            l *= 10;
            l -= (i + 2) % 10;
        }
        for (i = 1; i < max_sig; i++) {
            cout << sample.Str(l) << flush;
            cout << " -> |";
            cout << a.Str(l);
            cout << '|' << endl;
            l -= i / pow(10.0, i);
        }
        l = 9.1;
        for (i = 2; i < max_sig; i++) {
            cout << sample.Str(l) << flush;
            cout << " -> |";
            cout << a.Str(l);
            cout << '|' << endl;
            int d = ( 2 * i + 1 ) % 10;
            l += d / pow(10.0, i);
        }
        l = .1;
        for (i = 2; i < max_sig; i++) {
            cout << sample.Str(l) << flush;
            cout << " -> |";
            cout << a.Str(l);
            cout << '|' << endl;
            l = i / pow(10.0, i);
        }

    }
}

void show_width(int width)
{
    int i;

    for (i = 0; i < SAMPLEWIDTH - 5; i++) 
        cout << ' ';
    cout << "Input";
    cout << "    |";
    if (width) {
        for (i = 0; i < width; i++)
            cout << '-';
    }
    else {
        cout << "Unlimited";
    }
    cout << '|' << endl;;
}
