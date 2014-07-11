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


#if _CPU == 8086 || _CPU == 386
mathfunc( "__LOG",   1, O_LOG ),
mathfunc( "__COS",   1, O_COS ),
mathfunc( "__SIN",   1, O_SIN ),
mathfunc( "__TAN",   1, O_TAN ),
mathfunc( "__SQRT",  1, O_SQRT ),
mathfunc( "__FABS",  1, O_FABS ),
mathfunc( "__POW",   2, O_POW ),
mathfunc( "__ATAN2", 2, O_ATAN2 ),
mathfunc( "__FMOD",  2, O_FMOD ),
mathfunc( "__ACOS",  1, O_ACOS ),
mathfunc( "__ASIN",  1, O_ASIN ),
mathfunc( "__ATAN",  1, O_ATAN ),
mathfunc( "__COSH",  1, O_COSH ),
mathfunc( "__EXP",   1, O_EXP ),
mathfunc( "__LOG10", 1, O_LOG10 ),
mathfunc( "__SINH",  1, O_SINH ),
mathfunc( "__TANH",  1, O_TANH ),
#endif
