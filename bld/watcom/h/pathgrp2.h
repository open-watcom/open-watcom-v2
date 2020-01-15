/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2007-2019 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  PathGroup2 structure declaration macro (for _splitpath2)
*
****************************************************************************/


#define PGROUP2 struct \
{ \
    char    *drive;                 \
    char    *dir;                   \
    char    *fname;                 \
    char    *ext;                   \
    char    buffer[PATH_MAX + 4];   \
}
