:H2.Augmented Device Definitions
:I1.augmented devices
:P.
Certain device operations are not selectable through
the device and driver definitions.
&WGML. augments some device definitions by directly
supporting these operations.
:P.
The augmented device definitions are recognized by
the starting characters of the driver name.
For example, :HP2.HPLDRV:EHP2. is recognized
as the name of a driver definition for the HP LaserJet printer.
:DL compact tsize=13.
:DTHD.Name Prefix
:DDHD.Augmented Device
:DT.HPL
:DD.HP LaserJet
:DT.HPLP
:DD.HP LaserJet Plus
:DT.MLT
:DD.Multiwriter V (emulation mode)
:DT.MLTE
:DD.Multiwriter V (express mode)
:DT.PCG
:DD.IBM PC Graphics
:DT.PS
:DD.PostScript
:eDL.
:P.
If the driver definition name begins with &mono.HPL&emono.
(but not &mono.HPLP&emono.), the value returned by the
%X_ADDRESS and %TABWIDTH device functions is in terms
of decipoints instead of dots.
