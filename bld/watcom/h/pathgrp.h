/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2007-2019 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  PathGroup structure declaration macro (for _splitpath)
*
****************************************************************************/


#define PGROUP  struct {        \
    char    drive[_MAX_DRIVE];  \
    char    dir[_MAX_DIR];      \
    char    fname[_MAX_FNAME];  \
    char    ext[_MAX_EXT];      \
}
