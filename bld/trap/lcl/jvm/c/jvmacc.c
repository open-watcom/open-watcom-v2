/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <windows.h>
#include "trpimp.h"
#include "javadbg.h"
#include <malloc.h>
#include <string.h>
#include "madregs.h"
#include "javaname.h"
#include "jvmerr.h"


ClassClass              *cbCallJava;
HANDLE                  EventSem;
HANDLE                  EventDoneSem;
int                     LastClassGiven; // 1 based index of last class returned
int                     AppsLoaded;

trap_conditions         Conditions;

typedef struct brk {
    struct brk  *next;
    char        saved;
    unsigned    pc;
} brk;
brk *BPList = NULL;
brk *StartBPList = NULL;

#define MAX_THREADS     256

Hjava_lang_Thread *Threads[MAX_THREADS];
sys_thread_t *SysThreads[MAX_THREADS];
Hjava_lang_ThreadGroup *ThreadGroup;

static Hsun_tools_debug_BreakpointQueue *TheBkptQ = 0;

int CurrThreadIdx;
int NumThreads;
#define CurrThread ( ( CurrThreadIdx == -1 ) \
                   ? ( NULL ) \
                   : ((Classjava_lang_Thread*)unhand( Threads[CurrThreadIdx] )) )

#define OP_BREAK        "\xCA"

#define EventDone()     ReleaseSemaphore( EventDoneSem, 1, NULL )
#define Event( i )      Conditions = i; ReleaseSemaphore( EventSem, 1, NULL )
#define WaitForEvent()  WaitForSingleObject( EventSem, INFINITE )
#define WaitEventDone() WaitForSingleObject( EventDoneSem, INFINITE )

#define JS( x )         makeJavaString( x, strlen( x ) )
ClassClass                              *CbMain;

static void ThreadName( Classjava_lang_Thread* trd, char *buff, size_t len )
/**************************************************************************/
{
    char        *p;
    unicode     *src;

    src = unhand( trd->name )->body;
    len = obj_length( trd->name );
    p = buff;
    while( *src != '\0' && len-- > 0 ) {
        *p++ = *src++;
    }
    *p = '\0';
}

static void TheBigSleep()
/***********************/
{
    for( ;; ) {
        Sleep( 1000 );
    }
}

unsigned DoRead( int addr, char *buff, size_t len )
/*************************************************/
{
    DWORD               bytes;

    ReadProcessMemory( GetCurrentProcess(), (LPVOID)addr, buff,
                        len, (LPDWORD)&bytes );
    return( bytes );
}

unsigned DoWrite( int addr, char *buff, size_t len )
/**************************************************/
{
    DWORD               bytes;

    WriteProcessMemory( GetCurrentProcess(), (LPVOID)addr, buff,
                        len, (LPDWORD)&bytes );
    return( bytes );
}

void AddStartingBreakpoint( unsigned pc )
/***************************************/
{
    brk         *new;

    new = LocalAlloc( LMEM_FIXED, sizeof( *new ) );
    new->next = StartBPList;
    StartBPList = new;
    new->pc = pc;
    DoRead( pc, &new->saved, 1 );
    DoWrite( pc, OP_BREAK, 1 );
}

long * getclassvariable(struct ClassClass *cb, char *fname)
/*********************************************************/
{
    int    n = cb->fields_count;
    struct fieldblock *fb;

    for (fb = cbFields(cb); --n >= 0; fb++)
    if ((fb->access & ACC_STATIC) && strcmp(fname, fieldname(fb)) == 0) {
        char *signature = fieldsig(fb);
        if (signature[0] == SIGNATURE_LONG ||
            signature[0] == SIGNATURE_DOUBLE)
            return (long *)twoword_static_address(fb);
        else
            return (long *)normal_static_address(fb);
    }
    return (long *)0;
}

void ResumeThreads()
/******************/
{
    int i;

    for( i = 0; i < NumThreads; ++i ) {
        if( SysThreads[i] && SysThreads[i]->handle ) {
            ResumeThread( SysThreads[i]->handle );
        }
    }
}

void SuspendThreads()
/*******************/
{
    int i;

    for( i = 0; i < NumThreads; ++i ) {
        SysThreads[i] = SYSTHREAD(Threads[i]);
        if( Threads[i] == unhand(TheBkptQ)->thread ) {
            CurrThreadIdx = i;
        }
        if( SysThreads[i] && SysThreads[i]->handle ) {
            SuspendThread( SysThreads[i]->handle );
        }
    }
}

trap_conditions HandleBreak( void )
/*********************************/
{
    trap_conditions conditions;
    brk             **owner, *bp;

    conditions = COND_TRACE;
    for( owner = &StartBPList; *owner != NULL; owner = &(bp->next) ) {
        bp = *owner;
        if( bp->pc == unhand(TheBkptQ)->pc ) {
            unhand(TheBkptQ)->opcode = bp->saved;
            DoWrite( bp->pc, &bp->saved, 1 );
            *owner = bp->next;
            LocalFree( bp );
            break;
        }
    }
    for( bp = BPList; bp != NULL; bp = bp->next ) {
        DoWrite( bp->pc, &bp->saved, 1 );
        if( bp->pc == unhand(TheBkptQ)->pc ) {
            DoWrite( bp->pc, &bp->saved, 1 );
            unhand(TheBkptQ)->opcode = bp->saved;
            conditions = COND_BREAK;
        }
    }
    return( conditions );
}

#pragma aux (cdecl) BreakpointLoop "Java_sun_tools_debug_jvmhandler_BreakpointLoop_stub" export
void BreakpointLoop( stack_item *p, ExecEnv *ee )
/******************************************************/
{
    Hsun_tools_debug_BreakpointQueue **bkptQ;
    ClassClass                  *jvmhandler_class;
    ClassClass                  *bkpthandler_class;
    Classjava_lang_ThreadGroup *tg;
    ArrayOfObject               *threads;
    char                        buff[256];
    int                         i;
    trap_conditions             conditions;
    int                         num_threads;
    int                         foundBPThread;

    ThreadGroup = (Hjava_lang_ThreadGroup *)p[0].h;
    jvmhandler_class = FindClass(ee, "sun/tools/debug/jvmhandler", TRUE);
    /* Find the static variable that contains the queue */
    bkptQ =(Hsun_tools_debug_BreakpointQueue **) getclassvariable(jvmhandler_class, "bkptQ");
    TheBkptQ = *bkptQ;

    bkpthandler_class = FindClass(ee, "sun/tools/debug/BreakpointHandler", TRUE);
    bkptQ =(Hsun_tools_debug_BreakpointQueue **) getclassvariable(bkpthandler_class, "the_bkptQ");
    *bkptQ = TheBkptQ;

    monitorEnter(obj_monitor(TheBkptQ));
    for( ;; ) {
        monitorWait(obj_monitor(TheBkptQ),INFINITE);
        if( unhand(TheBkptQ)->exception == NULL ) {
            conditions = HandleBreak();
        } else {
            conditions = 0;
            // nyi - catching exceptions???
        }
        unhand(TheBkptQ)->updated = TRUE;
        tg = unhand(ThreadGroup);
        threads  = unhand((HArrayOfObject*)tg->threads);
        num_threads = obj_length(tg->threads);
        // enumerate all threads
        NumThreads = 0;
        CurrThreadIdx = -1;
        foundBPThread = FALSE;
        for( i = 0; i < num_threads; ++i ) {
            Threads[NumThreads] = (Hjava_lang_Thread*)threads->body[i];
            if( Threads[NumThreads] == NULL )
                continue;
            ThreadName( unhand( Threads[NumThreads] ), buff, sizeof( buff ) );
            if( memcmp( buff, "jvmhelp", sizeof( "jvmhelp" )-1 ) == 0 )
                continue;
            if( Threads[NumThreads] == unhand(TheBkptQ)->thread ) {
                foundBPThread = TRUE;
            }
            ++NumThreads;
        }

        if( !foundBPThread ) {
            Threads[NumThreads] = unhand(TheBkptQ)->thread;
            ThreadName( unhand( Threads[NumThreads] ), buff, sizeof( buff ) );
            ++NumThreads;
        }

        SuspendThreads();
        Event( conditions );
        WaitEventDone();
        ResumeThreads();
    }
}

#pragma aux (cdecl) HandleExit;
void HandleExit( void )
/*********************/
{
    Event( COND_TERMINATE );
    TheBigSleep();
}

trap_retval TRAP_CORE( Get_sys_config )( void )
/*********************************************/
{
    get_sys_config_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->os = DIG_OS_NT;
    ret->osmajor = 1;
    ret->osminor = 0;
    ret->huge_shift = 3;
    ret->cpu = 0;
    ret->fpu = 0;
    ret->arch = DIG_ARCH_JVM;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Map_addr )( void )
/***************************************/
{
    map_addr_req        *acc;
    map_addr_ret        *ret;
    WORD                seg;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    seg = acc->in_addr.segment;
    ret->out_addr = acc->in_addr;
    switch( seg ) {
    case MAP_FLAT_CODE_SELECTOR:
    case MAP_FLAT_DATA_SELECTOR:
        ret->out_addr.segment = MAD_JVM_USER_MEM_SELECTOR;
        break;
    default:
        ret->out_addr.segment = MAD_JVM_UNKNOWN_SELECTOR;
        break;
    }
    ret->lo_bound = 0;
    ret->hi_bound = ~(addr48_off)0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Machine_data )( void )
/*******************************************/
{
//    machine_data_req    *acc;
    machine_data_ret    *ret;
//    machine_data_spec   *data;

//    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
//    data = GetOutPtr( sizeof( *ret ) );
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Checksum_mem )( void )
/*******************************************/
{
    DWORD               offset;
    size_t              len;
    WORD                value;
    DWORD               sum;
    checksum_mem_req    *acc;
    checksum_mem_ret    *ret;

    acc = GetInPtr( 0 );
    offset = acc->in_addr.offset;
    sum = 0;
    for( len = acc->len; len > 0; ) {
        DoRead( offset, &value, sizeof( value ) );
        sum += value & 0xff;
        offset++;
        len--;
        if( len > 0 ) {
            sum += value >> 8;
            offset++;
            len--;
        }
    }
    ret = GetOutPtr( 0 );
    ret->result = sum;
    return( sizeof( *ret ) );
}


static union {
    mad_jvm_findlinecue_acc     l;
    mad_jvm_findaddrcue_acc     a;
} FindLineInfo;

static void LineNumLookup( mad_jvm_findline_ret *info )
/*************************************************************/
{
    unsigned            mb_idx;
    struct methodblock  *mb;
    unsigned            ln_idx;
    struct lineno       *ln;
    unsigned long       best_ln;
    unsigned long       curr_ln;
    ClassClass          *cb;
    int                 cb_idx;
    int                 nbinclasses = get_nbinclasses();
    ClassClass          **binclasses = get_binclasses();

    best_ln = ~0L;
    for( cb_idx = 0; cb_idx < nbinclasses; ++cb_idx ) {
        cb = binclasses[cb_idx];
        if( cb->source_name == NULL )
            continue;
        if( stricmp( cb->source_name, ((ClassClass*)(FindLineInfo.l.class_pointer))->source_name ) != 0 )
            continue;
        mb = cbMethods(cb);
        for( mb_idx = 0; mb_idx < cb->methods_count; ++mb_idx, ++mb ) {
            ln = mb->line_number_table;
            for( ln_idx = 0; ln_idx < mb->line_number_table_length; ++ln_idx, ++ln ) {
                curr_ln = ln->line_number;
                if( curr_ln == FindLineInfo.l.line_num ) {
                    info->class_pointer = (unsigned_32)cb;
                    info->line_index = ln_idx;
                    info->method_index = mb_idx;
                    info->ret = SR_EXACT;
                    return;
                }
                if( (curr_ln > FindLineInfo.l.line_num ) && (curr_ln < best_ln) ) {
                    best_ln = curr_ln;
                    info->class_pointer = (unsigned_32)cb;
                    info->line_index = ln_idx;
                    info->method_index = mb_idx;
                }
            }
        }
    }
    info->ret = best_ln != ~0L ? SR_CLOSEST : SR_NONE;
}

static void LineAddrLookup( mad_jvm_findline_ret *info )
/******************************************************/
{
    unsigned            i;
    long                lo;
    long                hi;
    long                target;
    unsigned long       pc;
    ClassClass          *cc;
    struct methodblock  *mb;
    struct lineno       *ln_tbl;

    info->ret = SR_NONE;
    cc = (ClassClass *)FindLineInfo.a.class_pointer;
    mb = cbMethods( cc );
    i = 0;
    for( ;; ) {
        if( i >= cc->methods_count )
            return;
        if( !(mb->fb.access & ACC_NATIVE) ) {
            if( FindLineInfo.a.addr >= (unsigned_32)mb->code
             && FindLineInfo.a.addr < (unsigned_32)mb->code + mb->code_length ) {
                break;
            }
        }
        ++mb;
        ++i;
    }
    info->class_pointer = (unsigned_32)cc;
    info->method_index = i;
    FindLineInfo.a.addr -= (unsigned_32)mb->code;
    ln_tbl = mb->line_number_table;
    hi = mb->line_number_table_length - 1;
    lo = 0;
    while( lo <= hi ) {
        target = (lo + hi) >> 1;
        pc = ln_tbl[target].pc;
        if( FindLineInfo.a.addr < pc ) {
            hi = target - 1;
        } else if( FindLineInfo.a.addr > pc ) {
            lo = target + 1;
        } else {                  /* exact match */
            info->line_index = target;
            info->ret = SR_EXACT;
            return;
        }
    }
    if( hi < 0 )
        return;
    info->line_index = hi;
    info->ret = SR_CLOSEST;
}

trap_retval TRAP_CORE( Read_mem )( void )
/***************************************/
{
    read_mem_req        *acc;
    char                *data;
    size_t              len;
    ClassClass          *cb;
    char                buff[512], *p;

    acc = GetInPtr( 0 );
    data = GetOutPtr( 0 );
    len = acc->len;
    switch( acc->mem_addr.segment ) {
    case MAD_JVM_USER_MEM_SELECTOR:
    case MAD_JVM_UNKNOWN_SELECTOR:
    case MAD_JVM_DIP_MEM_SELECTOR:
    default:
        return( DoRead( acc->mem_addr.offset, data, len ) );
    case MAD_JVM_FINDCLASS_SELECTOR:
        DoRead( acc->mem_addr.offset, buff, sizeof( buff ) );
        p = strchr( buff, ';' );
        if( p != NULL )
            *p = '\0';
        cb = FindClass( EE(), buff, FALSE );
        if( len < sizeof( cb ) )
            return( 0 );
        memcpy( data, (char const*)&cb, sizeof( cb ) );
        return( sizeof( cb ) );
    case MAD_JVM_FINDLINECUE_SELECTOR:
        if( len < sizeof( mad_jvm_findline_ret ) )
            return( 0 );
        LineNumLookup( (mad_jvm_findline_ret*)data );
        return( sizeof( mad_jvm_findline_ret ) );
    case MAD_JVM_FINDADDRCUE_SELECTOR:
        if( len < sizeof( mad_jvm_findline_ret ) )
            return( 0 );
        LineAddrLookup( (mad_jvm_findline_ret*)data );
        return( sizeof( mad_jvm_findline_ret ) );
    }
}

trap_retval TRAP_CORE( Write_mem )( void )
/****************************************/
{
    DWORD               offset;
    LPSTR               data;
    write_mem_req       *acc;
    write_mem_ret       *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    ret->len = 0;
    offset = acc->mem_addr.offset;
    data = (LPSTR)GetInPtr( sizeof( *acc ) );
    switch( acc->mem_addr.segment ) {
    case MAD_JVM_FINDLINECUE_SELECTOR:
        FindLineInfo.l = *(mad_jvm_findlinecue_acc*)data;
        ret->len = sizeof( mad_jvm_findlinecue_acc );
        break;
    case MAD_JVM_FINDADDRCUE_SELECTOR:
        FindLineInfo.a = *(mad_jvm_findaddrcue_acc*)data;
        ret->len = sizeof( mad_jvm_findaddrcue_acc );
        break;
    default:
        ret->len = DoWrite( offset, data, GetTotalSizeIn() - sizeof( *acc ) );
        break;
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Read_io )( void )
/**************************************/
{
    // never called
    return( 0 );
}


trap_retval TRAP_CORE( Write_io )( void )
/***************************************/
{
    write_io_ret        *ret;

    ret = GetOutPtr( 0 );
    ret->len = 0;
    return( sizeof( *ret ) );
}

static void GetRegs( unsigned_32 *pc,
                     unsigned_32 *frame,
                     unsigned_32 *optop,
                     unsigned_32 *vars )
/**************************************/
{
    ExecEnv             *ee;

    if( CurrThread == NULL ) {
        if( pc != NULL )
            *pc = 0;
        if( frame != NULL )
            *frame = 0;
        if( optop != NULL )
            *optop = 0;
        if( vars != NULL )
            *vars = 0;
        return;
    }
    ee = (ExecEnv*)CurrThread->eetop;
    if( pc != NULL )
        *pc = (unsigned_32)ee->current_frame->lastpc;
    if( optop != NULL )
        *optop = (unsigned_32)ee->current_frame->optop;
    if( vars != NULL )
        *vars = (unsigned_32)ee->current_frame->vars;
    if( frame != NULL ) {
        *frame = (unsigned_32)ee->current_frame;
    }
}

trap_retval TRAP_CORE( Read_regs )( void )
/****************************************/
{
    mad_registers       _WCUNALIGNED *mr;

    mr = GetOutPtr( 0 );
    memset( mr, 0, sizeof( mr->jvm ) );
    GetRegs( &mr->jvm.pc, &mr->jvm.frame, &mr->jvm.optop, &mr->jvm.vars );
    return( sizeof( mr->jvm ) );
}

trap_retval TRAP_CORE( Write_regs )( void )
/*****************************************/
{
    mad_registers       _WCUNALIGNED *mr;

    if( CurrThread == NULL ) {
        return( 0 );
    }
    mr = GetInPtr( sizeof( write_regs_req ) );
    // NYI: cannot write registers
    return( 0 );
}

static struct methodblock *
FindMethod(ClassClass *cb, char *name, char *sig)
/***********************************************/
{
    int i = cb->methods_count;
    struct methodblock *mb = cbMethods(cb);

    while (--i >= 0) {
        struct fieldblock *fb = &mb->fb;
        if (strcmp(fieldname(fb), name) == 0 &&
            strcmp(fieldsig(fb), sig) == 0) {
            return mb;
        }
        mb++;
    }
    return NULL;
}

/*
 * Build argument array to be passed to "main"
 */
static HArrayOfString *
BuildArguments( char **argv, struct execenv *ee)
/**********************************************/
{
    int argc;
    HArrayOfString *args;

    for( argc = 0; argv[argc] != NULL; ++argc )
        {}
    args = (HArrayOfString *)ArrayAlloc(T_CLASS, argc);
    if (args == NULL) {
        return NULL;
    }
    /* Give the array a "type" */
    unhand((HArrayOfObject *)args)->body[argc] =
                        (JHandle *)FindClass(ee, JAVAPKG "String", TRUE);
    while (--argc >= 0) {
        char *s = argv[argc];
        if( (unhand( args )->body[argc] = makeJavaString( s, strlen( s ) )) == NULL ) {
            return NULL;
        }
    }
    return args;
}

static int SplitParms( char *p, const char **args, size_t len )
/**************************************************************
 * Break up program arguments passed in as a single string into
 * individual components. Useful for passing argv style array to
 * exec().
 */
{
    int     i;
    char    endc;

    i = 0;
    if( len == 1 )
        goto done;
    for( ;; ) {
        for( ;; ) {
            if( len == 0 )
                goto done;
            if( *p != ' ' && *p != '\t' )
                break;
            ++p;
            --len;
        }
        if( len == 0 )
            goto done;
        if( *p == '"' ) {
            --len;
            ++p;
            endc = '"';
        } else {
            endc = ' ';
        }
        if( args != NULL )
            args[i] = p;
        ++i;
        for( ;; ) {
            if( len == 0 )
                goto done;
            if( *p == endc || *p == '\0' || ( endc == ' ' && *p == '\t' ) ) {
                /*
                 * if output array is not specified then source string
                 * is not changed and it calculates number of parameters only
                 * as soon as output array is specified then source is modified
                 */
                if( args != NULL ) {
                    *p = '\0';
                }
                ++p;
                --len;
                if( len == 0 )
                    goto done;
                break;
            }
            ++p;
            --len;
        }
    }
done:
    return( i );
}

#pragma aux (cdecl) PlantAppletBreak "Java_sun_tools_debug_jvmhelp_PlantAppletBreak_stub" export
stack_item *PlantAppletBreak( stack_item *p, ExecEnv *ee )
/********************************************************/
{
    size_t      len;
    Classjava_lang_String *applet;
    char        *dst;
    unicode     *src;
    char        *buff;
    ClassClass  *cb;
    struct methodblock  *mb;
    char        *dot;

    applet  = unhand( (HString *)p[0].h );
    len = obj_length( applet->value );
    buff = walloca( len + 1 );
    dst = buff;
    src = unhand( applet->value )->body;
    while( *src != '\0' && len-- > 0 ) {
        *dst++ = *src++;
    }
    *dst = '\0';
    dst = buff;
    for( ;; ) {
        dot = strchr( dst, '.' );
        if( dot == NULL )
            break;
        if( stricmp( dot, ".class" ) == 0 ) {
            *dot = '\0';
            break;
        }
        dst = dot + 1;
    }
    cb = FindClass( ee, buff, TRUE );
    if( cb == NULL )
        return( p );
    mb = FindMethod( cb, "init", "()V" );
    if( mb == NULL )
        return( p );
    AddStartingBreakpoint( (unsigned)mb->code );
    ++AppsLoaded;
    return( p );
}

#pragma aux (cdecl) LoadCallBack "Java_sun_tools_debug_jvmhelp_LoadCallBack_stub" export
stack_item *LoadCallBack( stack_item *p, ExecEnv *ee )
/****************************************************/
{
    prog_load_req       *acc;
    prog_load_ret       *ret;
    char                **args;
    char                *parms;
    char                *parm_start;
    int                 i;
    size_t              len;
    struct methodblock  *mb;
    bool                html;
    char                *p;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    /* the IS_STARTED flag is to stop the "go main" in the profile. We're
       already at main */
    ret->flags = LD_FLAG_IS_STARTED | LD_FLAG_IGNORE_SEGMENTS | LD_FLAG_HAVE_RUNTIME_DLLS;
    parms = parm_start = GetInPtr( sizeof( *acc ) );
    len = GetTotalSizeIn() - sizeof( *acc );
    if( acc->true_argv ) {
        i = 1;
        while( len-- > 0 ) {
            if( *parms++ == '\0' ) {
                i++;
            }
        }
        args = walloca( i * sizeof( *args ) );
        parms = parm_start;
        len = GetTotalSizeIn() - sizeof( *acc );
        i = 1;
        while( len-- > 0 ) {
            if( *parms++ == '\0' ) {
                args[i++] = parms;
            }
        }
    } else {
        while( --len, *parms++ != '\0' )
            {}
        i = SplitParms( parms, NULL, len ) + 2;
        args = alloca( i * sizeof( *args ) + len );
        p = memcpy( (void *)( args + i ), parms, len );
        SplitParms( p, args + 1, len );
    }
    args[0] = parm_start;
    args[i - 1] = NULL;

    html = FALSE;
    CbMain = FindClass( ee, parm_start, TRUE );
    if( CbMain == NULL ) {
        html = TRUE;
        CbMain = FindClass( ee, "sun/applet/AppletViewer", TRUE );
        if( CbMain == NULL ) {
            ret->err = ERR_JVM_CANT_FIND_APPLETVIEWER;
            Event( 0 );
            return( p );
        }
    }
    mb = FindMethod( CbMain, "main", "([Ljava/lang/String;)V" );
    if( mb == NULL ) {
        ret->err = ERR_JVM_CANT_FIND_MAIN;
        Event( 0 );
        return( p );
    }
    if( html ) {
        execute_java_static_method(0, cbCallJava, "ParseHTML", "([Ljava/lang/String;)V", BuildArguments( args, ee ) );
    } else {
        ++AppsLoaded;
        AddStartingBreakpoint( (unsigned)mb->code );
    }
    ret->task_id = 1;
    ret->mod_handle = -1; // not an index!
    do_execute_java_method( ee, CbMain, 0, 0, mb, TRUE, (JHandle *) BuildArguments( args, ee ) );
    if( html ) {
        TheBigSleep();
    } else {
        CurrThreadIdx = -1;
        NumThreads = 0;
        Event( COND_TERMINATE );
    }
    return( p );
}

trap_retval TRAP_CORE( Prog_load )( void )
/****************************************/
{
    prog_load_ret       *ret;

    ret = GetOutPtr( 0 );
    AppsLoaded = 0;
    execute_java_static_method(0, cbCallJava, "Load", "()V", NULL );
    WaitForEvent();
    if( AppsLoaded == 0 ) {
        if( ret->err == 0 ) {
            ret->err = ERR_JVM_CANT_LOAD;
        }
    }
    return( sizeof( prog_load_ret ) );
}

trap_retval TRAP_CORE( Prog_kill )( void )
/****************************************/
{
    prog_kill_ret       *ret;

    AppsLoaded = 0;
    ret = GetOutPtr( 0 );
    ret->err = 0;
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Set_watch )( void )
/****************************************/
{
    // nyi
    return( 0 );
}

trap_retval TRAP_CORE( Clear_watch )( void )
/******************************************/
{
    // nyi
    return( 0 );
}

trap_retval TRAP_CORE( Set_break )( void )
/****************************************/
{
    set_break_req       *acc;
    set_break_ret       *ret;
    brk                 *new;
    char                ch;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    DoRead( acc->break_addr.offset, &ch, sizeof(ch) );
    ret->old = ch;
    new = LocalAlloc( LMEM_FIXED, sizeof( *new ) );
    new->next = BPList;
    BPList = new;
    new->pc = acc->break_addr.offset;
    new->saved = ch;
    DoWrite( acc->break_addr.offset, OP_BREAK, 1 );
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Clear_break )( void )
/******************************************/
{
    char                        ch;
    clear_break_req             *acc;
    brk                         *bp, *next;

    // we can assume all breaks are cleared at once

    for( bp = BPList; bp != NULL; bp = next ) {
        next = bp->next;
        LocalFree( bp );
    }
    BPList = NULL;
    acc = GetInPtr( 0 );
    ch = acc->old;
    DoWrite( acc->break_addr.offset, &ch, sizeof(ch) );
    return( 0 );
}

/*
 * runProg - run threads
 */
static unsigned runProg( bool single_step )
/*****************************************/
{
    prog_go_ret *ret;
    Classjava_lang_Thread *trd;
    unsigned_32 new_pc,old_pc;

    ret = GetOutPtr( 0 );
    if( CurrThread == NULL ) {
        ret->conditions = COND_TERMINATE;
        return( sizeof( *ret ) );
    }

    if( !single_step ) {
        EventDone(); // previous event is done
        WaitForEvent();
    } else {
        GetRegs( &old_pc, NULL, NULL, NULL );
        do {
            trd = CurrThread;
            set_single_stepping( TRUE );
            trd->single_step = TRUE;
            EventDone(); // previous event is done
            WaitForEvent();
            set_single_stepping( FALSE );
            trd->single_step = FALSE; // nyi - what if thread is dead?
            GetRegs( &new_pc, NULL, NULL, NULL );
        } while( new_pc == old_pc );
    }
    ret->conditions = Conditions | COND_THREAD;
    if( get_nbinclasses() > LastClassGiven )
        ret->conditions |= COND_LIBRARIES;

    GetRegs( &ret->program_counter.offset, &ret->stack_pointer.offset, NULL, NULL );
    ret->program_counter.segment = 0;
    ret->stack_pointer.segment = 0;
    return( sizeof( *ret ) );
} /* runProg */

trap_retval TRAP_CORE( Prog_go )( void )
/**************************************/
{
    return( runProg( FALSE ) );
}

trap_retval TRAP_CORE( Prog_step )( void )
/****************************************/
{
    return( runProg( TRUE ) );
}

trap_retval TRAP_CORE( Redirect_stdin )( void )
/*********************************************/
{
    // never called
    return( 0 );
}

trap_retval TRAP_CORE( Redirect_stdout )( void )
/**********************************************/
{
    // never called
    return( 0 );
}

trap_retval TRAP_CORE( Get_next_alias )( void )
/*********************************************/
{
    get_next_alias_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->seg = 0;
    ret->alias = 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Get_err_text )( void )
/*******************************************/
{

    // never called
    return( 0 );
}

trap_retval TRAP_CORE( Get_lib_name )( void )
/*******************************************/
{
    get_lib_name_req    *acc;
    get_lib_name_ret    *ret;
    char                *name;
    int                 first;
    ClassClass          *cb;
    int                 nbinclasses = get_nbinclasses();
    ClassClass          **binclasses = get_binclasses();
    size_t              name_maxlen;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    ret->mod_handle = 0;
    // first is 0 based, LastClassGiven is 1 based, so no increment is required
    if( acc->mod_handle == 0 ) {
        if( nbinclasses <= LastClassGiven ) {
            return( sizeof( *ret ) );
        }
        first = LastClassGiven;
    } else {
        first = acc->mod_handle;
    }

    if( first >= nbinclasses )
        return( sizeof( *ret ) );

    cb = binclasses[first];
    if( cb == CbMain ) {
        ++first;
        if( first >= nbinclasses )
            return( sizeof( *ret ) );
        cb = binclasses[first];
    }

    name_maxlen = GetTotalSizeOut() - sizeof( *ret ) - 1;
    name = GetOutPtr( sizeof( *ret ) );
    strncpy( name, JAVAPREFIX, name_maxlen );
    name[name_maxlen] = '\0';
    strncat( name, cb->name, name_maxlen - strlen( name ) );
    name[name_maxlen] = '\0';
    ret->mod_handle = LastClassGiven = first + 1;
    return( sizeof( *ret ) + strlen( name ) + 1 );
}

trap_retval TRAP_CORE( Get_message_text )( void )
/***********************************************/
{
#if 0
    // we only need to implement this if we return COND_MESSAGE
    get_message_text_ret        *ret;
    char                        *err_txt;

    ret = GetOutPtr( 0 );
    ret->flags = MSG_NEWLINE | MSG_ERROR;
    err_txt = GetOutPtr( sizeof( *ret ) );
    strcpy( err_txt, "message text is NYI" );
    return( sizeof( *ret ) + strlen( err_txt ) + 1 );
#else
    return( 0 );
#endif
}

trap_retval TRAP_THREAD( get_next )( void )
/*****************************************/
{
    thread_get_next_req *acc;
    thread_get_next_ret *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    if( acc->thread == 0 ) {
        ret->thread = 1;
    } else {
        ret->thread = acc->thread+1;
        if( ret->thread > NumThreads ) {
            ret->thread = 0;
            return( sizeof( *ret ) );
        }
    }
    if( SysThreads[ret->thread - 1] && SysThreads[ret->thread - 1]->state == RUNNABLE ) {
        ret->state = THREAD_THAWED;
    } else {
        ret->state = THREAD_FROZEN;
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_THREAD( set )( void )
/************************************/
{
    thread_set_req      *acc;
    thread_set_ret      *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->old_thread = CurrThreadIdx + 1;
    ret->err = 0;
    if( acc->thread != 0 ) {
        if( acc->thread <= NumThreads && SysThreads[acc->thread - 1] &&
            SysThreads[acc->thread - 1]->state == RUNNABLE ) {
            CurrThreadIdx = acc->thread - 1;
        } else {
            ret->err = ERR_JVM_THREAD_NOT_RUNNABLE;
        }
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_THREAD( freeze )( void )
/***************************************/
{
    thread_freeze_req   *acc;
    thread_freeze_ret   *ret;

    // nyi
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = ERR_JVM_THREADS_NOT_SUPPORTED;
    return( sizeof( *ret ) );
}


trap_retval TRAP_THREAD( thaw )( void )
/*************************************/
{
    thread_thaw_req     *acc;
    thread_thaw_ret     *ret;

    // nyi
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = ERR_JVM_THREADS_NOT_SUPPORTED;
    return( sizeof( *ret ) );
}

trap_retval TRAP_THREAD( get_extra )( void )
/******************************************/
{
    thread_get_extra_req        *acc;
    char                        *name;
    char                        buff[256];
    Classjava_lang_Thread*      trd;

    acc = GetInPtr( 0 );
    name = GetOutPtr( 0 );
    strcpy( name, "" );
    if( acc->thread != 0 ) {
        trd = (Classjava_lang_Thread*)unhand(Threads[acc->thread - 1]);
        ThreadName( trd, buff, sizeof( buff ) );
        strcat( name, buff );

        strcat( name, " (" );
        if( SysThreads[acc->thread - 1] ) {
            switch( SysThreads[acc->thread - 1]->state ) {
            case RUNNABLE:
                strcat( name, "running, " );
                break;
            case SLEEPING:
                strcat( name, "sleeping, " );
                break;
            case MONITOR_WAIT:
                strcat( name, "waiting for monitor, " );
                break;
            case CONDVAR_WAIT:
                strcat( name, "waiting for conditional variable, " );
                break;
            }
        } else {
            strcat( name, "unknown state, " );
        }
        strcat( name, "priority=" );
        itoa( trd->priority, buff, 10 );
        strcat( name, buff );

        strcat( name, ")" );
    }
    return( strlen( name ) + 1 );
}

trap_retval TRAP_FILE( get_config )( void )
/*****************************************/
{
    // never called
    return( 0 );
}

trap_retval TRAP_FILE( file_to_fullpath )( void )
/***********************************************/
{
    // never called
    return( 0 );
}

static ClassClass *CbOpened;
trap_retval TRAP_FILE( open )( void )
/***********************************/
{
    file_open_req       *acc;
    file_open_ret       *ret;
    char                *buff;
    int                 i;
    int                 nbinclasses = get_nbinclasses();
    ClassClass          **binclasses = get_binclasses();

    ret = GetOutPtr( 0 );
    acc = GetInPtr( 0 );
    buff = GetInPtr( sizeof(*acc) );


    ret->err = ERR_JVM_INTERNAL_ERROR;
    if( memcmp( buff, JAVAPREFIX, sizeof( JAVAPREFIX )-1 ) == 0 ) {
        buff += sizeof( JAVAPREFIX ) - 1;
        for( i = 0; i < nbinclasses; ++i ) {
            if( strcmp( binclasses[i]->name, buff ) == 0 ) {
                ret->err = 0;
                CbOpened = binclasses[i];
                break;
            }
        }
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( seek )( void )
/***********************************/
{
    // never called
    return( 0 );
}

trap_retval TRAP_FILE( write )( void )
/************************************/
{
    // never called
    return( 0 );
}

trap_retval TRAP_FILE( write_console )( void )
/********************************************/
{
    // never called
    return( 0 );
}

#define PREFIX "JAVA"
#define PREFIX_SIZE (sizeof(PREFIX)-1)
#define LENGTH (PREFIX_SIZE+sizeof(void*))

trap_retval TRAP_FILE( read )( void )
/***********************************/
{
    file_read_req       *acc;
    file_read_ret       *ret;
    char                tmp[LENGTH];
    size_t              bytes;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    if( CbOpened == NULL ) {
        return( sizeof( *ret ) );
    }
    strcpy( tmp, PREFIX );
    *((void **)( tmp + PREFIX_SIZE )) = (void *)CbOpened;
    bytes = LENGTH;
    if( bytes > acc->len )
        bytes = acc->len;
    memcpy( GetOutPtr( sizeof( *ret ) ), tmp, bytes );
    return( sizeof( *ret ) + bytes );
}

trap_retval TRAP_FILE( close )( void )
/************************************/
{
    // never called
    return( 0 );
}

trap_retval TRAP_FILE( erase )( void )
/************************************/
{
    // never called
    return( 0 );

}

trap_retval TRAP_FILE( run_cmd )( void )
/**************************************/
{
    // never called
    return( 0 );
}

trap_version TRAPENTRY TrapInit( const char *parms, char *err, bool remote )
/**************************************************************************/
{
    trap_version        ver;

    /* unused parameters */ (void)remote; (void)parms;

    err[0] = '\0'; /* all ok */
    ver.major = TRAP_VERSION_MAJOR;
    ver.minor = TRAP_VERSION_MINOR;
    ver.remote = FALSE;
    sysAtexit( HandleExit );
    cbCallJava = FindClass( EE(), "sun/tools/debug/jvmhelp", TRUE );
    EventSem = CreateSemaphore( NULL, 0, 1, NULL );
    EventDoneSem = CreateSemaphore( NULL, 0, 1, NULL );
    return( ver );
}

void TRAPENTRY TrapFini( void )
/*****************************/
{
    CloseHandle( EventSem );
    CloseHandle( EventDoneSem );
}
