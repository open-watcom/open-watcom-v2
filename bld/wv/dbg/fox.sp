* Syntax:       fox <symbol_file_name>
*
* Synopsis:     Setup for debugging a FoxPro External Routine.
*               The symbolic information is in <symbol_file_name>.
*
*               NOTE: Make sure that you have coded a call to BreakPoint()
*                     in your external routine.  This call should be in the
*                     first segment.  When debugging locally under DOS you
*                     may have to use the /DYNAMIC option to increase the
*                     amount of dynamic memory above 40K.
*
* Example:      wd /dynamic=60k foxpro
*               (start external routine)
*               fox hello.plb
*
new/sym <1> CS
