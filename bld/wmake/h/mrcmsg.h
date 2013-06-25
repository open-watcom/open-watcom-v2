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
* Description:  Make utility message numbers and message interfaces.
*
****************************************************************************/


/*
 * Adding new messages: if the new message has two parameters (not including
 * %Z and %L), the parameters of this message must be added to the para_table
 * at the end of this file so that the resource functions know what kinds of
 * parameters are in the message
 */

/*
 * Error/warning messages - please do not change the ordering of any of these
 * err/wrn msgs.  Please group all err/wrn msgs here.  These messages have
 * numbers output with them; hence, at some point those numbers may be
 * documented.
 */

/*
 * Note: Because of the previous note, I refrained from totally obliterating
 * this list in favor of the more modernized methods used in WDISASM, WASM,
 * etc.  In the new method, the list of #define's is automatically generated
 * using vi and splice by the makefile.  I am, however, still going to trash
 * the usage messages and create a usage.sp file.  CSK
 *
 * continued down below because numbers are not enough
 */

#define OUT_OF_MEMORY                     1
#define MAKE_ABORT                        2
#define INVALID_OPTION                    3
#define INVALID_FILE_OPTION               4
#define NO_TARGETS_SPECIFIED              5
#define MAKEINIT_HAS_TARGET               6
#define EXPECTING_M                       7
#define INVALID_MACRO_NAME                8
#define IGNORE_OUT_OF_PLACE_M             9
#define CANNOT_NEST_FURTHER              10
#define UNKNOWN_PERCENT_CMD              11
#define COMMAND_TOO_LONG                 12
#define CTRL_CHAR_IN_CMD                 13
#define UNABLE_TO_EXEC                   14
#define SYNTAX_ERROR_IN                  15
#define NO_NESTED_FOR                    16
#define MAXIMUM_TOKEN_IS                 17
#define UNKNOWN_TOKEN                    18
#define TARGET_ALREADY_M                 19
#define CLIST_HAS_NO_OWNER               20
#define SUFFIX_DOESNT_EXIST              21
#define NO_EXISTING_FILE_MATCH           22
#define EXTENSIONS_REVERSED              23
#define MORE_THAN_ONE_CLIST              24
#define REDEF_OF_SUFFIX                  25
#define UNK_PREPROC_DIRECTIVE            26
#define MACRO_UNDEFINED                  27
#define IF_NESTED_TOO_DEEP               28
#define UNMATCHED_WITH_IF                29
#define SKIPPING_AFTER_ELSE              30
#define NOT_ALLOWED_AFTER_ELSE           31
#define UNABLE_TO_INCLUDE                32
#define USER_ERROR                       33
#define EOF_BEFORE_ENDIF                 34
#define TRYING_UNDEF_UNDEF               35
#define ATTEMPT_MAKE_SPECIAL             36
#define RECURSIVE_DEFINITION             37
#define UNABLE_TO_MAKE                   38
#define TARGET_NOT_MENTIONED             39
#define COULD_NOT_TOUCH                  40
#define NO_DEF_CMDS_FOR_MAKE             41
#define LAST_CMD_MAKING_RET_BAD          42
#define SYSERR_DELETING_FILE             43
#define S_COMMAND_RET_BAD                44
#define MAXIMUM_STRING_LENGTH            45
#define BARF_CHARACTER                   46
#define ASSUMING_SYMBOLIC                47
#define PERCENT_MAKE_DEPTH               48
#define OPENING_FOR_WRITE                49
#define DOING_THE_WRITE                  50
#define CHANGING_DIR                     51
#define CHANGING_DRIVE                   52
#define OS_CORRUPTED                     53
#define READ_ERROR                       54
#define INCOMPATIBLE__OPTIONS            55
#define ERR_BLANK_3                      56
#define ERR_BLANK_4                      57
#define ERR_BLANK_5                      58
#define PARSE_IFEXPR                     59
#define TMP_PATH_TOO_LONG                60
#define UNEXPECTED_EOF                   61
#define NOKEEP_ONLY                      62
#define NON_MATCHING_QUOTE               63
#define INVALID_STRING_SUBSTITUTE        64
#define DOT_DEFAULT_REDEFINITION         65
#define MAX_FILE_LENGTH_EXCEED           66
#define NON_MATCHING_CURL_PAREN          67
#define INVALID_SUFSUF                   68
#define PATH_TOO_LONG                    69
#define UNABLE_TO_LOAD_DLL               70
#define DLL_BAD_INIT_STATUS              71
#define DLL_BAD_RETURN_STATUS            72
#define ILLEGAL_CHARACTER_IN_MAC         73
#define ERROR_CLOSING_FILE               74
#define ERROR_OPENING_FILE               75
#define ERROR_WRITING_FILE               76
#define USER_BREAK_ENCOUNTERED           77
#define ERROR_TRMEM                      78
#define MICROSOFT_MAKEFILE               79
#define ERROR_STRING_OPEN                80

/*
 * all msgs beyond here should not have a number printed with them
 * ie: these are inform/debug/%M messages only
 */

/* %M msgs */

#define M_FORM_QUALIFIER                104
#define ERR_BLANK_6                     105
#define M_UNKNOWN_TOKEN                 106
#define M_SCOLON                        107
#define M_DCOLON                        108
#define M_FILENAME                      109
#define M_DOTNAME                       120
#define M_CMD                           121
#define M_SUF                           122
#define M_SUFSUF                        123
#define M_PATH                          124
#define M_ERROR                         125
#define M_WARNING                       126

/* inform/debug msgs */

#define ERR_BLANK_7                     150
#define SKIPPING_BLOCK                  151
#define ENTERING_BLOCK                  152
#define AT_ENDIF                        153
#define ENTERING_FILE                   154
#define FINISHED_FILE                   155
#define PTARG_IS_TYPE_M                 156
#define PTARG_DEPENDS_ON                157
#define PTARG_TAB_TAB_ENV               158
#define PTARG_WOULD_EXECUTE_CMDS        159
#define PTARG_NO_DEPENDENTS             160
#define PTARG_NAME                      161
#define PTARG_DOTNAME                   162
#define PDOT_CMDS                       163
#define PSUF_SUFFIX                     164
#define PSUF_FOUND_IN                   165
#define PSUF_MADE_FROM                  166
#define PSUF_USING_CMDS                 167
#define PMAC_NAME_HAS_VALUE             168
#define ERR_BLANK_8                     169
#define UPDATING_TARGET                 170
#define M_EXPLICIT_RULE                 171
#define M_NO                            172
#define TARGET_IS_UPDATED               173
#define IMP_ENV_M                       174
#define M_CHECKING_IMPLICIT             175
#define M_HAS_NO_IMPLICIT               176
#define M_IS_CLEAR_WITH                 177
#define M_HAD_TO_BE_UPDATED             178
#define M_COULD_NOT_BE_IMPLIED          179
#define M_YOUNGEST_DATE                 180
#define M_OLDEST_DATE                   181
#define GETDATE_MSG                     182
#define WILL_BE_BUILT_BECAUSE_OF        183
#define DELETING_FILE                   184
#define TARGET_FORCED_UPTODATE          185
#define EXECING_CMD                     186
#define CMD_RETCODE                     187

#define JUST_A_TAB                      188
#define NEWLINE                         189
#define FAR_STRING                      190
#define DO_YOU_WISH_TO_CONT             191
#define SHOULD_FILE_BE_DELETED          192
#define STRING_YES_NO                   193
#define PRESS_KEY                       194

#define SYS_ERR_0                       195
#define SYS_ERR_1                       196
#define SYS_ERR_2                       197
#define SYS_ERR_3                       198
#define SYS_ERR_4                       199
#define SYS_ERR_5                       200
#define SYS_ERR_6                       201
#define SYS_ERR_7                       202
#define SYS_ERR_8                       203
#define SYS_ERR_9                       204
#define SYS_ERR_10                      205
#define SYS_ERR_11                      206
#define SYS_ERR_12                      207
#define SYS_ERR_13                      208
#define SYS_ERR_14                      209
#define SYS_ERR_15                      210

#define SIG_ERR_0                       211
#define SIG_ERR_1                       212
#define SIG_ERR_2                       213
#define SIG_ERR_3                       214
#define SIG_ERR_4                       215
#define SIG_ERR_5                       216
#define SIG_ERR_6                       217
#define SIG_ERR_7                       218
#define SIG_ERR_8                       219
#define SIG_ERR_9                       220
#define SIG_ERR_10                      221
#define SIG_ERR_11                      222
#define SIG_ERR_12                      223
#define SIG_ERR_13                      224
#define SIG_ERR_14                      225
#define SIG_ERR_15                      226

#define MSG_USAGE_BASE                  230

#include "usageend.gh"

#define END_OF_RESOURCE_MSG             MSG_USAGE_LAST


#define MSG_LANG_SPACING                1000

#define MAX_RESOURCE_SIZE               128


typedef struct table_type {
    int     msgid;
    char    *msgtype;
} TABLE_TYPE;

extern int      MsgInit( void );
extern int      MsgGet( int resourceid, char *buffer );
extern void     MsgGetTail( int resourceid, char *buffer );
extern void     MsgFini( void );
extern int      MsgReOrder( int resourceid, char *buff, char **paratype );
extern BOOLEAN  ResAutoDep( char *name, time_t stamp, BOOLEAN (*chk)(time_t,time_t), time_t *pmax_time );
