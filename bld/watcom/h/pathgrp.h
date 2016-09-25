/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2007-2016 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  PathGroup structure declaration macro
*
****************************************************************************/


#define PGROUP  struct {    \
    char    *drive;         \
    char    *dir;           \
    char    *fname;         \
    char    *ext;           \
    char    buffer[PATH_MAX + 4];   \
}
