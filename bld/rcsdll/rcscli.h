#include "rcsapi.h"

#if !defined( __NT__ ) && !defined( __WINDOWS__ ) && !defined( __OS2__ )

/* common functions */
extern RCSGetVersionFn  RCSGetVersion;
extern RCSSetSystemFn   RCSSetSystem;
extern RCSQuerySystemFn RCSQuerySystem;
extern RCSRegBatchCbFn  RCSRegisterBatchCallback;
extern RCSRegMsgBoxCbFn RCSRegisterMessageBoxCallback;
/* system specific functions -- mapped to function for appropriate system */
extern RCSInitFn        RCSInit;
extern RCSCheckoutFn    RCSCheckout;
extern RCSCheckinFn     RCSCheckin;
extern RCSHasShellFn    RCSHasShell;
extern RCSRunShellFn    RCSRunShell;
extern RCSFiniFn        RCSFini;
extern RCSSetPauseFn    RCSSetPause;

#else

/* common functions */
extern RCSGetVersionFn  *RCSGetVersion;
extern RCSSetSystemFn   *RCSSetSystem;
extern RCSQuerySystemFn *RCSQuerySystem;
extern RCSRegBatchCbFn  *RCSRegisterBatchCallback;
extern RCSRegMsgBoxCbFn *RCSRegisterMessageBoxCallback;
/* system specific functions -- mapped to function for appropriate system */
extern RCSInitFn        *RCSInit;
extern RCSCheckoutFn    *RCSCheckout;
extern RCSCheckinFn     *RCSCheckin;
extern RCSHasShellFn    *RCSHasShell;
extern RCSRunShellFn    *RCSRunShell;
extern RCSFiniFn        *RCSFini;
extern RCSSetPauseFn    *RCSSetPause;

#ifdef __WINDOWS__
 #define RCS_DLLNAME                    "rcsdll.dll"
 #define CHECKIN_FN_NAME                "RCSCHECKIN"
 #define CHECKOUT_FN_NAME               "RCSCHECKOUT"
 #define GETSYS_FN_NAME                 "RCSQUERYSYSTEM"
 #define GETVER_FN_NAME                 "RCSGETVERSION"
 #define FINI_FN_NAME                   "RCSFINI"
 #define HAS_SHELL_FN_NAME              "RCSHASSHELL"
 #define INIT_FN_NAME                   "RCSINIT"
 #define REG_BAT_CB_FN_NAME             "RCSREGISTERBATCHCALLBACK"
 #define REG_MSGBOX_CB_FN_NAME          "RCSREGISTERMESSAGEBOXCALLBACK"
 #define RUNSHELL_FN_NAME               "RCSRUNSHELL"
 #define SETSYS_FN_NAME                 "RCSSETSYSTEM"
 #define SET_PAUSE_FN_NAME              "RCSSETPAUSE"
#elif defined( __NT__ )
 #define RCS_DLLNAME                    "rcsdll.dll"
 #define CHECKIN_FN_NAME                "_RCSCheckin@16"
 #define CHECKOUT_FN_NAME               "_RCSCheckout@16"
 #define GETSYS_FN_NAME                 "_RCSQuerySystem@4"
 #define GETVER_FN_NAME                 "_RCSGetVersion@0"
 #define FINI_FN_NAME                   "_RCSFini@4"
 #define HAS_SHELL_FN_NAME              "_RCSHasShell@4"
 #define INIT_FN_NAME                   "_RCSInit@8"
 #define REG_BAT_CB_FN_NAME             "_RCSRegisterBatchCallback@12"
 #define REG_MSGBOX_CB_FN_NAME          "_RCSRegisterMessageBoxCallback@12"
 #define RUNSHELL_FN_NAME               "_RCSRunShell@4"
 #define SETSYS_FN_NAME                 "_RCSSetSystem@8"
 #define SET_PAUSE_FN_NAME              "_RCSSetPause@8"
#elif defined( __OS2__ )
 #define RCS_DLLNAME                    "rcsdll"
 #define CHECKIN_FN_NAME                "RCSCheckin"
 #define CHECKOUT_FN_NAME               "RCSCheckout"
 #define GETSYS_FN_NAME                 "RCSQuerySystem"
 #define GETVER_FN_NAME                 "RCSGetVersion"
 #define FINI_FN_NAME                   "RCSFini"
 #define HAS_SHELL_FN_NAME              "RCSHasShell"
 #define INIT_FN_NAME                   "RCSInit"
 #define REG_BAT_CB_FN_NAME             "RCSRegisterBatchCallback"
 #define REG_MSGBOX_CB_FN_NAME          "RCSRegisterMessageBoxCallback"
 #define RUNSHELL_FN_NAME               "RCSRunShell"
 #define SETSYS_FN_NAME                 "RCSSetSystem"
 #define SET_PAUSE_FN_NAME              "RCSSetPause"
#endif

#endif
