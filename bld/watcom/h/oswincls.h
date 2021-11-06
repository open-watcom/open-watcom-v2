/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2021-2021 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  OS Window system class names macros definition
*
****************************************************************************/


#if defined( __OS2__ )

#define WC_SYS_FRAME            "#1"
#define WC_SYS_COMBOBOX         "#2"
#define WC_SYS_BUTTON           "#3"
#define WC_SYS_MENU             "#4"
#define WC_SYS_STATIC           "#5"
#define WC_SYS_ENTRYFIELD       "#6"
#define WC_SYS_LISTBOX          "#7"
#define WC_SYS_SCROLLBAR        "#8"
#define WC_SYS_TITLEBAR         "#9"
#define WC_SYS_MLE              "#10"

#elif defined( __WINDOWS__ ) || defined( __NT__ )

#define WC_SYS_MENU             "#32768"
#define WC_SYS_DESKTOPWIN       "#32769"
#define WC_SYS_DIALOGBOX        "#32770"
#define WC_SYS_TASKSWITCHWIN    "#32771"
#define WC_SYS_TITLEICON        "#32772"

#endif
