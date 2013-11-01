/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2011-2013 The Open Watcom Contributors. All Rights Reserved.
* Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
* Copyright (c) 1987-1992 Rational Systems, Incorporated. All Rights Reserved.
*
* =========================================================================
*
* Description:  loader interface
*
****************************************************************************/

#define LOADER_INIT(lv)         ((lv)->loader_actions[0])
#define LOADER_LOAD(lv)         ((lv)->loader_actions[1])
#define LOADER_REL(lv)          ((lv)->loader_actions[2])
#define LOADER_UNREL(lv)        ((lv)->loader_actions[3])
#define LOADER_UNLOAD(lv)       ((lv)->loader_actions[4])
#define LOADER_FREEINFO(lv)     ((lv)->loader_actions[5])
#define LOADER_CANLOAD(lv)      ((lv)->loader_actions[6])
#define LOADER_GETLOADTABLE(lv) ((lv)->loader_actions[7])
#define LOADER_GETLOADNAME(lv)  ((lv)->loader_actions[8])

typedef struct loader_vector {
    PACKAGE FarPtr  loader_package;
    ACTION          *loader_actions[9];
} LOADER_VECTOR;

typedef long LoaderCookie;

extern int  loader_bind( char FarPtr package_name, LOADER_VECTOR *lv );
