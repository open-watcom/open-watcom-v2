/*
 * Define IOCTL codes for use with the boxdev sample driver.
 */

#define IOCTL_BOXDEV_GET_IDS \
    CTL_CODE( FILE_DEVICE_UNKNOWN, 0x900, METHOD_BUFFERED, FILE_ANY_ACCESS )
