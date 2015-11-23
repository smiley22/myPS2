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

#include "include/png.h"
#include "libpng.h"
#include <tamtypes.h>
#include <kernel.h>

typedef struct {
	png_structp	png_ptr;
	png_infop	info_ptr, end_info;

	u8 *buf;
	int pos;

	u8 *data;
} pngPrivate;

static void read_data_fn( png_structp png_ptr, png_bytep buf, png_size_t size )
{
	pngPrivate *priv = (pngPrivate*) png_get_io_ptr(png_ptr);

	memcpy( buf, priv->buf + priv->pos, size );
	priv->pos += size;
}

pngData *pngOpenRAW( u8 *data, int size )
{
	pngData		*png;
	pngPrivate	*priv;

	if( png_sig_cmp( data, 0, 8 ) != 0 )
		return NULL;

	if( (png = malloc( sizeof(pngData) )) == NULL )
		return NULL;

	memset( png, 0, sizeof(pngData) );

	if( (priv = malloc( sizeof(pngPrivate) )) == NULL )
		return NULL;

	png->priv = priv;

	priv->png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
	if( !priv->png_ptr ) {
		free(png);
		return NULL;
	}

	priv->info_ptr = png_create_info_struct(priv->png_ptr);
	if( !priv->info_ptr ) {
		free(png);
		png_destroy_read_struct( &priv->png_ptr, NULL, NULL );
		return NULL;
	}

	priv->end_info = png_create_info_struct(priv->png_ptr);
	if( !priv->end_info ) {
		free(png);
		png_destroy_read_struct( &priv->png_ptr, &priv->info_ptr, NULL );
		return NULL;
	}

	priv->buf	= data;
	priv->pos	= 0;

	png_set_read_fn( priv->png_ptr, (png_voidp)priv, read_data_fn );
	png_read_png( priv->png_ptr, priv->info_ptr, PNG_TRANSFORM_IDENTITY, NULL );

	png->width	= priv->info_ptr->width;
	png->height	= priv->info_ptr->height;
	png->bpp	= priv->info_ptr->channels * 8;

	return png;
}

pngData *pngOpen( char *filename )
{
	pngData *png;
	pngPrivate *priv;
	int fh, size;
	u8 *data;

	if( (fh = fioOpen( filename, O_RDONLY )) < 0 )
		return NULL;

	size = fioLseek( fh, 0, SEEK_END );
	fioLseek( fh, 0, SEEK_SET );

	if( (data = malloc(size)) == NULL ) {
		fioClose(fh);
		return NULL;
	}

	fioRead( fh, data, size );
	fioClose(fh);

	if( (png = pngOpenRAW( data, size )) == NULL )
		return NULL;

	priv = png->priv;
	priv->data = data;

	return png;
}

int pngReadImage( pngData *png, u8 *dest )
{
	pngPrivate *priv = png->priv;
	u8 **row_pointers;
	int i, row_ptr;

	int y;

	row_pointers = png_get_rows( priv->png_ptr, priv->info_ptr );
	row_ptr = 0;		

	for( i = 0; i < priv->info_ptr->height; i++ ) {
		memcpy( dest + row_ptr, row_pointers[i], priv->info_ptr->rowbytes );

		// need to normalize alpha channel to ps2 range
		if( priv->info_ptr->channels == 4 ) {
			for( y = 3; y < priv->info_ptr->rowbytes; y += 4 )
				*(dest + row_ptr + y ) /= 2;
		}

		row_ptr += priv->info_ptr->rowbytes;
	}

	return 1;

}

void pngClose( pngData *png )
{
	pngPrivate *priv = png->priv;

	png_destroy_read_struct( &priv->png_ptr, &priv->info_ptr, &priv->end_info );

	if( priv->data )
		free(priv->data);

	free(priv);
	free(png);
}
