/****************************************************************************
FILE   : empty-dir.js
SUBJECT: Script to name all empty folders beneath the current folder.

****************************************************************************/

//
// This function accepts a folder and processes all contained objects.
//
function process_folder(current)
{
  var empty = 1;
  var fc;

  // Enumerate all the files.
  fc = new Enumerator(current.Files);
  while (!fc.atEnd()) {
    // WScript.echo(fc.item());
    empty = 0;
    fc.moveNext();
  }

  // Enumerate all the subfolders.
  fc = new Enumerator(current.SubFolders);
  while (!fc.atEnd()) {
    empty = 0;
    // WScript.echo(fc.item());
    process_folder(fc.item());
    fc.moveNext();
  }

  // Display the name of this folder if it is empty.
  if (empty) {
    WScript.echo(current);
  }
}


// ===============
// The main script
// ===============

var fso = new ActiveXObject("Scripting.FileSystemObject");
var f = fso.GetFolder(".");
process_folder(f);

