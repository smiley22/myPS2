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
#include <File.h>
#include <../lib/libscew/scew.h>

int GUI_LoadMenu( const char *lpXMLFile, GUI_t *lpGUI, GUIMenu_t *lpGUIMenu,
				  float fScaleX, float fScaleY )
{
	unsigned int	nSize, nNum, i;
	unsigned char	*pBuf;
	const char		*pName;
	scew_parser		*pParser;
	scew_tree		*pTree;
	scew_element	*pElem, **pList, *pChild;
	GUIControl_t	*pControl;
	FHANDLE			fHandle;

	fHandle = FileOpen( lpXMLFile, O_RDONLY );

	if( fHandle.fh  < 0 )
		return 0;

	nSize = FileSeek( fHandle, 0, SEEK_END );
	FileSeek( fHandle, 0, SEEK_SET );

	if( (pBuf = malloc( nSize )) == NULL )
	{
		FileClose(fHandle);
		return 0;
	}

	FileRead( fHandle, pBuf, nSize );
	FileClose(fHandle);

	if( (pParser = scew_parser_create()) == NULL )
	{
		free(pBuf);
		return 0;
	}

	if( !scew_parser_load_buffer( pParser, pBuf, nSize ) )
	{
		free(pBuf);
		return 0;
	}

	if( (pTree = scew_parser_tree(pParser)) == NULL )
	{
		free(pBuf);
		scew_parser_free(pParser);
		return 0;
	}

	if( (pElem = scew_tree_root(pTree)) == NULL )
	{
		free(pBuf);
		scew_tree_free(pTree);
		scew_parser_free( pParser );
		return 0;
	}

	if( (pName = scew_element_name(pElem)) == NULL )
	{
		free(pBuf);
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	if( strcmp( pName, "window" ) )
	{
		free(pBuf);
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	if( (pChild = scew_element_by_name( pElem, "defaultcontrol" )) == NULL )
	{
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	if( (pName = scew_element_contents(pChild)) == NULL )
	{
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	lpGUIMenu->iSelID = atoi(pName);

	if( (pElem = scew_element_by_name( pElem, "controls" )) == NULL )
	{
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	if( (pList = scew_element_list( pElem, "control", &nNum )) == NULL )
	{
		scew_tree_free(pTree);
		scew_parser_free(pParser);
		return 0;
	}

	for( i = 0; i < nNum; i++ )
	{
		pElem = *(pList + i);

		// parse type field
		if( (pChild = scew_element_by_name( pElem, "type" )) == NULL )
			break;

		if( (pName = scew_element_contents(pChild)) == NULL )
			break;

		pControl = malloc( sizeof(GUIControl_t) * ( lpGUIMenu->iNumControls + 1 ) );

		if( !pControl )
			break;

		memcpy( pControl, lpGUIMenu->pControls, lpGUIMenu->iNumControls * sizeof(GUIControl_t) );
		free(lpGUIMenu->pControls );

		lpGUIMenu->pControls = pControl;

		pControl = &lpGUIMenu->pControls[ lpGUIMenu->iNumControls ];

		memset( pControl, 0, sizeof(GUIControl_t) );

		pControl->pParent	= lpGUIMenu;
		pControl->nVisible	= 1;

		if( !stricmp( pName, "LABEL" ) )
		{
			if( !GUI_Load_Label( pElem, lpGUI, lpGUIMenu, pControl, fScaleX, fScaleY ) )
				break;
		}
		else if( !stricmp( pName, "BUTTON" ) )
		{
			if( !GUI_Load_Button( pElem, lpGUI, lpGUIMenu, pControl, fScaleX, fScaleY ) )
				break;
		}
		else if( !stricmp( pName, "IMAGE" ) )
		{
			if( !GUI_Load_Image( pElem, lpGUI, lpGUIMenu, pControl, fScaleX, fScaleY ) )
				break;
		}
		else if( !stricmp( pName, "SLIDER" ) )
		{
			if( !GUI_Load_Slider( pElem, lpGUI, lpGUIMenu, pControl, fScaleX, fScaleY ) )
				break;
		}
		else if( !stricmp( pName, "PROGRESS" ) )
		{
			if( !GUI_Load_Progress( pElem, lpGUI, lpGUIMenu, pControl, fScaleX, fScaleY ) )
				break;
		}
		else if( !stricmp( pName, "THUMBNAILPANEL" ) )
		{
			if( !GUI_Load_ThumbnailPanel( pElem, lpGUI, lpGUIMenu, pControl, fScaleX, fScaleY ) )
				break;
		}
		else if( !stricmp( pName, "LIST" ) )
		{
			if( !GUI_Load_List( pElem, lpGUI, lpGUIMenu, pControl, fScaleX, fScaleY ) )
				break;
		}
		else if( !stricmp( pName, "DIRVIEW" ) )
		{
			if( !GUI_Load_Dirview( pElem, lpGUI, lpGUIMenu, pControl, fScaleX, fScaleY ) )
				break;
		}
		else if( !stricmp( pName, "TOGGLEBUTTON" ) )
		{
			if( !GUI_Load_ToggleButton( pElem, lpGUI, lpGUIMenu, pControl, fScaleX, fScaleY ) )
				break;
		}
		else if( !stricmp( pName, "COMBO" ) )
		{
			if( !GUI_Load_Combo( pElem, lpGUI, lpGUIMenu, pControl, fScaleX, fScaleY ) )
				break;
		}
		else
		{
			// ETC.
		}


		// look for properties all controls share
		if( (pChild = scew_element_by_name( pElem, "id" )) )
		{
			if( (pName = scew_element_contents(pChild)) )
				pControl->nID = atoi(pName);
		}

		if( (pChild = scew_element_by_name( pElem, "posX" )) )
		{
			if( (pName = scew_element_contents(pChild)) )
				pControl->nPosX = atoi(pName) * fScaleX;
		}

		if( (pChild = scew_element_by_name( pElem, "posY" )) )
		{
			if( (pName = scew_element_contents(pChild)) )
				pControl->nPosY = atoi(pName) * fScaleY;
		}

		if( (pChild = scew_element_by_name( pElem, "onup" )) )
		{
			if( (pName = scew_element_contents(pChild)) )
				pControl->nUpID = atoi(pName);
		}

		if( (pChild = scew_element_by_name( pElem, "onright" )) )
		{
			if( (pName = scew_element_contents(pChild)) )
				pControl->nRightID = atoi(pName);
		}

		if( (pChild = scew_element_by_name( pElem, "ondown" )) )
		{
			if( (pName = scew_element_contents(pChild)) )
				pControl->nDownID = atoi(pName);
		}

		if( (pChild = scew_element_by_name( pElem, "onleft" )) )
		{
			if( (pName = scew_element_contents(pChild)) )
				pControl->nLeftID = atoi(pName);
		}

		if( (pChild = scew_element_by_name( pElem, "visible" )) )
		{
			if( (pName = scew_element_contents(pChild)) )
				pControl->nVisible = atoi(pName);
		}

		lpGUIMenu->iNumControls++;
	}

	free(pBuf);
	scew_element_list_free(pList);
	scew_tree_free(pTree);
	scew_parser_free(pParser);

	return 1;
}

int GUI_Load_Label( const scew_element *lpElement, GUI_t *lpGUI, GUIMenu_t *lpGUIMenu,
				    GUIControl_t *lpCtrl, float fScaleX, float fScaleY )
{
	scew_element		*pChild;
	const char			*pValue;
	GUICtrl_Label_t		*pLabel;
	int					nIdx;

	if( (pLabel = malloc( sizeof(GUICtrl_Label_t) )) == NULL )
		return 0;

	memset( pLabel, 0, sizeof(GUICtrl_Label_t) );

	lpCtrl->pCtrl		= pLabel;
	lpCtrl->nType		= GUI_CTRL_LABEL;

	pLabel->nFontIdx	= -1;

	// look for width field
	if( (pChild = scew_element_by_name( lpElement, "width" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pLabel->nWidth = atoi(pValue) * fScaleX;
	}

	// look for align field
	if( (pChild = scew_element_by_name( lpElement, "align" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			if( !stricmp( pValue, "LEFT" ) )
			{
				pLabel->nAlign = GUI_LABEL_LEFT;
			}
			else if( !stricmp( pValue, "CENTER" ) )
			{
				pLabel->nAlign = GUI_LABEL_CENTER;
			}
			else if( !stricmp( pValue, "RIGHT" ) )
			{
				pLabel->nAlign = GUI_LABEL_RIGHT;
			}
		}
	}

	// look for label field
	if( (pChild = scew_element_by_name( lpElement, "label" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			if( (pValue = GUI_GetLangStr(atoi(pValue))) )
			{
				pLabel->lpStr = malloc( strlen(pValue) + 1 );
				strcpy( pLabel->lpStr, pValue );
			}
		}
	}

	// look for textcolor field
	if( (pChild = scew_element_by_name( lpElement, "textcolor" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pLabel->nTextColor = strtol( pValue, NULL, 0 );
	}

	// look for font field
	if( (pChild = scew_element_by_name( lpElement, "font" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_FontLookup( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			// add font index to list of menu font indices
			if( GUI_MenuGetFont( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddFont( lpGUIMenu, nIdx );

			pLabel->nFontIdx = nIdx;
		}
	}

	return 1;
}

int GUI_Load_Button( const scew_element *lpElement, GUI_t *lpGUI, GUIMenu_t *lpGUIMenu,
					 GUIControl_t *lpCtrl, float fScaleX, float fScaleY )
{
	scew_element		*pChild;
	const char			*pValue;
	GUICtrl_Button_t	*pButton;
	int					nIdx;

	if( (pButton = malloc( sizeof(GUICtrl_Button_t)) ) == NULL )
		return 0;

	memset( pButton, 0, sizeof(GUICtrl_Button_t) );

	pButton->nTexture		= -1;
	pButton->nTextureFocus	= -1;
	pButton->nFontIdx		= -1;

	lpCtrl->pCtrl			= pButton;
	lpCtrl->nType			= GUI_CTRL_BUTTON;

	if( (pChild = scew_element_by_name( lpElement, "texturefocus" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			// add image index to list of menu indices
			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pButton->nTextureFocus = nIdx;
		}
	}
	
	if( (pChild = scew_element_by_name( lpElement, "texturenofocus" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			// add image index to list of menu indices
			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pButton->nTexture = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "width" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nWidth = atoi(pValue) * fScaleX;
	}

	if( (pChild = scew_element_by_name( lpElement, "height" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nHeight = atoi(pValue) * fScaleY;
	}

	if( (pChild = scew_element_by_name( lpElement, "colordiffuse" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nColorDiffuse = strtol( pValue, NULL, 0 );
	}

	if( (pChild = scew_element_by_name( lpElement, "texturewidth" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nTexWidth = atoi(pValue) * fScaleX;
	}

	if( (pChild = scew_element_by_name( lpElement, "textureheight" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nTexHeight = atoi(pValue) * fScaleY;
	}

	if( (pChild = scew_element_by_name( lpElement, "texturealign" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			if( !stricmp( pValue, "LEFT" ) )
			{
				pButton->nTexAlign = GUI_BUTTON_TEX_LEFT;
			}
			else if( !stricmp( pValue, "CENTER" ) )
			{
				pButton->nTexAlign = GUI_BUTTON_TEX_CENTER;
			}
			else if( !stricmp( pValue, "RIGHT" ) )
			{
				pButton->nTexAlign = GUI_BUTTON_TEX_RIGHT;
			}
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "label" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			if( (pValue = GUI_GetLangStr(atoi(pValue))) )
			{
				pButton->lpStr = malloc( strlen(pValue) + 1 );
				strcpy( pButton->lpStr, pValue );
			}
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "textcolor" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nTextColor = strtol( pValue, NULL, 0 );
	}

	if( (pChild = scew_element_by_name( lpElement, "selectedcolor" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nSelectedColor = strtol( pValue, NULL, 0 );
	}

	if( (pChild = scew_element_by_name( lpElement, "disabledcolor" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nDisabledColor = strtol( pValue, NULL, 0 );
	}

	if( (pChild = scew_element_by_name( lpElement, "font" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_FontLookup( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			if( GUI_MenuGetFont( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddFont( lpGUIMenu, nIdx );

			pButton->nFontIdx = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "align" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			if( !stricmp( pValue, "LEFT" ) )
			{
				pButton->nAlign = GUI_LABEL_LEFT;
			}
			else if( !stricmp( pValue, "CENTER" ) )
			{
				pButton->nAlign = GUI_LABEL_CENTER;
			}
			else if( !stricmp( pValue, "RIGHT" ) )
			{
				pButton->nAlign = GUI_LABEL_RIGHT;
			}
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "aligny" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			if( !stricmp( pValue, "TOP" ) )
			{
				pButton->nAlignY = GUI_LABEL_VALIGN_TOP;
			}
			else if( !stricmp( pValue, "MIDDLE" ) )
			{
				pButton->nAlignY = GUI_LABEL_VALIGN_MIDDLE;
			}
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "textoffsetx" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nTextOffsetX = atoi(pValue) * fScaleX;
	}

	if( (pChild = scew_element_by_name( lpElement, "textoffsety" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nTextOffsetY = atoi(pValue) * fScaleY;
	}

	return 1;
}

int GUI_Load_Image( const scew_element *lpElement, GUI_t *lpGUI, GUIMenu_t *lpGUIMenu,
				    GUIControl_t *lpCtrl, float fScaleX, float fScaleY )
{
	scew_element		*pChild;
	const char			*pValue;
	GUICtrl_Image_t		*pImage;
	int					nIdx;

	if( (pImage = malloc( sizeof(GUICtrl_Image_t)) ) == NULL )
		return 0;

	memset( pImage, 0, sizeof(GUICtrl_Image_t) );

	pImage->nTexture	= -1;

	lpCtrl->pCtrl		= pImage;
	lpCtrl->nType		= GUI_CTRL_IMAGE;

	// look for texure field
	if( (pChild = scew_element_by_name( lpElement, "texture" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			// add image index to list of menu indices
			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pImage->nTexture = nIdx;
		}
	}

	// look for width field
	if( (pChild = scew_element_by_name( lpElement, "width" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pImage->nWidth = atoi(pValue) * fScaleX;
	}

	// look for height field
	if( (pChild = scew_element_by_name( lpElement, "height" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pImage->nHeight = atoi(pValue) * fScaleY;
	}

	// look for keepaspectratio field
	if( (pChild = scew_element_by_name( lpElement, "keepaspectratio" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pImage->nAspectRatio = atoi(pValue);
	}

	// look for colordiffuse field
	if( (pChild = scew_element_by_name( lpElement, "colordiffuse" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pImage->nColorDiffuse = strtol( pValue, NULL, 0 );
	}

	return 1;
}

int GUI_Load_Slider( const scew_element *lpElement, GUI_t *lpGUI, GUIMenu_t *lpGUIMenu,
					 GUIControl_t *lpCtrl, float fScaleX, float fScaleY )
{
	scew_element		*pChild;
	const char			*pValue;
	GUICtrl_Slider_t	*pSlider;
	int					nIdx;

	if( (pSlider = malloc( sizeof(GUICtrl_Slider_t)) ) == NULL )
		return 0;

	memset( pSlider, 0, sizeof(GUICtrl_Slider_t) );

	pSlider->nTextureBg			= -1;
	pSlider->nTextureNib		= -1;
	pSlider->nTextureNibFocus	= -1;

	lpCtrl->pCtrl		= pSlider;
	lpCtrl->nType		= GUI_CTRL_SLIDER;

	if( (pChild = scew_element_by_name( lpElement, "texturebg" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pSlider->nTextureBg	= nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "textureslidernib" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pSlider->nTextureNib = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "textureslidernibfocus" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pSlider->nTextureNibFocus = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "width" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pSlider->nWidth = atoi(pValue) * fScaleX;
	}

	if( (pChild = scew_element_by_name( lpElement, "height" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pSlider->nHeight = atoi(pValue) * fScaleY;
	}

	return 1;
}

int GUI_Load_Progress( const scew_element *lpElement, GUI_t *lpGUI, GUIMenu_t *lpGUIMenu,
					   GUIControl_t *lpCtrl, float fScaleX, float fScaleY )
{
	scew_element		*pChild;
	const char			*pValue;
	GUICtrl_Progress_t	*pProgress;
	int					nIdx;

	if( (pProgress = malloc( sizeof(GUICtrl_Progress_t)) ) == NULL )
		return 0;

	memset( pProgress, 0, sizeof(GUICtrl_Progress_t) );

	pProgress->nTextureBg		= -1;
	pProgress->nTextureLeft		= -1;
	pProgress->nTextureRight	= -1;
	pProgress->nTextureMid		= -1;

	lpCtrl->pCtrl		= pProgress;
	lpCtrl->nType		= GUI_CTRL_PROGRESS;

	if( (pChild = scew_element_by_name( lpElement, "texturebg" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pProgress->nTextureBg = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "textureleft" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pProgress->nTextureLeft = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "textureright" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pProgress->nTextureRight = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "texturemid" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pProgress->nTextureMid = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "width" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pProgress->nWidth = atoi(pValue) * fScaleX;
	}

	if( (pChild = scew_element_by_name( lpElement, "height" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pProgress->nHeight = atoi(pValue) * fScaleY;
	}

	return 1;
}

int GUI_Load_ThumbnailPanel( const scew_element *lpElement, GUI_t *lpGUI, GUIMenu_t *lpGUIMenu,
							 GUIControl_t *lpCtrl, float fScaleX, float fScaleY )
{
	scew_element				*pChild;
	const char					*pValue;
	GUICtrl_ThumbnailPanel_t	*pPanel;
	int							nIdx;

	if( (pPanel = malloc( sizeof(GUICtrl_ThumbnailPanel_t)) ) == NULL )
		return 0;

	memset( pPanel, 0, sizeof(GUICtrl_ThumbnailPanel_t) );

	pPanel->nItemTexture		= -1;
	pPanel->nItemTextureFocus	= -1;
	pPanel->nUpTexture			= -1;
	pPanel->nDownTexture		= -1;
	pPanel->nFontIdx			= -1;

	lpCtrl->pCtrl		= pPanel;
	lpCtrl->nType		= GUI_CTRL_THUMBNAILPANEL;

	if( (pChild = scew_element_by_name( lpElement, "itemtexture" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pPanel->nItemTexture = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "itemtexturefocus" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pPanel->nItemTextureFocus = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "width" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pPanel->nWidth = atoi(pValue) * fScaleX;
	}

	if( (pChild = scew_element_by_name( lpElement, "height" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pPanel->nHeight = atoi(pValue) * fScaleY;
	}

	if( (pChild = scew_element_by_name( lpElement, "itemwidth" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pPanel->nItemWidth = atoi(pValue) * fScaleX;
	}

	if( (pChild = scew_element_by_name( lpElement, "itemheight" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pPanel->nItemHeight = atoi(pValue) * fScaleY;
	}

	if( (pChild = scew_element_by_name( lpElement, "texturewidth" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pPanel->nTextureWidth = atoi(pValue) * fScaleX;
	}

	if( (pChild = scew_element_by_name( lpElement, "textureheight" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pPanel->nTextureHeight = atoi(pValue) * fScaleY;
	}

	if( (pChild = scew_element_by_name( lpElement, "paddingx" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pPanel->nPaddingX = atoi(pValue) * fScaleX;
	}

	if( (pChild = scew_element_by_name( lpElement, "paddingy" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pPanel->nPaddingY = atoi(pValue) * fScaleY;
	}

	if( (pChild = scew_element_by_name( lpElement, "thumbwidth" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pPanel->nThumbWidth = atoi(pValue) * fScaleX;
	}

	if( (pChild = scew_element_by_name( lpElement, "thumbheight" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pPanel->nThumbHeight = atoi(pValue) * fScaleY;
	}

	if( (pChild = scew_element_by_name( lpElement, "thumbalign" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			if( !stricmp( pValue, "CENTER" ) )
				pPanel->nThumbAlign = GUI_THUMB_ALIGN_CENTER;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "thumboffsetx" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pPanel->nThumbOffsetX = atoi(pValue) * fScaleX;
	}

	if( (pChild = scew_element_by_name( lpElement, "thumboffsety" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pPanel->nThumbOffsetY = atoi(pValue) * fScaleY;
	}

	if( (pChild = scew_element_by_name( lpElement, "textcolor" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pPanel->nTextColor = strtol( pValue, NULL, 0 );
	}

	if( (pChild = scew_element_by_name( lpElement, "selectedcolor" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pPanel->nSelectedColor = strtol( pValue, NULL, 0 );
	}

	if( (pChild = scew_element_by_name( lpElement, "font" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_FontLookup( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			if( GUI_MenuGetFont( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddFont( lpGUIMenu, nIdx );

			pPanel->nFontIdx = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "texturedown" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pPanel->nDownTexture = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "textureup" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pPanel->nUpTexture = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "scrolldownposx" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pPanel->nDownTexPosX = atoi(pValue) * fScaleX;
	}

	if( (pChild = scew_element_by_name( lpElement, "scrolldownposy" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pPanel->nDownTexPosY = atoi(pValue) * fScaleY;
	}

	if( (pChild = scew_element_by_name( lpElement, "scrollupposx" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pPanel->nUpTexPosX = atoi(pValue) * fScaleX;
	}

	if( (pChild = scew_element_by_name( lpElement, "scrollupposy" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pPanel->nUpTexPosY = atoi(pValue) * fScaleY;
	}

	return 1;
}

int GUI_Load_List( const scew_element *lpElement, GUI_t *lpGUI, GUIMenu_t *lpGUIMenu,
				   GUIControl_t *lpCtrl, float fScaleX, float fScaleY )
{
	scew_element	*pChild;
	const char		*pValue;
	GUICtrl_List_t	*pList;
	int				nIdx;

	if( (pList = malloc( sizeof(GUICtrl_List_t)) ) == NULL )
		return 0;

	memset( pList, 0, sizeof(GUICtrl_List_t) );

	pList->nTexture			= -1;
	pList->nTextureFocus	= -1;
	pList->nTextureMarked	= -1;
	pList->nUpTexture		= -1;
	pList->nDownTexture		= -1;
	pList->nFontIdx			= -1;

	lpCtrl->pCtrl		= pList;
	lpCtrl->nType		= GUI_CTRL_LIST;

	if( (pChild = scew_element_by_name( lpElement, "texture" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pList->nTexture = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "texturefocus" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pList->nTextureFocus = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "texturemarked" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pList->nTextureMarked = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "width" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pList->nWidth = atoi(pValue) * fScaleX;
	}

	if( (pChild = scew_element_by_name( lpElement, "height" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pList->nHeight = atoi(pValue) * fScaleY;
	}

	if( (pChild = scew_element_by_name( lpElement, "colordiffuse" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pList->nColorDiffuse = strtol( pValue, NULL, 0 );
	}

	if( (pChild = scew_element_by_name( lpElement, "textcolor" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pList->nTextColor = strtol( pValue, NULL, 0 );
	}

	if( (pChild = scew_element_by_name( lpElement, "selectedcolor" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pList->nSelectedColor = strtol( pValue, NULL, 0 );
	}

	if( (pChild = scew_element_by_name( lpElement, "markedcolor" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pList->nMarkedColor = strtol( pValue, NULL, 0 );
	}

	if( (pChild = scew_element_by_name( lpElement, "itemheight" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pList->nItemHeight = atoi(pValue) * fScaleY;
	}

	if( (pChild = scew_element_by_name( lpElement, "padding" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pList->nPadding = atoi(pValue) * fScaleY;
	}

	if( (pChild = scew_element_by_name( lpElement, "textoffset" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pList->nTextOffset = atoi(pValue) * fScaleX;
	}

	if( (pChild = scew_element_by_name( lpElement, "font" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_FontLookup( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			if( GUI_MenuGetFont( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddFont( lpGUIMenu, nIdx );

			pList->nFontIdx = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "texturedown" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pList->nDownTexture = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "textureup" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pList->nUpTexture = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "scrolldownposx" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pList->nDownTexPosX = atoi(pValue) * fScaleX;
	}

	if( (pChild = scew_element_by_name( lpElement, "scrolldownposy" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pList->nDownTexPosY = atoi(pValue) * fScaleY;
	}

	if( (pChild = scew_element_by_name( lpElement, "scrollupposx" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pList->nUpTexPosX = atoi(pValue) * fScaleX;
	}

	if( (pChild = scew_element_by_name( lpElement, "scrollupposy" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pList->nUpTexPosY = atoi(pValue) * fScaleY;
	}


	return 1;
}

int GUI_Load_Dirview( const scew_element *lpElement, GUI_t *lpGUI, GUIMenu_t *lpGUIMenu,
					  GUIControl_t *lpCtrl, float fScaleX, float fScaleY )
{
	scew_element		*pChild;
	const char			*pValue;
	GUICtrl_Dirview_t	*pDirview;
	int					nIdx;

	if( (pDirview = malloc( sizeof(GUICtrl_Dirview_t)) ) == NULL )
		return 0;

	memset( pDirview, 0, sizeof(GUICtrl_Dirview_t) );

	pDirview->nTexture			= -1;
	pDirview->nTextureFocus		= -1;
	pDirview->nTextureMarked	= -1;
	pDirview->nUpTexture		= -1;
	pDirview->nDownTexture		= -1;
	pDirview->nFontIdx			= -1;

	lpCtrl->pCtrl		= pDirview;
	lpCtrl->nType		= GUI_CTRL_DIRVIEW;

	if( (pChild = scew_element_by_name( lpElement, "texture" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pDirview->nTexture = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "texturefocus" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pDirview->nTextureFocus = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "texturemarked" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pDirview->nTextureMarked = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "width" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pDirview->nWidth = atoi(pValue) * fScaleX;
	}

	if( (pChild = scew_element_by_name( lpElement, "height" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pDirview->nHeight = atoi(pValue) * fScaleY;
	}

	if( (pChild = scew_element_by_name( lpElement, "colordiffuse" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pDirview->nColorDiffuse = strtol( pValue, NULL, 0 );
	}

	if( (pChild = scew_element_by_name( lpElement, "textcolor" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pDirview->nTextColor = strtol( pValue, NULL, 0 );
	}

	if( (pChild = scew_element_by_name( lpElement, "selectedcolor" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pDirview->nSelectedColor = strtol( pValue, NULL, 0 );
	}

	if( (pChild = scew_element_by_name( lpElement, "markedcolor" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pDirview->nMarkedColor = strtol( pValue, NULL, 0 );
	}

	if( (pChild = scew_element_by_name( lpElement, "itemheight" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pDirview->nItemHeight = atoi(pValue) * fScaleY;
	}

	if( (pChild = scew_element_by_name( lpElement, "padding" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pDirview->nPadding = atoi(pValue) * fScaleY;
	}

	if( (pChild = scew_element_by_name( lpElement, "textoffset" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pDirview->nTextOffset = atoi(pValue) * fScaleX;
	}

	if( (pChild = scew_element_by_name( lpElement, "font" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_FontLookup( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			if( GUI_MenuGetFont( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddFont( lpGUIMenu, nIdx );

			pDirview->nFontIdx = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "texturedown" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pDirview->nDownTexture = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "textureup" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pDirview->nUpTexture = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "scrolldownposx" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pDirview->nDownTexPosX = atoi(pValue) * fScaleX;
	}

	if( (pChild = scew_element_by_name( lpElement, "scrolldownposy" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pDirview->nDownTexPosY = atoi(pValue) * fScaleY;
	}

	if( (pChild = scew_element_by_name( lpElement, "scrollupposx" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pDirview->nUpTexPosX = atoi(pValue) * fScaleX;
	}

	if( (pChild = scew_element_by_name( lpElement, "scrollupposy" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pDirview->nUpTexPosY = atoi(pValue) * fScaleY;
	}

	// need to init this control
	GUI_Ctrl_Dirview_Init(lpCtrl);

	return 1;
}

int GUI_Load_ToggleButton( const scew_element *lpElement, GUI_t *lpGUI, GUIMenu_t *lpGUIMenu,
						   GUIControl_t *lpCtrl, float fScaleX, float fScaleY )
{
	scew_element			*pChild;
	const char				*pValue;
	GUICtrl_ToggleButton_t	*pButton;
	int						nIdx;

	if( (pButton = malloc( sizeof(GUICtrl_ToggleButton_t)) ) == NULL )
		return 0;

	memset( pButton, 0, sizeof(GUICtrl_ToggleButton_t) );

	pButton->nTexture			= -1;
	pButton->nTextureFocus		= -1;
	pButton->nAltTexture		= -1;
	pButton->nAltTextureFocus	= -1;
	pButton->nFontIdx			= -1;

	lpCtrl->pCtrl			= pButton;
	lpCtrl->nType			= GUI_CTRL_TOGGLEBUTTON;

	if( (pChild = scew_element_by_name( lpElement, "texturefocus" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			// add image index to list of menu indices
			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pButton->nTextureFocus = nIdx;
		}
	}
	
	if( (pChild = scew_element_by_name( lpElement, "texturenofocus" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			// add image index to list of menu indices
			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pButton->nTexture = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "alttexturefocus" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			// add image index to list of menu indices
			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pButton->nAltTextureFocus = nIdx;
		}
	}
	
	if( (pChild = scew_element_by_name( lpElement, "alttexturenofocus" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			// add image index to list of menu indices
			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pButton->nAltTexture = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "width" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nWidth = atoi(pValue) * fScaleX;
	}

	if( (pChild = scew_element_by_name( lpElement, "height" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nHeight = atoi(pValue) * fScaleY;
	}

	if( (pChild = scew_element_by_name( lpElement, "colordiffuse" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nColorDiffuse = strtol( pValue, NULL, 0 );
	}

	if( (pChild = scew_element_by_name( lpElement, "texturewidth" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nTexWidth = atoi(pValue) * fScaleX;
	}

	if( (pChild = scew_element_by_name( lpElement, "textureheight" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nTexHeight = atoi(pValue) * fScaleY;
	}

	if( (pChild = scew_element_by_name( lpElement, "texturealign" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			if( !stricmp( pValue, "LEFT" ) )
			{
				pButton->nTexAlign = GUI_BUTTON_TEX_LEFT;
			}
			else if( !stricmp( pValue, "CENTER" ) )
			{
				pButton->nTexAlign = GUI_BUTTON_TEX_CENTER;
			}
			else if( !stricmp( pValue, "RIGHT" ) )
			{
				pButton->nTexAlign = GUI_BUTTON_TEX_RIGHT;
			}
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "label" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			if( (pValue = GUI_GetLangStr(atoi(pValue))) )
			{
				pButton->lpStr = malloc( strlen(pValue) + 1 );
				strcpy( pButton->lpStr, pValue );
			}
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "textcolor" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nTextColor = strtol( pValue, NULL, 0 );
	}

	if( (pChild = scew_element_by_name( lpElement, "selectedcolor" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nSelectedColor = strtol( pValue, NULL, 0 );
	}

	if( (pChild = scew_element_by_name( lpElement, "disabledcolor" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nDisabledColor = strtol( pValue, NULL, 0 );
	}

	if( (pChild = scew_element_by_name( lpElement, "align" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			if( !stricmp( pValue, "LEFT" ) )
			{
				pButton->nAlign = GUI_LABEL_LEFT;
			}
			else if( !stricmp( pValue, "CENTER" ) )
			{
				pButton->nAlign = GUI_LABEL_CENTER;
			}
			else if( !stricmp( pValue, "RIGHT" ) )
			{
				pButton->nAlign = GUI_LABEL_RIGHT;
			}
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "aligny" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			if( !stricmp( pValue, "TOP" ) )
			{
				pButton->nAlignY = GUI_LABEL_VALIGN_TOP;
			}
			else if( !stricmp( pValue, "MIDDLE" ) )
			{
				pButton->nAlignY = GUI_LABEL_VALIGN_MIDDLE;
			}
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "textoffsetx" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nTextOffsetX = atoi(pValue) * fScaleX;
	}

	if( (pChild = scew_element_by_name( lpElement, "textoffsety" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nTextOffsetY = atoi(pValue) * fScaleY;
	}

	if( (pChild = scew_element_by_name( lpElement, "font" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_FontLookup( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			if( GUI_MenuGetFont( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddFont( lpGUIMenu, nIdx );

			pButton->nFontIdx = nIdx;
		}
	}

	return 1;
}

int GUI_Load_Combo( const scew_element *lpElement, GUI_t *lpGUI, GUIMenu_t *lpGUIMenu,
					GUIControl_t *lpCtrl, float fScaleX, float fScaleY )
{
	scew_element		*pChild;
	const char			*pValue;
	GUICtrl_Combo_t		*pButton;
	int					nIdx;

	if( (pButton = malloc( sizeof(GUICtrl_Combo_t)) ) == NULL )
		return 0;

	memset( pButton, 0, sizeof(GUICtrl_Combo_t) );

	pButton->nTexture		= -1;
	pButton->nTextureFocus	= -1;
	pButton->nFontIdx		= -1;

	lpCtrl->pCtrl			= pButton;
	lpCtrl->nType			= GUI_CTRL_COMBO;

	if( (pChild = scew_element_by_name( lpElement, "texturefocus" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			// add image index to list of menu indices
			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pButton->nTextureFocus = nIdx;
		}
	}
	
	if( (pChild = scew_element_by_name( lpElement, "texturenofocus" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_ImageLookup( lpGUI, pValue );

			if( nIdx < 0 )
				nIdx = GUI_ImageAdd( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			// add image index to list of menu indices
			if( GUI_MenuGetImage( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddImage( lpGUIMenu, nIdx );

			pButton->nTexture = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "width" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nWidth = atoi(pValue) * fScaleX;
	}

	if( (pChild = scew_element_by_name( lpElement, "height" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nHeight = atoi(pValue) * fScaleY;
	}

	if( (pChild = scew_element_by_name( lpElement, "colordiffuse" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nColorDiffuse = strtol( pValue, NULL, 0 );
	}

	if( (pChild = scew_element_by_name( lpElement, "texturewidth" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nTexWidth = atoi(pValue) * fScaleX;
	}

	if( (pChild = scew_element_by_name( lpElement, "textureheight" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nTexHeight = atoi(pValue) * fScaleY;
	}

	if( (pChild = scew_element_by_name( lpElement, "texturealign" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			if( !stricmp( pValue, "LEFT" ) )
			{
				pButton->nTexAlign = GUI_BUTTON_TEX_LEFT;
			}
			else if( !stricmp( pValue, "CENTER" ) )
			{
				pButton->nTexAlign = GUI_BUTTON_TEX_CENTER;
			}
			else if( !stricmp( pValue, "RIGHT" ) )
			{
				pButton->nTexAlign = GUI_BUTTON_TEX_RIGHT;
			}
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "label" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			if( (pValue = GUI_GetLangStr(atoi(pValue))) )
			{
				pButton->lpStr = malloc( strlen(pValue) + 1 );
				strcpy( pButton->lpStr, pValue );
			}
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "textcolor" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nTextColor = strtol( pValue, NULL, 0 );
	}

	if( (pChild = scew_element_by_name( lpElement, "selectedcolor" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nSelectedColor = strtol( pValue, NULL, 0 );
	}

	if( (pChild = scew_element_by_name( lpElement, "disabledcolor" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nDisabledColor = strtol( pValue, NULL, 0 );
	}

	if( (pChild = scew_element_by_name( lpElement, "font" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			nIdx = GUI_FontLookup( lpGUI, pValue );

			if( nIdx < 0 )
				return 0;

			if( GUI_MenuGetFont( lpGUIMenu, nIdx ) == NULL )
				GUI_MenuAddFont( lpGUIMenu, nIdx );

			pButton->nFontIdx = nIdx;
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "align" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			if( !stricmp( pValue, "LEFT" ) )
			{
				pButton->nAlign = GUI_LABEL_LEFT;
			}
			else if( !stricmp( pValue, "CENTER" ) )
			{
				pButton->nAlign = GUI_LABEL_CENTER;
			}
			else if( !stricmp( pValue, "RIGHT" ) )
			{
				pButton->nAlign = GUI_LABEL_RIGHT;
			}
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "aligny" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
		{
			if( !stricmp( pValue, "TOP" ) )
			{
				pButton->nAlignY = GUI_LABEL_VALIGN_TOP;
			}
			else if( !stricmp( pValue, "MIDDLE" ) )
			{
				pButton->nAlignY = GUI_LABEL_VALIGN_MIDDLE;
			}
		}
	}

	if( (pChild = scew_element_by_name( lpElement, "textoffsetx" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nTextOffsetX = atoi(pValue) * fScaleX;
	}

	if( (pChild = scew_element_by_name( lpElement, "textoffsety" )) )
	{
		if( (pValue = scew_element_contents(pChild)) )
			pButton->nTextOffsetY = atoi(pValue) * fScaleY;
	}

	return 1;
}
