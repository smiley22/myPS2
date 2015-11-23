/*
# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright (c) 2003 Marcus R. Brown <mrbrown@0xd6.org>
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.
#
# $Id: ansistat.h 2006-03-06 dlanor $
# File attributes and directory entries.
# Special case for code needing ansi stat struct
# Separated from 'stat.h' for backwards compatibility
*/

#ifndef _ANSISTAT_H_
#define _ANSISTAT_H_

#include <sys/stat.h>

/* ANSI C stat structure */
#define	S_IFMT		0170000	/* type of file */
#define	S_IFDIR		0040000	/* directory */
#define	S_IFREG		0100000	/* regular */
#define	S_IFLNK		0120000	/* symbolic link */

#define	S_ISDIR(m)  (((m)&S_IFMT) == S_IFDIR)
#define	S_ISREG(m)  (((m)&S_IFMT) == S_IFREG)
#define	S_ISLNK(m)  (((m)&S_IFMT) == S_IFLNK)

struct stat {
	unsigned st_mode;  /* mode */
	unsigned st_size;  /* file size */

	time_t st_mtime;   /* modification time */
	time_t st_atime;   /* access time */
	time_t st_ctime;   /* creation time */
};

#endif  /* _ANSISTAT_H_ */