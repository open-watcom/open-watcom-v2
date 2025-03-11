/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Interface to the setupinf module.
*
****************************************************************************/


#include <time.h>
#include <stdarg.h>
#include "vhandle.h"

// Possible timing for Spawn execution
typedef enum {
    WHEN_BEFORE,
    WHEN_AFTER,
    WHEN_END
} when_time;

typedef enum {
    DOING_DEFAULTS,
    PRESCAN_FILE,
    FINAL_SCAN,
} pass_type;

typedef enum {
    AM_OVERWRITE,
    AM_BEFORE,
    AM_AFTER
} append_mode;

typedef unsigned long long      disk_size;
typedef long long               disk_ssize;

// Possible return values from sending the SIM_INITIALIZE message.
typedef enum {
    SIM_INIT_NOERROR,
    SIM_INIT_NOMEM,
    SIM_INIT_NOFILE
} SIM_INITIALIZE_ERROR;

// Defines used by the SETUP program.
#define MAXBUF          128
#define MAXVALUE        256
#define MAXENVVAR       2048


typedef enum {
    NULL_TYPE,
    STRING_TYPE
} VBL_TYPE;

typedef enum {
    DLG_CANCEL,
    DLG_NEXT,
    DLG_PREV,
    DLG_START,
    DLG_SAME,
    DLG_SKIP,
    DLG_DONE,
} dlg_state;

typedef struct control_info_ext {
    char        *pVisibilityConds;
} control_info_ext;

#define MAX_VARS 75
typedef struct a_dialog_header {
    char                        *name;                  /* name of dlg */
    char                        *condition;
    int                         rows;
    int                         cols;
    int                         num_push_buttons;
    int                         num_controls;
    bool                        def_dlg;
    bool                        defaults_set;
    bool                        adjusted;
    gui_control_info            *controls;              /* controls associated with dlg */
    control_info_ext            *controls_ext;          /* controls ext.info associated with dlg */
    vhandle                     pVariables[MAX_VARS];   /* pointer to variables */
    char                        *pConditions[MAX_VARS]; /* pointer to initial value */
    gui_rect                    original_rect;          /* original window position (used in UpdateControlVisibility() )*/
    gui_ord                     height_change;          /* how much dialog height has changed (used in UpdateControlVisibiliy() ) */
    struct a_dialog_header      *next;                  /* next dialog box */
    struct a_dialog_header      *prev;                  /* prev dialog box */
    dlg_state                   ret_val;
    char                        *title;
    vhandle                     any_check;
} a_dialog_header;

#define HEIGHT_BORDER  1
#define WIDTH_BORDER   4

#define DUMMY_VAR_SIZE 25


extern void             CheckStateVars( void );
extern const char       *SimGetTargetFullPath( int parm, VBUF *buff );
extern bool             SimFileAdd( int parm );
extern bool             SimFileUpToDate( int parm );
extern bool             SimFileRemove( int parm );
extern int              SimNumSubFiles( int parm );
extern int              SimPermanentFile( int parm );
extern bool             SimSubFileExists( int parm, int subfile );
extern void             SimSubFileName( int parm, int subfile, VBUF *buff );
extern bool             SimSubFileInNewDir( int parm, int subfile );
extern bool             SimSubFileInOldDir( int parm, int subfile );
extern time_t           SimSubFileDate( int parm, int subfile );
extern bool             SimSubFileNewer( int parm, int subfile );
extern bool             SimSubFileExecutable( int parm, int subfile );
extern bool             SimSubFileTextCRLF( int parm, int subfile );
extern bool             SimSubFileIsNLM( int parm, int subfile );
extern bool             SimSubFileIsDLL( int parm, int subfile );
extern bool             SimSubFileReadOnly( int parm, int subfile );
extern long             SimSubFileSize( int parm, int subfile );
extern long             SimInit( const VBUF *inf_name );
extern void             SimSetTargetTempDisk( int parm, const char *fs_path );
extern const char       *SimGetTargetTempDisk( int parm );
extern int              SimGetTargetNumFiles( int parm );
extern int              SimNumTargets( void );
extern disk_ssize       SimTargetSpaceNeeded( int parm );
extern void             SimTargetDir( int i, VBUF *buff );
extern void             SimTargetDirName( int i, VBUF *buff );
extern int              SimNumDirs( void );
extern void             SimDirNoEndSlash( int parm, VBUF *buff );
extern bool             SimDirUsed( int parm );
extern int              SimDirTargetNum( int parm );
extern int              SimDirParent( int parm );
extern void             SimGetDir( int parm, VBUF *buff );
extern int              SimNumFiles( void );
extern void             SimGetFileDesc( int parm, VBUF *buff );
extern void             SimGetFileName( int parm, VBUF *buff );
extern long             SimFileSize( int parm );
extern bool             SimFileOldDir( int parm, VBUF *buff );
extern void             SimFileDir( int parm, VBUF *buff );
extern int              SimFileDirNum( int parm );
extern bool             SimCheckFileCondition( int parm );
extern bool             SimCheckFileInitialCondition( int parm );
extern void             SimGetPMApplGroupFile( VBUF *buff );
extern void             SimGetPMApplGroupIconFile( VBUF *buff );
extern void             SimGetPMApplGroupName( VBUF *buff );
extern bool             SimIsPMApplGroupDefined( void );
extern int              SimGetPMsNum( void );
extern int              SimGetPMProgInfo( int parm, VBUF *buff );
extern bool             SimPMIsShadow( int parm );
extern bool             SimPMIsGroup( int parm );
extern void             SimGetPMParms( int parm, VBUF *buff );
extern void             SimGetPMDesc( int parm, VBUF *buff );
extern int              SimGetPMIconInfo( int parm, VBUF *iconfile, int *iconindex );
extern bool             SimCheckPMCondition( int parm );
extern void             SimGetPMGroupName( int parm, VBUF *buff );
extern void             SimGetPMGroupFile( int parm, VBUF *buff );
extern int              SimGetPMGroupsNum( void );
extern int              SimNumProfile( void );
extern void             SimProfInfo( int parm, VBUF *app_name, VBUF *key_name, VBUF *value, VBUF *file_name, VBUF *hive_name );
extern bool             SimCheckProfCondition( int parm );
extern int              SimNumAutoExec( void );
extern append_mode      SimGetAutoExecStrings( int parm, VBUF *new_var, VBUF *buff );
extern bool             SimCheckAutoExecCondition( int parm );
extern int              SimNumConfig( void );
extern append_mode      SimGetConfigStrings( int parm, VBUF *new_var, VBUF *buff );
extern bool             SimCheckConfigCondition( int parm );
extern int              SimNumEnvironment( void );
extern bool             SimCheckEnvironmentCondition( int parm );
extern append_mode      SimGetEnvironmentStrings( int parm, VBUF *new_var, VBUF *buff );
extern int              SimNumAssociations( void );
extern void             SimGetAssociationExt( int parm, VBUF *buff );
extern void             SimGetAssociationKeyName( int parm, VBUF *buff );
extern void             SimGetAssociationDescription( int parm, VBUF *buff );
extern void             SimGetAssociationProgram( int parm, VBUF *buff );
extern void             SimGetAssociationIconFileName( int parm, VBUF *buff );
extern int              SimGetAssociationIconIndex( int parm );
extern bool             SimCheckAssociationCondition( int parm );
extern int              SimNumLabels( void );
extern void             SimGetLabelDir( int parm, VBUF *buff );
extern void             SimGetLabelLabel( int parm, VBUF *buff );
extern bool             SimCalcTargetSpaceNeeded( void );
extern void             CheckDLLCount( const char * );
extern void             SimSetNeedGetDiskSizes( void );
extern void             MsgPut( int resourceid, va_list args );
extern void             FreeAllStructs( void );
extern bool             EvalCondition( const char *str );
extern bool             SimTargetNeedsUpdate( int parm );
extern vhandle          SimSubFileVar( int parm, int subfile );
extern int              SimNumSpawns( void );
extern bool             SimGetSpawnCommand( int i, VBUF *buff );
extern when_time        SimWhen( int i );
extern bool             SimEvalSpawnCondition( int i );
extern int              SimNumDeletes( void );
extern const char       *SimDeleteName( int );
extern bool             SimDeleteIsDialog( int );
extern bool             SimDeleteIsDir( int );
extern vhandle          MakeDummyVar( void );

extern bool             GetOptionVarValue( vhandle var_handle );
extern char             *TrimQuote( char * );
extern void             SetDefaultAutoSetValue( vhandle var_handle );
extern bool             CheckForceDLLInstall( const VBUF *name );
