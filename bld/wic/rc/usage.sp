:segment ENGLISH
Watcom Interface Converter.
Usage: wic [options] filename [[options] [filename] ...]

Options must be preceeded by '-' or '/'.  They may be specified in any order
but in case of conflict, the rightmost takes precedence. Available options are:
  -?, -h         Print this message
  -16            16-bit mode:  Equivalient to \"-I16 -P16\"
  -32            32-bit mode:  Equivalient to \"-I32 -P32\" [DEFAULT]
  -a             Generate Assembly code
  -ad            Attach 'D' to the end of every decimal number when
                 generating assembly code
  -f             Generate Fortran code [DEFAULT]
  -F[name]       Use [name] as a separator between field and structure name
                 [DEFAULT: -F for fortran, -F_FS_ for assembly]
  -i<paths>      Search <paths> for include files
  -j             Change char default from unsigned to signed
  -I<numbits>    Number of bits that an integer has
  -l<number>     Maximum number of characters allowed on each output line
                 before the line is continued on next line.
                 [DEFAULT: 72 chars for fortran, 79 for assembly]
  -p[name]       Use [name] as a prefix to identifiers which conflict with
                 keywords in target language [DEFAULT: -pWIC_]
  -P16           Equivalent to \"-Pn16 -Pd16 -Pf32 -Ph32\"
  -P32           Equivalent to \"-Pn32 -Pd32 -Pf32 -Ph48\"
  -P<t><numbits> Specify size of pointer of type <t> in bits. <t> is one of:
                   n      - Near pointer (example: -Pn32)
                   d or p - Default type pointer (example: -Pd32)
                   f      - Far pointer (example: -Pf32)
                   h      - Huge pointer (example: -Ph48)
  -s+ (-s-)      Prefix (do not prefix) every field in a structure with
                 structure name.  If -s- is specified, '-F' is also issued
		 [DEFAULT: -as- for fortran, -as+ for assembler]
  -s0, -s        Display all error messages
  -s1            Supress conversion error messages  [DEFAULT]
  -s2            Supress conversion and reading error messages
  -s3            Supress conversion, reading and general error messages
                 NOTE: 'ERR' indicator on status line displays number of
                 errors that were not output to screen.  All errors are logged
                 to a file regardless of option specified.
:segment HIDDEN
  -d             Turn debugging on (Internal option. Can only be activated when
                 when wic is compiled without NDEBUG being defined)
:endsegment

<paths> has a form of [drive:]directory[;[drive:]directory].
<number> is a decimal integer.
<numbits> is the number of bits, one of 16, 32 or 48.
Example: wic os2.h -a -s- -F -pOS2_
:elsesegment JAPANESE
Japanese usage message goes here
:endsegment
END
