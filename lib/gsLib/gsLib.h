/*
=================================================================
Copyright (C) 2005-2006 Torben Koenke

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA  02110-1301, USA.
=================================================================
*/

#ifndef _GSLIB_H_
#define _GSLIB_H_

#include <tamtypes.h>
#include <kernel.h>
#include <string.h>

#ifdef HAVE_LIBJPG
#include <../libjpg/libjpg.h>
#endif

#ifdef HAVE_LIBPNG
#include <../libpng/libpng.h>
#endif

#include "gsTexture.h"
#include "gsCore.h"
#include "gsPrimitive.h"
#include "gsFont.h"
#include "gsMisc.h"

#include "../dmaKit/dmaKit.h"

extern GSGLOBAL gsGlobal;

#endif
