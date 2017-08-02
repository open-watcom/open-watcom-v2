/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   %       Copyright (C) 1992, by WATCOM Systems Inc. All rights     %
   %       reserved. No part of this software may be reproduced      %
   %       in any form or by any means - graphic, electronic or      %
   %       mechanical, including photocopying, recording, taping     %
   %       or information storage and retrieval systems - except     %
   %       with the written permission of WATCOM Systems Inc.        %
   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
   Date		By		Reason
   ----		--		------
   18-jun-92	Craig Eisler	defined
   21-jun-92	D.J.Gaudet	added ExitWithUsage()
*/
#ifndef _INCLUDED_GETOPT_H
#define _INCLUDED_GETOPT_H

int GetOpt( int *argc, char *argv[], char *optstr, char *usage[] );

/*
 * optstr: contains a list of option characters.  If an option character
 * 	   is followed by a ':', then the option requires a parameter
 *	   If an option character is followed by '::', then the option
 *	   has an optional parameter, which must be specified after the
 * 	   option character.
 *
 *	   If the first character in optstr is a '#', then an option
 *	   specified as a number is returned in its entirety, and
 *	   GetOpt returns a '#'; e.g., if -1234 is typed, OptArg
 *	   points to the string "1234", and GetOpt returns '#'.
 *
 * usage:  is an array of strings describing the functionality of the
 *	   command, must be NULL terminated.  The first element of
 *	   the array must describe the command in brief.
 *
 * When there are no options left, GetOpt returns -1 and argc contains
 * the number of non-option parameters.  Argv is collapsed to contain only
 * the non-option paramters
 */

extern char	*OptArg;
extern int	OptInd;
extern char	*OptEnvVar; /* must be defined, specifies env var to search */
extern char	OptChar;
extern char	AltOptChar;

enum {
    _USAGE_ALL,
    _USAGE_BRIEF
};

extern void	ExitWithUsage( const char *__usage[], int __type);
/*
 * __usage:	same as the usage parameter for GetOpt
 * __type:	_USAGE_ALL: print entire usage text
 *		_USAGE_ERROR: print just the brief line
 */

#endif
