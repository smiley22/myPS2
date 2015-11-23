#ifndef _SAMBA_H_
#define _SAMBA_H_

#include <sysconf.h>
#include <misc.h>
#include <file.h>

#include "../lib/libscew/scew.h"
#include "../lib/libps2time/libps2time.h"
#include "../lib/libsmb/ps2libsmb.h"

typedef struct {
	char	*pShareName;
	char	*pSharePath;
} smbShare_t;

int SMB_Init( void );
int SMB_GetNumShares( void );
const smbShare_t *SMB_GetShare( unsigned int i );
const char *SMB_GetShareNameByPath( const char *pPath );
int SMB_ParseShares( void );

#endif // _SAMBA_H_
