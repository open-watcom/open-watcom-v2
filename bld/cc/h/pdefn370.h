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


#define NC '\0'

struct  aux_info        InlineInfo;

hw_reg_set DefaultParms[] = {
        HW_D( HW_EMPTY )
};

linkage_regs DefaultLinkage = {
        HW_D( HW_G10 ),  /* GP */
        HW_D( HW_G12 ),  /* SP */
        HW_D( HW_G11 ),  /* LN */
        HW_D( HW_G13 ),  /* RA */
        HW_D( HW_G1 ),   /* PR */
        HW_D( HW_G13 )   /* SA */
};

linkage_regs OSLinkage[] = {
        HW_D( HW_G10 ),  /* GP */
        HW_D( HW_G12 ),  /* SP */
        HW_D( HW_G15 ),  /* LN */
        HW_D( HW_G14 ),  /* RA */
        HW_D( HW_G1 ),   /* PR */
        HW_D( HW_G13 )   /* SA */
};

#define NC '\0'
char Registers[] = {            /* table for TableLookup*/
            '0',NC,
            '1',NC,
            '2',NC,
            '3',NC,
            '4',NC,
            '5',NC,
            '6',NC,
            '7',NC,
            '8',NC,
            '9',NC,
        '1','0',NC,
        '1','1',NC,
        '1','2',NC,
        '1','3',NC,
        '1','4',NC,
        '1','5',NC,
        NC
};

hw_reg_set RegBits[] ={
        HW_D( HW_G0 ),
        HW_D( HW_G1 ),
        HW_D( HW_G2 ),
        HW_D( HW_G3 ),
        HW_D( HW_G4 ),
        HW_D( HW_G5 ),
        HW_D( HW_G6 ),
        HW_D( HW_G7 ),
        HW_D( HW_G8 ),
        HW_D( HW_G9 ),
        HW_D( HW_G10 ),
        HW_D( HW_G11 ),
        HW_D( HW_G12 ),
        HW_D( HW_G13 ),
        HW_D( HW_G14 ),
        HW_D( HW_G15 ),
 };

hw_reg_set DefaultVarParms[] = {
        HW_D( HW_EMPTY )
};
