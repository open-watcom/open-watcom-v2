call startit
mkdisk -0 c 1.4 mkdisk.c .\pack h:\rel2
copy setup.inf x:\installs\cbuild\wc10.5\setup.inf
copy setup.inf x:\installs\cbuild\wc10.5\diskimgs\disk01\setup.inf
wtouch /f x:\installs\cbuild\wc10.5\setup.exe x:\installs\cbuild\wc10.5\setup.inf
wtouch /f x:\installs\cbuild\wc10.5\diskimgs\disk01\setup.exe x:\installs\cbuild\wc10.5\diskimgs\disk01\setup.inf
