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
* Description:  define names and codes of run-time functions
*
****************************************************************************/


//
//  use: rtfuncod.h -- define codes of run-time functions
//

// module initialization
                                            // FUNCTIONS:
// static initialization

  RTFUN( RTF_STATIC_INIT,static_init )      // - -bm static init semaphore

// C Library

, RTFNC( RTF_SETJMP    ,_setjmp   )         // - C: setjmp

// exception handling

, RTFUN( RTF_THROW     ,throw )             // - throw expression (~const 0)
, RTFUN( RTF_RETHROW   ,rethrow )           // - throw;
, RTFUN( RTF_CATCH_END ,catch_done )        // - catch completed
, RTFUN( RTF_THROW_ZERO,throw_zero )        // - throw expression (const 0)

// registration

#if _CPU == _AXP
, RTFUN( RTF_REG_FUN   ,0 )                 // - not used on ALPHA
#else
, RTFUN( RTF_REG_FUN   ,fun_register )      // - register a function
#endif
#if _CPU == _AXP
, RTFUN( RTF_DEREGISTER,0 )                 // - not used on ALPHA
#else
, RTFUN( RTF_DEREGISTER,base_deregister )   // - de-register one block
#endif
, RTFUN( RTF_REG_LCL   ,lcl_register )      // - register static init (local)

// data

//RTDAT( RTF_INLINE_FREG,stab_init_fun )    // - initialization for fun. reg.
, RTDAT( RTF_LONGJMP_REF,init_longjmp )     // - initialization for longjmp
, RTDAT( RTF_UNDEF_DATA,undefined_member_data ) //   static member data
, RTDAT( RTF_MOD_DTOR  ,module_dtor_ref )   // - DTOR module statics

// assignment

, RTFUN( RTF_ASSIGN_ARR,assign_array )      // - assign array

// construction

, RTFUN( RTF_CTAS_GS,ctor_array_storage_gs )// - CTOR ARRAY_STORAGE: gbl
, RTFUN( RTF_CTAS_GM,ctor_array_storage_gm )// - CTOR array memory: gbl
, RTFUN( RTF_CTAS_1S,ctor_array_storage_1s )// - CTOR ARRAY_STORAGE: 1 delete
, RTFUN( RTF_CTAS_1M,ctor_array_storage_1m )// - CTOR array memory: 1 delete
, RTFUN( RTF_CTAS_2S,ctor_array_storage_2s )// - CTOR ARRAY_STORAGE: 2 delete
, RTFUN( RTF_COPY_ARR  ,copy_array )        // - copy array elements
, RTFUN( RTF_COPY_VARR ,copy_varray )       // - copy array elements (virt.)
, RTFUN( RTF_CTOR_ARR  ,ctor_array )        // - array ctor()
, RTFUN( RTF_CTOR_VARR ,ctor_varray)        // - array ctor(,0)

// destruction

, RTFUN( RTF_DTOR_AR_STORE,dtor_array_store )//- DTOR ARRAY_STORAGE
, RTFUN( RTF_DTOR      ,destruct )          // - destruct to state value
, RTFUN( RTF_DTOR_ALL  ,destruct_all )      // - destruct to 0
, RTFUN( RTF_DTOR_ARR  ,dtor_array )        // - DTOR array elements

// error routines

, RTFUN( RTF_PURE_VIRT ,pure_error )        // - pure virtual call error
, RTFUN( RTF_UNDEF_CDTOR, undefed_cdtor )   // - undefined ctor,dtor
, RTFUN( RTF_UNDEF_VFUN,undef_vfun )        // - err rtn: called del'd virt.
, RTFUN( RTF_UNDEF_MEMBER,undefined_member_function )
                                            // - debug rtn: address of undefined
                                            //   member function

// fs registration routines/data

, RTDAT( RTD_FS_ROOT, init_fs_root )        // - fs root initialization
#if _CPU == _AXP
, RTFUN( RTF_PD_HANDLER, pd_handler )       // - pd handler (indirected)
#else
, RTFUN( RTF_FS_HANDLER, fs_handler )       // - fs handler (indirected)
#endif
, RTFNP( RTF_FS_PUSH, .FsPush )             // - #pragma push fs:0
, RTFNP( RTF_FS_POP,  .FsPop  )             // - #pragma pop fs:0
, RTFNP( RTD_TS_GENERIC, __compiled_under_generic ) // - checker xref: generic
, RTFNP( RTD_TS_OS2,     __compiled_under_OS2     ) // - checker xref: OS/2
, RTFNP( RTD_TS_NT,      __compiled_under_NT      ) // - checker xref: NT
#if _CPU == _AXP
, RTFUN( RTF_PD_HANDLER_RTN, pd_handler_rtn )//- fs handler (actual)
#else
, RTFUN( RTF_FS_HANDLER_RTN, fs_handler_rtn )//- fs handler (actual)
#endif

// RTTI run-time support

, RTFUN( RTF_DYN_CAST_PTR, dcptr )          // - dynamic_cast<? cv *>(p)
, RTFUN( RTF_DYN_CAST_REF, dcref )          // - dynamic_cast<? cv &>(r)
, RTFUN( RTF_DYN_CAST_VOID, dcvoid )        // - dynamic_cast<void cv *>(p)
, RTFUN( RTF_GET_TYPEID, gettid )           // - typeid(<lvalue>)

// add new codes here


, RTFUN( RTF_LAST, last )                   // - last function index
