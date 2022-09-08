/*
 * This file is part of FreeRCT.
 * FreeRCT is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * FreeRCT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with FreeRCT. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file window_constants.h %Window and widget handling constants. */

#ifndef WINDOW_CONSTANTS_H
#define WINDOW_CONSTANTS_H

#include "stdafx.h"
#include "enum_type.h"

/**
 * Available types of windows.
 * @ingroup window_group
 */
enum WindowTypes {
	WC_MAIN_MENU,       ///< Main menu screen.
	WC_MAINDISPLAY,     ///< Main display of the world.
	WC_TOOLBAR,         ///< Main toolbar.
	WC_BOTTOM_TOOLBAR,  ///< Bottom toolbar.
	WC_QUIT,            ///< Quit program window.
	WC_ERROR_MESSAGE,   ///< Error message window.
	WC_PERSON_INFO,     ///< Person window.
	WC_COASTER_MANAGER, ///< Roller coaster manager window.
	WC_COASTER_BUILD,   ///< Roller coaster build/edit window.
	WC_COASTER_REMOVE,  ///< Roller coaster remove window.
	WC_RIDE_BUILD,      ///< Simple ride build window.
	WC_PATH_BUILDER,    ///< %Path build GUI.
	WC_RIDE_SELECT,     ///< Ride selection window.
	WC_SHOP_MANAGER,    ///< Management window of a shop.
	WC_SHOP_REMOVE,     ///< Shop remove window.
	WC_GENTLE_THRILL_RIDE_MANAGER, ///< Management window of a gentle/thrill ride.
	WC_GENTLE_THRILL_RIDE_REMOVE,  ///< Gentle/Thrill ride remove window.
	WC_FENCE,           ///< Fence window.
	WC_SCENERY,         ///< Scenery window.
	WC_PATH_OBJECTS,    ///< Path objects window.
	WC_TERRAFORM,       ///< Terraform window.
	WC_FINANCES,        ///< Finance management window.
	WC_STAFF,           ///< Staff management window.
	WC_INBOX,           ///< Inbox window.
	WC_PARK_MANAGEMENT, ///< Park management window.
	WC_MINIMAP,         ///< Minimap window.
	WC_SETTING,         ///< Setting window.
	WC_LOADSAVE,        ///< Save/load game window.
	WC_LOADSAVE_CONFIRM, ///< Save/load game confirmation window.
	WC_DROPDOWN,        ///< Dropdown window.

	WC_NONE,            ///< Invalid window type.
};

/** Codes of the #NotifyChange function, which gets forwarded through the #Window::OnChange method. */
enum ChangeCode {
	CHG_VIEWPORT_ROTATED,    ///< Viewport rotated.
	CHG_DROPDOWN_RESULT,     ///< The selection of a dropdown window.
	CHG_RESOLUTION_CHANGED,  ///< The size of the FreeRCT window was changed.
	CHG_PERSON_DELETED,      ///< A person has been deleted from the world.
};

/** Various state flags of the %Window. */
enum WindowFlags {
	WF_HIGHLIGHT = 1 << 0, ///< %Window edge is highlighted.
};

/**
 * Known mouse buttons.
 * @ingroup window_group
 */
enum MouseButtons {
	MB_NONE   = 0, ///< No button down.
	MB_LEFT   = 1, ///< Left button down.
	MB_MIDDLE = 2, ///< Middle button down.
	MB_RIGHT  = 4, ///< Right button down.
};
DECLARE_ENUM_AS_BIT_SET(MouseButtons)

/**
 * Mouse events of the window manager. Value is returned from the Window::OnMouseButtonEvent.
 * @ingroup window_group
 */
enum WmMouseEvent {
	WMME_NONE,         ///< Do nothing special.
	WMME_CLOSE_WINDOW, ///< Close the window.
	WMME_MOVE_WINDOW,  ///< Initiate a window move.
};

/**
 * Key codes of the window manager.
 * @ingroup window_group
 */
enum WmKeyCode {
	WMKC_CURSOR_UP,    ///< Up arrow key is pressed.
	WMKC_CURSOR_LEFT,  ///< Left arrow key is pressed.
	WMKC_CURSOR_RIGHT, ///< Right arrow key is pressed.
	WMKC_CURSOR_DOWN,  ///< Down arrow key is pressed.
	WMKC_CURSOR_PAGEUP,    ///< PageUp key is pressed.
	WMKC_CURSOR_PAGEDOWN,  ///< PageDown key is pressed.
	WMKC_CURSOR_HOME,  ///< Home key is pressed.
	WMKC_CURSOR_END,   ///< End key is pressed.
	WMKC_BACKSPACE,    ///< Backspace is pressed.
	WMKC_DELETE,       ///< Delete is pressed.
	WMKC_CANCEL,       ///< Cancel is pressed.
	WMKC_CONFIRM,      ///< Confirm is pressed.
	WMKC_SYMBOL,       ///< A symbol is entered.
};

/**
 * Key modifiers of the window manager.
 * @ingroup window_group
 */
enum WmKeyMod {
	WMKM_NONE  = 0,       ///< No modifiers are pressed.
	WMKM_SHIFT = 1 << 0,  ///< Shift key is pressed.
	WMKM_CTRL  = 1 << 1,  ///< Ctrl key is pressed.
	WMKM_ALT   = 1 << 2,  ///< Alt or GUI key is pressed. Some operating systems reserve one of these two keys for special actions;
	                      ///< we therefore treat both modifiers the same to ensure every platform can use at least one of them.
};
DECLARE_ENUM_AS_BIT_SET(WmKeyMod)

/**
 * Tabs of the park management GUI.
 * @note These constants must be in sync with their #ParkManagementWidgets counterparts.
 */
enum ParkManagementGuiTabs {
	PARK_MANAGEMENT_TAB_GENERAL = 0,  ///< General settings tab button.
	PARK_MANAGEMENT_TAB_GUESTS,       ///< Guests graph tab button.
	PARK_MANAGEMENT_TAB_RATING,       ///< Park rating graph tab button.
	PARK_MANAGEMENT_TAB_OBJECTIVE,    ///< Objective tab button.
	PARK_MANAGEMENT_TAB_AWARDS,       ///< Awards tab button.
};

typedef uint32 WindowNumber; ///< Type of a window number.

static const WindowNumber ALL_WINDOWS_OF_TYPE = UINT32_MAX; ///< Window number parameter meaning 'all windows of the window type'.

#endif
