/* $Id: GETLONGO.CC 1.1 94/12/13 17:45:48 Anthony_Scian Exp $ */
/* S Manoharan. Advanced Computer Research Institute. Lyon. France */

#include "GetLongOpt.h"

GetLongOpt::GetLongOpt(const char optmark)
{
   table = last = 0;
   ustring = "[valid options and arguments]";
   enroll_done = 0;
   optmarker = optmark;
}

GetLongOpt::~GetLongOpt()
{
   Cell *t = table;

   while ( t ) {
      Cell *tmp = t;
      t = t->next;
      delete tmp;
   }
}

char *
GetLongOpt::basename(char * const pname) const
{
   char *s;
   char *n;
   char *p;
   size_t len;

   s = strrchr(pname, '\\');
   if ( s == 0 ) s = pname;
   else ++s;
   len = strlen( s );
   n = strchr( s, '.' );
   if( n ) len = n - s;
   p = new char[len+1];
   strncpy( p, s, len );
   p[len] = 0;
   strlwr( p );

   return p;
}

int
GetLongOpt::enroll(const char * const opt, const OptType t,
const char * const desc, const char * const val)
{
   if ( enroll_done ) return 0;

   Cell *c = new Cell;
   c->option = opt;
   c->type = t;
   c->description = desc ? desc : "no description available";
   c->value = val;
   c->next = 0;

   if ( last == 0 ) {
      table = last = c;
   }
   else {
      last->next = c;
      last = c;
   }

   return 1;
}

const char *
GetLongOpt::retrieve(const char * const opt) const
{
   Cell *t;
   for ( t = table; t != 0; t = t->next ) {
      if ( strcmp(opt, t->option) == 0 )
	 return t->value;
   }
   cout << "GetLongOpt::retrieve - unenrolled option ";
   cout << optmarker << opt << "\n";
   return 0;
}

int
GetLongOpt::parse(int argc, char * const *argv)
{
   int optind = 1;

   pname = strlwr( strdup( basename(*argv) ) );
   enroll_done = 1;
   if ( argc-- <= 1 ) return optind;

   while ( argc >= 1 ) {
      char *token = *++argv; --argc;

      if ( token[0] != optmarker || token[1] == optmarker )
	 break;	/* end of options */

      ++optind;
      char *tmptoken = ++token;
      while ( *tmptoken && *tmptoken != '=' )
	 ++tmptoken;
      /* (tmptoken - token) is now equal to the command line option
	 length. */

      Cell *t;
      enum { NoMatch, ExactMatch, PartialMatch } matchStatus = NoMatch;
      Cell *pc = 0;	// pointer to the partially-matched cell
      for ( t = table; t != 0; t = t->next ) {
	 if ( strncmp(t->option, token, (tmptoken - token)) == 0 ) {
	    if ( strlen(t->option) == (tmptoken - token) ) {
	       /* an exact match found */
	       int stat = setcell(t, tmptoken, *(argv+1), pname);
	       if ( stat == -1 ) return -1;
	       else if ( stat == 1 ) {
		  ++argv; --argc; ++optind;
	       }
	       matchStatus = ExactMatch;
	       break;
	    }
	    else {
	       /* partial match found */
	       matchStatus = PartialMatch;
	       pc = t;
	    }
	 } /* end if */
      } /* end for */

      if ( matchStatus == PartialMatch ) {
	 int stat = setcell(pc, tmptoken, *(argv+1), pname);
	 if ( stat == -1 ) return -1;
	 else if ( stat == 1 ) {
	    ++argv; --argc; ++optind;
	 }
      }
      else if ( matchStatus == NoMatch ) {
	 cout << pname << ": unrecognized option ";
	 cout << optmarker << strtok(token,"= ") << "\n";
	 return -1;		/* no match */
      }

   } /* end while */

   return optind;
}

int
GetLongOpt::parse(char * const str, char * const p)
{
   enroll_done = 1;
   char *token = strtok(str, " \t");
   char *name = p ? p : "GetLongOpt";

   while ( token ) {
      if ( token[0] != optmarker || token[1] == optmarker ) {
	 cout << name << ": nonoptions not allowed\n";
	 return -1;	/* end of options */
      }

      char *ladtoken = 0;	/* lookahead token */
      char *tmptoken = ++token;
      while ( *tmptoken && *tmptoken != '=' )
	 ++tmptoken;
      /* (tmptoken - token) is now equal to the command line option
	 length. */

      Cell *t;
      enum { NoMatch, ExactMatch, PartialMatch } matchStatus = NoMatch;
      Cell *pc =0;	// pointer to the partially-matched cell
      for ( t = table; t != 0; t = t->next ) {
	 if ( strncmp(t->option, token, (tmptoken - token)) == 0 ) {
	    if ( strlen(t->option) == (tmptoken - token) ) {
	       /* an exact match found */
	       ladtoken = strtok(0, " \t");
	       int stat = setcell(t, tmptoken, ladtoken, name);
	       if ( stat == -1 ) return -1;
	       else if ( stat == 1 ) {
		  ladtoken = 0;
	       }
	       matchStatus = ExactMatch;
	       break;
	    }
	    else {
	       /* partial match found */
	       matchStatus = PartialMatch;
	       pc = t;
	    }
	 } /* end if */
      } /* end for */

      if ( matchStatus == PartialMatch ) {
	 ladtoken = strtok(0, " \t");
	 int stat = setcell(pc, tmptoken, ladtoken, name);
	 if ( stat == -1 ) return -1;
	 else if ( stat == 1 ) {
	    ladtoken = 0;
	 }
      }
      else if ( matchStatus == NoMatch ) {
	 cout << name << ": unrecognized option ";
	 cout << optmarker << strtok(token,"= ") << "\n";
	 return -1;		/* no match */
      }

      token = ladtoken ? ladtoken : strtok(0, " \t");
   } /* end while */

   return 1;
}

/* ----------------------------------------------------------------
GetLongOpt::setcell returns
   -1	if there was an error
    0	if the nexttoken was not consumed
    1	if the nexttoken was consumed
------------------------------------------------------------------- */

int
GetLongOpt::setcell(Cell *c, char *valtoken, char *nexttoken, char *name)
{
   if ( c == 0 ) return -1;

   switch ( c->type ) {
   case GetLongOpt::NoValue :
      if ( *valtoken == '=' ) {
	 cout << name << ": unsolicited value for flag ";
	 cout << optmarker << c->option << "\n";
	 return -1;	/* unsolicited value specification */
      }
      c->value = (c->value) ? 0 : (char *) ~0;
      return 0;
   case GetLongOpt::OptionalValue :
      if ( *valtoken == '=' ) {
	 c->value = ++valtoken;
	 return 0;
      }
      else {
	 if ( nexttoken != 0 && nexttoken[0] != optmarker ) {
	    c->value = nexttoken;
	    return 1;
	 }
	 else return 0;
      }
      break;
   case GetLongOpt::MandatoryValue :
      if ( *valtoken == '=' ) {
	 c->value = ++valtoken;
	 return 0;
      }
      else {
	 if ( nexttoken != 0 && nexttoken[0] != optmarker ) {
	    c->value = nexttoken;
	    return 1;
	 }
	 else {
	    cout << name << ": mandatory value for ";
	    cout << optmarker << c->option << " not specified\n";
	    return -1;	/* mandatory value not specified */
	 }
      }
      break;
   default :
      break;
   }
   return -1;
}

void
GetLongOpt::usage(ostream &outfile) const
{
   Cell *t;

   outfile << "usage: " << pname << " " << ustring << "\n";
   for ( t = table; t != 0; t = t->next ) {
      outfile << "\t" << optmarker << t->option;
      if ( t->type == GetLongOpt::MandatoryValue )
	 outfile << " <$val>";
      else if ( t->type == GetLongOpt::OptionalValue )
	 outfile << " [$val]";
      outfile << " (" << t->description << ")\n";
   }
   outfile.flush();
}

