
 WDM Driver Sample
 -----------------

 This directory contains a small (not necessarily trivial) small WDM driver.
This driver works with the system PCI device in a VirtualBox virtual machine.

 The driver does not implement any device specific functionality, it only
claims a PCI device and inserts itself into the WDM driver stack.

 The sample driver supports one custom IOCTL which returns the device's
PCI IDs. The 'app' directory contains a small application which shows how
the IOCTL may be used.
