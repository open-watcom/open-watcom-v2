/*
 *  SETVAL.CMD - Open Watcom Sample WPS Object Maintenance Script
 *
 *  Syntax: setval n
 *
 *  The oject created by "wmake install" can be modified by the
 *  setup string TESTVALUE=n. If the properties notebook of the
 *  object is opened, the GUI control displaying the value will
 *  be updated.
 */

 call RxFuncAdd    'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
 call SysLoadFuncs

 TRUE  = (1 = 1);
 FALSE = (0 = 1);

 rc = 0;

 ObjectId   = '<OWSAMPLE_WPS>'
 LowerLimit = 0;
 UpperLimit = 10;

 DO 1
    /* check if object exists */
    IF (\SysSetObjectData( ObjectId, ';')) THEN
    DO
       SAY 'error: Object' ObjectId 'not found.';
       SAY '       Build the class DLL and execute "run install" first.';
       rc = 2;
       LEAVE;
    END;

    /* check parms */
    PARSE ARG NewValue .;
    NewValue = STRIP( NewValue);
    IF (NewValue = '') THEN
    DO
       SAY 'error: no value specified.';
       rc = 87;
       LEAVE;
    END;
    IF (DATATYPE( NewValue) \= 'NUM') THEN
    DO
       SAY 'error: specified value is not numeric:' NewValue;
       rc = 87;
       LEAVE;
    END;
    IF ((NewValue < LowerLimit) | (NewValue > UpperLimit)) THEN
    DO
       SAY 'error: specified value does not fit into range:' NewValue;
       SAY '       Valid range:' LowerLimit'-'UpperLimit;
       rc = 87;
       LEAVE;
    END;

    /* assemble setup string*/
    Setup = 'TESTVALUE='NewValue';'
    SAY 'Setup string is:' Setup;

    /* setup object */
    CALL CHAROUT, 'Send setup string to' ObjectId '...';
    IF (SysSetObjectData( ObjectId, Setup)) THEN
       SAY ' Ok.';
    ELSE
       SAY ' ERROR!';

 END;

 RETURN( rc);

