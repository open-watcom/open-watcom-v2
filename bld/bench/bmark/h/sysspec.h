/********************
**  INCLUDE FILES  **
********************/

/*
** Standard includes
*/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "nmglobal.h"

#ifndef MAC
#include <malloc.h>
#include <mem.h>
#endif


/*
** System-specific includes
*/

#ifdef DOS16MEM
#include "dos.h"
#endif

/* #include "time.h"
#include "io.h"
#include "fcntl.h"
#include "sys\stat.h" */
/* Removed for MSVC++
#include "alloc.h"
*/

/*
** MAC Time Manager routines (from Code Warrior)
*/
#ifdef MACTIMEMGR
#include <memory.h>
#include <lowmem.h>
#include <Types.h>
#include <Timer.h>
extern struct TMTask myTMTask;
extern long MacHSTdelay,MacHSTohead;
#endif

/*
** Windows 3.1 timer defines
*/
#ifdef WIN31TIMER
#include <windows.h>
#include <toolhelp.h>
TIMERINFO win31tinfo;
HANDLE hThlp;
FARPROC lpfn;
#endif

/****************************
**   FUNCTION PROTOTYPES   **
****************************/

farvoid *AllocateMemory(unsigned long nbytes,
                int *errorcode);

void FreeMemory(farvoid *mempointer,
                int *errorcode);

void MoveMemory( farvoid *destination,
                farvoid *source,
                unsigned long nbytes);

#ifdef DOS16MEM
void FarDOSmemmove(farvoid *destination,
                farvoid *source,
                unsigned long nbytes);
#endif

void ReportError(char *context, int errorcode);

void ErrorExit();

void CreateFile(char *filename,
                int *errorcode);

int bmOpenFile(char *fname,
                int *errorcode);

void CloseFile(int fhandle,
                int *errorcode);

void readfile(int fhandle,
                unsigned long offset,
                unsigned long nbytes,
                void *buffer,
                int *errorcode);

void writefile(int fhandle,
                unsigned long offset,
                unsigned long nbytes,
                void *buffer,
                int *errorcode);

unsigned long StartStopwatch();

unsigned long StopStopwatch(unsigned long startticks);

unsigned long TicksToSecs(unsigned long tickamount);

double TicksToFracSecs(unsigned long tickamount);

