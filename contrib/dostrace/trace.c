/*
 * trace - trace DOS system calls
 *
 * (C) Copyright 1991-1994 Diomidis Spinellis.  All rights reserved.
 *
 * Permission to use, copy, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dos.h>
#include <process.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <bios.h>
#include <errno.h>

#define MAXBUFF 1025

/*
 * Program options.  See main() for explanation.
 */
static int stringprint, hexprint, otherprint, nameprint, regdump, tracechildren;
static int timeprint, count, tracetsr, tsrpsp, numprint, worderror, pspprint;
static int branchprint, append;
static unsigned datalen = 15;
static int someprefix;
static char *fname = "trace.log";

static int mypsp, tracedpsp;        /* PSP of us and traced program */
static int execed;                  /* True after we run */
static char _far *critsectflag;     /* DOS critical section flag */
static char _far *criterrflag;      /* DOS citical error flag */

/* DOS interrupt */
#define DOS_INT 0x21

/* Old dos handler to chain to */
static void (_interrupt _far *old_dos_handler)();

/* Output file descriptor */
static int fd;

#pragma intrinsic(strlen)

/*
 * Write far buffer to a file
 */
void writebuf(void _far *buffer, int len, int fd);
#pragma aux writebuf =      \
    "xchg dx, ax"           \
    "push ds"               \
    "mov ds, ax"            \
    "mov ah, 40h"           \
    "int 21h"               \
    "pop ds"                \
    parm [ax dx] [cx] [bx]  \
    modify [ax dx];

/*
 * Output a string to the trace file
 */
static void
tputs(char *s)
{
    int len = strlen(s);
    char far *p = s;

    writebuf(p, len, fd);
}

int _doprnt(char *fmt, va_list marker, FILE *f);

#include "doprnt.h"

/*
 * Print a formatted string to the trace file
 */
int
tprintf(char *fmt, ...)
{
    static va_list marker;
    int result;
    static char msg[200];
    static FILE f;

    f._ptr = /*f._base =*/ msg;
    f._cnt = 32000;
    f._flag = _IOWRT | _IOFBF;
    va_start(marker, fmt);
    result = _doprnt(fmt, marker, &f);
    *f._ptr = '\0';
    va_end(marker);
    tputs(msg);
    return result;
}

/*
 * Convert a high low pair into a long
 */
static long
makelong(unsigned high, unsigned low)
{
    union {
        struct {
            unsigned low;
            unsigned high;
        } hl;
        long l;
    } v;

    v.hl.high = high;
    v.hl.low = low;
    return v.l;
}


/*
 * Convert a segment offset pair into a far pointer.
 */
static void _far *
makefptr(unsigned seg, unsigned off)
{
    union {
        struct {
            unsigned off;
            unsigned seg;
        } so;
        void _far *ptr;
    } v;

    v.so.seg = seg;
    v.so.off = off;
    return v.ptr;
}

static char buff[MAXBUFF];

/*
 * Convert a $ terminated string to a 0 terminated one
 */
static char *
makestring(unsigned sseg, unsigned soff)
{
    char _far *p, *s;
    unsigned count = 0;

    p = makefptr(sseg, soff);
    s = buff;
    while (*p != '$' && count < datalen) {
        *s++ = *p++;
        count++;
    }
    *s = '\0';
    return buff;
}

/*
 * Return day of week
 */
static char *
weekday(int n)
{
    switch (n) {
        case 0: return "Sun";
        case 1: return "Mon";
        case 2: return "Tue";
        case 3: return "Wed";
        case 4: return "Thu";
        case 5: return "Fri";
        case 6: return "Sat";
    }
    return( "???" );
}

/*
 * Decode device information as for ioctl 0
 */
static void
devinfo(unsigned n)
{
    if (n & 0x80) {
        tputs("\tCHARDEV: ");
        if (n & 0x01) tputs("STDIN ");
        if (n & 0x02) tputs("STDOUT ");
        if (n & 0x04) tputs("NUL ");
        if (n & 0x08) tputs("CLOCK$ ");
        if (n & 0x10) tputs("SPECIAL ");
        if (n & 0x20) tputs("RAW ");
        if (n & 0x40) tputs("NOT_EOF "); else tputs("EOF ");
        if (n & 0x1000) tputs("REMOTE "); else tputs("LOCAL ");
        if (n & 0x4000) tputs("CAN_IOCTL"); else tputs("NO_IOCTL");
    } else {
        tputs("\tFILE: ");
        tprintf("device=%u ", n & 0x1f);
        if (n & 0x40) tputs("NOT_WRITTEN "); else tputs("WRITTEN ");
        if (n & 0x800) tputs("FIXED "); else tputs("REMOVABLE ");
        if (n & 0x4000) tputs("KEEP_DATE "); else tputs("UPDATE_DATE ");
        if (n & 0x8000) tputs("REMOTE"); else tputs("LOCAL");
    }
    tputs("\r\n");
}

/*
 * Convert a boolean variable to on / off.
 */
static char *
makeonoff(int n)
{
    switch (n) {
    case 0:
        return "off";
    case 1:
        return "on";
    default:
        return itoa(n, buff, 10);
    }
}


/*
 * Print the string contained in the buffer specified of len bytes.
 */
static void
outbuff(unsigned sseg, unsigned soff, unsigned len)
{
    char _far *p;
    unsigned l, i;
    char *s = buff;
    int hex = 0;
    static char hexnum[] = "0123456789abcdef";

    p = makefptr(sseg, soff);
    l = min(datalen, len);
    for (i = 0; i < l; i++)
        if (!isascii(p[i]))
            if (hexprint)
                hex = 1;
            else
                return;
    *s++ = '\t';
    if (hex) {
        *s++ = '{';
        for (i = 0; i < l; i++) {
            *s++ = '0';
            *s++ = 'x';
            *s++ = hexnum[(unsigned char)p[i]>>4];
            *s++ = hexnum[p[i] & 0xf];
            *s++ = ',';
        }
        if (l < len) { *s++ = '.'; *s++ = '.'; *s++ = '.';}
        *s++ = '}';
    } else {
        *s++ = '"';
        for (i = 0; i < l; i++)
            switch (p[i]) {
            case '\n': *s++ = '\\'; *s++ = 'n'; break;
            case '\t': *s++ = '\\'; *s++ = 't'; break;
            case '\b': *s++ = '\\'; *s++ = 'b'; break;
            case '\r': *s++ = '\\'; *s++ = 'r'; break;
            case '\f': *s++ = '\\'; *s++ = 'f'; break;
            case '\v': *s++ = '\\'; *s++ = 'v'; break;
            case '\a': *s++ = '\\'; *s++ = 'a'; break;
            default:
                if (iscntrl(p[i])) {
                    *s++ = '\\'; *s++ = 'x';
                    *s++ = hexnum[(unsigned)p[i]>>4];
                    *s++ = hexnum[p[i] & 0xf];
                } else
                    *s++ = p[i];
            }
        if (l < len) { *s++ = '.'; *s++ = '.'; *s++ = '.';}
        *s++ = '"';
    }
    *s++ = '\0';
    tputs(buff);
}


/*
 * Return executing programs PSP
 */
unsigned getpsp(void);
#pragma aux getpsp =    \
    "mov ah, 51h"       \
    "int 21h"           \
    value [bx];

/*
 * Set executing program's PSP
 */
void setpsp(unsigned psp);
#pragma aux setpsp =    \
    "mov ah, 50h"       \
    "int 21h"           \
    parm [bx];          \

/*
 * Return the DOS critical section flag pointer.
 */
char _far *getcritsectflag(void);
#pragma aux getcritsectflag =   \
    "mov ah, 34h"               \
    "int 21h"                   \
    value [es bx];

/*
 * Return the DOS Data Transfer Address (DTA)
 */
void _far *getdta(void);
#pragma aux getdta =    \
    "mov ah, 2fh"       \
    "int 21h"           \
    value [es bx];

/*
 * Convert a file mode number into a Unix ls -l like string.
 */
static char *
strmode(unsigned mode)
{
    char *p = buff;

    if (!stringprint)
        return "";
    *p++ = '[';
    if (mode & 0x20) *p++ = 'a'; else *p++ = '-';
    if (mode & 0x10) *p++ = 'd'; else *p++ = '-';
    if (mode & 0x08) *p++ = 'v'; else *p++ = '-';
    if (mode & 0x04) *p++ = 's'; else *p++ = '-';
    if (mode & 0x02) *p++ = 'h'; else *p++ = '-';
    if (mode & 0x01) *p++ = 'r'; else *p++ = '-';
    *p++ = ']';
    *p++ = '\0';
    return buff;
}

/*
 * Print the DTA contents as filled by the find first / find next functions
 */
static void
outdta(void)
{
    struct s_dta {
        char reserve[21];
        unsigned char mode;
        unsigned short time;
        unsigned short date;
        long size;
        char name[13];
    } _far *d;
    int psp;

    psp = getpsp();
    setpsp(tracedpsp);
    d = getdta();
    setpsp(psp);
    tprintf("ok\t(%-12Fs %10lu %2u/%2u/%2u %2u:%02u.%-2u %s)\r\n", d->name, d->size, (d->date >> 9) + 80, (d->date >> 5) & 0xf, d->date & 0x1f, d->time >> 11, (d->time >> 5) & 0x3f, d->time & 0x1f, strmode(d->mode));
}

char *errcodes[] = {
    "Error 0",
    "Invalid function code",
    "File not found",
    "Path not found",
    "Too many open files",
    "Access denied",
    "Invalid handle",
    "Memory control blocks destroyed",
    "Insufficient memory",
    "Invalid memory block address",
    "Invalid environment",
    "Invalid format",
    "Invalid access code",
    "Invalid data",
    "Reserved 14",
    "Invalid drive",
    "Attempt to remove current directory",
    "Not same device",
    "No more files",
    "Disk is write-protected",
    "Bad disk unit",
    "Drive not ready",
    "Invalid disk command",
    "CRC error",
    "Invalid length",
    "Seek error",
    "Not an MS-DOS disk",
    "Sector not found",
    "Out of paper",
    "Write fault",
    "Read fault",
    "General failure",
    "Sharing violation",
    "Lock violation",
    "Wrong disk",
    "FCB unavailable",
};

/*
 * Print error without newline
 */
static void
errprint(int err)
{
    if (worderror && err <= 35)
        tprintf("Error (%s)", errcodes[err]);
    else
        tprintf("Error (%u)", err);
}

/*
 * Print error with newline
 */
static void
errprintln(int err)
{
    if (worderror && err <= 35)
        tprintf("Error (%s)\r\n", errcodes[err]);
    else
        tprintf("Error (%u)\r\n", err);
}

/*
 * Print error or ok
 */
static void
okerrorproc(unsigned flags, unsigned ax)
{
    if (flags & 1)
        errprintln(ax);
    else
        tputs("ok\r\n");
}

/*
 * Print error
 */
static void
nerrorproc(unsigned flags, unsigned ax)
{
    if (flags & 1)
        errprintln(ax);
    else
        tprintf("%u\r\n", ax);
}

/*
 * Print memory allocation strategy
 */
static void
print_memory_strategy(unsigned str)
{
    if (str & 0x40)
        tputs("high memory ");
    else if (str & 0x80)
        tputs("high then low memory ");
    else
        tputs("low memory ");
    switch (str & 7) {
    case 0: tputs("first fit"); break;
    case 1: tputs("best fit"); break;
    case 2: tputs("last fit"); break;
    }
}

/*
 * Print date and time as packed in DOS format
 */
static void
print_dtim(int date, int time)
{
    tprintf("%2u/%2u/%2u %2u:%02u.%-2u",
        (date >> 9) + 80, (date >> 5) & 0xf, date & 0x1f,
        time >> 11, (time >> 5) & 0x3f, time & 0x1f);
}

/*
 * Print time of day.
 */
static void
print_time(void)
{
#if 0
    _strtime(buff); // can't be called with SS != DS
    buff[8] = ' ';
    buff[9] = '\0';
    tputs(buff);
#endif
}

struct s_func {
    char *name;
    unsigned count;
} funcs[256] =
{
    {"terminate", 0},       /* 00H  DOS1 - TERMINATE PROGRAM */
    {"key_in_echo", 0},     /* 01H  DOS1 - KEYBOARD INPUT WITH ECHO */
    {"disp_out", 0},        /* 02H  DOS1 - DISPLAY OUTPUT */
    {"serial_in", 0},       /* 03H  DOS1 - SERIAL INPUT */
    {"serial_out", 0},      /* 04H  DOS1 - SERIAL OUTPUT */
    {"printer_out", 0},     /* 05H  DOS1 - PRINTER OUTPUT */
    {"direct_out", 0},      /* 06H  DOS1 - DIRECT CONSOLE OUT */
    {"dir_key_in", 0},      /* 07H  DOS1 - DIRECT KEYBOARD INPUT */
    {"key_in", 0},          /* 08H  DOS1 - KEYBOARD INPUT WITHOUT ECHO */
    {"disp_string", 0},     /* 09H  DOS1 - DISPLAY STRING */
    {"buf_key_in", 0},      /* 0AH  DOS1 - BUFFERED KEYBOARD INPUT */
    {"chk_key_stat", 0},    /* 0BH  DOS1 - CHECK KEYBOARD STATUS */
    {"clr_key_func", 0},    /* 0CH  DOS1 - CLEAR KEY BUFFER AND PERFORM FUNCTION */
    {"flush", 0},           /* 0DH  DOS1 - DISK RESET */
    {"sel_drive", 0},       /* 0EH  DOS1 - SELECT CURRENT DRIVE */
    {"open_file", 0},       /* 0FH  DOS1 - OPEN FILE */
    {"close_file", 0},      /* 10H  DOS1 - CLOSE FILE */
    {"search_first", 0},    /* 11H  DOS1 - SEARCH FOR FIRST MATCHING FILE */
    {"search_next", 0},     /* 12H  DOS1 - SEARCH FOR NEXT MATCHING FILE */
    {"delete_file", 0},     /* 13H  DOS1 - DELETE FILE */
    {"rd_seq_rec", 0},      /* 14H  DOS1 - READ SEQUENTIAL RECORD */
    {"wr_seq_rec", 0},      /* 15H  DOS1 - WRITE SEQUENTIAL RECORD */
    {"create_file", 0},     /* 16H  DOS1 - CREATE FILE */
    {"rename_file", 0},     /* 17H  DOS1 - RENAME FILE */
    {"reserved 0x18", 0},   /* 18h */
    {"rd_cur_drive", 0},    /* 19H  DOS1 - REPORT CURRENT DRIVE */
    {"set_dta", 0},         /* 1AH  DOS1 - SET DISK TRANSFER AREA FUCNTION */
    {"rd_fat_1", 0},        /* 1BH  DOS1 - READ CURRENT DRIVE'S FAT */
    {"drive_info", 0},      /* 1CH  DOS1 - READ ANY DRIVE'S FAT */
    {"reserved 0x1d", 0},   /* 1dh */
    {"reserved 0x1e", 0},   /* 1eh */
    {"reserved 0x1f", 0},   /* 1fh */
    {"reserved 0x20", 0},   /* 20h */
    {"rd_ran_rec1", 0},     /* 21H  DOS1 - READ RANDOM FILE RECORD */
    {"wr_ran_rec1", 0},     /* 22H  DOS1 - WRITE RANDOM FILE RECORD */
    {"rd_file_size", 0},    /* 23H  DOS1 - REPORT FILE SIZE */
    {"set_ran_rec", 0},     /* 24H  DOS1 - SET RANDOM RECORD FIELD SIZE */
    {"set_int_vec", 0},     /* 25H  DOS1 - SET INTERRUPT VECTOR */
    {"create_seg", 0},      /* 26H  DOS1 - CREATE PROGRAM SEGMENT FUCNTION */
    {"rd_ran_rec2", 0},     /* 27H  DOS1 - READ RANDOM FILE RECORD */
    {"wr_ran_rec2", 0},     /* 28H  DOS1 - WRITE RANDOM FILE RECORD FUCNTION */
    {"parse_name", 0},      /* 29H  DOS1 - PARSE FILENAME */
    {"get_date", 0},        /* 2AH  DOS1 - GET DATE */
    {"set_date", 0},        /* 2BH  DOS1 - SET DATE */
    {"get_time", 0},        /* 2CH  DOS1 - GET TIME */
    {"set_time", 0},        /* 2DH  DOS1 - SET TIME */
    {"set_verify", 0},      /* 2EH  DOS1 - SET DISK WRITE VERIFICATION MODE */
    {"get_dta", 0},         /* 2FH  DOS2 - GET DISK TRANSFER AREA ADDRESS */
    {"get_ver", 0},         /* 30H  DOS2 - GET DOS VERSION NUMBER */
    {"keep", 0},            /* 31H  DOS2 - ADVANCED TERMINATE BUT STAY RESIDENT */
    {"get_dpt", 0},         /* 32H  DOS2 - GET POINTER TO DRIVE PARAMETER TABLE */
    {"cntrl_brk", 0},       /* 33H  DOS2 - GET/SET CONTROL BREAK STATUS */
    {"critical_flag", 0},   /* 34h */
    {"get_int_vec", 0},     /* 35H  DOS2 - GET INTERRUPT VECTOR */
    {"get_space", 0},       /* 36H  DOS2 - GET DISK FREE SPACE */
    {"switchar", 0},        /* 37h */
    {"get_country", 0},     /* 38H  DOS2 - GET COUNTRY INFORMATION */
    {"mkdir", 0},           /* 39H  DOS2 - MAKE DIRECTORY */
    {"rmdir", 0},           /* 3AH  DOS2 - REMOVE DIRECTORY */
    {"chdir", 0},           /* 3BH  DOS2 - CHANGE CURRENT DIRECTORY FUCNTION */
    {"create", 0},          /* 3CH  DOS2 - CREATE FILE */
    {"open", 0},            /* 3DH  DOS2 - OPEN FILE */
    {"close", 0},           /* 3EH  DOS2 - CLOSE FILE */
    {"read", 0},            /* 3FH  DOS2 - READ FILE/DEVICE */
    {"write", 0},           /* 40H  DOS2 - WRITE FILE/DEVICE */
    {"delete", 0},          /* 41H  DOS2 - DELETE FILE */
    {"lseek", 0},           /* 42H  DOS2 - MOVE FILE POINTER */
    {"chmod", 0},           /* 43H  DOS2 - CHANGE FILE MODE */
    {"ioctl", 0},           /* 44H  DOS2 - DEVICE I/O CONTROL */
    {"dup", 0},             /* 45H  DOS2 - DUPLICATE FILE HANDLE */
    {"cdup", 0},            /* 46H  DOS2 - FORCE FILE HANDLE DUPLICATION */
    {"get_dir", 0},         /* 47H  DOS2 - GET CURRENT DIRECTORY */
    {"allocate", 0},        /* 48H  DOS2 - ALLOCATE MEMORY */
    {"free", 0},            /* 49H  DOS2 - FREE MEMORY */
    {"set_block", 0},       /* 4AH  DOS2 - MODIFY ALLOCATED MEMORY BLOCK */
    {"exec", 0},            /* 4BH  DOS2 - LOAD/EXECUTE PROGRAM */
    {"term_proc", 0},       /* 4CH  DOS2 - TERMINATE PROCESS */
    {"get_code", 0},        /* 4DH  DOS2 - GET SUBPROGRAM RETURN CODE */
    {"find_first", 0},      /* 4EH  DOS2 - FIND FIRST FILE MATCH */
    {"find_next", 0},       /* 4FH  DOS2 - FIND NEXT FILE MATCH */
    {"set_psp", 0},         /* 50H  DOS2 - SET CURRENT PROCESS ID */
    {"get_psp", 0},         /* 51H  DOS2 - GET CURRENT PROCESS ID */
    {"sysvars", 0},         /* 52h GET LIST OF LISTS */
    {"gen_dpt", 0},         /* 53H  DOS2 - GENERATE DRIVE PARAMETER TABLE */
    {"get_verify", 0},      /* 54H  DOS2 - GET FILE WRITE VERIFY STATE */
    {"reserved 0x55", 0},   /* 55h */
    {"rename", 0},          /* 56H  DOS2 - RENAME FILE */
    {"date_time", 0},       /* 57H  DOS2 - GET/SET FILE DATE/TIME */
    {"alloc_strategy", 0},  /* 58h */
    {"get_err", 0},         /* 59H  DOS3 - GET EXTENDED RETURN CODE */
    {"create_temp", 0},     /* 5AH  DOS3 - CREATE TEMPORARY FILE */
    {"create_new", 0},      /* 5BH  DOS3 - CREATE NEW FILE */
    {"file_lock", 0},       /* 5CH  DOS3 - LOCK/UNLOCK FILE ACCESS */
    {"crit_err_info", 0},   /* 5DH  DOS3 - CRITICAL ERROR INFORMATION */
    {"machine_name", 0},    /* 5eh */
    {"assign_list", 0},     /* 5fh */
    {"truename", 0},        /* 60H  DOS3 - GET FULLY QUALIFIED FILE NAME */
    {"reserved 0x61", 0},   /* 61h */
    {"get_psp", 0},         /* 62H  DOS3 - GET PROGRAM SEGMENT PREFIX ADDRESS */
    {"get_dbcs_table", 0},  /* 63H  DOSV - GET LEAD BYTE TABLE */
    {"set_look_ahead", 0},  /* 64H  DOS3 - SET DEVICE DRIVER LOOK AHEAD */
    {"get_country_ext", 0}, /* 65H  DOS3 - GET EXTENDED COUNTRY INFORMATION */
    {"global_codepage", 0}, /* 66H  DOS3 - GET/SET GLOBAL CODE PAGE */
    {"set_handles", 0},     /* 67H  DOS3 - SET HANDLE COUNT */
    {"flush_handle", 0},    /* 68H  DOS3 - FLUSH BUFFER USING HANDLE */
};


/*
 * Print any prefixes specified by the user before the actual function call.
 */
static void
prefix(int fun, unsigned cs, unsigned ip)
{
    if (timeprint)
        print_time();
    if (pspprint)
        tprintf("%04x ", tracedpsp);
    if (numprint)
        tprintf("%02x ", fun);
    if (branchprint)
        tprintf("%04X:%04X ", cs, ip - 2);
}


/*
 * The new DOS interrupt handler - this is supposed to receive an union INTPACK
 * argument, but we cheat and break down the union into individual args passed
 * on the stack for easier manipulation.
 */
static void _interrupt _far _cdecl
dos_handler(
    unsigned _gs,
    unsigned _fs,
    unsigned _es,
    unsigned _ds,
    unsigned _di,
    unsigned _si,
    unsigned _bp,
    unsigned _sp,
    unsigned _bx,
    unsigned _dx,
    unsigned _cx,
    unsigned _ax,
    unsigned _ip,
    unsigned _cs,
    unsigned _flags)
{
    static int trace = 1;
    static int fun, funl;

    /* Filter out interrupts generated by us */
    if (!execed) {
        if (_ax == 0x4b00)      /* Exec */
            execed = 1;
        _chain_intr(old_dos_handler);
    }
    if (!trace || *critsectflag || *criterrflag)
        _chain_intr(old_dos_handler);
    trace = 0;
    tracedpsp = getpsp();
    if (tracetsr && tracedpsp != tsrpsp) {
        trace = 1;
        _chain_intr(old_dos_handler);
    }
    fun = _ax >> 8;
    if (count) {
        funcs[fun].count++;
        trace = 1;
        _chain_intr(old_dos_handler);
    }
    /* Set PSP for file output to work properly */
    setpsp(mypsp);
    if (someprefix)
        prefix(fun, _cs, _ip);
    switch (fun) {
    case 0x02:              /* disp_out */
        tprintf("display_char('%c')\r\n", _dx & 0xff);
        break;
    case 0x06:              /* direct_out */
        tprintf("direct_out('%c')\r\n", _dx & 0xff);
        break;
    case 0x09:              /* disp_string */
        if (stringprint)
            tprintf("display(\"%s\")\r\n", makestring(_ds, _dx));
        else
            tprintf("display(%04X:%04X)\r\n", _ds, _dx);
        break;
    case 0x0b:
        tputs("chk_key_stat() = ");
        break;
    case 0x0d:
        tputs("flush()\r\n");
        break;
    case 0x0e:              /* set_current_disk */
        tprintf("set_current_disk(%c:) = ", (_dx & 0xff) < 27 ? (_dx & 0xff) + 'A' : '.');
        break;
    case 0x19:              /* get_current_disk */
        break;
    case 0x1a:              /* set_dta */
        tprintf("set_dta(%04X:%04X)\r\n", _ds, _dx);
        break;
    case 0x1c:              /* drive information */
        tprintf("drive_info(%d) = ", _dx & 0xff);
        break;
    case 0x25:              /* set_vector */
        tprintf("set_vector(%#x, %04X:%04X)\r\n", _ax & 0xff, _ds, _dx);
        break;
    case 0x29:              /* parse_name */
        tprintf("parse_name(%04X:%04X, %d, %04X:%04X) = ",
            _ds, _si, _ax & 0xff, _es, _di);
        break;
    case 0x2a:              /* get_date */
    case 0x2c:              /* get_time */
        break;
    case 0x2d:              /* set_time */
        tprintf("set_time(%02d:%02d:%02d.%d) = ", _cx >> 8, _cx & 0xff, _dx >> 8, _dx & 0xff);
        break;
    case 0x2f:              /* get_dta */
    case 0x30:              /* get_version */
        break;
    case 0x33:              /* cntrl_brk */
        switch (funl = (_ax & 0xff)) {
        case 0:
            break;
        case 1:
            tprintf("set_break(%s)\r\n", makeonoff(_dx & 0xff));
            break;
        case 2:
            tprintf("getset_break(%s) = ", makeonoff(_dx & 0xff));
            break;
        default:
            goto aka_default;
        }
        break;
    case 0x34:              /* critical_flag */
        break;
    case 0x35:              /* get_vector */
        tprintf("get_vector(%#x) = ", _ax & 0xff);
        break;
    case 0x39:              /* Mkdir */
        tprintf("mkdir(\"%Fs\") = ", makefptr(_ds, _dx));
        break;
    case 0x3a:              /* Rmdir */
        tprintf("rmdir(\"%Fs\") = ", makefptr(_ds, _dx));
        break;
    case 0x3b:              /* Chdir */
        tprintf("chdir(\"%Fs\") = ", makefptr(_ds, _dx));
        break;
    case 0x3c:              /* Creat */
        tprintf("creat(\"%Fs\", %02x%s) = ", makefptr(_ds, _dx), _cx, strmode(_cx));
        break;
    case 0x3d:              /* Open */
        tprintf("open(\"%Fs\", %d) = ", makefptr(_ds, _dx), _ax & 0xff);
        break;
    case 0x3e:              /* Close */
        tprintf("close(%u) = ", _bx);
        break;
    case 0x3f:              /* Read */
        tprintf("read(%u, %04X:%04X, %u) = ", _bx, _ds, _dx, _cx);
        break;
    case 0x40:              /* Write */
        tprintf("write(%u, %04X:%04X, %u) = ", _bx, _ds, _dx, _cx);
        break;
    case 0x41:              /* Unlink */
        tprintf("unlink(\"%Fs\") = ", makefptr(_ds, _dx));
        break;
    case 0x42:              /* Lseek */
        tprintf("lseek(%u, %ld, %d) = ",_bx, makelong(_cx, _dx), _ax & 0xff);
        break;
    case 0x43:              /* Chmod / getmod */
        switch (funl = (_ax & 0xff)) {
        case 0:
            tputs("getmod(\"");
            break;
        case 1:
            tputs("chmod(\"");
            break;
        default:
            goto aka_default;
        }
        tprintf("%Fs", makefptr(_ds, _dx));
        if ((_ax & 0xff) == 1)
            tprintf("\", %#x%s", _cx, strmode(_cx));
        tputs(") = ");
        break;
    case 0x44:              /* ioctl */
        switch (funl = (_ax & 0xff)) {
        default:
            goto aka_default;
        case 0x00:          /* get device info */
            tprintf("ioctl(GET_DEV_INFO, %d) = ", _bx);
            break;
        case 0x02:          /* read from chdev control */
            tprintf("ioctl(READ_CHDEV_CCHAN, %d, %u, %04X:%04X) = ",
                _bx, _cx, _ds, _dx);
            break;
        case 0x07:          /* get output status */
            tprintf("ioctl(GET_OUTPUT_STATUS, %d) = ", _bx);
            break;
        case 0x08:          /* check if removable */
            tprintf("ioctl(BD_ISREMOVABLE, %d) = ", _bx & 0xff);
            break;
        case 0x09:          /* check if remote */
            tprintf("ioctl(BD_ISREMOTE, %d) = ", _bx & 0xff);
            break;
        case 0x0a:          /* check if remote */
            tprintf("ioctl(HAN_ISREMOTE, %d) = ", _bx);
            break;
        case 0x0e:          /* get logical map */
            tprintf("ioctl(GET_LMAP, %d) = ", _bx & 0xff);
            break;
        case 0x0f:          /* set logical map */
            tprintf("ioctl(SET_LMAP, %d) = ", _bx & 0xff);
            break;
        }
        break;
    case 0x45:              /* Dup */
        tprintf("dup(%u) = ", _bx);
        break;
    case 0x46:              /* Dup2 */
        tprintf("dup2(%u, %u) = ", _bx, _cx);
        break;
    case 0x47:              /* Getcwd */
        tprintf("getcwd(%d, %04X:%04X) = ", _dx & 0xff, _ds, _si);
        break;
    case 0x48:              /* Alloc */
        tprintf("alloc(%#x0)= ", _bx);
        break;
    case 0x49:              /* Free */
        tprintf("free(%04X:0000) = ", _es);
        break;
    case 0x4a:              /* Realloc */
        tprintf("realloc(%04X:0000, %#x0) = ", _es, _bx);
        break;
    case 0x4b:              /* Exec */
        switch (funl = (_ax & 0xff)) {
        case 0:
            tprintf("exec(\"%Fs", makefptr(_ds, _dx));
            if (tracechildren)
                tputs("\") = ...\r\n");
            else
                tputs("\") = ");
            if (tracechildren) {
                setpsp(tracedpsp);
                trace = 1;
                _chain_intr(old_dos_handler);
            }
            break;
        default:
            tprintf("exec(%d,\"%Fs) = ", funl, makefptr(_ds, _dx));
        }
        break;
    case 0x4c:              /* Exit */
        tprintf("exit(%d)\r\n", _ax & 0xff);
        if (tracechildren) {
            setpsp(tracedpsp);
            trace = 1;
            _chain_intr(old_dos_handler);
        }
        break;
    case 0x4d:              /* Get return code */
        break;
    case 0x4e:              /* Findfirst */
        tprintf("findfirst(\"%Fs\", %#x%s) = ", makefptr(_ds, _dx), _cx, strmode(_cx));
        break;
    case 0x4f:              /* Findnext */
        tputs("findnext() = ");
        break;
    case 0x50:              /* set_psp */
        tprintf("set_psp(%04X)\r\n", _bx);
        break;
    case 0x51:              /* get_psp */
        break;
    case 0x52:              /* sysvars */
        tputs("sysvars() = ");
        break;
    case 0x57:              /* get/set date time */
        switch (funl = (_ax & 0xff)) {
        case 0:             /* get time */
            tprintf("get_time(%d) = ", _bx);
            break;
        case 1:             /* set time */
            tprintf("set_time(%d, ");
            print_dtim(_dx, _cx);
            tputs(") = ");
            break;
        default:
            goto aka_default;
        }
        break;
    case 0x58:              /* memory allocation */
        switch (funl = (_ax & 0xff)) {
        case 0:             /* get strategy */
            tputs("get_alloc_str() = ");
            break;
        case 1:             /* set strategy */
            tputs("set_alloc_str(");
            print_memory_strategy(_bx & 0xff);
            tputs(") = ");
            break;
        case 2:             /* get umb state */
            tputs("get_umb_state() = ");
            break;
        case 3:             /* set umb state */
            tprintf("set_umb_state(%d) = ", _bx);
            break;
        default:
            goto aka_default;
        }
        break;
    case 0x5b:              /* Creat new */
        tprintf("creat_new(\"%Fs\", %02x%s) = ", makefptr(_ds, _dx),
             _cx, strmode(_cx));
        break;
    case 0x62:              /* get_psp */
        break;
    case 0x55:              /* Create child PSP */
        tprintf("child_psp(%04X, %04X) = ", _dx, _si);
        break;
    case 0x5a:              /* Tmpfile */
        tprintf("tmpfile(\"%Fs\", %#x) = ", makefptr(_ds, _dx),
            _cx & 0xff);
        break;
    case 0x68:              /* Flush handle */
        tprintf("flush_handle(%u) = ", _bx);
        break;
    case 0x6c:              /* Open */
        tprintf("open4(\"%Fs\", %d, %s, %d) = ", makefptr(_ds, _si), _bx, strmode(_cx), _dx);
        break;

    aka_default:            /* All unknown functions arrive here */
    default:
        if (otherprint) {
            if (nameprint && fun <= 0x68)
                tputs(funcs[fun].name);
            else
                tprintf("%02x", fun);
            if (regdump)
                tprintf(" :ax=%04X bx=%04X cx=%04X dx=%04X si=%04X di=%04X ds=%04X es=%04X\r\n", _ax, _bx, _cx, _dx, _si, _di, _ds, _es);
            else
                tputs("\r\n");
        }
        setpsp(tracedpsp);
        trace = 1;
        _chain_intr(old_dos_handler);
    }
    /*
     * This is the best place to flush the file.
     * We have printed the function and its parameters, so if the
     * systems crashes the information will be saved.
     */
    if (append) {
        close(fd);
        if ((fd = open(fname, O_TEXT | O_APPEND | O_WRONLY, 0666)) == -1) {
            fd = 1;
            tprintf("ERROR %s \r\n", sys_errlist[errno]);
        }
        lseek(fd, 0l, SEEK_END);
    }
    /*
     * Call the DOS interrupt.  Transfer all function variables to the
     * machine registers, call DOS and transfer the machine registers
     * back to the function variables.
     */
    setpsp(tracedpsp);
    _asm {
        pushf
        push ds
        mov ax, _flags
        push ax
        popf
        mov ax, _es
        mov es, ax
        mov ax, _ds
        mov ds, ax
        mov ax, _ax
        mov bx, _bx
        mov cx, _cx
        mov dx, _dx
        mov si, _si
        mov di, _di
        int 21h
        mov _ax, ax
        mov _bx, bx
        mov _cx, cx
        mov _dx, dx
        mov _si, si
        mov _di, di
        mov ax, es
        mov _es, ax
        mov ax, ds
        mov es, ax
        pushf
        pop ax
        mov _flags, ax
        pop ds
        popf
    }
    setpsp(mypsp);
    switch (fun) {
    case 0x02:              /* disp_out */
    case 0x06:              /* direct_out */
    case 0x09:              /* disp_string */
    case 0x0d:              /* flush */
        break;
    case 0x0b:              /* chk_key_stat */
        if (!(_ax & 0xff))
            tputs("not ");
        tputs("available\r\n");
        break;
    case 0x0e:              /* set_current_disk */
        tprintf("%d\r\n", _ax & 0xff);
        break;
    case 0x19:              /* get_current_disk */
        tprintf("get_current_disk() = %c:\r\n", (_ax & 0xff) + 'A');
        break;
    case 0x1a:              /* set_dta */
        break;
    case 0x1c:              /* drive information */
        tprintf("s/c=%u b/s=%u nc=%u ty=", _ax & 0xff, _cx, _dx);
        switch (*(unsigned char *)makefptr(_ds, _bx)) {
        case 0xff: tputs("320K\r\n"); break;
        case 0xfe: tputs("160K\r\n"); break;
        case 0xfd: tputs("360K\r\n"); break;
        case 0xfc: tputs("180K\r\n"); break;
        case 0xf9: tputs("1.2M\r\n"); break;
        case 0xf8: tputs("HD\r\n"); break;
        default: tputs("other\r\n"); break;
        }
        break;
    case 0x25:              /* set_vector */
        break;
    case 0x29:              /* parse_name */
        tprintf("%d (%04X:%04X)\r\n", _ax & 0xff, _ds, _si);
        break;
    case 0x2a:              /* get_date */
        tprintf("get_date() = %2u/%2u/%2u (%s)\r\n", _cx, _dx >> 8,  _dx & 0xff, weekday(_ax & 0xff));
        break;
    case 0x2c:              /* get_time */
        tprintf("get_time() = %02d:%02d:%02d.%d\r\n", _cx >> 8, _cx & 0xff, _dx >> 8, _dx & 0xff);
        break;
    case 0x2d:              /* set_time */
        if ((_ax & 0xff) == 0)
            tputs("ok\r\n");
        else
            tputs("invalid\r\n");
        break;
    case 0x2f:              /* get_dta */
        tprintf("get_dta() = %04X:%04X\r\n", _es, _bx);
        break;
    case 0x30:              /* get_version */
        tprintf("get_version() = %u.%u\r\n", _ax & 0xff, _ax >> 8);
        break;
    case 0x33:              /* cntrl_brk */
        switch (funl) {
        case 0:
            tprintf("get_break() = %s\r\n", makeonoff(_dx & 0xff));
            break;
        case 1:
            break;
        case 2:
            tprintf("%s\r\n", makeonoff(_dx & 0xff));
            break;
        }
        break;
    case 0x34:              /* critical_flag */
        tprintf("critical_flag() = %04X:%04X\r\n", _es, _bx);
        break;
    case 0x35:              /* get_vector */
        tprintf("%04X:%04X\r\n", _es, _bx);
        break;
    case 0x39:              /* Mkdir */
    case 0x3a:              /* Rmdir */
    case 0x3b:              /* Chdir */
        okerrorproc(_flags, _ax);
        break;
    case 0x3c:              /* Creat */
    case 0x3d:              /* Open */
        nerrorproc(_flags, _ax);
        break;
    case 0x3e:              /* Close */
        okerrorproc(_flags, _ax);
        break;
    case 0x3f:              /* Read */
    case 0x40:              /* Write */
        if (_flags & 1)
            errprint(_ax);
        else
            tprintf("%u", _ax);
        if (stringprint)
            outbuff(_ds, _dx, _ax);
        tputs("\r\n");
        break;
    case 0x41:              /* Unlink */
        okerrorproc(_flags, _ax);
        break;
    case 0x42:              /* Lseek */
        if (_flags & 1)
            errprintln(_ax);
        else
            tprintf("%ld\r\n", makelong(_dx, _ax));
        break;
    case 0x43:              /* Chmod / getmod */
        if (_flags & 1)
            errprint(_ax);
        else {
            if ((_ax & 0xff) == 0)
                tprintf("%u%s", _cx, strmode(_cx));
            else
                tputs("ok");
        }
        tputs("\r\n");
        break;
    case 0x44:              /* ioctl */
        switch (funl) {
        case 0x00:          /* get device info */
            if (_flags & 1)
                errprintln(_ax);
            else {
                if (stringprint)
                    devinfo(_dx);
                else
                    tprintf("%04X\r\n", _dx);
            }
            break;
        case 0x02:          /* read from chdev control */
            if (_flags & 1)
                errprint(_ax);
            else
                tprintf("%u", _ax);
            if (stringprint)
                outbuff(_ds, _dx, _ax);
            tputs("\r\n");
            break;
        case 0x07:          /* get output status */
            if (_flags & 1)
                errprint(_ax);
            else
                tputs((_ax & 0xff) ? "NOT_READY" : "READY");
            tputs("\r\n");
            break;
        case 0x08:          /* check if removable */
            if (_flags & 1)
                errprint(_ax);
            else
                tputs(_ax ? "FIXED" : "REMOVABLE");
            tputs("\r\n");
            break;
        case 0x09:          /* check if remote */
            if (_flags & 1)
                errprint(_ax);
            else {
                tputs((_dx & 0x8000) ? "SUBST " : "NO_SUBST ");
                tputs((_dx & 0x1000) ? "REMOTE " : "LOCAL ");
                tputs((_dx & 0x0200) ? "NOIO" : "IO");
            }
            tputs("\r\n");
            break;
        case 0x0a:          /* check if remote */
            if (_flags & 1)
                errprint(_ax);
            else
                tputs((_dx & 0x8000) ? "REMOTE" : "LOCAL");
            tputs("\r\n");
            break;
        case 0x0e:          /* get logical map */
            if (_flags & 1)
                errprint(_ax);
            else
                tprintf("%d", _ax & 0xff);
            tputs("\r\n");
            break;
        case 0x0f:          /* set logical map */
            okerrorproc(_flags, _ax);
            break;
        }
        break;
    case 0x45:              /* Dup */
        nerrorproc(_flags, _ax);
        break;
    case 0x46:              /* Dup2 */
        okerrorproc(_flags, _ax);
        break;
    case 0x47:              /* Getcwd */
        if (_flags & 1)
            errprintln(_ax);
        else {
            if (stringprint)
                tprintf("ok\t\"%Fs\"\r\n", makefptr(_ds, _si));
            else
                tputs("ok\r\n");
        }
        break;
    case 0x48:              /* Alloc */
        if (_flags & 1) {
            errprint(_ax);
            tprintf("\t(free = %#x0 bytes)\r\n", _bx);
        } else
            tprintf("%04X:0000\r\n", _ax);
        break;
    case 0x49:              /* Free */
        okerrorproc(_flags, _ax);
        break;
    case 0x4a:              /* Realloc */
        if (_flags & 1) {
            errprint(_ax);
            tprintf("\t(free = %#x0 bytes)\r\n", _bx);
        } else
            tputs("ok\r\n");
        break;
    case 0x4b:              /* Exec */
        okerrorproc(_flags, _ax);
        break;
    case 0x4c:              /* Exit */
        /* NOTREACHED */
        break;
    case 0x4d:              /* Get return code */
        tprintf("get_code() = %d %d\r\n", _ax >> 8, _ax & 0xff);
        break;
    case 0x4e:              /* Findfirst */
        if (_flags & 1)
            errprintln(_ax);
        else {
            if (stringprint)
                outdta();
            else
                tputs("ok\r\n");
        }
        break;
    case 0x4f:              /* Findnext */
        if (_flags & 1)
            errprintln(_ax);
        else {
            if (stringprint)
                outdta();
            else
                tputs("ok\r\n");
        }
        break;
    case 0x50:              /* set_psp */
        tracedpsp = _bx;
        break;
    case 0x52:              /* sysvars */
        tprintf("%04X:%04X\r\n", _es, _bx);
        break;
    case 0x51:              /* get_psp */
    case 0x62:              /* get_psp */
        tprintf("get_psp() = %04X\r\n", _bx);
        break;
    case 0x55:              /* Create child PSP */
        if (_flags & 1)
            errprintln(_ax);
        else {
            tputs("ok\r\n");
            tracedpsp = _dx;
        }
        break;
    case 0x57:              /* get/set date time */
        switch (funl = (_ax & 0xff)) {
        case 0:             /* get time */
            if (_flags & 1)
                errprint(_ax);
            else
                print_dtim(_dx, _cx);
            tputs("\r\n");
            break;
        case 1:             /* set time */
            okerrorproc(_flags, _ax);
            break;
        }
        break;
    case 0x58:              /* memory allocation */
        switch (funl) {
        case 0:             /* get strategy */
            if (_flags & 1)
                errprint(_ax);
            else
                print_memory_strategy(_ax);
            tputs("\r\n");
            break;
        case 1:             /* set strategy */
            okerrorproc(_flags, _ax);
            break;
        case 2:             /* get umb state */
            nerrorproc(_flags, _ax & 0xff);
            break;
        case 3:             /* set umb state */
            okerrorproc(_flags, _ax);
            break;
        }
        break;
    case 0x5a:              /* Tmpfile */
        nerrorproc(_flags, _ax);
        break;
    case 0x5b:              /* Creat new */
        nerrorproc(_flags, _ax);
        break;
    case 0x68:              /* Flush handle */
        nerrorproc(_flags, _ax);
        break;
    case 0x6c:              /* Open 4 */
        if (_flags & 1)
            errprint(_ax);
        else {
            tprintf("%u ", _ax);
            if (stringprint)
                switch (_cx) {
                case 0: tputs("(OPEN)"); break;
                case 1: tputs("(CREAT)"); break;
                case 2: tputs("(TRUNC)"); break;
                }
        }
        tputs("\r\n");
    }
    setpsp(tracedpsp);
    trace = 1;
}

/*
 * Restore the previous handler.
 * Called by signal handlers.
 */
void
restore_handler(int sig)
{
    /* Restore old handler */
    _dos_setvect(DOS_INT, old_dos_handler);
}

/*
 * Main program starts here.
 */
int
main(int argc, char *argv[])
{
    int status = 0;
    extern int optind;
    extern char *optarg;
    int errflag = 0;
    char *usagestring = "usage: %s [-o fname] [-l len] [-help] [-abcfinrstvwxy] [-p psp] [command options ...]\n";
    int c;
    static char revstring[] = "$Revision: 1.28 $", revision[30];
    char *p;

    strcpy(revision, strchr(revstring, ':') + 2);
    *strchr(revision, '$') = '\0';
    strlwr(argv[0]);
    if (p = strrchr(argv[0], '\\'))
        argv[0] = p + 1;
    if (p = strrchr(argv[0], '/'))
        argv[0] = p + 1;
    if (p = strrchr(argv[0], '.'))
        *p = '\0';
    if (!*argv[0])
        argv[0] = "trace";
    while ((c = getopt(argc, argv, "abfo:h:sxl:nitrevcp:wy")) != EOF)
        switch (c) {
        case 'i':           /* Print PSP */
            pspprint = 1;
            break;
        case 'p':           /* Trace with given PSP */
            if (optarg) {
                tsrpsp = atoi(optarg);
                tracetsr = 1;
            } else {
                fprintf(stderr, "%s: -p needs a PSP parameter\n", argv[0]);
                errflag = 1;
            }
            break;
        case 'e':           /* Trace children */
            tracechildren = 1;
            break;
        case 'v':           /* Verbose */
            branchprint = pspprint = worderror = numprint = timeprint = tracechildren = regdump = stringprint = hexprint = otherprint = nameprint = 1;
            break;
        case 'f':           /* Print function number */
            numprint = 1;
            break;
        case 'b':           /* Print interrupt branch address */
            branchprint = 1;
            break;
        case 't':           /* Print time of each call */
            timeprint = 1;
            break;
        case 'r':           /* Dump registers */
            regdump = 1;
            break;
        case 'o':           /* Output file */
            if (optarg)
                fname = optarg ;
            else {
                fprintf(stderr, "%s: -o needs a file parameter\n", argv[0]);
                errflag = 1;
            }
            break ;
        case 'w':           /* Print errors as words */
            worderror = 1;
            break;
        case 'c':           /* Produce summary count */
            count = 1;
            break;
        case 's':           /* Print strings in I/O */
            stringprint = 1;
            break;
        case 'y':           /* Close file after every write */
            append = 1;
            break;
        case 'x':           /* Print binary data in I/O */
            hexprint = 1;
            break;
        case 'a':           /* Print all DOS functions */
            otherprint = 1;
            break;
        case 'n':           /* Print names of other DOS functions */
            nameprint = 1;
            break;
        case 'l':           /* Specify length */
            if (optarg) {
                datalen = atoi(optarg);
                if (datalen >= MAXBUFF) {
                    fprintf(stderr, "%s: Data length %u is greater than maximum length %u.  %u used.\n", argv[0], datalen, MAXBUFF - 1, MAXBUFF - 1);
                    datalen = MAXBUFF - 1;
                }
            } else {
                fprintf(stderr, "%s: -l needs a length parameter\n", argv[0]);
                errflag = 1;
            }
            break;
        case 'h':           /* Help */
            fprintf(stdout, "Trace Version %s (C) Copyright 1991-1994 D. Spinellis.  All rights reserved.\n", revision);
            fprintf(stdout, usagestring, argv[0]);
            fputs("-a\tTrace all DOS functions\n", stdout);
            fputs("-b\tPrint interrupt branch address\n", stdout);
            fputs("-c\tProduce a count summary only\n", stdout);
            fputs("-e\tTrace across exec calls\n", stdout);
            fputs("-f\tPrefix calls with function number\n", stdout);
            fputs("-h\tPrint this list\n", stdout);
            fputs("-i\tPrefix calls with process id (psp address)\n", stdout);
            fputs("-l L\tSpecify I/O printed data length\n", stdout);
            fputs("-n\tPrint other functions by name\n", stdout);
            fputs("-o F\tSpecify output file name\n", stdout);
            fputs("-p P\tTrace resident process with PSP P\n", stdout);
            fputs("-r\tDump registers on other functions\n", stdout);
            fputs("-s\tPrint I/O strings\n", stdout);
            fputs("-t\tPrefix calls with time\n", stdout);
            fputs("-v\tVerbose (-abefinrstwx)\n", stdout);
            fputs("-w\tPrint errors as words\n", stdout);
            fputs("-x\tPrint I/O binary data (needs -s)\n", stdout);
            fputs("-y\tClose file after everY write\n", stdout);
            return 0;
        case '?':           /* Error */
            errflag = 1;
            break ;
        }
    if (errflag) {
        fprintf(stderr, usagestring, argv[0]);
        return 2;
    }
    if ((fd = open(fname, O_CREAT | O_TRUNC | O_TEXT | O_WRONLY, 0666)) == -1) {
        perror(fname);
        return 1;
    }
    someprefix = pspprint | timeprint | numprint | branchprint;
    mypsp = getpsp();
    critsectflag = getcritsectflag();
    if (_osmajor == 2)
        criterrflag = critsectflag + 1;
    else
        criterrflag = critsectflag - 1;
    /* Save old handler and install new one */
    old_dos_handler = _dos_getvect(DOS_INT);
    (void)signal(SIGABRT, restore_handler);
    (void)signal(SIGINT, restore_handler);
    (void)signal(SIGTERM, restore_handler);
    _dos_setvect(DOS_INT, (void (__interrupt *)())dos_handler);
    if (tracetsr) {
        printf("Tracing process with psp %d (0x%04x)\n", tsrpsp, tsrpsp);
        puts("Press any key to exit.");
        execed = 1;
        (void)_bios_keybrd(_KEYBRD_READ);
    } else if (optind != argc) {
        status = spawnvp(P_WAIT, argv[optind], (const char * const *)(argv + optind));
    } else {
        puts("Tracing system activity.\nPress any key to exit.");
        execed = 1;
        (void)_bios_keybrd(_KEYBRD_READ);
    }
    execed = 0;
    restore_handler(SIGTERM);
    if (count)
        for (c = 0; c < 256; c++)
            if (funcs[c].count)
                tprintf("%02X %20s %10u\r\n", c, funcs[c].name ? funcs[c].name : "???", funcs[c].count);

    close(fd);
    if (status == -1) {
        perror(argv[optind]);
        return 1;
    } else
        return 0;
}
