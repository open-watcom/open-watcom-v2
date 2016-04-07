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


#include "mad.h"
#define REG_SELECTED            1
#define REG_DESTROY             2

typedef struct {
    int                 num_possible;
    mad_reg_info        *curr_info;
    void                *curr_value;
    mad_reg_set_data    *reg_set;
    mad_modify_list     *m_list;
    mad_type_handle     th;
    unsigned            maxv;
} RegModifyData;

typedef union {
    long double     d;
    unsigned_64     i;
} InputUnion;

HWND CreateRegString( HWND parent, int x, int y, int width, int height, char *text, int id );
void UpdateRegString( HWND string, HWND list, int x, int y, int width, int height, char *text );
void SetRegStringDestroyFlag( HWND hwnd );
BOOL GetRegStringDestroyFlag( HWND hwnd );
void InitRegString(void);
void GetChildPos( HWND parent, HWND child, RECT *c_rect );
