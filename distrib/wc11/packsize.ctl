[ LOG packsize.log ]

[ INCLUDE startit.ctl ]

[ BLOCK .<DROOT> . ]
    echo.
    echo **********************************
    echo ** You need to run DSTART first **
    echo **********************************
    echo.
    
[ BLOCK . . ]    

    watfile /bye (t:200)..\supp\pcksize1.pgm <DCTRL>
    ..\supp\packsize <DROOT>
    watfile /bye (t:200)..\supp\pcksize2.pgm <DCTRL>
    del sizem.*
