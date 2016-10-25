ctl_combo SETFS_LANGUAGESELECT
    fs_get, fs_set
    0, VI_LANG_FIRST, VI_LANG_LAST

ctl_check SETFS_PPKEYWORDONLY
    fs_get, fs_set
    *
    
ctl_check SETFS_CMODE
    fs_get, fs_set
    *
    
ctl_check SETFS_READENTIREFILE
    fs_get, fs_set
    *
    
ctl_check SETFS_READONLYCHECK
    fs_get, fs_set
    *
    
ctl_check SETFS_IGNORECTRLZ
    fs_get, fs_set
    *
    
ctl_check SETFS_CRLFAUTODETECT
    fs_get, fs_set
    *
    
ctl_check SETFS_WRITECRLF
    fs_get, fs_set
    *
   
ctl_check SETFS_EIGHTBITS
    fs_get, fs_set
    *
    
ctl_rint SETFS_TABAMOUNT
    fs_get, fs_set
    0, -1
    
ctl_check SETFS_REALTABS
    fs_get, fs_set
    *
    
ctl_rint SETFS_HARDTAB
    fs_get, fs_set
    0, -1
    
ctl_check SETFS_AUTOINDENT
    fs_get, fs_set
    *
    
ctl_rint SETFS_SHIFTWIDTH
    fs_get, fs_set
    0, -1
   
ctl_text SETFS_TAGFILENAME
    fs_get, fs_set
    TAGFILENAMEWIDTH
    
ctl_check SETFS_IGNORETAGCASE
    fs_get, fs_set
    *
    
ctl_check SETFS_TAGPROMPT
    fs_get, fs_set
    *
    
ctl_text SETFS_GREPDEFAULT
    fs_get, fs_set
    GREPDEFAULTWIDTH
    
ctl_check SETFS_SHOWMATCH
    fs_get, fs_set
    *
