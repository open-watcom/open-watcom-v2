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
* Description:  Calling convention structures and setup for front ends.
*
****************************************************************************/


#include <string.h>
#include <stdio.h>

#if defined( BY_C_FRONT_END )
    #define AUX_MEMALLOC    CMemAlloc
    #define AUX_STRALLOC    CStrSave
    #define AUX_MEMFREE     CMemFree
#elif defined( BY_CPP_FRONT_END )
    #define AUX_MEMALLOC    CMemAlloc
    #define AUX_STRALLOC    strsave
    #define AUX_MEMFREE     CMemFree
#elif defined( BY_FORTRAN_FRONT_END )
    #define AUX_MEMALLOC    FMemAlloc
    #define AUX_STRALLOC    FMemAlloc
    #define AUX_MEMFREE     FMemFree
#else
    #error "Unknown front end"
#endif


#if _INTEL_CPU

static  hw_reg_set  StackParms[] = {
    HW_D( HW_EMPTY )
};

#if _CPU == 386
static  hw_reg_set  metaWareParms[] = {
    HW_D( HW_EMPTY )
};
static  hw_reg_set  OptlinkParms[] = {
    HW_D_4( HW_EAX, HW_ECX, HW_EDX, HW_FLTS ),
    HW_D( HW_EMPTY )
};
static  hw_reg_set  FastcallParms[] = {
    HW_D( HW_ECX ), HW_D( HW_EDX ),
    HW_D( HW_EMPTY )
};
#else
static  hw_reg_set  FastcallParms[] = {
    HW_D( HW_AX ), HW_D( HW_DX ), HW_D( HW_BX ),
    HW_D( HW_EMPTY )
};
#endif

void PragmaAuxInfoInit( int flag_stdatnum )
/*****************************************/
{
    hw_reg_set  full_no_segs;
    call_class  call_type;

    HW_CAsgn( full_no_segs, HW_FULL );
    HW_CTurnOff( full_no_segs, HW_SEGS );

    call_type = WatcallInfo.cclass & FAR;

/*************************************************
 *  __fortran calling convention
 *************************************************/
    FortranInfo.objname = AUX_STRALLOC( "^" );

/*************************************************
 *  __cdecl calling convention
 *************************************************/
    CdeclInfo.cclass =    call_type |
#if _CPU == 8086
                         LOAD_DS_ON_CALL |
#endif
                         //REVERSE_PARMS |
                         CALLER_POPS |
                         //GENERATE_STACK_FRAME |
#if _CPU == 8086
                         NO_FLOAT_REG_RETURNS |
#endif
                         NO_STRUCT_REG_RETURNS |
                         ROUTINE_RETURN |
                         //NO_8087_RETURNS |
                         //SPECIAL_RETURN |
                         SPECIAL_STRUCT_RETURN;
    CdeclInfo.parms = StackParms;
    CdeclInfo.objname = AUX_STRALLOC( "_*" );

    HW_CAsgn( CdeclInfo.returns, HW_EMPTY );
    HW_CAsgn( CdeclInfo.streturn, HW_EMPTY );
    HW_TurnOn( CdeclInfo.save, full_no_segs );
    HW_CTurnOff( CdeclInfo.save, HW_FLTS );
#if _CPU == 386
    HW_CAsgn( CdeclInfo.streturn, HW_EAX );
    HW_CTurnOff( CdeclInfo.save, HW_EAX );
//    HW_CTurnOff( CdeclInfo.save, HW_EBX );
    HW_CTurnOff( CdeclInfo.save, HW_ECX );
    HW_CTurnOff( CdeclInfo.save, HW_EDX );
#else
    HW_CAsgn( CdeclInfo.streturn, HW_AX );
    HW_CTurnOff( CdeclInfo.save, HW_ABCD );
    HW_CTurnOff( CdeclInfo.save, HW_ES );
#endif

/*************************************************
 *  __pascal calling convention
 *************************************************/
    PascalInfo.cclass =   call_type |
                         REVERSE_PARMS |
                         //CALLER_POPS |
                         //GENERATE_STACK_FRAME |
                         NO_FLOAT_REG_RETURNS |
                         NO_STRUCT_REG_RETURNS |
                         //ROUTINE_RETURN |
                         //NO_8087_RETURNS |
                         //SPECIAL_RETURN |
                         SPECIAL_STRUCT_RETURN;
    PascalInfo.parms = StackParms;
    PascalInfo.objname = AUX_STRALLOC( "^" );

    HW_CAsgn( PascalInfo.returns, HW_EMPTY );
    HW_CAsgn( PascalInfo.streturn, HW_EMPTY );
    HW_TurnOn( PascalInfo.save, full_no_segs );
    HW_CTurnOff( PascalInfo.save, HW_FLTS );
#if _CPU == 386
    HW_CTurnOff( PascalInfo.save, HW_EAX );
    HW_CTurnOff( PascalInfo.save, HW_EBX );
    HW_CTurnOff( PascalInfo.save, HW_ECX );
    HW_CTurnOff( PascalInfo.save, HW_EDX );
#else
    HW_CTurnOff( PascalInfo.save, HW_ABCD );
    HW_CTurnOff( PascalInfo.save, HW_ES );
#endif


/*************************************************
 *  __stdcall calling convention
 *************************************************/
    StdcallInfo.cclass =  call_type |
                         //REVERSE_PARMS |
                         //CALLER_POPS |
                         //GENERATE_STACK_FRAME |
                         //NO_FLOAT_REG_RETURNS |
                         //NO_STRUCT_REG_RETURNS |
                         //ROUTINE_RETURN |
                         //NO_8087_RETURNS |
                         //SPECIAL_RETURN |
                         SPECIAL_STRUCT_RETURN;
    StdcallInfo.parms = StackParms;
#if _CPU == 386
    if( flag_stdatnum ) {
        StdcallInfo.objname = AUX_STRALLOC( "_*#" );
    } else {
        StdcallInfo.objname = AUX_STRALLOC( "_*" );
    }
#else
    StdcallInfo.objname = AUX_STRALLOC( "_*" );
#endif

    HW_CAsgn( StdcallInfo.returns, HW_EMPTY );
    HW_CAsgn( StdcallInfo.streturn, HW_EMPTY );
    HW_TurnOn( StdcallInfo.save, full_no_segs );
    HW_CTurnOff( StdcallInfo.save, HW_FLTS );
#if _CPU == 386
//    HW_CAsgn( StdcallInfo.streturn, HW_EAX );
    HW_CTurnOff( StdcallInfo.save, HW_EAX );
//    HW_CTurnOff( StdcallInfo.save, HW_EBX );
    HW_CTurnOff( StdcallInfo.save, HW_ECX );
    HW_CTurnOff( StdcallInfo.save, HW_EDX );
#else
    HW_CAsgn( StdcallInfo.streturn, HW_AX );
    HW_CTurnOff( StdcallInfo.save, HW_ABCD );
    HW_CTurnOff( StdcallInfo.save, HW_ES );
#endif


/*************************************************
 *  __fastcall calling convention
 *************************************************/
#if _CPU == 386
    FastcallInfo.cclass = call_type |
                         //REVERSE_PARMS |
                         //CALLER_POPS |
                         //GENERATE_STACK_FRAME |
                         //NO_FLOAT_REG_RETURNS |
                         //NO_STRUCT_REG_RETURNS |
                         //ROUTINE_RETURN |
                         //NO_8087_RETURNS |
                         //SPECIAL_RETURN |
                         SPECIAL_STRUCT_RETURN;
    FastcallInfo.parms = FastcallParms;
    FastcallInfo.objname = AUX_STRALLOC( "@*#" );

    HW_CAsgn( FastcallInfo.returns, HW_EMPTY );
    HW_CAsgn( FastcallInfo.streturn, HW_EMPTY );
    HW_TurnOn( FastcallInfo.save, full_no_segs );
    HW_CTurnOff( FastcallInfo.save, HW_FLTS );
    HW_CTurnOff( FastcallInfo.save, HW_EAX );
//    HW_CTurnOff( FastcallInfo.save, HW_EBX );
    HW_CTurnOff( FastcallInfo.save, HW_ECX );
    HW_CTurnOff( FastcallInfo.save, HW_EDX );
#else
    FastcallInfo.cclass = call_type |
                         PARMS_PREFER_REGS |
                         //REVERSE_PARMS |
                         //CALLER_POPS |
                         //GENERATE_STACK_FRAME |
                         //NO_FLOAT_REG_RETURNS |
                         //NO_STRUCT_REG_RETURNS |
                         //ROUTINE_RETURN |
                         //NO_8087_RETURNS |
                         //SPECIAL_RETURN |
                         SPECIAL_STRUCT_RETURN;
    FastcallInfo.parms = FastcallParms;
    FastcallInfo.objname = AUX_STRALLOC( "@*" );

    HW_CAsgn( FastcallInfo.returns, HW_EMPTY );
    HW_CAsgn( FastcallInfo.streturn, HW_EMPTY );
    HW_TurnOn( FastcallInfo.save, full_no_segs );
    HW_CTurnOff( FastcallInfo.save, HW_FLTS );
    HW_CTurnOff( FastcallInfo.save, HW_ABCD );
    HW_CTurnOff( FastcallInfo.save, HW_ES );
#endif

/*************************************************
 *  _Optlink calling convention
 *************************************************/
    OptlinkInfo.cclass =  call_type |
#ifdef PARMS_STACK_RESERVE
                         PARMS_STACK_RESERVE |
#endif
                         //REVERSE_PARMS |
                         CALLER_POPS  |
                         //GENERATE_STACK_FRAME |
                         //NO_FLOAT_REG_RETURNS |
                         NO_STRUCT_REG_RETURNS |
                         //ROUTINE_RETURN |
                         //NO_8087_RETURNS |
                         //SPECIAL_RETURN |
                         SPECIAL_STRUCT_RETURN;
#if _CPU == 386
    OptlinkInfo.parms = OptlinkParms;
#else
    OptlinkInfo.parms = StackParms;
#endif
    OptlinkInfo.objname = AUX_STRALLOC( "*" );

    HW_CAsgn( OptlinkInfo.returns, HW_EMPTY );
//    HW_CAsgn( OptlinkInfo.returns, HW_FLTS );
    HW_CAsgn( OptlinkInfo.streturn, HW_EMPTY );
    HW_TurnOn( OptlinkInfo.save, full_no_segs );
    HW_CTurnOff( OptlinkInfo.save, HW_FLTS );
#if _CPU == 386
    HW_CTurnOff( OptlinkInfo.save, HW_EAX );
//    HW_CTurnOff( OptlinkInfo.save, HW_EBX );
    HW_CTurnOff( OptlinkInfo.save, HW_ECX );
    HW_CTurnOff( OptlinkInfo.save, HW_EDX );
#else
    HW_CTurnOff( OptlinkInfo.save, HW_ABCD );
    HW_CTurnOff( OptlinkInfo.save, HW_ES );
#endif

/*************************************************
 *  __syscall calling convention
 *************************************************/
    SyscallInfo.cclass =  call_type |
                         //REVERSE_PARMS |
                         CALLER_POPS |
                         //GENERATE_STACK_FRAME |
                         //NO_FLOAT_REG_RETURNS |
                         NO_STRUCT_REG_RETURNS |
                         //ROUTINE_RETURN |
                         //NO_8087_RETURNS |
                         //SPECIAL_RETURN |
                         SPECIAL_STRUCT_RETURN;
    SyscallInfo.parms = StackParms;
    SyscallInfo.objname = AUX_STRALLOC( "*" );

    HW_CAsgn( SyscallInfo.returns, HW_EMPTY );
    HW_CAsgn( SyscallInfo.streturn, HW_EMPTY );
    HW_TurnOn( SyscallInfo.save, full_no_segs );
    HW_CTurnOff( SyscallInfo.save, HW_FLTS );
#if _CPU == 386
    HW_CTurnOff( SyscallInfo.save, HW_EAX );
//    HW_CTurnOff( SyscallInfo.save, HW_EBX );
    HW_CTurnOff( SyscallInfo.save, HW_ECX );
    HW_CTurnOff( SyscallInfo.save, HW_EDX );
#else
    HW_CTurnOff( SyscallInfo.save, HW_ABCD );
    HW_CTurnOff( SyscallInfo.save, HW_ES );
#endif

#if _CPU == 386
/****************************************************
 *  OS/2 32-bit->16-bit calling convention ( _Far16 )
 ****************************************************/
    /* these are internal, and will never be pointed to by
     * an aux_entry, so we don't have to worry about them
     */

    Far16CdeclInfo = CdeclInfo;
    Far16CdeclInfo.cclass |= FAR16_CALL;
    Far16CdeclInfo.parms = StackParms;
    Far16CdeclInfo.objname = AUX_STRALLOC( CdeclInfo.objname );
    // __far16 __cdecl depends on EBX being trashed in __cdecl
    // but NT 386 __cdecl preserves EBX
    HW_CTurnOff( Far16CdeclInfo.save, HW_EBX );

    Far16PascalInfo = PascalInfo;
    Far16PascalInfo.cclass |= FAR16_CALL;
    Far16PascalInfo.parms = StackParms;
    Far16PascalInfo.objname = AUX_STRALLOC( PascalInfo.objname );
#endif
}

#if _CPU == 386
void SetAuxStackConventions( void )
/*********************************/
{
    WatcallInfo.cclass &= ( GENERATE_STACK_FRAME | FAR );
    WatcallInfo.cclass |= CALLER_POPS | NO_8087_RETURNS;
    WatcallInfo.parms = metaWareParms;
    HW_CTurnOff( WatcallInfo.save, HW_EAX );
    HW_CTurnOff( WatcallInfo.save, HW_EDX );
    HW_CTurnOff( WatcallInfo.save, HW_ECX );
    HW_CTurnOff( WatcallInfo.save, HW_FLTS );
    WatcallInfo.objname = AUX_STRALLOC( "*" );
}
#endif

#endif

int IsAuxParmsBuiltIn( hw_reg_set *parms )
/***************************************/
{
    if( parms == DefaultParms ) {
        return( TRUE );
#if _INTEL_CPU
    } else if( parms == StackParms ) {
        return( TRUE );
    } else if( parms == FastcallParms ) {
        return( TRUE );
#if _CPU == 386
    } else if( parms == OptlinkParms ) {
        return( TRUE );
    } else if( parms == metaWareParms ) {
        return( TRUE );
#endif
#endif
    } else if( parms == NULL ) {
        return( TRUE );
    } else {
        return( FALSE );
    }
}

void SetAuxWatcallInfo( void )
/****************************/
{
    DftCallConv         = &WatcallInfo;

    WatcallInfo.cclass  = 0;
    WatcallInfo.code    = NULL;
    WatcallInfo.parms   = DefaultParms;
#if _CPU == 370
    WatcallInfo.linkage = &DefaultLinkage;
#endif
    HW_CAsgn( WatcallInfo.returns, HW_EMPTY );
    HW_CAsgn( WatcallInfo.streturn, HW_EMPTY );
    HW_CAsgn( WatcallInfo.save, HW_FULL );
    WatcallInfo.use     = 0;
    WatcallInfo.objname = NULL;
}

void SetAuxDefaultInfo( void )
/****************************/
{
    DefaultInfo = *DftCallConv;
    if( DefaultInfo.objname != NULL ) {
        DefaultInfo.objname = AUX_STRALLOC( DefaultInfo.objname );
    }
}

int IsAuxInfoBuiltIn( struct aux_info *inf )
/*****************************************/
{
    if( inf == &DefaultInfo )
        return( TRUE );
    if( inf == &WatcallInfo )
        return( TRUE );
    if( inf == &CdeclInfo )
        return( TRUE );
    if( inf == &PascalInfo )
        return( TRUE );
    if( inf == &FortranInfo )
        return( TRUE );
    if( inf == &SyscallInfo )
        return( TRUE );
    if( inf == &StdcallInfo )
        return( TRUE );
    if( inf == &FastcallInfo )
        return( TRUE );
    if( inf == &OptlinkInfo )
        return( TRUE );
#if _CPU == 386
    if( inf == &Far16PascalInfo )
        return( TRUE );
    if( inf == &Far16CdeclInfo )
        return( TRUE );
#endif
    return( FALSE );
}

/* Variables need name mangling different from functions in many cases
 * (__pascal, __stdcall, etc.). Note that _System and _Optlink do not
 * decorate variable names. This is required for OS/2 SOM support to work
 * and useful for interoperability with IBM compilers.
 * NB: WatcallInfo.objname value depends on the -3r/3s switch.
 */
char *VarNamePattern( struct aux_info *inf )
/******************************************/
{
    if( inf == &DefaultInfo )
        inf = DftCallConv;
    if( inf == &WatcallInfo )
        return( WatcallInfo.objname );
    if( inf == &CdeclInfo || inf == &StdcallInfo || inf == &FastcallInfo )
        return( "_*" );
    if( inf == &PascalInfo  || inf == &FortranInfo )
        return( "^" );
    if( inf == &SyscallInfo || inf == &OptlinkInfo )
        return( "*" );
    return( inf->objname );
}
