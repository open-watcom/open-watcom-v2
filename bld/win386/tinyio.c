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


#include <dos.h>
#include "watcom.h"
#include "tinyio.h"
#include "dpmi.h"

/*
 * different pragma for fortran than in stddef.h to allow structures
 * to be returned in registers (namely the 4 byte ones returned by
 * TinyGetDate/TinyGetTime...)
 */
 #pragma aux fortran "^" parm routine reverse []\
   modify [ax bx cx dx es];

static unsigned short DTA=0;
extern unsigned short DPL;

void far fortran __nTinySetDTA( void far *a )
{
    long                rc;
    unsigned long       base;

    /*
     * get a DTA descriptor that will survive across the 16/32 bit
     * interface
     */
    if( DTA == 0 ) {
        rc = DPMIAllocateLDTDescriptors( 1 );
        if( rc <= 0 ) return;
        /*
         * set up a basic DTA descriptor
         */
        DTA = (unsigned short) rc;
        DPMISetSegmentLimit( DTA, sizeof(tiny_find_t) );
        DPMISetDescriptorAccessRights( DTA, DPL+ACCESS_DATA16 );
    }

    base = DPMIGetSegmentBaseAddress( FP_SEG( a ) );
    base += FP_OFF( a );
    DPMISetSegmentBaseAddress( DTA, base );
    _fTinySetDTA( MK_FP( DTA,0) );
}
tiny_ret_t far fortran __nTinyAccess( char far *pathname, unsigned pmode ){
    return( _fTinyAccess( pathname, pmode ));
}
tiny_ret_t far fortran __nTinyOpen( char far *name, open_attr mode ){
    return( _fTinyOpen( name, mode ));
}
tiny_ret_t far fortran __nTinyCreate( char far *name, create_attr attr ){
    return( _fTinyCreate( name, attr ));
}
tiny_ret_t far fortran __TinyClose( tiny_handle_t a ){
    return( _TinyClose( a ));
}
tiny_ret_t far fortran __nTinyWrite( tiny_handle_t a, void far *b, unsigned c ){
    return( _fTinyWrite( a, b, c ));
}
tiny_ret_t far fortran __nTinyRead( tiny_handle_t a, void far *b, unsigned c ){
    return( _fTinyRead( a, b, c ));
}
tiny_ret_t far fortran __TinySeek( tiny_handle_t a, unsigned long b, seek_info mode ){
    return( _TinySeek( a, b, mode ));
}
tiny_ret_t far fortran __nTinyDelete( char far *a ){
    return( _fTinyDelete( a ));
}
tiny_ret_t far fortran __nTinyRename( char far *old,char far *new){
    return( _fTinyRename( old,new));
}
tiny_ret_t far fortran __nTinyMakeDir( char far *name ){
    return( _fTinyMakeDir( name ));
}
tiny_ret_t far fortran __nTinyRemoveDir( char far *name ){
    return( _fTinyRemoveDir( name ));
}
tiny_ret_t far fortran __nTinyChangeDir( char far *name ){
    return( _fTinyChangeDir( name ));
}
tiny_ret_t far fortran __TinyDup( tiny_handle_t a ){
    return( _TinyDup( a ));
}
tiny_ret_t far fortran __TinyDup2( tiny_handle_t exist, tiny_handle_t alias ){
    return( _TinyDup2( exist, alias ));
}
tiny_ret_t far fortran __TinyAllocBlock( paras ){
    return( _TinyAllocBlock( paras ));
}
tiny_ret_t far fortran __TinyTestAllocBlock( paras ){
    return( _TinyTestAllocBlock( paras ));
}
unsigned far fortran __TinyMaxAlloc( void ){
    return( _TinyMaxAlloc());
}
tiny_ret_t far fortran __TinyFreeBlock( unsigned seg ){
    return( _TinyFreeBlock( seg ));
}
tiny_ret_t far fortran __TinySetBlock( unsigned paras, unsigned seg ){
    return( _TinySetBlock( paras, seg ));
}
unsigned far fortran __TinyMaxSet( unsigned seg ){
    return( _TinyMaxSet( seg ));
}
tiny_ret_t far fortran __nTinyGetCWDir( char far *buff, uint_8 drive ){
    return( _fTinyGetCWDir( buff, drive ));
}
tiny_ret_t far fortran __TinyGetDeviceInfo( tiny_handle_t dev ){
    return( _TinyGetDeviceInfo( dev ));
}
tiny_ret_t far fortran __TinySetDeviceInfo( tiny_handle_t dev, uint_8 info){
    return( _TinySetDeviceInfo( dev, info));
}
uint_8 far fortran __TinyGetCtrlBreak( void ){
    return( _TinyGetCtrlBreak( ));
}
void far fortran __TinySetCtrlBreak( uint_8 new_setting ){
    _TinySetCtrlBreak( new_setting );
}
void far fortran __TinyTerminateProcess( int ret_code ){
    _TinyTerminateProcess( ret_code );
}
tiny_date_t far fortran __TinyGetDate( void ){
    return( _TinyGetDate( ));
}
tiny_time_t far fortran __TinyGetTime( void ){
    return( _TinyGetTime( ));
}
uint_8 far fortran __TinyGetCurrDrive( void ){
    return( _TinyGetCurrDrive());
}
void far fortran __TinySetCurrDrive( uint_8 a ){
    _TinySetCurrDrive( a );
}
tiny_ret_t far fortran __nTinyFindFirst(char far *pattern,create_attr attr){
    return( _fTinyFindFirst(pattern,attr));
}
tiny_ret_t far fortran __TinyFindNext( void ){
    return( _TinyFindNext( ));
}
tiny_ret_t far fortran __TinyGetFileStamp( fd ){
    return( _TinyGetFileStamp( fd ));
}
#ifdef __OLD_TINYIO_
tiny_ret_t far fortran __TinySetFileStamp( tiny_handle_t fd, unsigned p_hms, unsigned p_ymd ){
    return( _TinySetFileStamp( fd, p_hms, p_ymd ));
}
#else
tiny_ret_t far fortran __TinySetFileStamp( tiny_handle_t fd, tiny_ftime_t hms, tiny_fdate_t ymd ){
    return( _TinySetFileStamp( fd, hms, ymd ));
}
#endif
tiny_ret_t far fortran __nTinyGetFileAttr( char far *file ){
    return( _fTinyGetFileAttr( file ));
}
tiny_ret_t far fortran __nTinySetFileAttr( char far *file, create_attr a ){
    return( _fTinySetFileAttr( file, a ));
}
void far fortran __TinySetIntr( uint_8 a, unsigned b){
    _TinySetIntr( a, b );
}
tiny_dos_version far fortran __TinyDOSVersion( void ){
    return( _TinyDOSVersion());
}
char far fortran __TinyGetCH( void ){
    return( _TinyGetCH());
}
char far fortran __TinyGetCHE( void ){
    return( _TinyGetCHE());
}
char far fortran __TinyGetSwitchChar( void ){
    return( _TinyGetSwitchChar());
}
void far fortran __TinySetSwitchChar( a ){
    _TinySetSwitchChar( a );
}
uint_32 far fortran __TinyFreeSpace( uint_8 a){
    return( _TinyFreeSpace( a ));
}
tiny_ret_t far fortran __nTinyGetCountry( tiny_country_info far *a ){
    return( _fTinyGetCountry( a ));
}
tiny_ret_t far fortran __TinySetCountry( uint_16 a ){
    return( _TinySetCountry( a ));
}
tiny_ret_t far fortran __nTinyFCBPrsFname( char far *str, tiny_fcb_t far *fcb, uint_8 ctrl ){
    return( _fTinyFCBPrsFname( str, fcb, ctrl ));
}
tiny_ret_t far fortran __nTinyFCBDeleteFile( tiny_fcb_t far *fcb ){
    return( _fTinyFCBDeleteFile( fcb ));
}
tiny_ret_t far fortran __TinyLock(tiny_handle_t a, unsigned long start, unsigned long length ){
    return( _TinyLock(a, start, length ));
}
tiny_ret_t far fortran __TinyUnlock( tiny_handle_t a, unsigned long start, unsigned long length ){
    return( _TinyUnlock( a, start, length ) );
}
void far fortran __TinySetVect( uint_8 a, void far *func ){
    _TinySetVect( a, func );
}
void far * far fortran __TinyGetVect( uint_8 a ) {
    return( _TinyGetVect( a) );
}
