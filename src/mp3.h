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

/*
# myPS2
# Author: tk
# Date: 11-29-2005
#
# File: MP3 playback
#
*/

#ifndef _MP3_H
#define	_MP3_H

#include <../lib/libsample/sample.h>
#include <sjpcm.h>
#include <../lib/libmad/mad.h>

//
// The MP3_* functions can be called from the UI to control
// MP3 playback.
//

int	MP3_Play( const char *pFileName );
void MP3_Stop( void );
int MP3_GetStatus( void );
void MP3_SetVolume( int nVolume );
int MP3_GetVolume( void );
void MP3_SetLooping( int nValue );
int MP3_GetLooping( void );
const char *MP3_GetTrackName( void );
int MP3_GetTrackLength( void );
int MP3_GetCurrentTime( void );
void MP3_SetPause( int nPause );
void MP3_NextTrack( void );
void MP3_PrevTrack( void );

int MP3_GetStreamStatus( void );
int MP3_OpenStream( const char *pURL );
void MP3_PlayStream( void );

// return values for MP3_OpenStream
#define STREAM_ERROR_BASE 70

enum {
	STREAM_ERROR_INIT		= STREAM_ERROR_BASE,
	STREAM_ERROR_URL,
	STREAM_ERROR_SOCKET,
	STREAM_ERROR_RESOLVE,
	STREAM_ERROR_CONNECT,
	STREAM_ERROR_SEND,
	STREAM_ERROR_ICY,

	STREAM_ERROR_OK,

	STREAM_ERROR_NUM
};

///////////////////////////////////////////
//

typedef struct listEntry_s listEntry_t;

struct listEntry_s {
	char *filename;

	listEntry_t *next;
	listEntry_t *prev;
};

enum {
	MP3_STOPPED,
	MP3_PLAYING,
	MP3_PAUSED,
	MP3_STREAMING	// streaming from a radio station
};

enum {
	MP3_STREAM_UNDERRUN,
	MP3_STREAM_NORMAL
};

#define MP3_STREAM_CHUNK	8000

#define MP3_FRAME_SYNC				0xFFE00000
#define MP3_MPEG_VERSION			0x180000
#define MP3_MPEG_VERSION_OFFSET		0x13
#define MP3_LAYER_DESC				0x60000
#define MP3_LAYER_DESC_OFFSET		0x11
#define MP3_BITRATE_INDEX			0xF000
#define MP3_BITRATE_INDEX_OFFSET	0x0C

#define MP3_NUM_MPEG_VERSION		2
#define MP3_NUM_LAYERS				3
#define MP3_NUM_BITRATES			15

#define MP3_MPEG_VERSION_1			3
#define MP3_MPEG_VERSION_2			2
#define MP3_MPEG_VERSION_2_5		0

#define MP3_LAYER_1					3
#define MP3_LAYER_2					2
#define MP3_LAYER_3					1

// ID3V2 stuff, see http://www.id3.org/id3v2.4.0-structure.txt
#define	MP3_ID3V2_FOOTER			0x10

// these should only be called from the mp3 playback thread
//

int _mp3_init( void );
void _mp3_playback( void *args );
int _mp3_decode( unsigned char const *start, unsigned long length );
enum mad_flow _mp3_input( void *data, struct mad_stream *stream );
enum mad_flow _mp3_output( void *data, struct mad_header const *header, struct mad_pcm *pcm );
enum mad_flow _mp3_error( void *data, struct mad_stream *stream, struct mad_frame *frame );
int _mp3_getbitrate( const char *pFileName );
int _mp3_getbitrate_mem( const u8 *pBuffer, int nBufSize );

void _mp3_stream_thread( void *args );
void _mp3_net_thread( void *args );

int mp3_stream_addBuffer( unsigned char *data, int size );
unsigned int _mp3_stream_pollBuffer( void );
int _mp3_stream_getBuffer( unsigned char *pOut, unsigned int size );
int _mp3_stream_flushBuffer( void );

typedef struct {
	char	name[128];
	char	genre[128];
	char	url[128];
	int		bitrate;
	int		meta_int;
} ICY_Header_t;

int _mp3_readIcyHeader( s32 s, ICY_Header_t *pICYHeader );
int _mp3_parseIcyLine( char *pLine, ICY_Header_t *pICYHeader );
int _mp3_parseMetaTitle( const char *pMeta, char *pTrackName, int nMaxSize );

struct audio_dither  {
	mad_fixed_t error[3];
	mad_fixed_t random;
};

inline unsigned long prng( unsigned long state );
inline short audio_linear_dither( unsigned int bits, mad_fixed_t sample, struct audio_dither *dither );

#endif
