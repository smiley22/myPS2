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

typedef struct
{
	unsigned int	nWidth;
	unsigned int	nAlign;
	char			*lpStr;
	unsigned int	nTextColor;
	int				nFontIdx;

	unsigned int	nShowCursor;
	unsigned int	nCursorPos;

} GUICtrl_Label_t;

#define GUI_LABEL_LEFT			0x00000001
#define GUI_LABEL_RIGHT			0x00000002
#define GUI_LABEL_CENTER		0x00000004		

typedef struct
{
	int				nTexture;
	int				nTextureFocus;
	unsigned int	nWidth;
	unsigned int	nHeight;
	unsigned int	nColorDiffuse;
	unsigned int	nTexWidth;
	unsigned int	nTexHeight;
	unsigned int	nTexAlign;
	char			*lpStr;
	unsigned int	nTextColor;
	unsigned int	nSelectedColor;
	unsigned int	nDisabledColor;
	int				nFontIdx;
	unsigned int	nAlign;
	unsigned int	nAlignY;
	unsigned int	nTextOffsetX;
	unsigned int	nTextOffsetY;
} GUICtrl_Button_t;

#define GUI_BUTTON_TEX_CENTER	0x00000001
#define GUI_BUTTON_TEX_LEFT		0x00000002
#define GUI_BUTTON_TEX_RIGHT	0x00000004

#define GUI_LABEL_VALIGN_TOP	0x00000001
#define GUI_LABEL_VALIGN_MIDDLE	0x00000002

typedef struct
{
	int				nTexture;
	unsigned int	nWidth;
	unsigned int	nHeight;
	unsigned int	nAspectRatio;
	unsigned int	nColorDiffuse;

} GUICtrl_Image_t;

typedef struct
{
	int				nTextureBg;
	int				nTextureNib;
	int				nTextureNibFocus;
	unsigned int	nWidth;
	unsigned int	nHeight;

	s64				nMins;
	s64				nMaxs;
	s64				nPos;
	u64				nStepSize;
} GUICtrl_Slider_t;

typedef struct
{
	int				nTextureBg;
	int				nTextureLeft;
	int				nTextureRight;
	int				nTextureMid;
	unsigned int	nWidth;
	unsigned int	nHeight;

	u64				nMins;
	u64				nMaxs;
	u64				nPos;
} GUICtrl_Progress_t;

typedef struct
{
	GSTEXTURE		*pTexture;
	const char		*pStr;
	const void		*pUser;

} GUIThumbItem_t;

typedef struct
{
	int				nItemTexture;
	int				nItemTextureFocus;
	unsigned int	nWidth;
	unsigned int	nHeight;
	unsigned int	nPaddingX;
	unsigned int	nPaddingY;
	unsigned int	nItemWidth;
	unsigned int	nItemHeight;
	unsigned int	nTextureWidth;
	unsigned int	nTextureHeight;
	unsigned int	nThumbWidth;
	unsigned int	nThumbHeight;
	unsigned int	nThumbAlign;
	unsigned int	nThumbOffsetX;
	unsigned int	nThumbOffsetY;
	unsigned int	nTextColor;
	unsigned int	nSelectedColor;
	int				nFontIdx;

	int				nUpTexture;
	int				nDownTexture;
	unsigned int	nUpTexPosX;
	unsigned int	nUpTexPosY;
	unsigned int	nDownTexPosX;
	unsigned int	nDownTexPosY;

	unsigned int	nNumItems;
	GUIThumbItem_t	*pItems;
	unsigned int	nSelectedItem;
	unsigned int	nStartItem;

} GUICtrl_ThumbnailPanel_t;

#define GUI_THUMB_ALIGN_CENTER	0x00000001

typedef struct
{
	char			*pStr;
	unsigned int	nFlags;
	unsigned int	nValue;
} GUIListItem_t;

typedef struct
{
	int				nTexture;
	int				nTextureFocus;
	int				nTextureMarked;
	unsigned int	nWidth;
	unsigned int	nHeight;
	unsigned int	nColorDiffuse;
	unsigned int	nTextColor;
	unsigned int	nSelectedColor;
	unsigned int	nMarkedColor;
	unsigned int	nItemHeight;
	unsigned int	nPadding;
	unsigned int	nTextOffset;
	int				nFontIdx;

	int				nUpTexture;
	int				nDownTexture;
	unsigned int	nUpTexPosX;
	unsigned int	nUpTexPosY;
	unsigned int	nDownTexPosX;
	unsigned int	nDownTexPosY;

	unsigned int	nNumItems;
	GUIListItem_t	*pItems;
	unsigned int	nSelectedItem;
	unsigned int	nStartItem;

} GUICtrl_List_t;

typedef struct
{
	char			*pName;
	fileInfo_t		Item;
} GUIDVItem_t;

typedef struct
{
	int				nTexture;
	int				nTextureFocus;
	int				nTextureMarked;
	unsigned int	nWidth;
	unsigned int	nHeight;
	unsigned int	nColorDiffuse;
	unsigned int	nTextColor;
	unsigned int	nSelectedColor;
	unsigned int	nMarkedColor;
	unsigned int	nItemHeight;
	unsigned int	nPadding;
	unsigned int	nTextOffset;
	int				nFontIdx;

	int				nUpTexture;
	int				nDownTexture;
	unsigned int	nUpTexPosX;
	unsigned int	nUpTexPosY;
	unsigned int	nDownTexPosX;
	unsigned int	nDownTexPosY;

	unsigned int	nNumItems;
	GUIDVItem_t		*pItems;
	unsigned int	nSelectedItem;
	unsigned int	nStartItem;
	char			szFilter[ MAX_PATH + 1 ];
	char			szPath[ MAX_PATH + 1 ];

} GUICtrl_Dirview_t;

typedef struct
{
	int				nTexture;
	int				nTextureFocus;
	int				nAltTexture;
	int				nAltTextureFocus;
	unsigned int	nWidth;
	unsigned int	nHeight;
	unsigned int	nColorDiffuse;
	unsigned int	nTexWidth;
	unsigned int	nTexHeight;
	unsigned int	nTexAlign;
	char			*lpStr;
	unsigned int	nTextColor;
	unsigned int	nSelectedColor;
	unsigned int	nDisabledColor;
	unsigned int	nAlign;
	unsigned int	nAlignY;
	unsigned int	nTextOffsetX;
	unsigned int	nTextOffsetY;
	int				nFontIdx;

	unsigned int	nState;
} GUICtrl_ToggleButton_t;

typedef struct
{
	char			*pStr;
	unsigned int	nValue;
} GUIComboItem_t;

typedef struct
{
	int				nTexture;
	int				nTextureFocus;
	unsigned int	nWidth;
	unsigned int	nHeight;
	unsigned int	nColorDiffuse;
	unsigned int	nTexWidth;
	unsigned int	nTexHeight;
	unsigned int	nTexAlign;
	char			*lpStr;
	unsigned int	nTextColor;
	unsigned int	nSelectedColor;
	unsigned int	nDisabledColor;
	int				nFontIdx;
	unsigned int	nAlign;
	unsigned int	nAlignY;
	unsigned int	nTextOffsetX;
	unsigned int	nTextOffsetY;

	unsigned int	nNumItems;
	GUIComboItem_t	*pItems;
	unsigned int	nSelectedItem;

} GUICtrl_Combo_t;

/////////////////////////////////////////////////
// Prototypes

// Button Control
void	GUI_Ctrl_Button_Draw( const GUIControl_t *pCtrl );
void	GUI_Ctrl_Button_SetText( GUIControl_t *pCtrl, const char *lpText );

// Image Control
void	GUI_Ctrl_Image_Draw( const GUIControl_t *pCtrl );

// Label Control
void			GUI_Ctrl_Label_Draw( const GUIControl_t *pCtrl );
void			GUI_Ctrl_Label_SetText( GUIControl_t *pCtrl, const char *lpText );
const char*		GUI_Ctrl_Label_GetText( const GUIControl_t *pCtrl );
void			GUI_Ctrl_Label_ShowCursor( GUIControl_t *pCtrl, unsigned int bShow );
unsigned int	GUI_Ctrl_Label_GetCursor( const GUIControl_t *pCtrl );
void			GUI_Ctrl_Label_SetCursor( GUIControl_t *pCtrl, unsigned int nPos );
void			GUI_Ctrl_Label_Insert( GUIControl_t *pCtrl, unsigned int nPos, char c );
void			GUI_Ctrl_Label_Delete( GUIControl_t *pCtrl, unsigned int nPos );

// Progress Control
void	GUI_Ctrl_Progress_Draw( const GUIControl_t *pCtrl );

void	GUI_Ctrl_Progress_SetBounds( GUIControl_t *pCtrl, u64 nMins, u64 nMaxs );
void	GUI_Ctrl_Progress_GetBounds( const GUIControl_t *pCtrl, u64 *pMins, u64 *pMaxs );
void	GUI_Ctrl_Progress_SetPos( GUIControl_t *pCtrl, u64 nPos );
u64		GUI_Ctrl_Progress_GetPos( const GUIControl_t *pCtrl );

// Slider Control
void	GUI_Ctrl_Slider_Draw( const GUIControl_t *pCtrl );

void	GUI_Ctrl_Slider_SetPos( GUIControl_t *pCtrl, s64 nPos );
s64		GUI_Ctrl_Slider_GetPos( const GUIControl_t *pCtrl );
void	GUI_Ctrl_Slider_SetBounds( GUIControl_t *pCtrl, s64 nMins, s64 nMaxs );
void	GUI_Ctrl_Slider_SetStep( GUIControl_t *pCtrl, u64 nStepSize );
u64		GUI_Ctrl_Slider_GetStep( const GUIControl_t *pCtrl );
int		GUI_Ctrl_Slider_Input( GUIControl_t *pCtrl, unsigned int nPadBtns );

#define	GUI_NOT_SL_POS_CHANGED	1

// Thumbnail Panel
void	GUI_Ctrl_ThumbnailPanel_Draw( const GUIControl_t *pCtrl );

void	GUI_Ctrl_ThumbnailPanel_AddItem( GUIControl_t *pCtrl, void *pUser );
void	GUI_Ctrl_ThumbnailPanel_Clean( GUIControl_t *pCtrl );
void	GUI_Ctrl_ThumbnailPanel_VramClean( GUIControl_t *pCtrl );
int		GUI_Ctrl_ThumbnailPanel_Input( GUIControl_t *pCtrl, unsigned int nPadBtns );

#define GUI_NOT_NEED_INFO		0x00000010
#define GUI_NOT_REMOVE			0x00000020
#define GUI_NOT_THUMB			0x00000040

// Toggle Button
void	GUI_Ctrl_ToggleButton_Draw( const GUIControl_t *pCtrl );

int		GUI_Ctrl_ToggleButton_Input( GUIControl_t *pCtrl, unsigned int nPadBtns );
void	GUI_Ctrl_ToggleButton_SetState( GUIControl_t *pCtrl, unsigned int bState );
int		GUI_Ctrl_ToggleButton_GetState( const GUIControl_t *pCtrl );

#define GUI_BUTTON_OFF	0
#define GUI_BUTTON_ON	1

#define	GUI_NOT_BUTTON_TOGGLED	0x00000010

// List
void			GUI_Ctrl_List_Draw( const GUIControl_t *pCtrl );

int				GUI_Ctrl_List_AddItem( GUIControl_t *pCtrl, const char *pStr, unsigned int nValue );
void			GUI_Ctrl_List_Clean( GUIControl_t *pCtrl );
void			GUI_Ctrl_List_SetCursor( GUIControl_t *pCtrl, unsigned int nPos );
int				GUI_Ctrl_List_Input( GUIControl_t *pCtrl, unsigned int nPadBtns );
const char*		GUI_Ctrl_List_GetItem( GUIControl_t *pCtrl, unsigned int nIndex );
unsigned int	GUI_Ctrl_List_GetItemData( GUIControl_t *pCtrl, unsigned int nIndex );
unsigned int	GUI_Ctrl_List_GetCount( const GUIControl_t *pCtrl );
const char*		GUI_Ctrl_List_GetSel( const GUIControl_t *pCtrl );
unsigned int	GUI_Ctrl_List_GetSelIndex( const GUIControl_t *pCtrl );
unsigned int	GUI_Ctrl_List_NumMark( const GUIControl_t *pCtrl );
const char*		GUI_Ctrl_List_GetMarked( const GUIControl_t *pCtrl, unsigned int n );
void			GUI_Ctrl_List_SetMarked( const GUIControl_t *pCtrl, unsigned int nIndex,
										 unsigned int bMarked );
int				GUI_Ctrl_List_Empty( const GUIControl_t *pCtrl );

#define GUI_NOT_LIST_CLICK		0x00000010
#define GUI_NOT_LIST_MARK		0x00000020
#define GUI_NOT_LIST_UNMARK		0x00000040
#define GUI_NOT_LIST_POS		0x00000080

// Dirview
void				GUI_Ctrl_Dirview_Init( GUIControl_t *pCtrl );
void				GUI_Ctrl_Dirview_Draw( const GUIControl_t *pCtrl );
int					GUI_Ctrl_Dirview_AddItem( GUIControl_t *pCtrl, const fileInfo_t *pInfo,
											  const char *pName );
const fileInfo_t*	GUI_Ctrl_Dirview_FindItem( const GUIControl_t *pCtrl, const char *pFileName );
void				GUI_Ctrl_Dirview_Clean( GUIControl_t *pCtrl );
void				GUI_Ctrl_Dirview_SetDir( GUIControl_t *pCtrl, const char *pDirectory );
void				GUI_Ctrl_Dirview_SetCursor( GUIControl_t *pCtrl, unsigned int nPos );
const char*			GUI_Ctrl_Dirview_GetDir( const GUIControl_t *pCtrl );
int					GUI_Ctrl_Dirview_Input( GUIControl_t *pCtrl, unsigned int nPadBtns );
void				GUI_Ctrl_Dirview_Refresh( GUIControl_t *pCtrl );
const fileInfo_t*	GUI_Ctrl_Dirview_GetSel( const GUIControl_t *pCtrl );
unsigned int		GUI_Ctrl_Dirview_NumMark( const GUIControl_t *pCtrl );
const fileInfo_t*	GUI_Ctrl_Dirview_GetMarked( const GUIControl_t *pCtrl, unsigned int n );
void				GUI_Ctrl_Dirview_SetMarked( const GUIControl_t *pCtrl, unsigned int nIndex,
											    unsigned int bMarked );
void				GUI_Ctrl_Dirview_SetFilter( GUIControl_t *pCtrl, const char *pFilter );
int					GUI_Ctrl_Dirview_IsRoot( GUIControl_t *pCtrl );
unsigned int		GUI_Ctrl_Dirview_GetCount( const GUIControl_t *pCtrl );
const fileInfo_t*	GUI_Ctrl_Dirview_GetItem( const GUIControl_t *pCtrl, unsigned int nIndex );
void				GUI_Ctrl_Dirview_Sort( GUIControl_t *pCtrl, int nSortFunc );
int					GUI_Ctrl_Dirview_SortByName( const void *a, const void *b );
int					GUI_Ctrl_Dirview_SortPanelBySize( const void *a, const void *b );
int					GUI_Ctrl_Dirview_SortPanelByType( const void *a, const void *b );
int					GUI_Ctrl_Dirview_Root( const GUIControl_t *pCtrl );

#define GUI_NOT_DIRVIEW_DIR			0x00000010
#define GUI_NOT_DIRVIEW_CLICKED		0x00000020
#define GUI_NOT_DIRVIEW_UNMARK		0x00000040
#define GUI_NOT_DIRVIEW_MARK		0x00000080
#define GUI_NOT_DIRVIEW_POS			0x00000100

#define GUI_SORT_NAME				0
#define GUI_SORT_SIZE				1
#define GUI_SORT_TYPE				2

// Combo Button
void	GUI_Ctrl_Combo_Draw( const GUIControl_t *pCtrl );
int		GUI_Ctrl_Combo_Input( GUIControl_t *pCtrl, unsigned int nPadBtns );
void	GUI_Ctrl_Combo_Add( GUIControl_t *pCtrl, const char *pStr, unsigned int nValue );
void	GUI_Ctrl_Combo_Clean( GUIControl_t *pCtrl );
int		GUI_Ctrl_Combo_Empty( const GUIControl_t *pCtrl );
void	GUI_Ctrl_Combo_SetCurSel( GUIControl_t *pCtrl, unsigned int nIndex );

#define	GUI_NOT_CB_SEL_CHANGED		0x00000010

/////////////////////////////////////////////////
// Control Notification Messages

#define GUI_NOT_CLICKED			0x00000001

