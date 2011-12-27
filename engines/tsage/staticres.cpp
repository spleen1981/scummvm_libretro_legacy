/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "tsage/staticres.h"

namespace TsAGE {

const byte CURSOR_ARROW_DATA[] = {
	15, 0, 15, 0, 0, 0, 0, 0, 9, 0,
	0x00, 0x00, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	0x00, 0x00, 0x00, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	0x00, 0xFF, 0x00, 0x00, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	0x00, 0xFF,	0xFF, 0x00, 0x00, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,	0xFF, 0x00, 0x00, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x09, 0x09, 0x09, 0x09, 0x09,
	0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x09, 0x09, 0x09, 0x09,
	0x00, 0xFF, 0xFF, 0x00, 0x00, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	0x00, 0xFF, 0x00, 0x00, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,	0x09, 0x09, 0x09, 0x09, 0x09,
	0x00, 0x00, 0x00, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	0x00, 0x00, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,	0x09, 0x09, 0x09,
	0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09
};

const byte CURSOR_WALK_DATA[] = {
	15, 0, 15, 0, 7, 0, 7, 0, 9, 0,
	0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x00, 0xFF, 0x00, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x00, 0xFF, 0x00, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x00, 0xFF, 0x00, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x00, 0xFF, 0x00, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	0x09, 0x09, 0x09, 0x09,	0x09, 0x09, 0x00, 0xFF, 0x00, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x09,	0x09, 0x09, 0x09, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x09, 0x09, 0x09, 0x09, 0x09, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x09, 0x09,	0x09, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x00, 0xFF, 0x00, 0x09,	0x09, 0x09, 0x09, 0x09, 0x09,
	0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x00, 0xFF, 0x00, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x00, 0xFF, 0x00, 0x09, 0x09, 0x09,	0x09, 0x09, 0x09,
	0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x00, 0xFF, 0x00, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x00, 0xFF, 0x00, 0x09, 0x09, 0x09, 0x09, 0x09,	0x09,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09
};

const char *LOOK_SCENE_HOTSPOT = "You see nothing special.";
const char *USE_SCENE_HOTSPOT = "That accomplishes nothing.";
const char *TALK_SCENE_HOTSPOT = "Yak, yak.";
const char *SPECIAL_SCENE_HOTSPOT = "That is a unique use for that.";
const char *DEFAULT_SCENE_HOTSPOT = "That accomplishes nothing.";
const char *SAVE_ERROR_MSG = "Error occurred saving game. Please do not try to restore this game!";
const char *SAVING_NOT_ALLOWED_MSG = "Saving is not allowed at this time.";
const char *RESTORING_NOT_ALLOWED_MSG = "Restoring is not allowed at this time.";
const char *INV_EMPTY_MSG = "You have nothing in your possesion.";

const char *QUIT_CONFIRM_MSG = "Do you want to quit playing this game?";
const char *RESTART_MSG = "Do you want to restart this game?";
const char *GAME_PAUSED_MSG = "Game is paused.";
const char *OK_BTN_STRING = " Ok ";
const char *CANCEL_BTN_STRING = "Cancel";
const char *QUIT_BTN_STRING = " Quit ";
const char *RESTART_BTN_STRING = "Restart";
const char *SAVE_BTN_STRING = "Save";
const char *RESTORE_BTN_STRING = "Restore";
const char *SOUND_BTN_STRING = "Sound";
const char *RESUME_BTN_STRING = " Resume \rplay";
const char *LOOK_BTN_STRING = "Look";
const char *PICK_BTN_STRING = "Pick";


namespace Ringworld {

// Dialog resources
const char *HELP_MSG = "Ringworld\rRevenge of the Patriarch\x14\rScummVM Version\r\r\
\x01 Keyboard shortcuts...\rF2 - Sound options\rF3 - Quit\r\
F4 - Restart\rF5 - Save game\rF7 - Restore Game\rF10 - Pause game";
const char *WATCH_INTRO_MSG = "Do you wish to watch the introduction?";
const char *START_PLAY_BTN_STRING = " Start Play ";
const char *INTRODUCTION_BTN_STRING = "Introduction";
const char *OPTIONS_MSG = "\x01Options...";

// Scene specific resources
const char *EXIT_MSG = "   EXIT   ";
const char *SCENE6100_CAREFUL = "Be careful! The probe cannot handle too much of that.";
const char *SCENE6100_TOUGHER = "Hey! This is tougher than it looks!";
const char *SCENE6100_ONE_MORE_HIT = "You had better be more careful. One more hit like that \
and the probe may be destroyed.";
const char *SCENE6100_DOING_BEST = "I'm doing the best I can. I just hope it holds together!";
const char *SCENE6100_REPAIR = "\r\rQuinn and Seeker repair the probe....";
const char *SCENE6100_ROCKY_AREA = "The rocky area should be directly ahead of you. Do you see it?";
const char *SCENE6100_REPLY = "Yes. Now if I can just avoid those sunbeams.";
const char *SCENE6100_TAKE_CONTROLS = "You had better take the controls Seeker. My hands are sweating.";
const char *SCENE6100_SURPRISE = "You surprise me Quinn. I would have thought you of hardier stock.";
const char *SCENE6100_SWEAT = "Humans sweat, Kzin twitch their tail. What's the difference?";
const char *SCENE6100_VERY_WELL = "Very well. I will retrieve the stasis box and return the probe. \
Wait for it's return in the lander bay.";

const char *DEMO_HELP_MSG = " Help...\rF2 - Sound Options\rF3 - Exit demo\r\rPress ENTER\rto continue";
const char *DEMO_PAUSED_MSG = "Ringworld\x14 demo is paused";
const char *DEMO_EXIT_MSG = "Press ENTER to resume the Ringworld\x14 demo. Press ESC to exit";
const char *EXIT_BTN_STRING = "Exit";
const char *DEMO_BTN_STRING = "Demo";
const char *DEMO_RESUME_BTN_STRING = "Resume";

} // End of namespace Ringworld

namespace BlueForce {

// Dialog resources
const char *HELP_MSG = "Blue Force\x14\rScummVM Version\r\r\
Keyboard shortcuts...\rF2 - Sound options\rF3 - Quit\r\
F4 - Restart\rF5 - Save game\rF7 - Restore Game\rF10 - Pause game";
const char *WATCH_INTRO_MSG = "Do you wish to watch the introduction?";
const char *START_PLAY_BTN_STRING = " Play ";
const char *INTRODUCTION_BTN_STRING = " Watch ";
const char *OPTIONS_MSG = "Options...";

// Blue Force general messages
const char *BF_NAME = "Blue Force";
const char *BF_COPYRIGHT = " Copyright, 1993 Tsunami Media, Inc.";
const char *BF_ALL_RIGHTS_RESERVED = "All Rights Reserved";
const char *BF_19840518 = "May 18, 1984";
const char *BF_19840515 = "May 15, 1984";
const char *BF_3_DAYS = "Three days later";
const char *BF_11_YEARS = "Eleven years later.";
const char *BF_NEXT_DAY = "The Next Day";
const char *BF_ACADEMY = "Here we are at the Academy";

// Scene 50 hotspots
const char *GRANDMA_FRANNIE = "Grandma Frannie";
const char *MARINA = "Marina";
const char *POLICE_DEPARTMENT = "Police Department";
const char *TONYS_BAR = "Tony's Bar";
const char *CHILD_PROTECTIVE_SERVICES = "Child Protective Services";
const char *ALLEY_CAT = "Alley Cat";
const char *CITY_HALL_JAIL = "City Hall & Jail";
const char *JAMISON_RYAN = "Jamison & Ryan";
const char *BIKINI_HUT = "Bikini Hut";

// Scene 60 radio messages
const char *RADIO_BTN_LIST[8] = { "10-2 ", "10-4 ", "10-13", "10-15", "10-27", "10-35", "10-97", "10-98" };

// Scene 570 computer messageS
const char *SCENE570_PASSWORD = "PASSWORD -> ";
const char *SCENE570_C_DRIVE = "C:\\";
const char *SCENE570_RING = "RING";
const char *SCENE570_PROTO = "PROTO";
const char *SCENE570_WACKY = "WACKY";
const char *SCENE570_COBB = "COBB";
const char *SCENE570_LETTER = "LETTER";
const char *SCENE570_RINGEXE = "RINGEXE";
const char *SCENE570_RINGDATA = "RINGDATA";
const char *SCENE570_PROTOEXE = "PROTOEXE";
const char *SCENE570_PROTODATA = "PROTODATA";
const char *SCENE570_WACKYEXE = "WACKYEXE";
const char *SCENE570_WACKYDATA = "WACKYDATA";

// Scene 180 messages
const char *THE_NEXT_DAY = "The Next Day";

} // End of namespace BlueForce

namespace Ringworld2 {

const char *CONSOLE_MESSAGES[] = {
	NULL, "Select Language", "Computer Services", "Food Services", "Entertainment Services",
	"Main Menu", "Exit Menu", "Previous Menu", "Interworld", "Hero's Tongue", "Personal Log",
	"Database", "Starchart", "Iso-Opto Disk Reader", "Eject Disk", "Meals", "Snacks",
	"Beverages", "Human Basic Snacks", "Kzin Basic Snacks", "Hot Beverages", "Cold Beverages",
	"Music", "Outpost Alpha", " ", " ", "Done", "A-G", "H-O", "P-S", "T-Z", "Tchaikovsky",
	"Mozart", "Bach", "Rossini"
};

// Scene 825 Autodoc messages
const char *MAIN_MENU = "main menu";
const char *DIAGNOSIS = "diagnosis";
const char *ADVANCED_PROCEDURES = "advanced procedures";
const char *VITAL_SIGNS = "vital signs";
const char *OPEN_DOOR = "open door";
const char *TREATMENTS = "treatments";
const char *NO_MALADY_DETECTED =  "no malady detected";
const char *NO_TREATMENT_REQUIRED = "no treatment required";
const char *ACCESS_CODE_REQUIRED = "access code required";
const char *INVALID_ACCESS_CODE = "invalid access code";
const char *FOREIGN_OBJECT_EXTRACTED = "foreign object extracted";

const char *HELP_MSG = "\x1\rRETURN TO\r RINGWORLD\x14";
const char *CHAR_TITLE = "\x01Select Character:";
const char *CHAR_QUINN_MSG = "  Quinn  ";
const char *CHAR_SEEKER_MSG = "  Seeker  ";
const char *CHAR_MIRANDA_MSG = "Miranda";
const char *CHAR_CANCEL_MSG = "  Cancel  ";

const char *GAME_VERSION = "ScummVM Version";
const char *SOUND_OPTIONS = "Sound options";
const char *QUIT_GAME = "Quit";
const char *RESTART_GAME = "Restart";
const char *SAVE_GAME = "Save game";
const char *RESTORE_GAME = "Restore game";
const char *SHOW_CREDITS = "Show credits";
const char *PAUSE_GAME = "Pause game";
const char *RESUME_PLAY = "  Resume play  ";
const char *F2 = "F2";
const char *F3 = "F3";
const char *F4 = "F4";
const char *F5 = "F5";
const char *F7 = "F7";
const char *F8 = "F8";
const char *F10 = "F10";

} // End of namespace Ringworld2

} // End of namespace TsAGE
