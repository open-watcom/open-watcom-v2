/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   %     Copyright (C) 1992, by WATCOM International Inc.  All rights    %
   %     reserved.  No part of this software may be reproduced or        %
   %     used in any form or by any means - graphic, electronic or       %
   %     mechanical, including photocopying, recording, taping or        %
   %     information storage and retrieval systems - except with the     %
   %     written permission of WATCOM International Inc.                 %
   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
   Date		By		Reason
   ----		--		------
   920707	D.J.Gaudet	added audit trail
*/
#ifndef MISC_H_INCLUDED
#define MISC_H_INCLUDED

void Quit( const char *usage_msg[], const char *msg, ... );
void Die( const char *str, ... );

long GetClusterSize( unsigned drive );

void *MemAlloc( unsigned size );
void *MemRealloc( void *orig, unsigned size );
void MemFree( void *ptr );

#endif
