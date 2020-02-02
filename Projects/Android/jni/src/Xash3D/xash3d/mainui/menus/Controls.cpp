/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include <controls/CheckBox.h>
#include <controls/Slider.h>
#include "Framework.h"
#include "keydefs.h"
#include "Bitmap.h"
#include "PicButton.h"
#include "Action.h"
#include "YesNoMessageBox.h"
#include "MessageBox.h"
#include "Table.h"

#define ART_BANNER		"gfx/shell/head_controls"


class CMenuControllerModesModel : public CMenuBaseModel
{
public:
    void Update();
    int GetColumns() const { return 1; }
    int GetRows() const { return m_iNumModes; }
    const char *GetCellText(int line, int column) { return m_szModes[line]; }
private:
    int m_iNumModes;
    const char *m_szModes[8];
};

static class CMenuControls : public CMenuFramework
{
public:
	CMenuControls() : CMenuFramework("CMenuControls") { }

	void _Init();
    void _VidInit();
    void SetConfig( );
    void GetConfig( void );

private:
    void Cancel( void )
    {
        EngFuncs::CvarSetValue( "vr_control_scheme", prevMode );
        Hide();
    }
    
	CMenuBannerBitmap banner;

    CMenuCheckBox	hmdWalkDirection;
    CMenuSlider	snapTurnAngle;

    CMenuTable	controllerList;
    CMenuControllerModesModel controllerListModel;

    int prevMode;

} uiControls;


/*
=================
UI_Controls_Init
=================
*/
void CMenuControls::_Init( void )
{
	banner.SetPicture( ART_BANNER );

    controllerList.SetRect( 320, 230, -20, 200 );
    controllerList.SetupColumn( 0, "VR Controller Modes", 1.0f );
    controllerList.SetModel( &controllerListModel );

	AddItem( background );
	AddItem( banner );

    AddButton( "OK", "Apply changes and return to configuration menu", PC_OK, VoidCb( &CMenuControls::SetConfig ) );
	AddButton( "Cancel", "Return to configuration menu", PC_CANCEL,	VoidCb( &CMenuControls::Cancel ) );

    hmdWalkDirection.SetNameAndStatus( "HMD Oriented Movement Direction", "Check to enable Gaze based directional movement" );
    hmdWalkDirection.SetCoord( 320, 450 );
    hmdWalkDirection.LinkCvar( "vr_walkdirection" );

    snapTurnAngle.SetNameAndStatus( "Snap/Smooth Turn: %.1f degrees", "Controller turn angle, < 10 is smooth turning per frame" );
    snapTurnAngle.Setup( 0.0, 90.0, 1.0f );
    snapTurnAngle.onChanged = CMenuEditable::WriteCvarCb;
    snapTurnAngle.SetCoord( 320, 560 );
    snapTurnAngle.SetSize( 520, 40 );
    snapTurnAngle.SetDrawValue(true);
    snapTurnAngle.LinkCvar("vr_snapturn_angle");

    AddItem( controllerList );
    AddItem( hmdWalkDirection );
    AddItem( snapTurnAngle );

}

void CMenuControls::_VidInit()
{
    prevMode = (int)EngFuncs::GetCvarFloat( "vr_control_scheme" );

    if (prevMode > 10)
        prevMode = (prevMode-10) + 4;
    controllerList.SetCurrentIndex( prevMode );
}


/*
=================
UI_VidModes_SetConfig
=================
*/
void CMenuControls::SetConfig( )
{
    int index = controllerList.GetCurrentIndex();
    int mode = (index < 4) ? index : (index - 4) + 10;
    EngFuncs::CvarSetValue( "vr_control_scheme", mode );

    hmdWalkDirection.WriteCvar();
    snapTurnAngle.WriteCvar();

    // We're done now, just close
    Hide();
}
/*
=================
UI_VidModes_GetModesList
=================
*/
void CMenuControllerModesModel::Update( void )
{
    unsigned int i;

    m_szModes[0] = "Default - Right Handed";
    m_szModes[1] = "Alt 1 - Right Handed (Y Button to open weapon selector)";
    m_szModes[2] = "Alt 2 - Right Handed (Movement & Button controls are switched hands)";
    m_szModes[3] = "One Controller - Right Handed (Flashlight is still other controller)";
    m_szModes[4] = "Default - Left Handed";
    m_szModes[5] = "Alt 1 - Left Handed (B Button to open weapon selector)";
    m_szModes[6] = "Alt 2 - Left Handed (Movement controls are switched hands)";
    m_szModes[7] = "One Controller - Left Handed (Flashlight is still other controller)";
    m_iNumModes = 8;
}
/*
=================
UI_Controls_Precache
=================
*/
void UI_Controls_Precache( void )
{
	EngFuncs::PIC_Load( ART_BANNER );
}

/*
=================
UI_Controls_Menu
=================
*/
void UI_Controls_Menu( void )
{
	uiControls.Show();
}
ADD_MENU( menu_controls, UI_Controls_Precache, UI_Controls_Menu );
