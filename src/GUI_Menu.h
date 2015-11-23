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

#include <GUI.h>
#include <File.h>

/////////////////////////////////////////////////
// GUI_M_DlgKeyboard

unsigned int GUI_CB_DlgKeyboard( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
								 unsigned int nCtrlParam, unsigned int nOther );

void	GUI_DlgKeyboard( const char *lpEdit, const char *lpCaption, unsigned int nDlgID );

#define	DLG_RET_SUBMIT	0
#define DLG_RET_CANCEL	1

/////////////////////////////////////////////////
// GUI_M_DlgYesNo

unsigned int GUI_CB_DlgYesNo( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
							  unsigned int nCtrlParam, unsigned int nOther );

void	GUI_DlgYesNo( const char *lpText, const char *lpCaption, unsigned int nDlgID );

#define DLG_RET_NO		0
#define	DLG_RET_YES		1

/////////////////////////////////////////////////
// GUI_M_Language

unsigned int GUI_CB_Language( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
							  unsigned int nCtrlParam, unsigned int nOther );

/////////////////////////////////////////////////
// GUI_M_Main

unsigned int GUI_CB_Main( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg, unsigned int nCtrlParam,
						  unsigned int nOther );

/////////////////////////////////////////////////
// GUI_M_MyPictures

unsigned int GUI_CB_MyPictures( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
								unsigned int nCtrlParam, unsigned int nOther );

void	MyPictures_Init( void );
int		BrowsingRoot( const char *lpPath );
void	SetBrowseDir( const char *pDirPath );
void	RefreshPanel( void );
int		SortPanelByName( const void *a, const void *b );
int		SortPanelBySize( const void *a, const void *b );
int		SortPanelByType( const void *a, const void *b );
int		CreateThumbnail( const fileInfo_t *pFile, const char *pCurrentDir, GSTEXTURE **pTexture );
int		TbnCacheLoad( const fileInfo_t *pFile, const char *pDir, GSTEXTURE **pTexture );
int		TbnCacheSave(	const fileInfo_t *pFile, const char *pCurrentDir, int nWidth,
						int nHeight, void *pRGBData );

typedef struct
{
	u8		magic[2];
	u32		size;
	u8		width;
	u8		height;
} TBN_HEADER;

unsigned int GUI_CB_DlgThumb( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
							  unsigned int nCtrlParam, unsigned int nOther );

/////////////////////////////////////////////////
// GUI_M_MyPrograms

unsigned int GUI_CB_MyPrograms( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
								unsigned int nCtrlParam, unsigned int nOther );

/////////////////////////////////////////////////
// GUI_M_MyPictures

unsigned int GUI_CB_MyFiles( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
							 unsigned int nCtrlParam, unsigned int nOther );

void	StartELF( void );

/////////////////////////////////////////////////
// GUI_M_DlgMsgBox

unsigned int GUI_CB_DlgMsgBox( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
							  unsigned int nCtrlParam, unsigned int nOther );

void GUI_DlgMsgBox( const char *lpText, const char *lpCaption, u32 nFlags );

#define MB_ICON_ERROR	0x00000001
#define MB_ICON_INFO	0x00000002
#define MB_ICON_OKAY	0x00000004

/////////////////////////////////////////////////
// GUI_CB_DlgPartition

unsigned int GUI_CB_DlgPartition1( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
								   unsigned int nCtrlParam, unsigned int nOther );

unsigned int GUI_CB_DlgPartition2( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
								   unsigned int nCtrlParam, unsigned int nOther );

#define	DLG_RET_CREATE		0

#define DLG_ERR_NOERROR		0
#define DLG_ERR_CREATE		1		// Error creating partition
#define DLG_ERR_MOUNT		2		// Error mounting partition

/////////////////////////////////////////////////
// GUI_M_MyFiles

unsigned int GUI_CB_DlgCopy( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
							 unsigned int nCtrlParam, unsigned int nOther );

/////////////////////////////////////////////////
// GUI_M_MyMusic

unsigned int GUI_CB_MyMusic( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
							 unsigned int nCtrlParam, unsigned int nOther );

/////////////////////////////////////////////////
// GUI_M_Network

unsigned int GUI_CB_Network( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
							 unsigned int nCtrlParam, unsigned int nOther );

/////////////////////////////////////////////////
// GUI_M_Partitions

unsigned int GUI_CB_Partitions( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
								unsigned int nCtrlParam, unsigned int nOther );

/////////////////////////////////////////////////
// GUI_M_Radio

unsigned int GUI_CB_Radio( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg, unsigned int nCtrlParam,
						   unsigned int nOther );

typedef struct
{
	char 	*name;
	char 	*genre;
	char 	*playing;
	char	*url;
	int		listeners;
	int		bitrate;
} shoutEntry_t;

typedef struct
{
	char	*name;
	char	*url;
} shoutBookmark_t;

int		GetRadioStations( void );
int		ParseStationXML( const char *pXMLFile, int nSize );
void	RefreshStationList( void );
int		GetStationURL( const shoutEntry_t *pEntry, char *pStationURL, int nSize );
int		RadioSortName( const void *a, const void *b );
int		RadioSortGenre( const void *a, const void *b );
int		RadioSortBR( const void *a, const void *b );
int		RadioSortLC( const void *a, const void *b );
void	LoadRadioBookmarks( void );
void	FreeRadioBookmarks( void );
void	RefreshBookmarkList( void );

/////////////////////////////////////////////////
// GUI_M_Samba

unsigned int GUI_CB_Samba( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
						   unsigned int nCtrlParam, unsigned int nOther );

/////////////////////////////////////////////////
// GUI_M_Settings

unsigned int GUI_CB_Settings( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
							  unsigned int nCtrlParam, unsigned int nOther );

void	LoadSettings( void );

/////////////////////////////////////////////////
// GUI_M_Skins

unsigned int GUI_CB_Skins( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
						   unsigned int nCtrlParam, unsigned int nOther );

/////////////////////////////////////////////////
// GUI_M_View

unsigned int GUI_CB_View( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
						  unsigned int nCtrlParam, unsigned int nOther );

void	GUI_OpenViewMenu( const char *lpImageName );
