/****************************************************************************
File: jamnewlg.h

Description:

Notes:
   Really have to redesign way of doing logging.
   
Date        Programmer  Modification
03 Jul 92   Jam         created from jamassrt
15 Jul 92   Jam         bugfix -- active() was not also checking if stream
                        opened
20 Jul 92   Jam         moved LOGing to functions because of (?) BC++ 3.1 bug

****************************************************************************/

#ifndef JAM_NewLog_H
#define JAM_NewLog_H

#ifdef JAM_IGNEWLOG  // ignore JAM_NewLog references; don't need to link

#define JAM_LOGNEW (void)0
#define JAM_LOGDEL (void)0

class ostream;

class JAM_NewLog {
public:
   static int activate() { return 0; }
   static void list_still_allocated() {}
   static void list_still_allocated(ostream&) {}
   static ostream& log() { JAM_crash("No newlog."); return *(ostream*)0; }

   static void* alloc(size_t);
   static void free(void*);
};

#else // do want new log

#include <stddef.h>     // for size_t
class ostream;
class ofstream;

#define JAM_LOGNEW JAM_NewLog::lognew(__FILE__, __LINE__)
#define JAM_LOGDEL JAM_NewLog::logdel(__FILE__, __LINE__)

/*## this caused weird bugs under BC++ 3.1 with inlining on
#define JAM_LOGNEW (JAM_NewLog::active() ? \
          ((JAM_NewLog::log() << __FILE__ << ',' << __LINE__ << '\t' << flush), 0) \
          : 0)
#define JAM_LOGDEL JAM_LOGNEW
*/

class JAM_NewLog {
public:
   static int activate()
      { if (newlog) return 1; else return create_newlog(); }
   static ostream& log();
   //: do not call unless activate() returns true

   static void list_still_allocated(ostream& os);

   static void list_still_allocated()
      { if (activate()) list_still_allocated(log()); }

   static void lognew(const char* file, int line)
      { put(file, line); }
   static void logdel(const char* file, int line)
      { put(file, line); }

   static void* alloc(size_t);
   static void free(void*);

protected:
   static void put(const char* file, int line);

   static int create_newlog();

   static ofstream* newlog;
};

#endif // JAM_IGNEWLOG

#endif // JAM_NewLog_H


