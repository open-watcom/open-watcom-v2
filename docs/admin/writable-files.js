/****************************************************************************
FILE   : writable-files.js
SUBJECT: Script to name all writable files beneath the current folder.

****************************************************************************/

//
// This function accepts a folder and processes all contained objects.
//
function process_folder(current)
{
  var fc;

  // Enumerate all the files.
  fc = new Enumerator(current.Files);
  while (!fc.atEnd()) {
    if (! (fc.item().attributes & 1) ) {
      WScript.echo(fc.item());
    }
    fc.moveNext();
  }

  // Enumerate all the subfolders.
  fc = new Enumerator(current.SubFolders);
  while (!fc.atEnd()) {
    process_folder(fc.item());
    fc.moveNext();
  }
}


// ===============
// The main script
// ===============

var fso = new ActiveXObject("Scripting.FileSystemObject");
var f = fso.GetFolder(".");
process_folder(f);

