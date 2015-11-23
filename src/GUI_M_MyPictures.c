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
#include <GUI_Ctrl.h>
#include <GUI_Menu.h>

#include <Imgscale.h>
#include <SysConf.h>

#define ID_CACHE_IMAGES		103
#define ID_SORT_BY_NAME		104
#define ID_SORT_BY_SIZE		105
#define ID_SORT_BY_TYPE		106
#define ID_THUMBNAIL_PANEL	108
#define ID_IMAGE_ERROR		109
#define ID_IMAGE_DIR		110

static char					szCurDir[ MAX_PATH + 1 ]	= {0};
static fileInfo_t			fileInfo[ MAX_DIR_FILES ];
static unsigned int			nNumFiles;
static const GUIMenuImage_t	*pErrImage, *pDirImage;

unsigned int GUI_CB_MyPictures( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
								unsigned int nCtrlParam, unsigned int nOther )
{
	GUIThumbItem_t		*pItem;
	const fileInfo_t	*pFile;
	unsigned int		nRet, nOffset;
	char				szNewPath[MAX_PATH + 1], *pStr;

	switch( nGUIMsg )
	{
		case GUI_MSG_OPEN:
			pErrImage = GUI_MenuGetImage( lpGUIMenu,
				((GUICtrl_Image_t*)GUI_ControlByID(ID_IMAGE_ERROR)->pCtrl)->nTexture );

			pDirImage = GUI_MenuGetImage( lpGUIMenu,
				((GUICtrl_Image_t*)GUI_ControlByID(ID_IMAGE_DIR)->pCtrl)->nTexture );

			if( !szCurDir[0] )
				MyPictures_Init();

			break;

		case GUI_MSG_CLOSE:
			GUI_Ctrl_ThumbnailPanel_VramClean( GUI_ControlByID(ID_THUMBNAIL_PANEL) );
			break;

		case GUI_MSG_CONTROL:
			switch( LOWORD(nCtrlParam) )
			{
				case ID_CACHE_IMAGES:
					GUI_OpenDialog( GUI_MENU_DLGTHUMB, lpGUIMenu, 0 );
					break;

				case ID_SORT_BY_NAME:
					if( nNumFiles )
					{
						qsort( fileInfo, nNumFiles, sizeof(fileInfo_t), SortPanelByName );
						RefreshPanel();
					}
					break;

				case ID_SORT_BY_SIZE:
					if( nNumFiles )
					{
						qsort( fileInfo, nNumFiles, sizeof(fileInfo_t), SortPanelBySize );
						RefreshPanel();
					}
					break;

				case ID_SORT_BY_TYPE:
					break;

				case ID_GO_BACK:
					GUI_OpenMenu( GUI_MENU_MAIN );
					break;

				case ID_THUMBNAIL_PANEL:
					switch( HIWORD(nCtrlParam) )
					{
						case GUI_NOT_NEED_INFO:
							pItem = (GUIThumbItem_t*) nOther;
							pFile = (fileInfo_t*) pItem->pUser;

							pItem->pStr	= pFile->name;

							// device list
							if( !szCurDir[0] )
							{
								if( !strncmp( pFile->name, "pfs", 3 ) )
								{
									pItem->pStr = HDD_GetPartition(pFile->name);
								}
								else if( !strncmp( pFile->name, "smb", 3 ) )
								{
									pItem->pStr = SMB_GetShareNameByPath(pFile->name);
								}
							}

							nRet = CreateThumbnail( pFile, szCurDir, &pItem->pTexture );
							if( nRet == 0 )
								pItem->pTexture = pErrImage->gsTexture;
							break;

						case GUI_NOT_REMOVE:
							pItem = (GUIThumbItem_t*) nOther;

							if( pItem->pTexture )
							{
								if( pItem->pTexture && pItem->pTexture != pErrImage->gsTexture &&
									pItem->pTexture != pDirImage->gsTexture )
									gsLib_texture_free( pItem->pTexture );
							}

							pItem->pTexture = NULL;
							pItem->pStr		= NULL;
							break;

						case GUI_NOT_THUMB:
							pFile = (fileInfo_t*) ((GUIThumbItem_t*)nOther)->pUser;
							pItem = (GUIThumbItem_t*) nOther;

							if( pFile->flags & FLAG_DIRECTORY )
							{
								if( !strcmp( pFile->name, ".." ) )
								{
									if( BrowsingRoot(szCurDir) )
									{
										MyPictures_Init();
										GUI_Render();
										return 0;
									}
									else
									{
										nOffset = strlen(szCurDir) - 1;
										strncpy( szNewPath, szCurDir, nOffset );
										szNewPath[ nOffset ] = 0;

										if( !(pStr = strrchr( szNewPath, '/' )) )
											break;

										nOffset = pStr - szNewPath + 1;
										strncpy( szNewPath, szCurDir, nOffset );
										szNewPath[ nOffset ] = 0;
									}
								}
								else
								{
									snprintf( szNewPath, MAX_PATH, "%s%s", szCurDir,
											  pFile->name );

									if( szNewPath[ strlen(szNewPath) - 1 ] != '/' )
										strncat( szNewPath, "/", MAX_PATH );
								}

								SetBrowseDir( szNewPath );
							}
							else
							{
								// clicked an image thumbnail
								if( pItem->pTexture != pErrImage->gsTexture &&
									pItem->pTexture != pDirImage->gsTexture )
								{
									snprintf( szNewPath, sizeof(szNewPath), "%s%s",
											  szCurDir, pFile->name );

									GUI_OpenViewMenu( szNewPath );
								}
							}
							break;
					}
					break;
			}
			break;
	}

	return 0;
}

void MyPictures_Init( void )
{
	int					nHDD, i;
	char				szPfs[32];
	const char			*pMnt;
	const smbShare_t	*smbShare;

	szCurDir[0]	= 0;
	nNumFiles	= 0;

	GUI_Ctrl_ThumbnailPanel_Clean( GUI_ControlByID(ID_THUMBNAIL_PANEL) );

	if( (nHDD = HDD_Available()) )
	{
		if( nHDD == HDD_AVAIL )
		{
			fileInfo[ nNumFiles ].flags		= FLAG_DIRECTORY;
			fileInfo[ nNumFiles ].size		= 0;

			strcpy( fileInfo[ nNumFiles ].name, "pfs0:/" );

			nNumFiles++;

			// add boot partition
			if( GetBootMode() == BOOT_HDD )
			{
				if( (pMnt = BootMntPoint()) )
				{
					if( strcmp( pMnt, "pfs0" ) )
					{
						fileInfo[ nNumFiles ].flags		= FLAG_DIRECTORY;
						fileInfo[ nNumFiles ].size		= 0;

						strcpy( fileInfo[ nNumFiles ].name, "pfs1:/" );

						nNumFiles++;
					}
				}
			}
		}

		// add other mounted partitions to list
		for( i = 0; i < HDD_NumMounted(); i++ )
		{
			snprintf( szPfs, sizeof(szPfs), "pfs%i:/", i + 2 );

			fileInfo[ nNumFiles ].flags		= FLAG_DIRECTORY;
			fileInfo[ nNumFiles ].size		= 0;

			strcpy( fileInfo[ nNumFiles ].name, szPfs );

			nNumFiles++;
		}
	}

	if( MC_Available(0) )
	{
		fileInfo[ nNumFiles ].flags		= FLAG_DIRECTORY;
		fileInfo[ nNumFiles ].size		= 0;

		strcpy( fileInfo[ nNumFiles ].name, "mc0:/" );

		nNumFiles++;
	}

	if( MC_Available(1) )
	{
		fileInfo[ nNumFiles ].flags		= FLAG_DIRECTORY;
		fileInfo[ nNumFiles ].size		= 0;

		strcpy( fileInfo[ nNumFiles ].name, "mc1:/" );

		nNumFiles++;
	}

	if( USB_Available() )
	{
		fileInfo[ nNumFiles ].flags		= FLAG_DIRECTORY;
		fileInfo[ nNumFiles ].size		= 0;

		strcpy( fileInfo[ nNumFiles ].name, "mass:/" );

		nNumFiles++;
	}

	for( i = 0; i < SMB_GetNumShares(); i++ )
	{
		smbShare = SMB_GetShare(i);

		fileInfo[ nNumFiles ].flags		= FLAG_DIRECTORY;
		fileInfo[ nNumFiles ].size		= 0;

		strcpy( fileInfo[ nNumFiles ].name, smbShare->pSharePath );

		nNumFiles++;
	}

	fileInfo[ nNumFiles ].flags		= FLAG_DIRECTORY;
	fileInfo[ nNumFiles ].size		= 0;

	strcpy( fileInfo[ nNumFiles ].name, "cdfs:/" );

	nNumFiles++;

	for( i = 0; i < nNumFiles; i++ )
		GUI_Ctrl_ThumbnailPanel_AddItem( GUI_ControlByID(ID_THUMBNAIL_PANEL), &fileInfo[i] );
}

int BrowsingRoot( const char *lpPath )
{
	unsigned int i;

	if( IsPartitionRoot(lpPath) )
		return 1;

	if( !strcmp( lpPath, "cdfs:/" ) || !strcmp( lpPath, "mc0:/" ) ||
		!strcmp( lpPath, "mc1:/" )  || !strcmp( lpPath, "mass:/" ) )
		return 1;

	for( i = 0; i < SMB_GetNumShares(); i++ )
	{
		if( !strcmp( lpPath, SMB_GetShare(i)->pSharePath ) )
			return 1;
	}

	return 0;
}


void SetBrowseDir( const char *pDirPath )
{
	int nCount, i;

	strncpy( szCurDir, pDirPath, MAX_PATH );
	szCurDir[MAX_PATH] = 0;

	fileInfo[0].flags	= FLAG_DIRECTORY;
	fileInfo[0].size	= 0;

	strcpy( fileInfo[0].name, ".." );

	nCount		= DirGetContents( szCurDir, "jpg", &fileInfo[1], MAX_DIR_FILES - 1);
	nNumFiles	= nCount + 1;

	GUI_Ctrl_ThumbnailPanel_Clean( GUI_ControlByID(ID_THUMBNAIL_PANEL) );

	for( i = 0; i < nNumFiles; i++ )
		GUI_Ctrl_ThumbnailPanel_AddItem( GUI_ControlByID(ID_THUMBNAIL_PANEL), &fileInfo[i] );

	GUI_Render();
}

void RefreshPanel( void )
{
	unsigned int i;

	GUI_Ctrl_ThumbnailPanel_Clean( GUI_ControlByID(ID_THUMBNAIL_PANEL) );

	for( i = 0; i < nNumFiles; i++ )
		GUI_Ctrl_ThumbnailPanel_AddItem( GUI_ControlByID(ID_THUMBNAIL_PANEL), &fileInfo[i] );

	GUI_Render();
}

int SortPanelByName( const void *a, const void *b )
{
	fileInfo_t *p1, *p2;

	p1 = (fileInfo_t*)a;
	p2 = (fileInfo_t*)b;

	if( (p1->flags & FLAG_DIRECTORY) && !(p2->flags & FLAG_DIRECTORY) )
		return -1;
	else if( !(p1->flags & FLAG_DIRECTORY) && (p2->flags & FLAG_DIRECTORY) )
		return 1;

	return strcmp( p1->name, p2->name );
}

int SortPanelBySize( const void *a, const void *b )
{
	fileInfo_t *p1, *p2;

	p1 = (fileInfo_t*)a;
	p2 = (fileInfo_t*)b;

	if( (p1->flags & FLAG_DIRECTORY) && !(p2->flags & FLAG_DIRECTORY) )
		return -1;
	else if( !(p1->flags & FLAG_DIRECTORY) && (p2->flags & FLAG_DIRECTORY) )
		return 1;

	return ( p1->size - p2->size );
}

int SortPanelByType( const void *a, const void *b )
{
	fileInfo_t *p1, *p2;
	char *ext1, *ext2;

	p1 = (fileInfo_t*)a;
	p2 = (fileInfo_t*)b;

	if( (p1->flags & FLAG_DIRECTORY) && !(p2->flags & FLAG_DIRECTORY) )
		return -1;
	else if( !(p1->flags & FLAG_DIRECTORY) && (p2->flags & FLAG_DIRECTORY) )
		return 1;

	// grab the file extensions
	ext1 = strrchr( p1->name, '.' );
	ext2 = strrchr( p2->name, '.' );

	if( !ext1 && !ext2 )
		return 0;

	if( !ext1 )
		return -1;

	if( !ext2 )
		return 1;

	// skip dot
	ext1++;
	ext2++;

	return strcmp( ext1, ext2 );
}

int CreateThumbnail( const fileInfo_t *pFile, const char *pDir, GSTEXTURE **pTexture )
{
	char 		*pFilename;
	u8 			*pRaw;
	FHANDLE		fh;
	jpgData		*pJpg;
	u8			*pImgData;
	int			nWidth, nHeight;
	float		fRatio;
	u8			*pResData;
	int			nRet, nSize;

	if( pFile->flags & FLAG_DIRECTORY )
	{
		if( pTexture )
			*pTexture = pDirImage->gsTexture;
		
		return 1;
	}

	if( TbnCacheLoad( pFile, pDir, pTexture ) )
		return 1;
	
	pFilename = (char*) malloc( strlen(pDir) + strlen(pFile->name) + 1 );
	if( !pFilename )
		return 0;
	
	strcpy( pFilename, pDir );
	strcat( pFilename, pFile->name );

	fh = FileOpen( pFilename, O_RDONLY );
	if( fh.fh < 0 )
	{
		free(pFilename);
		return 0;
	}

	nSize = FileSeek( fh, 0, SEEK_END );
	FileSeek( fh, 0, SEEK_SET );

	if( !nSize )
	{
		free(pFilename);
		return 0;
	}

	if( (pRaw = (u8*) malloc( nSize )) == NULL )
		return 0;


	FileRead( fh, pRaw, nSize );
	FileClose( fh );

	if( (pJpg = jpgOpenRAW( pRaw, nSize, JPG_WIDTH_FIX )) == NULL )
	{
		free(pFilename);
		free(pRaw);
		return 0;
	}

	if( pJpg->bpp != 24 )
	{
		jpgClose(pJpg);
		free(pFilename);
		free(pRaw);
		return 0;
	}

	pImgData = (u8*) malloc( ((3 * pJpg->width + 3) & ~3) * pJpg->height );
	if( !pImgData )
	{
		jpgClose(pJpg);
		free(pFilename);
		free(pRaw);
		return 0;
	}

	if( -1 == jpgReadImage( pJpg, pImgData ) )
	{
		jpgClose(pJpg);
		free(pFilename);
		free(pRaw);
		free(pImgData);
		return 0;
	}

	// resize dimensions
	if( pJpg->width > pJpg->height )
	{
		fRatio	= (float) pJpg->height / pJpg->width;
		nWidth 	= 64;
		nHeight	= (int) (nWidth * fRatio);

		// make sure Height is even
		if( nHeight % 2 )
			nHeight++;
	} 
	else
	{
		fRatio	= (float) pJpg->width / pJpg->height;
		nHeight	= 64;
		nWidth	= (int) (nHeight * fRatio);

		// make sure width is a multiple of 4 so
		// ScaleBitmap will always work correctly.
		nWidth -= (nWidth % 4);
	}

	nRet = ScaleBitmap( pImgData, pJpg->width, pJpg->height, &pResData, nWidth, nHeight );
	if( 0 == nRet )
	{
		jpgClose(pJpg);
		free(pFilename);
		free(pRaw);
		free(pImgData);
		return 0;
	}

	TbnCacheSave( pFile, pDir, nWidth, nHeight, pResData );

	if( pTexture )
		*pTexture = gsLib_texture_raw( nWidth, nHeight, GS_PSM_CT24, pResData, GS_CLUT_NONE,
									   NULL );

	jpgClose(pJpg);
	free(pRaw);
	free(pImgData);
	free( pFilename );

	if( pTexture && *pTexture == NULL )
		return 0;

	return 1;
}

int TbnCacheLoad( const fileInfo_t *pFile, const char *pDir, GSTEXTURE **pTexture )
{
	FHANDLE		fHandle;
	TBN_HEADER	tbnHeader;
	char		strPath[256], strTmp[256];
	char		*pStr;
	u8			*pImgData;
	int			nSize;

	if( !SC_GetValueForKey_Int( "tbn_caching", NULL ) )
		return 0;

	if( HDD_Available() == HDD_NOT_AVAIL )
		return 0;

	// build path to TBN file
	if( !SC_GetValueForKey_Str( "tbn_cache_path", strPath ) )
		return 0;

	if( strPath[ strlen(strPath) - 1 ] != '/' )
		strcat( strPath, "/" );

	strcpy( strTmp, pDir );

	if( (pStr = strchr( strTmp, ':' )) == NULL )
		return 0;

	*pStr = 0;
	pStr++;

	strcat( strPath, strTmp );
	strcat( strPath, pStr );

	// make sure directory path exists
	DirCreate( strPath );

	StripFileExt( strTmp, pFile->name );
	strcat( strPath, strTmp );
	strcat( strPath, ".TBN" );

	fHandle = FileOpen( strPath, O_RDONLY | O_CREAT );

	if( fHandle.fh < 0 )
		return 0;

	// file was just created by FileOpen
	nSize = FileSeek( fHandle, 0, SEEK_END );
	if( nSize == 0 )
	{
		FileClose( fHandle );
		return 0;
	}

	FileSeek( fHandle, 0, SEEK_SET );
	FileRead( fHandle, &tbnHeader, sizeof(tbnHeader) );

	if( tbnHeader.magic[0] != 'T' || tbnHeader.magic[1] != 'C' )
	{
		FileClose(fHandle);
		return 0;
	}

	if( tbnHeader.size != pFile->size )
		return 0;

	// read the RGB data
	pImgData = (u8*) memalign( 128, tbnHeader.width * tbnHeader.height * 3 );
	
	if( pImgData == NULL )
	{
		FileClose(fHandle);
		return 0;
	}

	FileRead( fHandle, pImgData, tbnHeader.width * tbnHeader.height * 3 );
	FileClose( fHandle );

	if( pTexture )
		*pTexture = gsLib_texture_raw( tbnHeader.width, tbnHeader.height, GS_PSM_CT24,
									   pImgData, GS_CLUT_NONE, NULL );

	free(pImgData);

	if( pTexture && *pTexture == NULL )
		return 0;

	return 1;
}

int TbnCacheSave( const fileInfo_t *pFile, const char *pCurrentDir, int nWidth,
				  int nHeight, void *pRGBData )
{
	FHANDLE		fHandle;
	TBN_HEADER	tbnHeader;
	char		strPath[256], strTmp[256];
	char		*pStr;

	if( !SC_GetValueForKey_Int( "tbn_caching", NULL ) )
		return 0;

	if( !SC_GetValueForKey_Str( "tbn_cache_path", strPath ) )
		return 0;

	if( HDD_Available() == HDD_NOT_AVAIL )
		return 0;

	// build path to TBN file
	if( strPath[ strlen(strPath) - 1 ] != '/' )
		strcat( strPath, "/" );

	strcpy( strTmp, pCurrentDir );

	if( (pStr = strchr( strTmp, ':' )) == NULL )
		return 0;

	*pStr = 0;
	pStr++;

	strcat( strPath, strTmp );
	strcat( strPath, pStr );

	StripFileExt( strTmp, pFile->name );
	strcat( strPath, strTmp );
	strcat( strPath, ".TBN" );

	fHandle = FileOpen( strPath, O_RDWR | O_CREAT | O_TRUNC );

	if( fHandle.fh < 0 )
		return 0;

	tbnHeader.magic[0]	= 'T';
	tbnHeader.magic[1]	= 'C';

	tbnHeader.size		= pFile->size;
	tbnHeader.width		= nWidth;
	tbnHeader.height	= nHeight;

	FileWrite( fHandle, &tbnHeader, sizeof(tbnHeader) );
	FileWrite( fHandle, pRGBData, nWidth * nHeight * 3 ); 
	FileClose( fHandle );

	return 1;
}

/////////////////////////////////////////////////
// GUI_M_DlgThumb

#define ID_PROGRESS		100
#define ID_OK_BUTTON	101
#define ID_PERCENT		102

unsigned int GUI_CB_DlgThumb( GUIMenu_t *lpGUIMenu, unsigned int nGUIMsg,
							  unsigned int nCtrlParam, unsigned int nOther )
{
	char szStr[32];
	unsigned int i, nSize = 0, nTotalSize = 0;

	switch( nGUIMsg )
	{
		case GUI_MSG_OPEN:
			for( i = 0; i < nNumFiles; i++ )
				nTotalSize += fileInfo[i].size;

			GUI_Ctrl_Progress_SetBounds( GUI_ControlByID(ID_PROGRESS), 0, nTotalSize );
			GUI_Ctrl_Progress_SetPos( GUI_ControlByID(ID_PROGRESS), 0 );
			GUI_Ctrl_Label_SetText( GUI_ControlByID(ID_PERCENT), "0 %" );

			for( i = 0; i < nNumFiles; i++ )
			{
				CreateThumbnail( &fileInfo[i], szCurDir, NULL );

				nSize = GUI_Ctrl_Progress_GetPos( GUI_ControlByID(ID_PROGRESS) );
				nSize += fileInfo[i].size;

				snprintf( szStr, sizeof(szStr), "%i %%",
					 (int) ((double) nSize / nTotalSize * 100.0) );

				GUI_Ctrl_Progress_SetPos( GUI_ControlByID(ID_PROGRESS), nSize );
				GUI_Ctrl_Label_SetText( GUI_ControlByID(ID_PERCENT), szStr );
				GUI_Render();
			}
			break;

		case GUI_MSG_CONTROL:
			switch( LOWORD(nCtrlParam) )
			{
				case ID_OK_BUTTON:
					GUI_CloseDialog( lpGUIMenu, 0, 0 );
					break;
			}
			break;
	}

	return 0;
}
