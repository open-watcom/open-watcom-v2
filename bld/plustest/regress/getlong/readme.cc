/* $Id: README.CC 1.1 93/03/02 13:02:40 Anthony_Scian Exp $ */
/* S Manoharan. Advanced Computer Research Institute. Lyon. France */

/*

Yes. Yet another GetLongOpt. What's special here?

GetLongOpt supports long options. In fact, there is no support for
explicit short options. For example, -a -b *cannot* be shortened
to -ab. However, long options can be abbreviated as long as there
is no ambiguity. An ambiguity is resolved by using the last option
in the sequence of options (we will come to this later).
If an option requires a value, then the value should be separated
from the option either by whitespace  or by a "=".

Other features:
o	GetLongOpt can be used to parse options given through environments.
o	GetLongOpt provides a usage function to print usage.
o	Flags & options with optional or mandatory values are supported.
o	The option marker ('-' in Unix) can be customized.
o	Parsing of command line returns optind (see getopt(3)).
o	Descriptive error messages.

Let's take a walk through the usage. 
*/


#include "getlongopt.h"
#include <iostream.h>
#include <stdlib.h>

int debug_index = 0;

int
main(int argc, char **argv)
{
   GetLongOpt option;
// Constructor for GetLongOpt takes an optional argument: the option
// marker. If unspecified, this defaults to '-', the standard (?)
// Unix option marker. For example, a DOS addict may want to have
// "GetLongOpt option('/');" instead!!

   char *scid = "a.out version 1.0 dated 21.01.1993";

   option.usage("[options and args]");

// GetLongOpt::usage is overloaded. If passed a string "s", it sets the
// internal usage string to "s". Otherwise it simply prints the
// command usage. More on it in a while.

   option.enroll("help", GetLongOpt::NoValue,
      "print this option summary", 0);
   option.enroll("version", GetLongOpt::NoValue,
      "print the version", 0);
   option.enroll("output", GetLongOpt::MandatoryValue,
      "print output in file $val", "a.out.output");
   option.enroll("verify", GetLongOpt::NoValue,
      "verify if ambiguities are resolved as they should be", "");
#ifdef DEBUG
   option.enroll("debug", GetLongOpt::MandatoryValue,
      "set debug level to $val", "0");
#endif

// GetLongOpt::enroll adds option specifications to its internal
// database. The first argument is the option sting. The second
// is an enum saying if the option is a flag (GetLongOpt::NoValue),
// if it requires a mandatory value (GetLongOpt::MandatoryValue) or
// if it takes an optional value (GetLongOpt::OptionalValue).
// The third argument is a string giving a brief description of
// the option. This description will be used by GetLongOpt::usage.
// GetLongOpt, for usage-printing, uses $val to represent values
// needed by the options. <$val> is a mandatory value and [$val]
// is an optional value. The final argument to GetLongOpt::enroll
// is the default string to be returned if the option is not
// specified. For flags (options with NoValue), use "" (empty
// string, or in fact any arbitrary string) for specifying TRUE
// and 0 (null pointer) to specify FALSE.

// Usage is printed with GetLongOpt::usage. The options and their 
// descriptions (as specified during enroll) are printed in the
// order they are enrolled.

   if ( option.parse(getenv("A_OUT"), "A_OUT") < 1 )
      return -1;

// GetLongOpt::parse is overloaded. It can either parse a string of
// options (typically given from the environment), or it can parse
// the command line args (argc, argv). In either case a return
// value < 1 represents a parse error. Appropriate error messages
// are printed when errors are seen. GetLongOpt::parse, in its first
// form, takes two strings: the first one is the string to be
// parsed and the second one is a string to be prefixed to the
// parse errors. In ts second form, GetLongOpt::parse returns the
// the optind (see getopt(3)) if parsing is successful.

   int optind = option.parse(argc, argv);
   if ( optind < 1 )
       return -1;

   const char *outfile = option.retrieve("output");

#ifdef DEBUG
   debug_index = atoi(option.retrieve("debug"));
#endif

   if ( option.retrieve("help") ) {
      option.usage();
      return 0;
   }
   if ( option.retrieve("version") ) {
      cout << scid << "\n";
      return 0;
   }
   if ( option.retrieve("verify") ) {
      cout << "verify turned on by default" << "\n";
   }
   else {
      cout << "verify turned off" << "\n";
   }

// The values of the options that are enrolled in the database
// can be retrieved using GetLongOpt::retrieve. This returns a string
// and this string should be converted to whatever type you want.
// See atoi, atof, atol etc. I suppose you would do a "parse" before
// retrieving. Otherwise all you would get are the default values
// you gave while enrolling!
// Ambiguities while retrieving (may happen when options are
// abbreviated) are resolved by taking the matching option that 
// was enrolled last. For example, -v will expand to -verify.

   
   for ( ; optind < argc; ++optind ) {
   } /* process all the arguments here */

   option.retrieve("foo");

// If you try to retrieve something you didn't enroll, you will
// get a warning message. If you had made a typo somewhere while
// enrolling or retrieving, now is the time to correct it.

   return 0;
}

/*

I tested GetLongOpt on gcc 2.3.3 and cfront 2.1 on Sun4s. It worked.
(Therefore, it works on all C++ compilers and all machines! :-))

S Manoharan                                 Email    : mano@acri.fr
Advanced Computer Research Institute        Fax      : +33 72 35 84 10
1 Boulevard Marius Vivier-Merle             Voice    : +33 72 35 80 44
69443 Lyon Cedex 03 France		    

*/

