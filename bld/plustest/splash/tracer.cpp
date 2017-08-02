/*
** File - tracer.cxx
*/

#ifndef TRACER_HXX
#include "tracer.h"
#endif

#include <iostream.h>
#include <string.h>

char*    Tracer::prog;           // program being worked on
int      Tracer::mode;           // global mode of Tracer
char*    Tracer::watch;          // function to watch for and trace
int      Tracer::level;
ostream* Tracer::str;

// ****************************************************************:
//      Tracer
//
// Description:
//      Initialize local as well as static information.  Set up
//      for total cleanup at destruction.  Also outputs Tracer
//      to stream using operator<<.  Implicit assumption is that
//      this is only called once per run of a program.
// ****************************************************************:
Tracer::Tracer(char* s, const int n, ostream &st)
{
        str= &st;
        prog = func = s;        // Set program name
        mode = n;               // Initialize the tracing mode (no default).
        lmode = 1;              // Recognize this as top-level.
        level = 0;
        if (mode)               // Should we output?
                *str << *this << ": entered" << endl;
}

// ****************************************************************:
//      Tracer
//
// Description:
//      Initialize only local information.  No total cleanup
//      at destuction.  Also outputs Tracer to stream using
//      operator<<.  Also checks to see if input character
//      string contains information that Tracer is watching
//      for and turns tracing on if it does.  Implicit assumption
//      is that this is the way Tracer will be set up when needed.
// ****************************************************************:
Tracer::Tracer(char* s, int l)
{
        func = s;               // Leave program alone, but set function
        emode= l;               // entry/exit mode
        level++;
        if (watch != 0)                 // Watching something?
        {
                if (strstr(func, watch) != 0)   // Watching for this?
                {                       // yes...
                        lmode = mode;   // remember global mode locally
                        mode = -1;      // turn tracing on
                }
        }
        else                            // no...
                lmode = 0;              // set local mode to not top level

        if (mode & emode){                  // Should we output?
                *str << *this << ": entered" << endl;
        }
}


// ****************************************************************:
//      ~Tracer
//
// Description:
//      Check for totalcleanup flag and cleanup.
//      Also outputs Tracer to stream using operator<<.
// ****************************************************************:
Tracer::~Tracer()
{
        if (mode & emode){                       // Should we output?
                *str << *this << ": exitted" << endl;
        }
        level--;

        if (watch != 0)                 // Watching something?
        {
                if (strstr(func, watch) != 0)   // Watching for the function?
                {                       // yes...
                        mode = lmode;   // reset remembered global mode
                        lmode = 0;      // this is not the top level
                }
        }

        if (lmode)                      // Is this top level?
                TotalCleanup();
}


// ****************************************************************:
//              TotalCleanup
//
// Description:
//      Do what's needed to finally cleanup Tracer.
//      For instance, might be a close of a log file.
// ****************************************************************:
void
Tracer::TotalCleanup()
{
        if (watch)
                delete watch;
}


// ****************************************************************:
//              SetWatch
//
// Description:
//      Set a function name to watch trace on.
// ****************************************************************:
void
Tracer::SetWatch(char* name)
{
        if (watch)
                delete watch;

        watch = new char[strlen(name)+1];
        strcpy(watch, name);
}

// ****************************************************************:
//              Operator<<
//
// Description:
//      Puts Tracer into an output stream.
// ****************************************************************:
ostream&
operator<<(ostream& str, Tracer& x)
{
        for(int i=0;i<x.level;i++) str << '.';
        str << x.func;   // output the Tracer object to stream
        return str;
}
