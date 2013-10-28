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
* Description:  Execution profiler messages.
*
****************************************************************************/


#ifdef __WATCOMC__
#pragma aux fatal aborts;
#endif


#ifdef DEFINE_STRINGS
#define LITSTR( sym, val ) const char _Literal_##sym[] = val;
#else
#if defined( __I86__ )
    #define MAKEFAR __far
#else
    #define MAKEFAR
#endif
#define LITSTR( sym, val ) extern char MAKEFAR _Literal_##sym[sizeof( val )];
#endif

#define LIT( sym )      (_Literal_##sym)



LITSTR( Assertion_Failed,           "Internal error: %s" )
LITSTR( Cannot_Open_Smp_File,       "Cannot open sample file: %s" )
LITSTR( Cannot_Process_Ovly,        "Cannot process overlay information for '%s'" )
LITSTR( Dip_Find_Failed,            "Cannot find a debug interface to load" )
LITSTR( Dip_Info_Failed,            "Cannot load the symbolic information for '%s': %s" )
LITSTR( Dip_Init_Failed,            "Cannot initialize the debug interface, status = %d" )
LITSTR( Dip_Load_Failed,            "Cannot load the debug interface '%s': %s" )
LITSTR( Exe_Has_Changed,            "'%s' has changed since the sample was taken" )
LITSTR( Exe_Not_Found,              "Cannot find the image '%s'" )
LITSTR( Incompat_Smp_File,          "'%s' is an incompatible sample file" )
LITSTR( Incompat_Ver_Ovly,          "Incompatible version of overlay information in executable '%s'" )
LITSTR( Invalid_Smp_File,           "'%s' is not a valid sample file" )
LITSTR( Memfull,                    "Out of memory" )
LITSTR( Memfull_Realloc,            "Unable to reallocate memory" )
LITSTR( Smp_File_IO_Err,            "An error occured while accessing '%s'" )
LITSTR( Usage,                      "Usage: wprof [-option] [sample_file]" )
LITSTR( Usage1,                     "Usage: wprof [-option] [sample_file]" )
LITSTR( Usage2,                     "  where option is one or more of" )
LITSTR( Usage3,                     "    -DIP=dip_name" )
LITSTR( Usage4,                     "    -?" )
LITSTR( Usage5,                     "    -Help" )
LITSTR( Usage6,                     "  for dos systems only" )
LITSTR( Usage7,                     "    -NOGraphicsmouse" )
LITSTR( Usage8,                     "    -NOCHarremap" )
LITSTR( Empty_Str,                  "" )
LITSTR( File_Does_Not_Exist,        "File '%s' does not exist" )
LITSTR( Bad_System_Load,            "Could not start the command processor" )
LITSTR( About_WPROF,                "About WPROF" )
LITSTR( WPROF_TITLE,                "Open Watcom Profiler" )
LITSTR( Enter_Sample,               "Enter Sample File Name" )
LITSTR( OK_Str,                     "OK" )
LITSTR( Cancel_Str,                 "Cancel" )
LITSTR( Unknown_Image,              "***Unknown_Image" )
LITSTR( Unknown_Module,             "***Unknown_Module" )
LITSTR( Unknown_File,               "***Unknown_File" )
LITSTR( Unknown_Routine,            "***Unknown_Routine" )
LITSTR( Gathered_Images,            "***Gathered_Images" )
LITSTR( Gathered_Modules,           "***Gathered_Modules" )
LITSTR( Gathered_Files,             "***Gathered_Files" )
LITSTR( Gathered_Routines,          "***Gathered_Routines" )
LITSTR( No_Symbol_Info,             "'%s' does not have symbol information" )
LITSTR( No_Routine_Names,           "'%s' does not have any routines" )
LITSTR( Src_File_Not_Found,         "Source file '%s' not found" )
LITSTR( Src_File_Not_Known,         "Source file is not known" )
LITSTR( Rel_Header,                 "Rel:" )
LITSTR( Abs_Header,                 "Abs:" )
LITSTR( Sample_Data,                "Sample: %s" )
LITSTR( Smp_File_Raw_Err,           "Sample file '%s' has an incorrect number of samples" )
LITSTR( Sample_Header,              "Sample:  " )
LITSTR( Image_Header,               "Image:  " )
LITSTR( Module_Header,              "Module:  " )
LITSTR( File_Header,                "File:  " )
LITSTR( Routine_Header,             "Routine:  " )
LITSTR( Source_Header,              "Source Lines" )
LITSTR( Image_Names,                "Image Names" )
LITSTR( Module_Names,               "Module Names" )
LITSTR( File_Names,                 "File Names" )
LITSTR( Routine_Names,              "Routine Names" )
LITSTR( Source_Line,                "Source Line" )
LITSTR( Assembler_Instructions,     "Assembler Instructions" )
LITSTR( Options,                    "Options" )
LITSTR( Cmd_Option_Not_Valid,       "Command line option '%s' is not valid" )
LITSTR( Unable_To_Open_Src,         "Unable to open source file" )
LITSTR( Unable_To_Open_Help,        "Unable to open help file '%s'" )
LITSTR( Convert_Data,               "Convert Data to a File" )
LITSTR( Convert_File_Name,          "Enter Convert File Name" )
LITSTR( Mad_Init_Failed,            "Cannot initialize the machine specific interface" )
LITSTR( LDS_FSEEK_FAILED,           "File seek failed" )
LITSTR( LDS_FREAD_FAILED,           "File read failed" )
LITSTR( LDS_FOPEN_FAILED,           "File open failed" )
LITSTR( LMS_INVALID_MAD_VERSION,    "Invalid machine architecture version" )
LITSTR( LMS_INVALID_MAD,            "Invalid machine architecture file" )
LITSTR( LMS_UNREGISTERED_MAD,       "Unknown machine architecture" )
LITSTR( LMS_RECURSIVE_MAD_FAILURE,  "Can not recover from machine architecture file failures" )
