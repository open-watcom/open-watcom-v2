rem ****** Colour DV colours ********

if _dbg@dbg$monitor == 7 {
    paint all plain menu white on black;
    paint all standout menu brown on black;
    paint all disabled menu blue on black;
    paint all active menu brown on white;
    paint all active standout menu brown on white;
    paint all frame menu white on black;
    paint all disabled title white on white;
    paint all active frame cyan on white;
    paint all disabled frame white on white;
    paint all icon red on white;
    paint all active disabled menu blue on white;
    paint all plain white on white;
    paint all active white on black;
    paint all selected green on white;
    paint all standout red on white;
    paint all button white on white;
    paint all active standout white on black;
    paint status white on white
} else {
    paint all plain menu white on blue;
    paint all standout menu bright white on blue;
    paint all disabled menu grey on blue;
    paint all active menu bright white on black;
    paint all active standout menu yellow on black;
    paint all frame menu yellow on blue;
    paint all disabled title grey on bright white;
    paint all active frame yellow on cyan;
    paint all disabled frame grey on bright white;
    paint all icon bright white on red;
    paint all active disabled menu grey on black;
    paint all plain black on bright white;
    paint all active bright white on black;
    paint all selected bright white on red;
    paint all standout red on bright white;
    paint all button red on bright white;
    paint all active standout bright red on black;
    paint status bright white on blue;
    paint dialog plain white on blue;
    paint dialog frame blue on cyan;
    paint dialog shadow black on blue;
    paint dialog plain button blue on white;
    paint dialog standout button bright white on white;
    paint dialog active button blue on cyan;
    paint dialog active standout button bright white on cyan
}
