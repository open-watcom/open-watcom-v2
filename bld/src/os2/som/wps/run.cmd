/*
 *  RUN.CMD - Open Watcom Sample WPS Class (De)Installation Script
 *
 *  This script is to be called by the sample makefile
 *
 *  Syntax: class I[nstall]|D[einstall]
 *
 *  Default action is "Install". The WPS Class DLL must be built
 *  before executing "Install".
 */

 call RxFuncAdd    'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
 call SysLoadFuncs

 TRUE  = (1 = 1);
 FALSE = (0 = 1);

 rc = 0;

 ClassName      = 'OWObject';
 ClassDll       = 'owobject.dll';
 ObjectName     = 'Open Watcom WPS Sample Object';
 ObjectLocation = '<WP_DESKTOP>';
 ObjectId       = '<OWSAMPLE_WPS>'

 DO 1
    /* get parm - choose install as default */
    PARSE ARG Action Flags;
    Action = TRANSLATE( STRIP( Action));
    Flags  = TRANSLATE( STRIP( Flags));
    IF (Action = '') THEN
       Action = 'I';

    /* check if DLL exists */
    ClassDllFullname = STREAM( ClassDll, 'C', 'QUERY EXISTS');
    IF (ClassDllFullname = '') THEN
    DO
       IF (WORDPOS( "-Q", Flags) = 0) THEN
          SAY 'class DLL' ClassDll 'not found, build it first.';
       rc = 2;
       LEAVE;
    END;

    /* ================================================================ */

    /* perform action */
    SELECT

       WHEN (POS( Action, 'INSTALL') = 1)   THEN
       DO
          /* register WPS class DLL */
          CALL CHAROUT, 'Registering WPS Class "'ClassName'" of' ClassDll '...';
          IF (SysRegisterObjectClass( ClassName, ClassDllFullname)) THEN
             SAY ' Ok.';
          ELSE
          DO
             SAY ' ERROR!';
             LEAVE;
          END;

          /* create WPS object of class */
          CALL CHAROUT, 'Creating WPS object "'ObjectName'" ...';
          IF (SysCreateObject( ClassName,  ObjectName,  ObjectLocation, 'OBJECTID='ObjectId';', 'U')) THEN
             SAY ' Ok.';
          ELSE
          DO
             SAY ' ERROR!';
             LEAVE;
          END;

          /* open properties */
          CALL CHAROUT, 'Opening object properties ...';
          IF (SysSetObjectData( ObjectId, 'OPEN=DEFAULT')) THEN
             SAY ' Ok.';
          ELSE
             SAY ' ERROR!';

       END;

       /* --------------------------------------------- */

       WHEN (POS( Action, 'DEINSTALL') = 1) THEN
       DO
          /* destroy object if it exists */
          IF (SysSetObjectData( ObjectId, ';')) THEN
          DO
             CALL CHAROUT, 'Destroying WPS object "'ObjectName'" ...';
             IF (SysDestroyObject( ObjectId)) THEN
                SAY ' Ok.';
             ELSE
                SAY ' ERROR!';
          END;
          ELSE
             SAY 'WPS object does not exist.'

          /* check if WPS class is registered */
          fDeregister = FALSE;
          List.0 = 0;
          rcx = SysQueryClassList( 'List.');
          IF (List.0 = 0) THEN
             fDeregister = TRUE;
          ELSE
          DO i = List.0 TO 1 BY -1
             PARSE VAR List.i ThisClass .;
             IF (ThisClass = ClassName) THEN
             DO
                fDeregister = TRUE;
                LEAVE;
             END;
          END;

          IF (fDeregister) THEN
          DO
             CALL CHAROUT, 'Deregistering WPS class "'ClassName'" ...';
             IF (SysDeregisterObjectClass( ClassName)) THEN
                SAY ' Ok.';
             ELSE
                SAY ' Error.';
          END;
          ELSE
             SAY 'WPS class is not registered.';

       END;

       /* --------------------------------------------- */

       OTHERWISE
       DO
          SAY 'error: invalid action specified.'
          rc = 87;
       END;
    END;


 END;

 RETURN( rc);

