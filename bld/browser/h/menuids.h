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


#ifndef __MENUIDS_H__
#define __MENUIDS_H__

// These enumeration constant have to match up as the index of the menuItem's
// index in their parent

enum MainMenus {
    MMFile,
    MMView,
    MMDetail,
    MMTree,
    MMLocate,
    MMOptions,
    MMWindows,
    MMHelp,
    MMNumMainMenus
};

enum CascadeMenus {
    CMViewInherit = MMNumMainMenus,
    CMViewCall,
    CMLastCascadeMenu,
    CMNumCascadeMenus = CMLastCascadeMenu - MMNumMainMenus,
};

enum FileSubMenus {
    FMNew,
    FMOpen,
    FMModules,
    FMSep0,
    FMLoad,
    FMSave,
    FMSaveAs,
#ifdef REPORT_IMPLEMENTED
    FMSep1,
    FMReport,
#endif
    FMSep2,
    FMExit,
    FMNumFileMenus
};

enum ViewSubMenus {
    VMList,
    VMInherit,
    VMCall,
    VMNumViewMenus
};

enum ViewInheritSubMenus {
    VMInheritTree,
    VMInheritOutline,
    VMNumViewInheritMenus
};

enum ViewCallSubMenus {
    VMCallTree,
    VMCallOutline,
    VMNumViewCallMenus
};

enum DetailSubMenus {
    DMDetail,
    DMDefinition,
    DMSep0,
    DMReferences,
    DMUsers,
#ifdef DETAIL_STUFF_IMPLEMENTED
    DMSep1,
    DMInheritance,
    DMStructure,
    DMSep2,
    DMCalls,
#endif
    DMNumDetailMenus
};

enum TreeSubMenus {
    TMRoots,
    TMArrangeAll,
    TMSep0,
    TMExpandOne,
    TMExpandBranch,
    TMExpandAll,
    TMCollapseBranch,
    TMCollapseAll,
    #ifdef DEBUGTREE
    TMSep1,
    TMDebugInfo,
    TMSibWidth,
    #endif
    TMNumTreeMenus
};

enum LocateSubMenus {
    LMFind,
    LMFindNext,
    LMSep0,
    LMFindSelected,
    LMNumLocateMenus
};

enum OptionsSubMenus {
    OMRegExp,
    OMEnumStyles,
    OMSep0,
    OMMemberFilters,
    OMQuery,
#ifdef OPTIONS_IMPLEMENTED
    OMMemberOrder,
#endif
    OMSep1,
    OMTreeAutoArrange,
    OMTreeSquareLines,
    OMTreeHorizontal,
    OMSep2,
    OMInheritTreeLegend,
    OMCallTreeLegend,
    OMSep3,
    OMEditorDLL,
    OMAutoSave,
    OMNumOptionsMenus
};

enum WindowsMenus {
    WMArrangeAll,
    WMSep0,
    WMNumWindowsMenus
};

enum HelpMenus {
    HMContents,
    HMSearch,
    HMUsingHelp,
    HMSep0,
    HMAbout,
    HMNumHelpMenus
};

#endif //__MENUIDS_H__
