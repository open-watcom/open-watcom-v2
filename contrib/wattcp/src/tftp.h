/*
 * Boot-ROM-Code to load an operating system across a TCP/IP network.
 *
 * Module:  tftp.h
 * Purpose: Definitions for implementing the TFTP protocol
 * Entries: none
 *
 **************************************************************************
 *
 * Copyright (C) 1995,1996,1997 Gero Kuhlmann <gero@gkminix.han.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __TFTP_H
#define __TFTP_H

extern int (*tftp_writer)     (const void *buf, size_t length);
extern int (*tftp_terminator) (void);

extern int   tftp_init        (void);
extern int   tftp_boot_load   (void);

extern char *tftp_set_server     (const char *name, int len);
extern char *tftp_set_boot_fname (const char *name, int len);
extern char *tftp_set_xfer_mode  (const char *name, int len);

#endif
