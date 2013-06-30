#ifdef CHICAGO
pick( STR_APP_NAME,
    "Dr. Watcom for Windows 9x",
    "Dr. Watcom for Windows 9x" )
#else
pick( STR_APP_NAME,
    "Dr. Watcom",
    "Dr. Watcom" )
#endif
pick( STR_CANT_LOAD_DIP,
    "Unable to load debugging interface",
    "Unable to load debugging interface" )
pick( STR_CANT_LOAD_WATCOM_DIP,
    "Unable to load Watcom DIP",
    "Unable to load Watcom DIP" )

// THREAD WAIT REASONS
pick( STR_WAIT_4_EXECUTIVE,
    "Waiting for executive",
    "Waiting for executive" )
pick( STR_WAIT_4_FREE_PAGE,
    "Waiting for free page",
    "Waiting for free page" )
pick( STR_WAIT_4_PAGE_IN,
    "Waiting for page in",
    "Waiting for page in" )
pick( STR_WAIT_4_POOL_ALLOC,
    "Waiting for pool allocation",
    "Waiting for pool allocation" )
pick( STR_WAIT_4_EXEC_DELAY,
    "Waiting for execution delay",
    "Waiting for execution delay" )
pick( STR_WAIT_4_SUSP_COND,
    "Waiting for suspended condition",
    "Waiting for suspended condition" )
pick( STR_WAIT_4_USER_REQUEST,
    "Waiting for user request",
    "Waiting for user request" )
pick( STR_WAIT_4_EVENT_PR_HIGH,
    "Waiting for event pair high",
    "Waiting for event pair high" )
pick( STR_WAIT_4_EVENT_PR_LOW,
    "Waiting for event pair low",
    "Waiting for event pair low" )
pick( STR_WAIT_4_LPC_RECIEVE,
    "Waiting for LPC recieve",
    "Waiting for LPC recieve" )
pick( STR_WAIT_4_LPC_REPLY,
    "Waiting for LPC reply",
    "Waiting for LPC reply" )
pick( STR_WAIT_4_VIRTUAL_MEMORY,
    "Waiting for virtual memory",
    "Waiting for virtual memory" )
pick( STR_WAIT_4_PAGE_OUT,
    "Waiting for page out",
    "Waiting for page out" )

// THREAD STATES
pick( STR_INITIALIZED,
    "Initialized",
    "Initialized" )
pick( STR_READY,
    "Ready",
    "Ready" )
pick( STR_RUNNING,
    "Running",
    "Running" )
pick( STR_STANDING_BY,
    "Standing by",
    "Standing by" )
pick( STR_TERMINATED,
    "Terminated",
    "Terminated" )
pick( STR_WAITING,
    "Waiting",
    "Waiting" )
pick( STR_TRANSITION,
    "Transition",
    "Transition" )
pick( STR_UNKNOWN,
    "Unknown",
    "Unknown" )

pick( STR_THREAD_RETURN_CODE,
    "Thread Return Code",
    "Thread Return Code" )
pick( STR_PROCESS_RETURN_CODE,
    "Process Return Code",
    "Process Return Code" )
pick( STR_THREAD_X,
    "Thread %08lX",
    "Thread %08lX" )
pick( STR_INVALID_RETURN_CODE,
    "The return code given is invalid.\nThe return code must be a numeric value",
    "The return code given is invalid.\nThe return code must be a numeric value" )
pick( STR_CANT_SET_THREAD_PRI,
    "Unable to set thread priority",
    "Unable to set thread priority" )
pick( STR_BRACED_UNKNOWN,
    "<unknown>",
    "<unknown>" )
pick( STR_WAIT_4_UNKNOWN,
    "Waiting for unknown reason",
    "Waiting for unknown reason" )
pick( STR_STATE,
    "State: %s",
    "State: %s" )
pick( STR_PRIORIY_X,
    "Priority:  base=%d  current=%d",
    "Priority:  base=%d  current=%d" )
pick( STR_THREAD_4_PROC_X,
    "Threads for process: %08lX",
    "Threads for process: %08lX" )
pick( STR_NO_SELECTED_THREAD,
    "There is no selected thread",
    "There is no selected thread" )
pick( STR_CANT_GET_HDL_4_THD_X,
    "Unable to get a handle for thread %08X.\nThe thread was not suspened",
    "Unable to get a handle for thread %08X.\nThe thread was not suspened" )
pick( STR_THREAD_SUSPEND,
    "Suspend",
    "Suspend" )
pick( STR_CANT_SUSPEND_THRD_X,
    "Unable to suspend thread %08X.\n",
    "Unable to suspend thread %08X.\n" )
pick( STR_THREAD_ALREADY_SUSP,
    "Thread %08X is already suspended.\nIts suspend count is %ld.\nDo you want to increase its suspend count?",
    "Thread %08X is already suspended.\nIts suspend count is %ld.\nDo you want to increase its suspend count?" )
pick( STR_THREAD_NOT_RESUMED,
    "Unable to get a handle for thread %08X.\nThe thread was not resumed.",
    "Unable to get a handle for thread %08X.\nThe thread was not resumed." )
pick( STR_RESUME,
    "resume",
    "resume" )
pick( STR_CANT_RESUME_THRD_X,
    "Unable to resume thread %08X.",
    "Unable to resume thread %08X." )
pick( STR_THRD_IS_NOT_SUSP,
    "Thread %08X is not suspended.\nNo action was taken.",
    "Thread %08X is not suspended.\nNo action was taken." )
pick( STR_SUSP_COUNT_DECREMENTED,
    "Thread %08X had a suspend count of %ld.\nIts suspend count was decremented.\nIt remains suspended.",
    "Thread %08X had a suspend count of %ld.\nIts suspend count was decremented.\nIt remains suspended." )
pick( STR_THRD_NOT_TERMINATED,
    "Unable to get a handle for thread %08X.\nThe thread was not terminated.",
    "Unable to get a handle for thread %08X.\nThe thread was not terminated." )
pick( STR_KILL,
    "Kill",
    "Kill" )
pick( STR_CANT_KILL_THRD_X,
    "Unable to kill thread %08X",
    "Unable to kill thread %08X" )
pick( STR_CANT_FIND,
    "Cannot find %s",
    "Cannot find %s" )
pick( STR_WRONG_VERSION_OF,
    "Wrong verion of %s",
    "Wrong verion of %s" )
pick( STR_EXECUTABLE_FILTER,
    "Executables",
    "Executables" )
pick( STR_LOAD_PROCESS,
    "Load Process",
    "Load Process" )
pick( STR_CANT_CREATE_PROCESS,
    "Unable to create process %s.",
    "Unable to create process %s." )

// memory states
pick( STR_COMMITTED,
    "COMMITTED",
    "COMMITTED" )
pick( STR_FREE,
    "FREE",
    "FREE" )
pick( STR_RESERVED,
    "RESERVED",
    "RESERVED" )
pick( STR_2_QUEST_MARKS,
    "??",
    "??" )

pick( STR_STACK_GUARD,
    "st guard",
    "st guard" )
pick( STR_STACK,
    "stack",
    "stack" )
pick( STR_MEM_NOT_COMMITTED,
    "Only Committed memory can be read",
    "Only Committed memory can be read" )
pick( STR_MEM_RANGE_NAME_PROC,
    "Memory Range %08lX to %08lX for Process %08lX (%s) ",
    "Memory Range %08lX to %08lX for Process %08lX (%s) " )
pick( STR_MEM_RANGE_UNNAMED_PROC,
    "Memory Range %08lX to %08lX for Process %08lX",
    "Memory Range %08lX to %08lX for Process %08lX" )
pick( STR_CANT_READ_MEM_AT_X,
    "Unable to read memory at %08lX.",
    "Unable to read memory at %08lX." )
pick( STR_MEM_WLK_NAMED_PROC,
    "Memory Walk for Process %08lX (%s)",
    "Memory Walk for Process %08lX (%s)" )
pick( STR_MEM_WLK_UNNAMED_PROC,
    "Memory Walk for Process %08lX",
    "Memory Walk for Process %08lX" )
pick( STR_MEM_WLK_IN_PROGRESS,
    "There is already a memory walk in progress\nDo you want to terminate it?",
    "There is already a memory walk in progress\nDo you want to terminate it?" )
pick( STR_MEMORY_WALKER_TITLE,
    "Memory Walker",
    "Memory Walker" )
pick( STR_ACCESS_VIOLATION,
    "Access Violation",
    "Access Violation" )
pick( STR_NADA,
    "",
    "" )
pick( STR_DATA_MISALIGNMENT,
    "Datatype Misalignment",
    "Datatype Misalignment" )
pick( STR_ARRAY_BNDS_EXCEEDED,
    "Array Bounds Exceeded",
    "Array Bounds Exceeded" )
pick( STR_DENORMAL_FLOAT,
    "Denormal Floating-point Value",
    "Denormal Floating-point Value" )
pick( STR_FLT_DIV_BY_ZERO,
    "Floating-point Divide by Zero",
    "Floating-point Divide by Zero" )
pick( STR_FLT_INEXACT_RESULT,
    "Floating-point Inexact Result",
    "Floating-point Inexact Result" )
pick( STR_FLT_INV_OP,
    "Invalid Floating-point Operation",
    "Invalid Floating-point Operation" )
pick( STR_FLT_PT_OVERFLOW,
    "Floating-point Overflow",
    "Floating-point Overflow" )
pick( STR_FLT_STACK_CHECK,
    "Floating-point Stack Overflow/Underflow",
    "Floating-point Stack Overflow/Underflow" )
pick( STR_FLT_UNDERFLOW,
    "Floating-point Underflow",
    "Floating-point Underflow" )
pick( STR_INT_DIV_BY_ZERO,
    "Integer Divide by Zero",
    "Integer Divide by Zero" )
pick( STR_INT_OVERFLOW,
    "Integer Overflow",
    "Integer Overflow" )
pick( STR_INV_INSTRUCTION,
    "Invalid Instruction",
    "Invalid Instruction" )
pick( STR_UNKNOWN_EXCEPTION_X,
    "Unknown Exception %08lX",
    "Unknown Exception %08lX" )
pick( STR_PROCESS_NAME,
    "Process Name",
    "Process Name" )
pick( STR_PROCESS_ID,
    "Process ID",
    "Process ID" )
pick( STR_PROCESS_STATUS,
    "Process &Status",
    "Process &Status" )
pick( STR_EXCEPTION_ENCOUNTERED,
    "%s:  An exception has ocurred...",
    "%s:  An exception has ocurred..." )
pick( STR_LINE_X_OF,
    "line %ld of",
    "line %ld of" )
pick( STR_N_A,
    "N/A",
    "N/A" )
pick( STR_CANT_TERMINATE_APP,
    "Unable to terminate the application",
    "Unable to terminate the application" )
pick( STR_PRIORITY_X,
    "Priority: %d",
    "Priority: %d" )
pick( STR_NAME_X,
    "Name: %s",
    "Name: %s" )
pick( STR_CANT_GET_PROC_HDL,
    "Unable to get a handle for process %08lX",
    "Unable to get a handle for process %08lX" )
pick( STR_SET_PRIORITY,
    "Set Priority",
    "Set Priority" )
pick( STR_CANT_SET_PROC_PRI,
    "Unable to set priority for process %08lX.",
    "Unable to set priority for process %08lX." )
pick( STR_CANT_ADD_PROCESS,
    "Unable to add process %08lX (%s)",
    "Unable to add process %08lX (%s)" )
pick( STR_NO_SELECTED_PROCESS,
    "There is no selected process",
    "There is no selected process" )
pick( STR_ALREADY_WATCHING,
    "%s is already watching process %08lX",
    "%s is already watching process %08lX" )
pick( STR_WATCH_PROCESS,
    "Watch Process",
    "Watch Process" )
pick( STR_PROC_NOT_TERMINATED,
    "Unable to get a handle for process %08lX.\nThe process was not terminated.",
    "Unable to get a handle for process %08lX.\nThe process was not terminated." )
pick( STR_KILL_PROCESS,
    "Kill Process",
    "Kill Process" )
pick( STR_CANT_KILL_PROCESS,
    "Unable to kill process %08lX.\n",
    "Unable to kill process %08lX.\n" )
pick( STR_VIEW_MEMORY,
    "View Memory",
    "View Memory" )
pick( STR_DEBUG_STRING,
    "DEBUG STRING",
    "DEBUG STRING" )
pick( STR_EXCEPTION,
    "EXCEPTION",
    "EXCEPTION" )
pick( STR_ERROR_OCCURRED_AT_X,
    "%*s %s occurred at %08lX",
    "%*s %s occurred at %08lX" )
pick( STR_INV_WRITE_TO_X,
    "%*s Invalid write to location %08lX",
    "%*s Invalid write to location %08lX" )
pick( STR_INV_READ_FROM_X,
    "%*s Invalid read from location %08lX",
    "%*s Invalid read from location %08lX" )
pick( STR_PROCESS_CREATED,
    "PROCESS CREATED",
    "PROCESS CREATED" )
pick( STR_PROC_CREATE_FMT_STR,
    "%*s image base = %08lX starting address = %08lX",
    "%*s image base = %08lX starting address = %08lX" )
pick( STR_THREAD_CREATED,
    "THREAD CREATED",
    "THREAD CREATED" )
pick( STR_THRD_CREATE_FMT_STR,
    "%s Starting Address = %08lX",
    "%s Starting Address = %08lX" )
pick( STR_THREAD_ENDED,
    "THREAD ENDED",
    "THREAD ENDED" )
pick( STR_THRD_ENDED_FMT_STR,
    "%s Exit Code = %lX",
    "%s Exit Code = %lX" )
pick( STR_PROCESS_ENDED,
    "PROCESS ENDED",
    "PROCESS ENDED" )
pick( STR_PROC_ENDED_FMT_STR,
    "%s Exit Code = %lX",
    "%s Exit Code = %lX" )
pick( STR_DLL_LOADED,
    "DLL LOADED",
    "DLL LOADED" )
pick( STR_DLL_LOAD_FMT_STR,
    "%*s name = \"%s\" base = %08lX",
    "%*s name = \"%s\" base = %08lX" )
pick( STR_DLL_UNLOADED,
    "DLL UNLOADED",
    "DLL UNLOADED" )
pick( STR_DLL_UNLOAD_FMT_STR1,
    "%s DLL base = %08lX",
    "%s DLL base = %08lX" )
pick( STR_DLL_UNLOAD_FMT_STR2,
    "%s DLL name = %s",
    "%s DLL name = %s" )
pick( STR_RIP,
    "RIP",
    "RIP" )
pick( STR_PROCESS_X_NAME,
    "Process %08lX (%s)",
    "Process %08lX (%s)" )
pick( STR_PROCESS_X,
    "Process %08lX",
    "Process %08lX" )
pick( STR_DRNT_STARTED,
    "%s started",
    "%s started" )
#if defined( CHICAGO )
pick( STR_ABOUT_NAME,
    "Dr. Watcom for Windows 9x" STR_BITNEES,
    "Dr. Watcom for Windows 9x" STR_BITNEES )
#else
pick( STR_ABOUT_NAME,
    "Dr. Watcom for Windows" STR_BITNEES,
    "Dr. Watcom for Windows" STR_BITNEES )
#endif
pick( STR_ABOUT_VERSION,
    banner1p2( _DRNT_VERSION_ ),
    banner1p2( _DRNT_VERSION_ ) )
pick( STR_ABOUT_COPYRIGHT_YEAR,
    "2002",
    "2002" )
#ifdef CHICAGO
pick( STR_ABOUT_TITLE,
    "About Dr. Watcom for Windows 9x",
    "About Dr. Watcom for Windows 9x" )
#else
pick( STR_ABOUT_TITLE,
    "About Dr. Watcom for Windows",
    "About Dr. Watcom for Windows" )
#endif
pick( STR_FAULT_PASSED_ON,
    "Fault was passed to the next handler.",
    "Fault was passed to the next handler." )
pick( STR_PROCESS_TERMINATED,
    "Process was terminated.",
    "Process was terminated." )
pick( STR_INSTR_RESTARTED,
    "Instruction was restarted.",
    "Instruction was restarted." )
pick( STR_DEL_BIG_LOG_FILE,
    "The log file is now %ld bytes.\nDo you want to delete it?",
    "The log file is now %ld bytes.\nDo you want to delete it?" )
pick( STR_USER_NOTES,
    "\nUser notes:\n",
    "\nUser notes:\n" )
pick( STR_FAULT_FOUND_ON_X,
    "%s detected a fault on %s\n",
    "%s detected a fault on %s\n" )
pick( STR_LOG_TAKEN_ON_X,
    "%s log taken on %s\n",
    "%s log taken on %s\n" )
pick( STR_COMPUTER_NAME,
    "Computer Name: %s\n",
    "Computer Name: %s\n" )
pick( STR_USER_NAME,
    "User Name: %s\n",
    "User Name: %s\n" )
pick( STR_OPERATING_SYSTEM,
    "Operating System: %s\n",
    "Operating System: %s\n" )
pick( STR_OS_VERSION,
    "OS Version: %d.%d\n",
    "OS Version: %d.%d\n" )
pick( STR_PROCESSOR_TYPE,
    "Processor type is %s\n",
    "Processor type is %s\n" )
pick( STR_NUM_PROCESSORS,
    "Number of Processors: %ld\n",
    "Number of Processors: %ld\n" )
pick( STR_STACK_DATA,
    "\nStack Data:\n",
    "\nStack Data:\n" )
pick( STR_REGISTER_CONTENTS,
    "\nRegister contents:\n",
    "\nRegister contents:\n" )
pick( STR_CURRENT_PROCESSES,
    "\nCurrent Processes\n",
    "\nCurrent Processes\n" )
pick( STR_NO_ACCESS,
    "%*sno access           %10d KB\n",
    "%*sno access           %10d KB\n" )
pick( STR_READ_ONLY,
    "%*sread only           %10d KB\n",
    "%*sread only           %10d KB\n" )
pick( STR_READ_WRITE,
    "%*sread/write          %10d KB\n",
    "%*sread/write          %10d KB\n" )
pick( STR_WRITE_COPY,
    "%*swrite/copy          %10d KB\n",
    "%*swrite/copy          %10d KB\n" )
pick( STR_EXECUTE,
    "%*sexecute             %10d KB\n",
    "%*sexecute             %10d KB\n" )
pick( STR_EXECUTE_READ,
    "%*sexecute/read        %10d KB\n",
    "%*sexecute/read        %10d KB\n" )
pick( STR_EXECUTE_READ_WRITE,
    "%*sexecute/read/write  %10d KB\n",
    "%*sexecute/read/write  %10d KB\n" )
pick( STR_EXECUTE_WRITE_COPY,
    "%*sexecute/write/copy  %10d KB\n",
    "%*sexecute/write/copy  %10d KB\n" )
pick( STR_TOTAL,
    "%*stotal               %10d KB\n\n",
    "%*stotal               %10d KB\n\n" )
pick( STR_VIRTUAL_MEM_INFO,
    "\nVirtual Memory Information\n",
    "\nVirtual Memory Information\n" )
pick( STR_VIRTUAL_PAGE_SIZE,
    "\n%*sVirtual Page Size is %ld bytes (%ld KB).\n",
    "\n%*sVirtual Page Size is %ld bytes (%ld KB).\n" )
pick( STR_RESERVED_MEM,
    "\n%*sRESERVED MEMORY\n",
    "\n%*sRESERVED MEMORY\n" )
pick( STR_COMMITTED_MEM,
    "%*sCOMMITTED MEMORY\n",
    "%*sCOMMITTED MEMORY\n" )
pick( STR_IMAGE_ADDR_SPACE_FOR,
    "%*sIMAGE ADDRESS SPACE FOR %s\n",
    "%*sIMAGE ADDRESS SPACE FOR %s\n" )
pick( STR_NOT_AVAILABLE,
    "%*s<< NOT AVAILABLE >>\n",
    "%*s<< NOT AVAILABLE >>\n" )
pick( STR_CANT_READ_MEM,
    "<< Unable to read memory >>\n",
    "<< Unable to read memory >>\n" )
pick( STR_MEM_DMP_X_TO_Y,
    "\nMemory Dump %08lX to %08lX\n\n",
    "\nMemory Dump %08lX to %08lX\n\n" )
pick( STR_PROCESS_MEM_DMP,
    "\nProcess Memory Dump\n\n",
    "\nProcess Memory Dump\n\n" )
pick( STR_OFFENDING_PROC_ULINE,
    "-------------------------------------\n",
    "-------------------------------------\n" )
pick( STR_OFFENDING_PROC_INFO,
    "--- Offending Process Information ---\n",
    "--- Offending Process Information ---\n" )
pick( STR_OFFENDING_PROC_X,
    "Offending Process: %s (pid=%08lX)\n",
    "Offending Process: %s (pid=%08lX)\n" )
pick( STR_ERR_OCCURRED_AT_X_Y,
    "%s occurred at %s\n",
    "%s occurred at %s\n" )
pick( STR_LOG_INV_WRITE_TO,
    "Invalid write to location %08lX\n",
    "Invalid write to location %08lX\n" )
pick( STR_LOG_INV_READ_FROM,
    "Invalid read from location %08lX\n",
    "Invalid read from location %08lX\n" )
pick( STR_MODULES_LOADED,
    "\nModules Loaded\n",
    "\nModules Loaded\n" )
pick( STR_SOURCE_INFORMATION,
    "\nSource Information:\n",
    "\nSource Information:\n" )
pick( STR_LOG_LINE_X_OF_FILE,
    "%*sline %ld of %s\n",
    "%*sline %ld of %s\n" )
pick( STR_LOG_N_A,
    "%*sN/A\n",
    "%*sN/A\n" )
pick( STR_DISASSEMBLY,
    "\nDisassembly:\n",
    "\nDisassembly:\n" )
pick( STR_8_QUEST_MARKS,
    "????????",
    "????????" )
pick( STR_MODULE_WITH_ADDR,
    "%*sMODULE: (addresses %08lX to %s) %s\n",
    "%*sMODULE: (addresses %08lX to %s) %s\n" )
pick( STR_MODULE,
    "%*sMODULE: %s\n",
    "%*sMODULE: %s\n" )
pick( STR_ERASE_LOG_FILE,
    "Erase log file: %s",
    "Erase log file: %s" )
pick( STR_CANT_OPEN_LOG_FILE,
    "Unable to find or open log file (%s)",
    "Unable to find or open log file (%s)" )
pick( STR_CANT_START_EDITOR,
    "Unable to start editor.\n(Make sure the Log Viewer field in the\nLog Options Dialog is set properly.)",
    "Unable to start editor.\n(Make sure the Log Viewer field in the\nLog Options Dialog is set properly.)" )
pick( STR_FIELD_MUST_BE_NUMERIC,
    "This field must contain a numeric value\n(Prefix hex values with 0x)",
    "This field must contain a numeric value\n(Prefix hex values with 0x)" )
pick( STR_MAX_LOG_FILE_SIZE_INV,
    "Max Log File Size is invalid.\n%s",
    "Max Log File Size is invalid.\n%s" )
pick( STR_DISASM_LINES_INVALID,
    "Disasm Lines field is invalid.\n%s",
    "Disasm Lines field is invalid.\n%s" )
pick( STR_DISASM_BKUP_INVALID,
    "Disasm Backup field is invalid.\n%s",
    "Disasm Backup field is invalid.\n%s" )
pick( STR_CANT_DEBUG_PROC_X_NAME,
    "Unable to debug process %08lX (%s)",
    "Unable to debug process %08lX (%s)" )
pick( STR_CANT_DEBUG_PROC_X,
    "Unable to debug process %08lX",
    "Unable to debug process %08lX" )
pick( STR_STATUS_4_PROC_X,
    "Status for process %08lX (%s)",
    "Status for process %08lX (%s)" )
pick( STR_CANT_OPEN_LOG,
    "Unable to open log file %s\n No log was created",
    "Unable to open log file %s\n No log was created" )
pick( STR_VIEW_MEM_HT_KEY,
    "&View Memory",
    "&View Memory" )
pick( STR_SRC_INFO_FMT,
    "Line %d of %s",
    "Line %d of %s" )
pick( STR_ALL_FILE_FILTER,
    "All Files",
    "All Files" )
pick( STR_PICK_LOG_FILE,
    "Pick log file name",
    "Pick log file name" )
pick( STR_PICK_LOG_VIEWER,
    "Pick log viewer",
    "Pick log viewer" )
pick( STR_CANT_INIT_DIP,
    "Unable to initialize the debugging information processor",
    "Unable to initialize the debugging information processor" )
pick( STR_DIP_NOT_FOUND,
    "not loaded - File not found.",
    "not loaded - File not found." )
pick( STR_WRONG_DIP_VERSION,
    "not loaded - Wrong processor version.",
    "not loaded - Wrong processor version." )
pick( STR_BAD_DIP,
    "not loaded - Invalid processor found.",
    "not loaded - Invalid processor found." )
pick( STR_TOO_MANY_DIPS,
    "not loaded - too many processors loaded.",
    "not loaded - too many processors loaded." )
pick( STR_DIP_ERR_OTHER,
    "not loaded - Reason unknown",
    "not loaded - Reason unknown" )
pick( STR_DIP_LOADED,
    "loaded",
    "loaded" )
pick( STR_NO_DIPS_LOADED,
    "No debugging information processors were found.",
    "No debugging information processors were found." )

pick( STR_MEM_INFO_FOR_PROC,
    "Memory Information for Process %08X (%s)",
    "Memory Information for Process %08X (%s)" )
pick( STR_MODULE_LST_UNAVAILABLE,
    "*** Module list unavailable ***\n",
    "*** Module list unavailable ***\n" )
pick( STR_CANT_GET_PROC_INFO,
    "Unable to get process information for process %08lX",
    "Unable to get process information for process %08lX" )
pick( STR_REFRESH,
    "Refresh",
    "Refresh" )
pick( STR_CANT_REFRESH_THRD,
    "Unable to refresh thread information",
    "Unable to refresh thread information" )
#include "mad.str"
