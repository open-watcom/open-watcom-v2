* Syntax:       ads <symbol_file_name>
*
* Synopsis:     Setup for debugging an AutoCAD ADS (or ADI) application.
*               The symbolic information is in <symbol_file_name>.
*
* Example:      ads fact.exp
*
*
/_dbg@ads_go_ok = 1;
if ?_dbg@ads_already {
    if ( ?ads_abort ) {
	/eip=&ads_abort;
	/*esp = 0;
	/*0 = 0;
    } {
	print {Could not locate symbol ads_abort. Cannot restart application}
	/_dbg@ads_go_ok = 0;
    }
}
if _dbg@ads_go_ok {
    print/p {Please type (xload "<1>")};
    go;
    if !?_dbg@ads_already {new/sym <1> _dbg@cs,_dbg@ds};
    /_dbg@ads_already = 1;
    if ?main {go main};
    if ?fmain {go fmain};
}
