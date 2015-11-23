#ifndef PS2_LIB_SMB_H
#define PS2_LIB_SMB_H

#include <sys/stat-dj.h>
#include "libsmbclient.h"

typedef void (*ps2_smb_log_callback)(const char *logMessage);

void ps2_smb_setInterface(char* ip, char* subnet);
void ps2_smb_getInterface(char *ip, char *subnet);
void ps2_smb_setWorkgroup(char *workgroup);
void ps2_smb_setLogCallback( ps2_smb_log_callback fn );

int lp_do_parameter(int snum, const char *pszParmName, const char *pszParmValue);

#endif // PS2_LIB_SMB_H
