ctl_combo SETFS_LANGUAGE
    fs_get, fs_set
    0, RC_VI_LANG_FIRST, RC_VI_LANG_LAST

ctl_check SETFS_LANGUAGE_PPKEYWORDONLY
    fs_get, fs_set
    *
    
ctl_check SETFS_LANGUAGE_CMODE
    fs_get, fs_set
    *
    
ctl_check SETFS_SOURCE_READENTIREFILE
    fs_get, fs_set
    *
    
ctl_check SETFS_SOURCE_READONLYCHECK
    fs_get, fs_set
    *
    
ctl_check SETFS_SOURCE_IGNORECTRLZ
    fs_get, fs_set
    *
    
ctl_check SETFS_SOURCE_CRLFAUTODETECT
    fs_get, fs_set
    *
    
ctl_check SETFS_SOURCE_WRITECRLF
    fs_get, fs_set
    *
   
ctl_check SETFS_SOURCE_EIGHTBITS
    fs_get, fs_set
    *
    
ctl_rint SETFS_TABS_TAB
    fs_get, fs_set
    0, -1
    
ctl_check SETFS_TABS_REAL
    fs_get, fs_set
    *
    
ctl_rint SETFS_TABS_HARD
    fs_get, fs_set
    0, -1
    
ctl_check SETFS_TABS_AUTOINDENT
    fs_get, fs_set
    *
    
ctl_rint SETFS_TABS_SHIFT
    fs_get, fs_set
    0, -1
   
ctl_text SETFS_TAGS_FILENAME
    fs_get, fs_set
    TAGFILENAMEWIDTH
    
ctl_check SETFS_TAGS_IGNORECASE
    fs_get, fs_set
    *
    
ctl_check SETFS_TAGS_PROMPT
    fs_get, fs_set
    *
    
ctl_text SETFS_MISC_GREP
    fs_get, fs_set
    GREPDEFAULTWIDTH
    
ctl_check SETFS_MISC_SHOWMATCH
    fs_get, fs_set
    *
