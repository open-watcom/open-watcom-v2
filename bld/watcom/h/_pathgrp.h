/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2007-2020 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  PathGroup structure declaration (for _splitpath)
*
****************************************************************************/


typedef struct pgroup {
    char    drive[_MAX_DRIVE];
    char    dir[_MAX_DIR];
    char    fname[_MAX_FNAME];
    char    ext[_MAX_EXT];
} pgroup;
