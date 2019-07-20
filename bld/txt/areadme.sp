
Open Watcom C/C++ Version 2.0 for Alpha AXP - README
----------------------------------------------------

Welcome to the Open Watcom C/C++ Alpha Release.  This README provides
helpful hints found during testing as well as updated information that was
not available at the time the documentation was produced.

1. The Alpha hosted tools are slow because of alignment issues. This issue
will be worked on for a later release.

2. No AXP-specific tuning has been done on the optimizer. However, most
optimizations are not CPU specific and will be effective on AXP platform.


Where possible, the Alpha tools have been made command-line compatible with
the Intel versions of those tools.  Differences are outlined below.

1. Intel-specific compiler switches have been removed, and a few
Alpha-specific switches added.  For details, see the usage screen for the
Alpha compilers (wccaxp and wppaxp).

2. The linker system directives for the Alpha are as follows:
    sys ntaxp       - alpha console mode app
    sys ntaxp_win   - alpha windowed app
    sys ntaxp_dll   - alpha dll
    
3. The Alpha librarian creates Microsoft Visual C++ compatible AR-format
libraries.

4. The Alpha assembler (wasaxp) accepts a dialect of AXP assembly which is
compatible with the Microsoft Visual C++ assembler (asaxp).  For examples
or help in determining the format, a useful guide is to disassemble C object
files with the Alpha disassembler using the "-a" switch and examine the
output.

5. The type names that the debugger uses correspond to the nomenclature from
the "Alpha Architecture Reference Manual". Accordingly, the command to
examine a 4-byte quantity in the debugger is "e/l", instead of "e/d".
Similarily, "e/d" will format as a double.

6. All 64 bits of the integer registers are displayed in the register window,
but only the low order 32 bits are used in a debugger expression. If you
wish to test and/or set the high-order longword, you can use the ".l1" field
selection on a register name. E.g.:

        print r4.l1
        
will print the high 32 bits of the r4 register. Similarly:

        do t0.l1 = 8
        
will assign 8 to the high 32 bits of the t0 register. The ".l0" field selects
the low order 32 bits of the given register.
