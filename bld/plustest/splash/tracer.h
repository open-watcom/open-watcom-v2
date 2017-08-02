/*
** Tracer object
**
** Purpose:
**      Mostly for tracing function calls, but may be adapted for
**      logging a trace of a program to somewhere else.
*/

#ifndef TRACER_HXX
#define TRACER_HXX

// .NAME Tracer - class to manage providing a trace of functions.
// .LIBRARY util
// .HEADER Utility Classes
// .INCLUDE tracer.hxx
// .FILE tracer.hxx
// .FILE tracer.cxx

// .SECTION Description
// This class makes use of some of the semantics of C++ to provide
// a convenient way of tracing function calls.  This code was initially
// suggested by Bjarne Stroustrup, but has now been modified to allow
// control of when tracing takes place.  Tracing messages are sprinkled
// about the user's code and, at the user's choosing, the type of
// tracing is enabled.  Users of this object class will rarely (if
// ever) directly see the class.  Instead, they will use the macro
// interface defined with this class.

// .SECTION Tracing Types
// Tracer may be set to output its tracing information according to the
// type of mode it has been put into.  At the simplest level, Tracer may
// be set to output all tracing information by setting its type to -1
// (which is what TRACE_ON does).  At the next level, messages that
// Tracer is told about may be given a positive integer type < 32.
// Tracer may also be set to output only messages that are of certain
// types (via bitwise AND of current type with message type).  Users
// can, therefore, monitor all types of messages, some of the types,
// or just one of the types depending on the user's interest.  The final
// level is that a user may turn on monitoring of a function call by
// telling Tracer the name of the function to WATCH.  When Tracer hits
// the function in question, it turns TRACE_ON and turns TRACE_OFF when
// it leaves the function (so, all lower functions are also traced).

#include <iostream.h>


class Tracer
{
public:
        Tracer(char*, const int, ostream &st);   // Full tracer invocation
        Tracer(char*, int= -1);                  // Lower level invocation

        ~Tracer();                      // Close tracer invocation

        void    TotalCleanup();         // Clean up when fully done

        void    SetMode(const int n){mode = n;} // Set current mode of tracing
        int     GetMode(){ return mode;}        // Determine current tracing mode

        void    SetWatch(char*);                // Function name to begin tracing in
        char*   GetWatch(){return watch;};      // What we are watching for?

        ostream& GetStream(void){ return *str; }
        friend ostream& operator<<(ostream&, Tracer&);
                                        // Output tracing information

private:
        char*           func;           // function being worked on
        int             lmode;          // local mode of Tracer
        int             emode;          // entry/exit mode, determines if printed
        static char*    prog;           // program being worked on
        static int      mode;           // global mode of Tracer
        static char*    watch;          // function to watch for and trace
        static int      level;          // function call nesting
        static ostream* str;            // stream to output onto
};


// ****************************************************************
//              Macro Interface to Tracer Object
// ****************************************************************
#ifdef DOTRACER

#define FTRACER(s,n,st)    /* Trace program s with type n */\
        Tracer  _trace(s, n, st);

#define TRACER(s)       /* Trace function s */\
        Tracer  _trace(s);

#define LTRACER(s, l)       /* Trace function s with type n */\
        Tracer  _trace(s, l);

#define TRACE(s)        /* If TRACE_ON then output s */\
        if (_trace.GetMode()) _trace.GetStream() << _trace << ": " << s << endl;

#define TRACEF(f)       /* If TRACE_ON then call function f */\
        if (_trace.GetMode()) f;

#define TRACE_ON        /* Turn tracing on */\
        _trace.SetMode(-1);

#define TRACE_OFF       /* Turn tracing off */\
        _trace.SetMode(0);

#define LTRACE(l,s)     /* If (type & l) then output s */\
        if (_trace.GetMode() & l) \
                _trace.GetStream() << _trace << ": " << s << endl;

#define LTRACEF(l,f)    /* If (type & l) then call function f */\
        if (_trace.GetMode() & l) f;

#define STRACE(m)       /* Set tracing to type m */\
        _trace.SetMode(m);

#define WATCH(s)        /* Set function to watch to s */\
        _trace.SetWatch(s);

#else

#define FTRACER(s,n,st) /**/
#define TRACER(s)       /**/
#define LTRACER(s,l)    /**/
#define TRACE(s)        /**/
#define TRACE_ON        /**/
#define TRACE_OFF       /**/
#define LTRACE(l,s)     /**/
#define LTRACEF(l,s)    /**/
#define STRACE(m)       /**/
#define WATCH(s)        /**/

#endif

#endif
