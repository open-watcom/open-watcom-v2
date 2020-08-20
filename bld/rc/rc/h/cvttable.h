/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2020-2020 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  declaration for conversion tool to convert Japanese text files
*                  Shift-JIS Windows CP932 <-> UTF-8
*
****************************************************************************/


typedef struct cvt_chr {
    unsigned short  s;
    unsigned short  u;
} cvt_chr;
