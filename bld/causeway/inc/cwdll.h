#pragma library (cwdll)

#ifdef __cplusplus
 #define _CWCAPI     extern "C" __declspec(__cdecl)
#else
 #define _CWCAPI     __declspec(__cdecl)
#endif

/****************************************************************************
;Load a module by file name. If the module already exists in memory a new
;version will still be loaded.
;
;Usage: LoadLibrary(file_name);
;
;Returns:
;
;EAX    zero on error else module handle.
;
;file_name is a standard null terminated string.
;
;Handles returned by this function should always be released via FreeLibrary()
;
;***************************************************************************/
_CWCAPI extern void         *LoadLibrary( const char * );


/****************************************************************************
;Releases a LoadLibrary module handle back to the system.
;
;Usage: FreeLibrary(module_handle);
;
;Returns:
;
;nothing.
;
;module_handle is the value returned by LoadLibrary();
;
;***************************************************************************/
_CWCAPI extern void         FreeLibrary( void * );


/******************************************************************************
;Load a module by module name. If the module is already in memory then just
;return the handle for the existing copy.
;
;Usage: LoadModule(module_name);
;
;Returns:
;
;EAX    zero on error else module handle.
;
;module_name is a standard zero terminated string.
;
;Handles returned by this function should always be released via FreeModule()
;
******************************************************************************/
_CWCAPI extern void         *LoadModule( const char * );


/****************************************************************************
;Releases a LoadModule() module handle back to the system.
;
;Usage: FreeModule(module_handle);
;
;Returns:
;
;nothing.
;
;module_handle is the value returned by LoadModule();
;
;***************************************************************************/
_CWCAPI extern void         FreeModule( void * );


/******************************************************************************
;Returns the address of a symbol in a module.
;
;Usage: GetProcAddress(module_handle,function_name);
;
;Returns:
;
;zero on error else function address. (EDX:EAX, use just EAX for FLAT)
;
;module_handle is the value returned by LoadModule() or LoadLibrary()
;module_name is a standard zero terminated string.
;
******************************************************************************/
_CWCAPI extern void         *GetProcAddress( void *, const char * );


/******************************************************************************
;Returns the address of a module file name.
;
;Usage: GetModuleFileName(module_PSP);
;
;Returns:
;
;EAX address of a module file name (standard null terminated string)
;
;module_PSP is the DOS PSP segment selector
;
******************************************************************************/
_CWCAPI extern const char   *GetModuleFileName( unsigned );

