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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifndef __RTEXCEPT_H__
#define __RTEXCEPT_H__

#ifndef __cplusplus
    #error RTEXCEPT.H only compiles under C++
#endif
struct _RTCTL;

typedef enum                    // TYPES OF DISPATCHABILITIES
{   DISPATCHABLE_NONE           // - not dispatchable
,   DISPATCHABLE_CATCH          // - dispatchable for catch
,   DISPATCHABLE_FNEXC          // - dispatchable for function exception spec.
,   DISPATCHABLE_NO_CATCH       // - no catch block dispatchable (throw)
,   DISPATCHABLE_STOP           // - stop search: error situation
,   DISPATCHABLE_SYS_EXC        // - uncatchable system exception
} DISPATCHABLE;


#ifndef NDEBUG
    #define GOOF_EXC( msg ) GOOF( msg )
#else
    #define GOOF_EXC( msg ) CPPLIB( corrupted_stack )()
#endif


//************************************************************************
// Commands referenced from state-variable table
//************************************************************************


struct RT_ARRAY_INIT                    // RT_ARRAY_INIT: array being init'ed
{   void* array;                        // - addr[ array ]
    RT_TYPE_SIG sig;                    // - type signature
    offset_t index;                     // - initialized count
};

struct DTOR_CMD_BASE                    // BASE FOR DTOR_CMD
{   DTOR_CMD_CODE code;                 // - command
};

struct DTOR_CMD_ARRAY                   // DTOR_CMD: ARRAY
{   DTOR_CMD_BASE base;                 // - base
    AlignPad1
    offset_t offset;                    // - RT_ARRAY_INIT offset
    offset_t count;                     // - # elements
    RT_TYPE_SIG sig;                    // - type signature
};

struct DTOR_CMD_SET_SV                  // DTOR_CMD: SET_SV
{   DTOR_CMD_BASE base;                 // - base
    AlignPad1
    RT_STATE_VAR state_var;             // - new state variable
};

struct DTOR_CMD_CTOR_TEST               // DTOR_CMD: CTOR_TEST
{   DTOR_CMD_BASE base;                 // - base
    AlignPad1
    offset_t index;                     // - index of flag
};

struct DTOR_CMD_FN_EXC                  // DTOR_CMD: FN_EXC
{   DTOR_CMD_BASE base;                 // - base
    AlignPad1
    offset_t count;                     // - number of elements
    RT_TYPE_SIG sigs[1];                // - type signatures permitted
};

struct DTOR_CMD_TRY                     // DTOR_CMD: TRY
{   DTOR_CMD_BASE base;                 // - base
    DTOR_CMD_BASE base_catch;           // - base: catch
    AlignPad2
    RT_STATE_VAR state;                 // - state preceding try
    offset_t jmp_buf;                   // - RW-offset[ longjmp jmp_buf ]
    offset_t offset;                    // - RW-offset[ try variable ]
    offset_t count;                     // - number of catch blocks
    RT_TYPE_SIG sigs[1];                // - type signatures for catches
};

#define TryFromCatch( cmd )     \
    (DTOR_CMD *)                \
    ( (char *)( cmd ) - __offsetof( DTOR_CMD_TRY, base_catch ) )

struct DTOR_CMD_TEST_FLAG               // DTOR_CMD: TEST_FLAG
{   DTOR_CMD_BASE base;                 // - base
    AlignPad1
    offset_t index;                     // - index of flag
    RT_STATE_VAR state_var_true;        // - state variable, when true
    RT_STATE_VAR state_var_false;       // - state variable, when false
};

struct DTOR_CMD_COMPONENT               // DTOR_CMD: COMPONENT
{   DTOR_CMD_BASE base;                 // - base
    AlignPad1
    offset_t offset_init;               // - offset to RT_OBJECT_INIT element
    offset_t offset_object;             // - offset of component object
    pFUNdtor dtor;                      // - DTOR address
};

struct DTOR_CMD_ARRAY_INIT              // DTOR_CMD: ARRAY_INIT
{   DTOR_CMD_BASE base;                 // - base
    AlignPad1
    offset_t offset;                    // - offset of RT_ARRAY_INIT
};

typedef void (*pFunDelete1)( void* );
typedef void (*pFunDelete2)( void*, size_t );
union pFunDelete {
    pFunDelete1 delete_1;
    pFunDelete2 delete_2;
};

struct DTOR_CMD_DLT_1                   // DTOR_CMD: DTC_DLT_..._1
{   DTOR_CMD_BASE base;                 // - base
    AlignPad1
    offset_t offset;                    // - offset to ptr to area
    pFunDelete1 op_del;                 // - operator delete( void* );
};

struct DTOR_CMD_DLT_2                   // DTOR_CMD: DTC_DLT_..._2
{   DTOR_CMD_BASE base;                 // - base
    AlignPad1
    offset_t offset;                    // - offset to ptr to area
    pFunDelete2 op_del;                 // - operator delete( void*, size_t );
    size_t size;                        // - size of object
};

union DTOR_CMD                          // DTOR_CMD: one of
{   DTOR_CMD_BASE base;                 // - base
    DTOR_CMD_ARRAY array;               // - array
    DTOR_CMD_SET_SV set_sv;             // - set_sv
    DTOR_CMD_FN_EXC fn_exc;             // - fn_exc
    DTOR_CMD_TRY try_cmd;               // - try
    DTOR_CMD_TEST_FLAG test_flag;       // - test_flag
    DTOR_CMD_COMPONENT component;       // - component of object
    DTOR_CMD_ARRAY_INIT array_init;     // - array being initialized
    DTOR_CMD_DLT_1 delete_1;            // - delete: operator delete(void*)
    DTOR_CMD_DLT_2 delete_2;            // - delete: operator delete(void*,size_t)
    DTOR_CMD_CTOR_TEST ctor_test;       // - ctor-test
};


//************************************************************************
// Static command codes
//************************************************************************

struct STATIC_CMDS                      // STATIC COMMANDS
{   DTOR_CMD_CODE direct_base;          // - direct base
    DTOR_CMD_CODE virtual_base;         // - virtual base
    DTOR_CMD_SET_SV set_sv_cmds[ MAX_SET_SV ]; // - set-sv commands
};


//************************************************************************
// State variable table
//************************************************************************

struct RO_STATE                         // RO_STATE -- R/O entry for state var.
{   pFUNdtor    dtor;                   // - NULL or DTOR address
    union {                             // - one of
        void* data_addr;                // - - data address
        offset_t data_offset;           // - - data offset
        DTOR_CMD* cmd_addr;             // - - command address
    } u;
};



//************************************************************************
// Registration Definitions
//************************************************************************

struct RO_DTREG_BASE            // RO_DTREG_BASE -- R/O base
{   offset_t reg_type;          // - type of registration
};

struct RW_DTREG_BASE            // RW_DTREG_BASE -- R/W base (automatic)
{
#ifdef RW_REGISTRATION
    RW_DTREG* prev;             // - previous entry
    FS_HANDLER* handler;        // - fs: handler
#endif
    RO_DTREG* ro;               // - R/O entry
    RT_STATE_VAR state_var;     // - state variable
};

struct RW_DTREG_BASE_STATIC     // RW_DTREG_BASE -- R/W base (static)
{   RW_DTREG* prev;             // - previous entry
    RO_DTREG* ro;               // - R/O entry
    RT_STATE_VAR state_var;     // - state variable
};


#ifdef RW_REGISTRATION

struct RW_DTREG_ROOT            // RW_DTREG_BASE -- R/W base
{   RW_DTREG* prev;             // - previous entry
    FS_HANDLER* handler;        // - fs: handler
    FS_HANDLER* signature;      // - fs: handler (again, used as signature)
    THREAD_CTL* pgm_thread;     // - thread when WATCOM program (across DLL'S)
};
#endif


//************************************************************************
// Registration for a function
//************************************************************************

struct RW_DTREG_FUN             // R/W registration
{   RW_DTREG_BASE base;         // - base
    uint_8 flags[1];            // - flags [optional]
};

struct RO_DTREG_FUN             // R/O registration
{   RO_DTREG_BASE base;         // - base
#ifdef PD_REGISTRATION
    offset_t rw_offset;         // - offset[ R/W from frame ]
#endif
    RO_STATE state_table[1];    // - state table
};


//************************************************************************
// Registration for an Object
//************************************************************************

struct RW_DTREG_OBJECT          // R/W registration (not linked, no state)
{   void* object;               // - object address
    uint_8 cdtor;               // - CDTOR used to ctor/dtor object
};


//************************************************************************
// Registration for static initialization
//************************************************************************

struct RW_DTREG_INITFS          // R/W registration, file scope
{   RW_DTREG_BASE_STATIC base;  // - base
};

struct RO_DTREG_INITFS          // R/O registration, file scope
{   RO_DTREG_BASE base;         // - base
    RO_STATE state_table[1];    // - state table
};

struct RW_DTREG_INITLS_STATIC   // R/W registration, local scope (memory)
{   RW_DTREG_BASE_STATIC base;  // - base
    void* object;               // - object to be DTOR'ED
};

struct RW_DTREG_INITLS          // R/W registration, local scope (stack)
{   RW_DTREG_BASE base;         // - base
    void* object;               // - object to be DTOR'ED
};

struct RO_DTREG_INITLS          // R/O registration, local scope
{   RO_DTREG_BASE base;         // - base
    RO_STATE state_table[1];    // - state table
};



//************************************************************************
// Registration Definitions
//************************************************************************

union RW_DTREG                  // R/W Registration: one of
{   RW_DTREG_BASE base;         // - base (automatic)
    RW_DTREG_BASE_STATIC base_st;//- base (static)
#ifdef RW_REGISTRATION
    RW_DTREG_ROOT root;         // - root
#endif
    RW_DTREG_FUN fun;           // - function
    RW_DTREG_INITLS_STATIC init_ls_st; // - static init., local scope (static)
};

union RO_DTREG                  // R/O Registration: one of
{   RO_DTREG_BASE base;         // - base
    RO_DTREG_FUN fun;           // - function
    RO_DTREG_INITLS init_ls;    // - static init., local scope
};



//************************************************************************
// Support for state-table traversal
//************************************************************************

enum                            // OB_TYPE -- type of object
{   OBT_OBJECT                  // - complete object
,   OBT_MEMBER                  // - member
,   OBT_DBASE                   // - direct base
,   OBT_VBASE                   // - virtual base
};
typedef uint_8 OB_TYPE;

struct STAB_TRAVERSE            // STAB_TRAVERSE -- control for traversal
{   _RTCTL* rtc;                // - R/T control info
    RT_STATE_VAR state_var;     // - state variable for position
    RT_STATE_VAR bound;         // - bound for traversal
    OB_TYPE obj_type;           // - type of object
    uint_8 unwinding :1;        // - true ==> unwinding stack
    unsigned :0;                // - alignment
};





//************************************************************************
// Static Initialization
//************************************************************************

struct MODRO_LS                 // local-scope R/O registration
{   offset_t reg_type;          // - registration type
    RO_STATE state_table[1];    // - state table (one entry)
};

struct MODRW_LS                 // local-scope R/W registration (one var.)
{   MODRW_LS *prev;             // - previous R/W block (NULL==>not init.ed)
    MODRO_LS *ro;               // - R/O for module
};


struct THROW_RW                 // THROW_RW - R/W INFORMATION FOR A THROW
{   THROW_RW *prev;             // - previous throw pending in thread
    THROW_RO *ro_blk;           // - R/O information for thrown object
    char thrown[1];             // - thrown object
};

enum EXCSTATE                   // STATES FOR EXCEPTIONS
{   EXCSTATE_UNWIND     = 0     // - attempting to unwind
,   EXCSTATE_DISPATCH   = 1     // - handler has been dispatched
,   EXCSTATE_UNEXPECTED = 2     // - "unexpected" routine has been called
,   EXCSTATE_TERMINATE  = 3     // - "terminate" routine has been called
,   EXCSTATE_ABORT      = 4     // - "abort" routine has been called
,   EXCSTATE_CTOR       = 5     // - copying exception to active area
,   EXCSTATE_DTOR       = 6     // - DTORing exception in active area
,   EXCSTATE_BAD_EXC    = 7     // - "bad_exception" thrown for fn-exc
};

struct DISPATCH_EXC;            // DISPATCH_EXC -- dispatch control

struct ACTIVE_EXC               // ACTIVE_EXC -- exception being handled
{   ACTIVE_EXC *prev;           // - stacked exception in thread
    THROW_RO *throw_ro;         // - R/O block for throw
    RT_TYPE_SIG sig;            // - type signature for exception
    EXCSTATE state;             // - state of exception EXCSTATE_...
    EXCSTATE fnexc_state;       // - state of exception before fn-exc dispatch
    uint_8 zero_thrown :1;      // - const zero was thrown
    unsigned :0;                // - alignment
    void *extra_object;         // - extra pointer object
    void *exc_area;             // - allocation area for exception
    DTOR_CMD* cat_try;          // - fnexc or try command for first catch
    DISPATCH_EXC* dispatch;     // - fnexc: dispatch information
    RW_DTREG* rw;               // - read/write block for cat_try
    char data[1];               // - exception saved
};

struct DISPATCH_EXC             // DISPATCH_EXC -- dispatch control
{   RW_DTREG* rw;               // - read/write block for dispatch
    THROW_RO* ro;               // - read/only try block
    _RTCTL* rtc;                // - R/T control info
    ACTIVE_EXC *exc;            // - exception to dispatch
    ACTIVE_EXC *exc_srch;       // - exception for search
    THROW_CNV *cnv_try;         // - try conversion
    DTOR_CMD* try_cmd;          // - try command
    unsigned catch_no;          // - catch number
    RT_STATE_VAR state_var;     // - state variable after dispatch
    DISPATCHABLE type;          // - type of dispatch
    uint_8 zero              :1;// - true ==> zero thrown
    uint_8 rethrow           :1;// - true ==> is re-throw
    uint_8 popped            :1;// - true ==> original catch handler popped
    uint_8 non_watcom        :1;// - true ==> non-watcom exc. & catch(...)
    unsigned :0;                // - alignment
    long system_exc;            // - system exc code
    RW_DTREG* fnexc_skip;       // - fn-exc skipping R/W block
    RW_DTREG* fs_last;          // - last WATCOM R/W block
    _EXC_PR* srch_ctl;          // - exception search control
#ifdef PD_REGISTRATION
    PData* pdata;               // - proc. descr. for dispatching rtn.
#endif
};


//************************************************************************
// PROTOTYPES
//************************************************************************

extern "C" {

ACTIVE_EXC *CPPLIB( alloc_exc )(// ALLOCATE AN EXCEPTION
    void *object,               // - address of object
    THROW_RO *throw_ro,         // - throw R/O block
    _RTCTL* rwc )               // - R/T control
;
#ifdef RW_REGISTRATION
_WPRTLINK
void CPPLIB( base_deregister )( // DE-REGISTRATION OF BASE
    void )
;
#endif
THREAD_CTL* CPPLIB( base_deregister_2 )( // DE-REGISTRATION OF 2 BASES
    void )
;
THREAD_CTL* CPPLIB( base_register )( // REGISTRATION OF BASE
    RW_DTREG* rw,               // - R/W element
    RO_DTREG* ro,               // - R/O element
    RT_STATE_VAR state_var )    // - initial state variable
;
void CPPLIB( bit_off )(         // TURN BIT OFF
    uint_8* bits,               // - bits
    offset_t bit_no )           // - bit_no
;
uint_8 CPPLIB( bit_test )(      // TEST IF BIT IS ON
    uint_8* bits,               // - bits
    offset_t bit_no )           // - bit_no
;
#if 0
DISPATCHABLE CPPLIB( catch_any )// TEST IF R/W BLOCK IS DISPATCHABLE FOR ...
    ( FsExcRec* rec_exc         // - exception record
    , RW_DTREG* rw )            // - R/W block: search block
;
#endif
_WPRTLINK
void CPPLIB( catch_done )(      // COMPLETION OF CATCH
#ifdef RW_REGISTRATION
    void
#else
    RW_DTREG *rw                // - current R/W block
#endif
    )
;
void CPPLIB( corrupted_stack )(// TERMINATE, WITH CORRUPTED STACK MSG
    void )
;
#ifndef NDEBUG
int CPPLIB( cmd_active )(       // INFO CAN BE DISPLAYED ABOUT IT
    RW_DTREG* rw,               // - active r/w entry
    DTOR_CMD* cmd )             // - command in question
;
#endif
_WPRTLINK
void* CPPLIB( ctor_array_storage_1m )( // CTOR ARRAY MEMORY, SUPPLIED DELETE[]
    void* array,                    // - array memory
    unsigned count,                 // - number of elements
    RT_TYPE_SIG sig,                // - type signature for array type
    void (*op_del)() )              // - operator delete[] to be used
;
_WPRTLINK
void* CPPLIB( ctor_array_storage_1s )( // CTOR ARRAY_STORAGE, SUPPLIED DELETE[]
    ARRAY_STORAGE *base,            // - array storage
    unsigned count,                 // - number of elements
    RT_TYPE_SIG sig,                // - type signature for array type
    void (*op_del)() )              // - operator delete[] to be used
;
_WPRTLINK
void* CPPLIB( ctor_array_storage_2s )( // CTOR ARRAY_STORAGE, SUPPLIED DELETE[]
    ARRAY_STORAGE *base,            // - array storage
    unsigned count,                 // - number of elements
    RT_TYPE_SIG sig,                // - type signature for array type
    void (*op_del)() )              // - operator delete[] to be used
;
_WPRTLINK
void* CPPLIB( ctor_array_storage_gm )( // CTOR ARRAY MEMORY, ::delete[]
    void* array,                    // - array memory
    unsigned count,                 // - number of elements
    RT_TYPE_SIG sig )               // - type signature for array type
;
_WPRTLINK
void* CPPLIB( ctor_array_storage_gs )( // CTOR ARRAY_STORAGE, ::delete[]
    ARRAY_STORAGE *base,            // - array storage
    unsigned count,                 // - number of elements
    RT_TYPE_SIG sig )               // - type signature for array type
;
_WPRTLINK
void CPPLIB( destruct )(        // R/T CALL -- destruct up to state variable
#ifdef __USE_PD
    RW_DTREG* rw,               // - active r/w entry
#endif
    RT_STATE_VAR state_var )    // - value of state variable
;
_WPRTLINK
void CPPLIB( destruct_all )(    // R/T CALL -- destruct remainder of block
#ifdef __USE_PD
    RW_DTREG* rw                // - active r/w entry
#else
    void
#endif
    )
;
void CPPLIB( ctor_done )(       // R/T CALL -- ctor completed
    void )
;
void CPPLIB( destruct_internal )// DESTRUCTION FOR BLK UNTIL STATE REACHED
    ( RT_STATE_VAR state_var    // - state variable
    , RW_DTREG *rw )            // - R/W for block
;
void CPPLIB( dispatch_dummy )   // CREATE DUMMY DISPATCH BLOCK
    ( DISPATCH_EXC* dispatch    // - dispatch block
    , _RTCTL* rtc )             // - R/T control
;
DISPATCHABLE CPPLIB( dispatchable )(// TEST IF R/W BLOCK IS DISPATCHABLE
    DISPATCH_EXC *dispatch,     // - dispatch control
    RW_DTREG* rw )              // - R/W block: search block
;
void CPPLIB( dtor_free_exc )    // DESTRUCT AND FREE EXCEPTION
    ( ACTIVE_EXC* active        // - exception
    , _RTCTL* rtc )             // - R/T control
;
void CPPLIB( exc_setup )        // SETUP DISPATCH, EXCEPTION RECORDS
    ( DISPATCH_EXC* disp        // - dispatch record
    , THROW_RO* throw_ro        // - throw r/o block
    , rboolean is_zero          // - true ==> thrown object is zero constant
    , _RTCTL* rt_ctl            // - R/T control
    , void* object              // - thrown object
    , FsExcRec* rec )           // - exception record
;
ACTIVE_EXC* CPPLIB( find_active )( // FIND EXCEPTION FOR A POSITION
    _RTCTL* rtc,                // - R/T control
    RW_DTREG* rw,               // - current r/w block
    DTOR_CMD* cmd )             // - command within it
;
void CPPLIB( free_exc )(        // FREE AN EXCEPTION
    _RTCTL* rtc,                // - R/T control
    ACTIVE_EXC *active )        // - exception to be freed
;
#ifdef RW_REGISTRATION
_WPRTLINK
void CPPLIB( fun_register )(    // REGISTRATION FOR FUNCTION
    RW_DTREG* rw,               // - R/W block
    RO_DTREG* ro )              // - R/O block
;
#endif
void CPPLIB( lcl_register )(    // REGISTRATION OF LOCAL INITIALIZATION
    RW_DTREG RT_FAR *rw )       // - read/write block
;
void CPPLIB( mod_register )(    // REGISTRATION FOR MODULE INITIALIZED OBJECTS
    RW_DTREG* rw )              // - R/W block
;
#ifdef __USE_PD
struct _PDITER;
int CPPLIB( PditerInit )        // START THE ITERATION
    ( _PDITER* pdit             // - iteration block
    , void* SP_reg              // - SP register contents
    , void (*RA_reg)( void ) )  // - RA (return register) register
;
int CPPLIB( PditerUnwind )      // UNWIND ONE PROCEDURE CALL
    ( _PDITER* pdit )           // - iteration block
;
void CPPLIB( PdUnwind )         // UNWIND USING PROCEDURE DESCRIPTORS
    ( FsExcRec* exc_rec )       // - exception record
;
#endif
_WPRTLINK
void CPPLIB( rethrow )(         // RE-THROW AN EXCEPTION
    void )
;
RO_STATE* CPPLIB( stab_entry )  // GET ENTRY FOR STATE VARIABLE
    ( RO_DTREG* ro              // - R/O entry
    , RT_STATE_VAR state_var )  // - state variable
;
void CPPLIB( stab_trav_comp )   // POINT STATE-TABLE ENTRY BEYOND COMPONENT
    ( STAB_TRAVERSE* ctl )      // - control for travsersal
;
void CPPLIB( stab_trav_init )   // INITIALIZE STATE-TABLE TRAVERSAL
    ( STAB_TRAVERSE* ctl        // - control for travsersal
    , _RTCTL* rtc )             // - R/T control
;
RO_STATE* CPPLIB( stab_trav_move)( // MOVE TO NEXT ACTUAL POSITION
    STAB_TRAVERSE *traverse )   // - traversal control information
;
RO_STATE* CPPLIB( stab_trav_next )// POINT AT NEXT STATE-TABLE ENTRY
    ( STAB_TRAVERSE* ctl )      // - control for travsersal
;
_WPRTLINK
void CPPLIB( throw )(           // THROW AN EXCEPTION OBJECT
    void *object,               // - address of object
    THROW_RO *throw_ro )        // - throw R/O block
;
_WPRTLINK
void CPPLIB( throw_zero )(      // THROW AN EXCEPTION OBJECT (CONST ZERO)
    void *object,               // - address of object
    THROW_RO *throw_ro )        // - throw R/O block
;

#ifdef RW_REGISTRATION

FSREGAPI unsigned CPPLIB( fs_handler_rtn ) // HANDLER FOR FS REGISTRATIONS
    ( FsExcRec* rec_exc         // - exception record
    , RW_DTREG* rw              // - current R/W block
    , FsCtxRec* rec_ctx         // - context record
    , unsigned context          // - dispatch context
    );

THREAD_CTL* CPPLIB( fs_lookup ) // LOOK THRU FS ENTRIES FOR LAST, THREAD_CTL
    ( RW_DTREG** a_last )       // - addr[ ptr to last WATCOM entry ]
;

#endif


#ifdef PD_REGISTRATION

#ifndef NDEBUG
void CPPLIB( pd_dump_rws )      // DEBUGGING -- DUMP R/W, R/O data structure
    ( register void (*dump_rw)( RW_DTREG*// - watcom block
                     , RO_DTREG* )
    , register void (*dump_pd)( PData* ) )// - non-watcom block
;
#endif
_WPRTLINK
unsigned CPPLIB( pd_handler_rtn ) // HANDLER FOR FS REGISTRATIONS
    ( FsExcRec* rec_exc         // - exception record
    , void*                     // - SP at function entry
    , _CONTEXT* ctx             // - context record
    , PD_DISP_CTX* dctx         // - dispatch context
    )
;
THREAD_CTL* CPPLIB( pd_lookup ) // LOOK THRU FS ENTRIES FOR LAST, THREAD_CTL
    ( RW_DTREG** a_last )       // - addr[ ptr to last WATCOM entry ]
;
RW_DTREG* CPPLIB( pd_top )      // LOOK THRU PD ENTRIES FOR FIRST R/W ENTRY
    ( void )
;

#endif

#ifdef SYSIND_REGISTRATION

void CPPLIB( raise_exception )  // RAISE AN EXCEPTION
    ( FsExcRec* excrec )        // - exception record
;
void CPPLIB( unwind_global )    // GLOBAL UNWIND ROUTINE
    ( RW_DTREG* rw              // - bounding R/W block
    , uint_32                   // - return address (not used)
    , FsExcRec* excrec )        // - exception record
;

#endif

// FS REGISTRATION ONLY

#ifdef FS_REGISTRATION

#define RwTop( thr )                            \
            FsTop()

#define RwDeregister( thr )                     \
            ( FsPop()                           \
            , (thr) )

#define RwRegister( thr, rw )                   \
            ( FsPush( (rw) )                    \
            , (thr) )

#define PgmThread() CPPLIB(pgm_thread)()

THREAD_CTL* CPPLIB( pgm_thread )    // LOCATE THREAD_CTL FOR PROGRAM (.EXE)
    ( void )
;

#else

// NOT FS REGISTRATION

#define RwTop( thr )                            \
            (thr)->registered

#define RwDeregister( thr )                     \
            ( (thr)->registered = RwTop(thr)->base.prev   \
            , (thr) )

#define RwRegister( thr, rw )                   \
            ( (rw)->base.prev = (thr)->registered   \
            , (thr)->registered = (rw)              \
            , (thr) )

#define PgmThread() (&_RWD_ThreadData)

#endif

#ifndef NDEBUG

// DEBUG ONLY:

void CPPLIB( DbgRtDumpAutoDtor )( // DUMP REGISTRATION BLOCKS
    void )
;
void CPPLIB( DbgRtDumpModuleDtor )( // DUMP MODULE DTOR BLOCKS
    void )
;

#endif

}   // extern "C"

#include "rtctl.h"

#endif
