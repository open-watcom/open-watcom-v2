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
*****************************************************************************/


PICK( ENUM_NONE,        enum_NONE )                 // no qualifiers to enumerate instructions
PICK( ENUM_OF_ADDL,     enum_OF_ADDL )              // operate format instructions such as ADDL
PICK( ENUM_DTI_CVTQL,   enum_DTI_CVTQL )            // Data Type Independent FP instructions such as CVTQL
PICK( ENUM_IEEE_ADDS,   enum_IEEE_ADDS_or_CVTTQ )   // IEEE FP instructions such as ADDS
PICK( ENUM_IEEE_CMPTEQ, enum_IEEE_CMPTEQ )          // IEEE FP instructions such as CMPTEQ
PICK( ENUM_IEEE_CVTQS,  enum_IEEE_CVTQS )           // IEEE FP instructions such as CVTQS
PICK( ENUM_IEEE_CVTTQ,  enum_IEEE_ADDS_or_CVTTQ )   // IEEE FP instructions such as CVTTQ
