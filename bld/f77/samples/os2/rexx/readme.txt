These instructions convert the example EZRXFUNC to a DLL called "MyDLL.DLL", add
a new function, delete an existing function and bring the code into IDE.


To create a new name for the DLL:

   1. Edit funcload.for by replacing all occurrences of "EZ" to "MyDLL"
      except in the data RxFncTable/ ... statement.

      ! funcload.for -- Defines the two functions 'MyDLLLoadFuncs' and 'MyDLLDropFuncs',

      c$pragma aux (RexxFunctionHandler) MyDLLLoadFuncs "MyDLLLoadFuncs"
      c$pragma aux (RexxFunctionHandler) MyDLLDropFuncs "MyDLLDropFuncs"

      data RxFncTable/ 'MyDLLLoadFuncs'c,  'MyDLLLoadFuncs'c, ...

      ! MyDLLLoadFuncs -- Register all the functions with REXX.

      integer function MyDLLLoadFuncs( name, numargs, args, queuename, retstr )

      parameter (DLLNAME = 'MyDLL'c)

      [Be sure previous line, which defines the storage for DLLNAME, is also
       changed if necessary.]

        if( numargs .gt. 0 )then
            MyDLLLoadFuncs = INVALID_ROUTINE

        MyDLLLoadFuncs = VALID_ROUTINE

        * MyDLLDropFuncs -- Deregister all the functions with REXX.
        integer function MyDLLDropFuncs( name, numargs, args, queuename, retstr )

        if( numargs .gt. 0 )then
            MyDLLDropFuncs = INVALID_ROUTINE

        MyDLLDropFuncs = VALID_ROUTINE

   2. In your REXX program you will need the lines:

        call RXFuncAdd 'MyDLLLoadFuncs', 'MyDLL', 'MyDLLLoadFuncs'
        call MyDLLLoadFuncs
        call MyDLLDropFuncs


To add a new function called "NewFunc" to the DLL:

   1. In funcload.for edit the line:

         data RxFncTable/ 'MyDLLLoadFuncs'c,  'MyDLLLoadFuncs'c, ...

   2. In funcload.fi edit the line:

         parameter (ENTRIES = 6)

   3. Copy an existing fortran source for a function, renaming it to match the
      new function's name.

   4. In the fortran source for the function edit the lines:

         c$pragma aux (RexxFunctionHandler) NewFunc "NewFunc"
         integer function NewFunc( name, numargs, args, queuename, retstr )

         if( numargs .gt. 0 )then
            NewFunc = INVALID_ROUTINE

         ! Return 0 if no error occurred
            NewFunc = VALID_ROUTINE

   5. On the first page of "Linker Switches"  edit the "Other options" line to
      add something like "exp NewFunc resident"


To drop a function:

   1. In funcload.for delete references to the functions by editing the line:

         data RxFncTable/ 'MyDLLLoadFuncs'c,  'MyDLLLoadFuncs'c, ...

   2. In funcload.fi edit the line:

         parameter (ENTRIES = 3)

   3. If you are using IDE, on the first page of "Linker Switches"  edit the
      "Other options" line to remove references to the function.  If you are
      using a makefile remove the lines like -

      func1.obj : func1.for ....
      @%append $(%lnk) export EZLoadFuncs resident

   4. Delete the fortran source files.


To use IDE:

   1. Linker Switches
    a. Basic Switches
         Other options -
           exp MyDLLLoadFuncs resident  exp EZFunc1 resident  exp EZFunc2 resident  exp EZFunc3 resident exp MyDLLDropFuncs resident exp NewFunc resident
         Check "Map file"

    b. Import, Export and Library Switches
         Libraries
           rexx

    c. Special Switches
         Select DGROUP is not shared

   2. Fortran Compiler Switches
    a. Application Type Switches
         Select Dynamic link library

    b. Miscellaneous Switches
         Select Extended length

   3. The target name in the IDE must match the name of the DLL.


Arguments to the DLL
   REXX arguments to functions in the DLL are received by the function as
   character strings.  To receive two integer arguments, termed ELAS_U and
   BperLine, do the following in the function source code.

      character*(*) ELAS_U_c,BperLine_c

      ! Allocate character variables and store argument strings.
      allocate( ELAS_U_c*args(1).strlength, location=args(1).strptr )
      allocate( BperLine_c*args(2).strlength, location=args(2).strptr )

      ! Convert to integer values.
      read(ELAS_U_c,*)    ELAS_U
      read(BperLine_c,*)  BperLine


Doug Rickman
MSFC/NASA August 5, 2003
