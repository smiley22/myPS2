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

#ifndef _GUI_H_
#define _GUI_H_

#include <../lib/libscew/scew.h>
#include <../lib/libjpg/libjpg.h>
#include <../lib/libpng/libpng.h>
#include <../lib/gsLib/gsLib.h>

#include <GUI_Lang.h>

#include <Misc.h>
#include <Floatlib.h>
#include <Gamepad.h>
#include <File.h>
#include <Charset.h>

typedef struct GUIMenu_s GUIMenu_t;

#ifndef MAX_PATH
	#define MAX_PATH 1024
#endif

enum eGUIMenus
{
	GUI_MENU_MAIN,
	GUI_MENU_MYPICTURES,
	GUI_MENU_DLGTHUMB,
	GUI_MENU_MYPROGRAMS,
	GUI_MENU_MYFILES,
	GUI_MENU_MSGBOX,
	GUI_MENU_DLGYESNO,
	GUI_MENU_DLGKEYBOARD,
	GUI_MENU_COPYDLG,
	GUI_MENU_MYMUSIC,
	GUI_MENU_RADIO,
	GUI_MENU_SETTINGS,
	GUI_MENU_NETWORK,
	GUI_MENU_PARTITIONS,
	GUI_MENU_SKINS,
	GUI_MENU_LANGUAGE,
	GUI_MENU_PARTITION1,
	GUI_MENU_PARTITION2,
	GUI_MENU_VIEW,
	GUI_MENU_SAMBA,

	GUI_MENU_NUM
};

enum eGUIMsg
{
	GUI_MSG_OPEN,
	GUI_MSG_CLOSE,
	GUI_MSG_CONTROL,
	GUI_MSG_DIALOG
};

typedef struct
{
	unsigned int	nType;
	unsigned int	nPosX;
	unsigned int	nPosY;
	unsigned int	nFlags;
	int				nID;
	void			*pCtrl;
/*const*/ GUIMenu_t	*pParent;
	unsigned int	nUpID;
	unsigned int	nDownID;
	unsigned int	nLeftID;
	unsigned int	nRightID;
	unsigned int	nVisible;
	unsigned int	nDisabled;
} GUIControl_t;

typedef struct
{
	unsigned int	nIndex;
	GSTEXTURE		*gsTexture;
} GUIMenuImage_t;

typedef struct
{
	unsigned int	nIndex;
	GSFONT			*gsFont;
} GUIMenuFont_t;

struct GUIMenu_s
{
	GUIControl_t	*pControls;
	unsigned int	iNumControls;

	unsigned int	(*pfnCallback)( GUIMenu_t *pGUIMenu, unsigned int nGUIMsg,
									unsigned int nCtrlParam, unsigned int nOther );
	int				iSelID;

	unsigned int	iNumImages;
	GUIMenuImage_t	*pImages;

	unsigned int	iNumFonts;
	GUIMenuFont_t	*pFonts;

	// for dialog menus
	GUIMenu_t		*pParent;
	unsigned int	nDlgID;

	// view menu needs this
	void			(*pfnInput)( unsigned int nPadBtns );
	void			(*pfnRender)( void );
};

typedef struct
{
	char			*pImageName;
	unsigned char	*pImage;
	unsigned int	nImageSize;
} GUIImageStore_t;

typedef struct
{
	char			*pGUIString;
	int				nStrID;
} GUIString_t;

typedef struct
{
	GUIString_t		*pEntry;
	int				nNumEntries;
} GUILangTable_t;

typedef struct
{
	char			*pFontName;
	unsigned char	*pPNGFile;
	unsigned char	*pDATFile;
	unsigned int	nPNGSize;
	unsigned int	nDATSize;
} GUIFontStore_t;

typedef struct
{
	int	nWeekday;		// 0 - 6
	int	nDay;			// 0 - 30
	int nMonth;			// 0 - 11
	int nYear;			// 0 - 99
	int nHour;			// 0 - 23
	int nMinute;		// 0 - 59
} GUITime_t;

typedef struct
{
	GUIImageStore_t	*pGUIImages;
	int				nNumGUIImages;
	GUIMenu_t		pGUIMenus[GUI_MENU_NUM];
	GUIMenu_t		*pActiveMenu;
	GUILangTable_t	LangTable;

	GUIFontStore_t	*pGUIFonts;
	int				nNumGUIFonts;

	GUITime_t		Time;
} GUI_t;

typedef struct
{
	const char		*pFileName;
	unsigned int	(*pfnCallback)( GUIMenu_t *pGUIMenu, unsigned int nGUIMsg,
									unsigned int nCtrlParam, unsigned int nOther );
} GUIMenus_t;

///////////////////////////////////////////////////////////
// Prototypes

int GUI_Init( void );
void GUI_Run( void *pThreadArgs );
void GUI_Input( unsigned int nPadBtns );
GUIControl_t *GUI_ControlByID( unsigned int nID );
void GUI_CtrlSetVisible( GUIControl_t *pCtrl, unsigned int bVisible );
void GUI_CtrlSetEnable( unsigned int nID, unsigned int bEnable );
int GUI_LoadLangTable( const char *lpLangFile );
int GUI_FreeLangTable( void );
const char *GUI_GetLangStr( unsigned int nStrID );
int GUI_LoadSkin( const char *lpSkinName );
int GUI_FreeSkin( void );

void GUI_OpenMenu( unsigned int nMenuID );
void GUI_OpenDialog( unsigned int nMenuID, GUIMenu_t *pParent, unsigned int nDlgID );
void GUI_CloseDialog( GUIMenu_t *pDialog, unsigned int nExitCode, unsigned int nRetValue );
void GUI_RenderMenu( const GUIMenu_t *pMenu );
void GUI_Render( void );
void GUI_UpdateSpecialCtrls( GUIMenu_t *pMenu );
void GUI_Screenshot( void );

int	GUI_ImageLookup( const GUI_t *lpGUI, const char *lpImageName );
int	GUI_ImageAdd( GUI_t *lpGUI, const char *lpImageName );
const GUIMenuImage_t *GUI_MenuGetImage( const GUIMenu_t *lpGUIMenu, unsigned int nIndex );
int GUI_MenuAddImage( GUIMenu_t *lpGUIMenu, unsigned int nImageIndex );

int GUI_LoadFonts( const char *lpFileName, GUI_t *lpGUI );
int GUI_FontAdd( GUI_t *lpGUI, const char *lpFontName, const char *lpFileName );
int GUI_FontLookup( const GUI_t *lpGUI, const char *lpFontName );
const GUIMenuFont_t *GUI_MenuGetFont( const GUIMenu_t *lpGUIMenu, unsigned int nIndex );
int GUI_MenuAddFont( GUIMenu_t *lpGUIMenu, unsigned int nFontIndex );

int GUI_LoadMenu( const char *lpXMLFile, GUI_t *lpGUI, GUIMenu_t *lpGUIMenu, float fScaleX, float fScaleY );
int GUI_Load_Label( const scew_element *lpElement, GUI_t *lpGUI, GUIMenu_t *lpGUIMenu, GUIControl_t *lpCtrl, float fScaleX, float fScaleY );
int GUI_Load_Button( const scew_element *lpElement, GUI_t *lpGUI, GUIMenu_t *lpGUIMenu, GUIControl_t *lpCtrl, float fScaleX, float fScaleY );
int GUI_Load_Image( const scew_element *lpElement, GUI_t *lpGUI, GUIMenu_t *lpGUIMenu, GUIControl_t *lpCtrl, float fScaleX, float fScaleY );
int GUI_Load_Slider( const scew_element *lpElement, GUI_t *lpGUI, GUIMenu_t *lpGUIMenu, GUIControl_t *lpCtrl, float fScaleX, float fScaleY );
int GUI_Load_Progress( const scew_element *lpElement, GUI_t *lpGUI, GUIMenu_t *lpGUIMenu, GUIControl_t *lpCtrl, float fScaleX, float fScaleY );
int GUI_Load_ThumbnailPanel( const scew_element *lpElement, GUI_t *lpGUI, GUIMenu_t *lpGUIMenu, GUIControl_t *lpCtrl, float fScaleX, float fScaleY );
int GUI_Load_List( const scew_element *lpElement, GUI_t *lpGUI, GUIMenu_t *lpGUIMenu, GUIControl_t *lpCtrl, float fScaleX, float fScaleY );
int GUI_Load_Dirview( const scew_element *lpElement, GUI_t *lpGUI, GUIMenu_t *lpGUIMenu, GUIControl_t *lpCtrl, float fScaleX, float fScaleY );
int GUI_Load_ToggleButton( const scew_element *lpElement, GUI_t *lpGUI, GUIMenu_t *lpGUIMenu, GUIControl_t *lpCtrl, float fScaleX, float fScaleY );
int GUI_Load_Combo( const scew_element *lpElement, GUI_t *lpGUI, GUIMenu_t *lpGUIMenu, GUIControl_t *lpCtrl, float fScaleX, float fScaleY );
///////////////////////////////////////////////////////////
// Control Types

enum eCtrlTypes
{
	GUI_CTRL_LABEL,
	GUI_CTRL_BUTTON,
	GUI_CTRL_IMAGE,
	GUI_CTRL_SLIDER,
	GUI_CTRL_PROGRESS,
	GUI_CTRL_COMBO,
	GUI_CTRL_THUMBNAILPANEL,
	GUI_CTRL_LIST,
	GUI_CTRL_DIRVIEW,
	GUI_CTRL_TOGGLEBUTTON

};

// Special ID's (1-100)

#define ID_GO_BACK			1
#define ID_MP3_PLAYTIME		2
#define ID_MP3_TRACKLENGTH	3
#define ID_MP3_TRACKNAME	4
#define ID_MP3_ISPLAYING	5
#define ID_TIME_DATESTRING	6		// Weekday, Month, Day
#define ID_TIME_WEEKDAY		7		// Weekday as String
#define ID_TIME_DAY			8		// Day of month	(01 - 31)
#define ID_TIME_MONTH_STR	9		// Month of year as String
#define ID_TIME_MONTH		10		// Month of year (01 - 12)
#define ID_TIME_YEAR		11
#define ID_TIME_CURRENT		12		// HH:MM

#endif
