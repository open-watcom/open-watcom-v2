/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  Watcom Interface Converter main module.
*
****************************************************************************/


#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <conio.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "wic.h"
#include "wressetr.h"
#include "wreslang.h"
#include "banner.h"
#ifdef TRMEM
    #include "trmem.h"
#endif

static int _fileNum = 0;
static unsigned MsgShift = 0;

static void reportBadHeap(int retval);

/*Forward declarations */
void incDebugCount(void);

const char *FingerMsg[] = {
    banner1w( "Interface Converter", _WIC_VERSION_ ),
    banner2,
    banner2a( "1993" ),
    banner3,
    banner3a,
    0
};

/*--------------------- Resources --------------------------------*/

#define NIL_HANDLE      ((int)-1)
static  HANDLE_INFO     hInstance = { 0 };
extern  long            FileShift;

static long res_seek( int handle, long position, int where )
/* fool the resource compiler into thinking that the resource information
 * starts at offset 0 */
{
    if( where == SEEK_SET ) {
        return( lseek( handle, position + FileShift, where ) - FileShift );
    } else {
        return( lseek( handle, position, where ) );
    }
}

WResSetRtns( open, close, read, write, res_seek, tell, malloc, free );

void initWicResources( char * fname )
{
    int initerror;
    hInstance.filename = fname;
    hInstance.handle = open( hInstance.filename, O_RDONLY | O_BINARY );
    if( hInstance.handle == NIL_HANDLE ) {
        initerror = 1;
    } else {
        initerror = FindResources( &hInstance );
    }
    if( !initerror ) {
        initerror = InitResources( &hInstance );
    }
    if( initerror ) {
        fprintf(stderr, "Internal error: Cannot open resources");
        wicExit(-1);
    }
    MsgShift = _WResLanguage() * MSG_LANG_SPACING;
}

int getResStr( int resourceid, char *buffer )
{
    if ( LoadString( &hInstance, resourceid + MsgShift, (LPSTR)buffer, MAX_RESOURCE_SIZE ) != 0 ) {
        buffer[0] = 0;
        return 0;
    } else {
        return 1;
    }
}

void zapWicResources(void)
{
    CloseResFile( &hInstance );
}

/*--------------------- Error reporting --------------------------*/

static FILE* errorFile = NULL;
static char errorFileName[_MAX_PATH];
void initErrorFile(char *name) {
    assert(errorFile == NULL);
    errorFile = wicFopen(setNewFileExt(errorFileName, name, "err"), "wt");
}

void zapErrorFile(void) {
    wicFclose(errorFile);
    errorFile = NULL;
}

void logError(char *s) {
    FILE *output = errorFile;
    if (errorFile == NULL) {
        output = stderr;
    }
    fprintf(output, "%s\n", s);
}


void reportError(WicErrors err, ...) {
    va_list arglist;
    static char errStr[MAX_TOKEN_SIZE];
    static char resStr[MAX_RESOURCE_SIZE];
    int errStrLen;
    int displayThisError = 0;
    enum { CERR, RERR, ERR, FATAL } errType;
    char *errPrefix[] = { "<WIC>C", "<WIC>R", "<WIC>E", "<WIC>FATAL" };

    errStr[0] = 0;
    errStrLen = 0;
    if (WITHIN_RANGE(err, ERR_MIN, ERR_MAX) || err == ERR_NONE) {
        errType = ERR;
    } else if (WITHIN_RANGE(err, RERR_MIN, RERR_MAX)) {
        errType = RERR;
    } else if (WITHIN_RANGE(err, CERR_MIN, CERR_MAX)) {
        errType = CERR;
    } else if (WITHIN_RANGE(err, FATAL_MIN, FATAL_MAX)) {
        errType = FATAL;
    } else {
        printf("Internal error inside ReportError, exiting...\n");
        exit(1);
    }
    switch (g_opt.supressLevel) {
        case 0:  // display all errors
            displayThisError = 1;
            break;
        case 1:  // Supress CERR
            if (errType != CERR) {
                displayThisError = 1;
            }
            break;
        case 2:  // Supress CERR and RERR
            if (errType != CERR && errType != RERR) {
                displayThisError = 1;
            }
            break;
        case 3:  // Supress CERR and RERR and ERR
            if (errType != CERR && errType != RERR && errType != ERR) {
                displayThisError = 1;
            }
            break;
    }

    if (g_currFileName != NULL) {
        errStrLen += sprintf(errStr+errStrLen, "%s", g_currFileName);
        if (g_currLineNum != 0) {
            errStrLen += sprintf(errStr+errStrLen, "(%d)", g_currLineNum);
        }
        if (g_currColNum != 0) {
            errStrLen += sprintf(errStr+errStrLen, "C%d ", g_currColNum);
        }
    }
    errStrLen += sprintf(errStr+errStrLen, "%s: ", errPrefix[errType]);

    if (!getResStr(err, resStr)) {
        printf("Internal error inside ReportError, can't get resource, exiting...\n");
        exit(1);
    }
    va_start(arglist, err);
    errStrLen += vsprintf(errStr+errStrLen, resStr, arglist);
    va_end(arglist);
    if (g_currPos == NULL || displayThisError)
    {
        wicPrintMessage(errStr);
    } else {
        g_numErrNotDisp++;
    }

    if (errType == FATAL) {
        wicExit(1);
    }

    logMessage(errStr);
    logError(errStr);
}

/*---------------------- Memory Management ---------------------------*/

#ifdef TRMEM
    static _trmem_hdl TrHdl;
    static unsigned NumMessages = 0;

    #pragma initialize 40;

    static enum {
        _MEMOUT_NORMAL,
        _MEMOUT_INFO
    } _memOutput = _MEMOUT_NORMAL;

    static void _printLine( void *dummy1, const char *buf, size_t dummy2 )
    {
        dummy1 = dummy1;  dummy2 = dummy2;
        if (_memOutput == _MEMOUT_NORMAL) {
            debugOut("%s", buf);
            NumMessages++;
            if (strstr(buf, "overrun") != NULL) {
                printf("%s", buf);
            }
        } else {
            printf("%s", buf);
        }
    }
#endif

void printMemUsage(void) {
    #ifdef TRMEM
        int save = _memOutput;
        _memOutput = _MEMOUT_INFO;
        _trmem_prt_usage( TrHdl );
        _memOutput = save;
    #else
        printf("Memory used: %d.\n", g_memUsed);
    #endif
}

void outOfMemory(void) {
    reportError(FATAL_OUT_OF_MEM);
}

#ifdef TRMEM
    void *_debugVar = 0;
#endif
void *BasicAlloc(size_t size) {
    void *temp;
    #ifdef TRMEM
        temp = _trmem_alloc( size, _trmem_guess_who(), TrHdl );
    #else
        temp = malloc(size);
    #endif
    if (temp == NULL && size != 0) {
        outOfMemory();
    }

    if (size != 0) {
        g_memUsed += _msize(temp);
    }
    incDebugCount();
    #ifdef TRMEM
        if (_debugVar == 0) {
            printf("Enter _debugVar (in hex): "); scanf("%x", &_debugVar);
        }
        if (temp == _debugVar) {
            printf("temp = _debugVar!\n");
        }
    #endif
    return temp;
}

char *wicStrdup(const char *src) {
    char *temp;
    unsigned len;

    if (src != NULL) {
        len = strlen(src)+1;
        temp = wicMalloc(len);
        memcpy(temp, src, len);
    } else {
        temp = NULL;
    }
    return temp;
}

void BasicFree(void *ptr) {
    incDebugCount();
    if (ptr != NULL) {
        unsigned temp = _msize(ptr);
        #ifdef TRMEM
            _trmem_free( ptr, _trmem_guess_who(), TrHdl );
        #else
            free(ptr);
        #endif
        g_memUsed -= temp;
    }
}

static void GetOffset(void) {
    GetOffset(); // Dummy function, needed just to get offest for os/2.
                     // (Ask Ivan for details).  It calls itself to avoid
                     // warning message of not being referenced.
}

void initMemory(void)
{
    reportBadHeap(_heapset(0));
#ifdef TRMEM
    TrHdl = _trmem_open( malloc, free, NULL, NULL, NULL, _printLine,
            _TRMEM_ALLOC_SIZE_0 | _TRMEM_REALLOC_SIZE_0 |
            _TRMEM_OUT_OF_MEMORY | _TRMEM_CLOSE_CHECK_FREE );
    if (TrHdl == NULL) {
        printf("Memory initialization failed.\n");
        exit(1);
    }
#endif
    InitFMem(BasicAlloc, BasicFree, NULL, 0);
}

void zapMemory(void)
{
    int check = FMEM_NO_CHECK;
    checkMemory();
#ifdef TRMEM
    _trmem_prt_list( TrHdl );
    _trmem_close( TrHdl );
    if( NumMessages > 1 ) {
        printf( "Memory Problems detected!\n" );
    }
    check = FMEM_CHECK;
#endif
    FiniFMem(check);
}

static int numCallsToCheckMemory = 0;
static void reportBadHeap(int retval) {
    switch(retval) {
    case _HEAPOK:
    case _HEAPEMPTY:
        return;

    case _HEAPBADBEGIN:
    case _HEAPBADNODE:
    case _HEAPBADPTR:
    default:
        printf("Call to checkMemory: #%d\n", numCallsToCheckMemory);
        reportError(FATAL_INTERNAL, "In checkMemory: Memory corruption");
    }
}

void checkMemory(void) {
    struct _heapinfo h_info;
    int ret;

    numCallsToCheckMemory++;
    reportBadHeap(_heapset(0));
    h_info._pentry = NULL;
    for(;;) {
        ret = _heapwalk(&h_info);
        if (ret == _HEAPEND) {
            break;
        }
        reportBadHeap(ret);
    }
}

/*------------------------ Exit -----------------------------------*/

void printUsageAndExit(void) {
    int i = MSG_USAGE_BASE;
    int j = 0;
    char resStr[MAX_RESOURCE_SIZE];

    if (!getResStr(i, resStr)) {
        reportError(FATAL_INTERNAL, "Can't get usage resource");
    }

    while (FingerMsg[j])
        printf("%s\n", FingerMsg[j++]);

    while (strcmp(resStr, "END") != 0) {
        printf("%s\n", resStr);
        ++i;
        if (!getResStr(i, resStr)) {
            reportError(FATAL_INTERNAL, "Can't get usage resource");
        }
    }
    wicExit(0);
}

/*--------------------- Debugging ----------------------------------*/

long _debugSize = 1029204;
int _modifyMe = 1;
void incDebugCount(void) {
    g_debugCount++;
    if (g_debugCount >= _debugSize) {
        _modifyMe = !_modifyMe;
    }
}

void wicAssert( int exprTrue, char *expr, char *file, int line) {
    if (!exprTrue) {
        fprintf(stderr, "Assertion failed: %s (file %s, line %d)\n",
            expr, file, line);
        reportError(FATAL_INTERNAL_ASSERTION_FAILED, expr, file, line);
    }
}


#ifndef NDEBUG
    static FILE *_debugFile;
#endif
void initDebug(void) {
    #ifndef NDEBUG
        _debugFile = wicFopen("debug.wic", "wt");
        if (_debugFile == NULL) {
            reportError(ERR_CLOSE_FILE, strerror(errno));
        }
    #endif
}

void zapDebug() {
    #ifndef NDEBUG
        wicFclose(_debugFile);
        if (_fileNum != 0) {
            printf("DEBUG: _fileNum = %d != 0,  at the end!", _fileNum);
        }
    #endif
}

void debugOut(char *format, ...) {
    #ifndef NDEBUG
        va_list arglist;

        va_start(arglist, format);
        if (_debugFile == NULL) {
            _debugFile = stderr;
        }
        if (vfprintf(_debugFile, format, arglist) < 0) {
            perror("Unable to write to debug file");
        }
        va_end(arglist);
        fflush(_debugFile);
    #else
        format = format;
    #endif
}

/*---------------------------- File IO ----------------------------------*/

#define MAX_OPEN_FILES  100   // Actually, OS itself will probably impose
                              // a limit of about 10-40 open files maximum

void wicFclose(FILE *fp) {
    assert(_fileNum > 0);
    if (fp != NULL) {
        if (fclose(fp)) {
            reportError(ERR_CLOSE_FILE,  strerror(errno));
        } else {
            _fileNum--;
        }
    }
}

FILE *wicFopen(const char *filename, const char *mode) {
    FILE *retval = NULL;
    assert(filename != NULL);
    assert(mode != NULL);
    if (_fileNum >= MAX_OPEN_FILES) {
        reportError(ERR_OPEN_FILE, filename, "Too many files open");
    } else {
        retval = fopen(filename, mode);
        if (retval == NULL) {
            reportError(ERR_OPEN_FILE, filename, strerror(errno));
        } else {
            _fileNum++;
        }
    }
    return retval;
}

int fileReadable(char *fname) {
    if (_fileNum >= MAX_OPEN_FILES) {
        return 0;
    } else {
        return !access(fname, F_OK);
    }
}

/*---------------------------- Other ----------------------------------*/

static int lineLen = 0;

void dribble(void) {
    int newLineLen = 0;
    static char line[200];
    int         len;

    g_dribbleCounter++;
    cprintf("\r");
    if (g_currFileName !=  NULL) {
        newLineLen += sprintf(line+newLineLen, "FILE: ");
        newLineLen += sprintf(line+newLineLen, "%s", g_currFileName);
        newLineLen += sprintf(line+newLineLen, "(%d)   ", g_currLineNum);
    }
    #ifdef NDEBUG
    newLineLen += sprintf(line+newLineLen, "TOTAL: %d   SYM: %d   ERR: %d",
        g_totalNumLines, g_numSymbols, g_numErrNotDisp);
    #else
        newLineLen += sprintf(line+newLineLen,
            "TOTAL: %d  SYM: %d   MEM: %d   ERR: %d",
            g_totalNumLines, g_numSymbols, g_memUsed, g_numErrNotDisp);
    #endif
    if (newLineLen > 79) {
        newLineLen = 79;
        line[newLineLen] = 0;
    }
    len = lineLen - newLineLen;
    if( len < 0 )
        len = 0;
    cprintf("%s%*s", line, len, "");
    lineLen = newLineLen;
    cprintf("\r");
}

void wicPrintMessage(char *s)
{
    int newLineLen;
    int len;

    newLineLen = printf("%s", s); fflush(stdout);
    len = lineLen - newLineLen;
    if( len < 0 )
        len = 0;
    cprintf("%*s", len, "");
    printf("\n");
    lineLen = 0;
}


char *setNewFileExt(char *newName, char *oldName, char *newExt) {
    int i;
    int len = strlen(oldName);
    int len1;

    if (oldName != newName) {
        memmove(newName, oldName, len);
    }
    len1 = len - 4;
    if( len1 < 0 )
        len1 = 0;
    for( i = len - 1; i >= len1; i-- ) {
        if( newName[i] == '.' ) {
            strcpy( newName + i + 1, newExt );
            return( newName );
        }
    }
    newName[len] = '.';
    strcpy(newName+len+1, newExt);
    return newName;
}

void setDefaultExt(char *fname, char *ext) {
    int i;
    int len = strlen(fname);
    int len1;

    len1 = len - 4;
    if( len1 < 0 )
        len1 = 0;
    for( i = len - 1; i >= len1; i-- ) {
        if( fname[i] == '.' ) {
            return;
        }
    }
    fname[len] = '.';
    strcpy( fname + len + 1, ext );
}

