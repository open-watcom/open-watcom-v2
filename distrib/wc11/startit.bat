if not [%started%] == [] goto end
    call ..\DSTART.BAT %relroot%\rel2 lang%ver%.dat 07-15-96 11:00:00 %setupdr%\setupgui -0
:end
set started=1
