/****************************************************************************
;Load a module by file name. If the module already exists in memory a new
;version will still be loaded.
;
;Usage: LoadLibrary(file_name);
;
;Returns:
;
;EAX	zero on error else module handle.
;
;file_name is a standard zero terminated string.
;
;Handles returned by this function should always be released via FreeLibrary()
;
;***************************************************************************/
extern void* _cdecl LoadLibrary(void *);


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
extern void _cdecl FreeLibrary(void *);


/******************************************************************************
;Load a module by module name. If the module is already in memory then just
;return the handle for the existing copy.
;
;Usage: LoadModule(module_name);
;
;Returns:
;
;EAX 	zero on error else module handle.
;
;module_name is a standard zero terminated string.
;
;Handles returned by this function should always be released via FreeModule()
;
******************************************************************************/
extern void* _cdecl LoadModule(void *);


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
extern void _cdecl FreeModule(void *);


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
extern void* _cdecl GetProcAddress(void *,void *);



