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


#ifdef __DEFINE_FNOVRESULT
#define result_pick( name, value )      name = value,
#undef __DEFINE_FNOVRESULT
#else
#define result_pick( name, value )      __STR( name ),
#endif
result_pick( FNOV_NONAMBIGUOUS,        0x00 )// is nonambiguous
result_pick( FNOV_AMBIGUOUS,           0x01 )// is ambiguous
result_pick( FNOV_NO_MATCH,            0x02 )// cannot be made
result_pick( FNOV_EXACT_MATCH,         0x03 )// is identical to existing
result_pick( FNOV_NOT_DISTINCT,        0x04 )// is not distinct from existing
result_pick( FNOV_NOT_DISTINCT_RETURN, 0x05 )// not distinct, by return type
result_pick( FNOV_DISTINCT,            0x06 )// is distinct from existing
result_pick( FNOV_ERR,                 0x07 )// error occurred
#undef result_pick
