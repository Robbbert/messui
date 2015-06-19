// For licensing and usage information, read docs/winui_license.txt
//****************************************************************************

 /***************************************************************************

  mui_opts.c

  Stores global options and per-game options;

***************************************************************************/

// standard windows headers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <winreg.h>
#include <commctrl.h>

// standard C headers
#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>
#include <math.h>
#include <direct.h>
#include <emu.h>
#include <emuopts.h>
#include <stddef.h>
#include <tchar.h>

// MAME/MAMEUI headers
#include "bitmask.h"
#include "winui.h"
#include "mui_util.h"
#include "treeview.h"
#include "splitters.h"
#include "mui_opts.h"
#include "winutf8.h"
#include "strconv.h"
#include "optionsms.h"
#include "drivenum.h"
#include "game_opts.h"

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

/***************************************************************************
    Internal function prototypes
 ***************************************************************************/

// static void LoadFolderFilter(int folder_index,int filters);

static void LoadSettingsFile(winui_options &opts, const char *filename);
static void SaveSettingsFile(winui_options &opts, const char *filename);
static void LoadSettingsFile(windows_options &opts, const char *filename);
static void SaveSettingsFile(windows_options &opts, const char *filename);

static void LoadOptionsAndSettings(void);

static void  CusColorEncodeString(const COLORREF *value, char* str);
static void  CusColorDecodeString(const char* str, COLORREF *value);

static void  SplitterEncodeString(const int *value, char* str);
static void  SplitterDecodeString(const char *str, int *value);

static void  FontEncodeString(const LOGFONT *f, char *str);
static void  FontDecodeString(const char* str, LOGFONT *f);

static void TabFlagsEncodeString(int data, char *str);
static void TabFlagsDecodeString(const char *str, int *data);

static DWORD DecodeFolderFlags(const char *buf);
static const char * EncodeFolderFlags(DWORD value);

static void ResetToDefaults(windows_options &opts, int priority);


#ifdef _MSC_VER
#define snprintf _snprintf
#endif

/***************************************************************************
    Internal defines
 ***************************************************************************/

#define GAMEINFO_INI_FILENAME				MAMENAME "_g.ini"

#define MUIOPTION_LIST_MODE				"list_mode"
#define MUIOPTION_CHECK_GAME				"check_game"
#define MUIOPTION_JOYSTICK_IN_INTERFACE			"joystick_in_interface"
#define MUIOPTION_KEYBOARD_IN_INTERFACE			"keyboard_in_interface"
#define MUIOPTION_CYCLE_SCREENSHOT			"cycle_screenshot"
#define MUIOPTION_STRETCH_SCREENSHOT_LARGER		"stretch_screenshot_larger"
#define MUIOPTION_SCREENSHOT_BORDER_SIZE		"screenshot_bordersize"
#define MUIOPTION_SCREENSHOT_BORDER_COLOR		"screenshot_bordercolor"
#define MUIOPTION_INHERIT_FILTER			"inherit_filter"
#define MUIOPTION_OFFSET_CLONES				"offset_clones"
#define MUIOPTION_DEFAULT_FOLDER_ID			"default_folder_id"
#define MUIOPTION_SHOW_IMAGE_SECTION			"show_image_section"
#define MUIOPTION_SHOW_SOFTWARE_SECTION			"show_software_section"
#define MUIOPTION_SHOW_FOLDER_SECTION			"show_folder_section"
#define MUIOPTION_HIDE_FOLDERS				"hide_folders"
#define MUIOPTION_SHOW_STATUS_BAR			"show_status_bar"
#define MUIOPTION_SHOW_TABS				"show_tabs"
#define MUIOPTION_SHOW_TOOLBAR				"show_tool_bar"
#define MUIOPTION_CURRENT_TAB				"current_tab"
#define MUIOPTION_WINDOW_X				"window_x"
#define MUIOPTION_WINDOW_Y				"window_y"
#define MUIOPTION_WINDOW_WIDTH				"window_width"
#define MUIOPTION_WINDOW_HEIGHT				"window_height"
#define MUIOPTION_WINDOW_STATE				"window_state"
#define MUIOPTION_CUSTOM_COLOR				"custom_color"
#define MUIOPTION_LIST_FONT				"list_font"
#define MUIOPTION_TEXT_COLOR				"text_color"
#define MUIOPTION_CLONE_COLOR				"clone_color"
#define MUIOPTION_HIDE_TABS				"hide_tabs"
#define MUIOPTION_HISTORY_TAB				"history_tab"
#define MUIOPTION_COLUMN_WIDTHS				"column_widths"
#define MUIOPTION_COLUMN_ORDER				"column_order"
#define MUIOPTION_COLUMN_SHOWN				"column_shown"
#define MUIOPTION_SPLITTERS				"splitters"
#define MUIOPTION_SORT_COLUMN				"sort_column"
#define MUIOPTION_SORT_REVERSED				"sort_reversed"
#define MUIOPTION_LANGUAGE				"language"
#define MUIOPTION_FLYER_DIRECTORY			"flyer_directory"
#define MUIOPTION_CABINET_DIRECTORY			"cabinet_directory"
#define MUIOPTION_MARQUEE_DIRECTORY			"marquee_directory"
#define MUIOPTION_TITLE_DIRECTORY			"title_directory"
#define MUIOPTION_CPANEL_DIRECTORY			"cpanel_directory"
#define MUIOPTION_PCB_DIRECTORY				"pcb_directory"
#define MUIOPTION_ICONS_DIRECTORY			"icons_directory"
#define MUIOPTION_BACKGROUND_DIRECTORY			"background_directory"
#define MUIOPTION_FOLDER_DIRECTORY			"folder_directory"
#define MUIOPTION_UI_KEY_UP				"ui_key_up"
#define MUIOPTION_UI_KEY_DOWN				"ui_key_down"
#define MUIOPTION_UI_KEY_LEFT				"ui_key_left"
#define MUIOPTION_UI_KEY_RIGHT				"ui_key_right"
#define MUIOPTION_UI_KEY_START				"ui_key_start"
#define MUIOPTION_UI_KEY_PGUP				"ui_key_pgup"
#define MUIOPTION_UI_KEY_PGDWN				"ui_key_pgdwn"
#define MUIOPTION_UI_KEY_HOME				"ui_key_home"
#define MUIOPTION_UI_KEY_END				"ui_key_end"
#define MUIOPTION_UI_KEY_SS_CHANGE			"ui_key_ss_change"
#define MUIOPTION_UI_KEY_HISTORY_UP			"ui_key_history_up"
#define MUIOPTION_UI_KEY_HISTORY_DOWN			"ui_key_history_down"
#define MUIOPTION_UI_KEY_CONTEXT_FILTERS		"ui_key_context_filters"
#define MUIOPTION_UI_KEY_SELECT_RANDOM			"ui_key_select_random"
#define MUIOPTION_UI_KEY_GAME_AUDIT			"ui_key_game_audit"
#define MUIOPTION_UI_KEY_GAME_PROPERTIES		"ui_key_game_properties"
#define MUIOPTION_UI_KEY_HELP_CONTENTS			"ui_key_help_contents"
#define MUIOPTION_UI_KEY_UPDATE_GAMELIST		"ui_key_update_gamelist"
#define MUIOPTION_UI_KEY_VIEW_FOLDERS			"ui_key_view_folders"
#define MUIOPTION_UI_KEY_VIEW_FULLSCREEN		"ui_key_view_fullscreen"
#define MUIOPTION_UI_KEY_VIEW_PAGETAB			"ui_key_view_pagetab"
#define MUIOPTION_UI_KEY_VIEW_PICTURE_AREA		"ui_key_view_picture_area"
#define MUIOPTION_UI_KEY_VIEW_SOFTWARE_AREA		"ui_key_view_software_area"
#define MUIOPTION_UI_KEY_VIEW_STATUS			"ui_key_view_status"
#define MUIOPTION_UI_KEY_VIEW_TOOLBARS			"ui_key_view_toolbars"
#define MUIOPTION_UI_KEY_VIEW_TAB_CABINET		"ui_key_view_tab_cabinet"
#define MUIOPTION_UI_KEY_VIEW_TAB_CPANEL		"ui_key_view_tab_cpanel"
#define MUIOPTION_UI_KEY_VIEW_TAB_FLYER			"ui_key_view_tab_flyer"
#define MUIOPTION_UI_KEY_VIEW_TAB_HISTORY		"ui_key_view_tab_history"
#define MUIOPTION_UI_KEY_VIEW_TAB_MARQUEE		"ui_key_view_tab_marquee"
#define MUIOPTION_UI_KEY_VIEW_TAB_SCREENSHOT		"ui_key_view_tab_screenshot"
#define MUIOPTION_UI_KEY_VIEW_TAB_TITLE			"ui_key_view_tab_title"
#define MUIOPTION_UI_KEY_VIEW_TAB_PCB   		"ui_key_view_tab_pcb"
#define MUIOPTION_UI_KEY_QUIT				"ui_key_quit"
#define MUIOPTION_UI_JOY_UP				"ui_joy_up"
#define MUIOPTION_UI_JOY_DOWN				"ui_joy_down"
#define MUIOPTION_UI_JOY_LEFT				"ui_joy_left"
#define MUIOPTION_UI_JOY_RIGHT				"ui_joy_right"
#define MUIOPTION_UI_JOY_START				"ui_joy_start"
#define MUIOPTION_UI_JOY_PGUP				"ui_joy_pgup"
#define MUIOPTION_UI_JOY_PGDWN				"ui_joy_pgdwn"
#define MUIOPTION_UI_JOY_HOME				"ui_joy_home"
#define MUIOPTION_UI_JOY_END				"ui_joy_end"
#define MUIOPTION_UI_JOY_SS_CHANGE			"ui_joy_ss_change"
#define MUIOPTION_UI_JOY_HISTORY_UP			"ui_joy_history_up"
#define MUIOPTION_UI_JOY_HISTORY_DOWN			"ui_joy_history_down"
#define MUIOPTION_UI_JOY_EXEC				"ui_joy_exec"
#define MUIOPTION_EXEC_COMMAND				"exec_command"
#define MUIOPTION_EXEC_WAIT				"exec_wait"
#define MUIOPTION_HIDE_MOUSE				"hide_mouse"
#define MUIOPTION_FULL_SCREEN				"full_screen"

// Options names
#define MUIOPTION_DEFAULT_GAME				"default_system"
#define MUIOPTION_HISTORY_FILE				"sysinfo_file"
#define MUIOPTION_MAMEINFO_FILE				"messinfo_file"
// Option values
#define MUIDEFAULT_SELECTION				"sms"
#define MUIDEFAULT_SPLITTERS				"152,310,468"
#define MUIHISTORY_FILE					"history.dat"
#define MUIMAMEINFO_FILE				"messinfo.dat"

#define MUIOPTION_VERSION				"version"



/***************************************************************************
    Internal structures
 ***************************************************************************/

 /***************************************************************************
    Internal variables
 ***************************************************************************/

//static object_pool *options_memory_pool;

static winui_options settings;

static windows_options global;			// Global 'default' options

static game_options game_opts;

// UI options in MESSui.ini
const options_entry winui_options::s_option_entries[] =
{
	// UI options
	{ NULL,						NULL,       OPTION_HEADER,     "APPLICATION OPTIONS" },
	{ MUIOPTION_VERSION,				"",         OPTION_STRING,                 NULL },
	{ NULL,						NULL,       OPTION_HEADER,     "DISPLAY STATE OPTIONS" },
	{ MUIOPTION_DEFAULT_GAME,			MUIDEFAULT_SELECTION, OPTION_STRING,       NULL },
	{ MUIOPTION_DEFAULT_FOLDER_ID,			"0",        OPTION_INTEGER,                 NULL },
	{ MUIOPTION_SHOW_IMAGE_SECTION,			"1",        OPTION_BOOLEAN,    NULL },
	{ MUIOPTION_FULL_SCREEN,			"0",        OPTION_BOOLEAN,    NULL },
	{ MUIOPTION_CURRENT_TAB,			"0",        OPTION_STRING,                 NULL },
	{ MUIOPTION_SHOW_TOOLBAR,			"1",        OPTION_BOOLEAN,    NULL },
	{ MUIOPTION_SHOW_STATUS_BAR,			"1",        OPTION_BOOLEAN,    NULL },
	{ MUIOPTION_HIDE_FOLDERS,			"",         OPTION_STRING,                 NULL },
	{ MUIOPTION_SHOW_FOLDER_SECTION,		"0",        OPTION_BOOLEAN,    NULL },
	{ MUIOPTION_SHOW_TABS,				"0",        OPTION_BOOLEAN,    NULL },
	{ MUIOPTION_HIDE_TABS,				"flyer, cabinet, marquee, title, cpanel, pcb", OPTION_STRING, NULL },
	{ MUIOPTION_HISTORY_TAB,			"1",        OPTION_INTEGER,                 NULL },
	{ MUIOPTION_SHOW_SOFTWARE_SECTION,		"1",        OPTION_BOOLEAN,    NULL },
	{ MUIOPTION_SORT_COLUMN,			"0",        OPTION_INTEGER,                 NULL },
	{ MUIOPTION_SORT_REVERSED,			"0",        OPTION_BOOLEAN,    NULL },
	{ MUIOPTION_WINDOW_X,				"0",        OPTION_INTEGER,                 NULL },
	{ MUIOPTION_WINDOW_Y,				"0",        OPTION_INTEGER,                 NULL },
	{ MUIOPTION_WINDOW_WIDTH,			"640",      OPTION_INTEGER,                 NULL },
	{ MUIOPTION_WINDOW_HEIGHT,			"400",      OPTION_INTEGER,                 NULL },
	{ MUIOPTION_WINDOW_STATE,			"1",        OPTION_INTEGER,                 NULL },
	{ MUIOPTION_TEXT_COLOR,				"-1",       OPTION_INTEGER,                 NULL },
	{ MUIOPTION_CLONE_COLOR,			"-1",       OPTION_INTEGER,                 NULL },
	{ MUIOPTION_CUSTOM_COLOR,			"0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0", OPTION_STRING, NULL },
	/* ListMode needs to be before ColumnWidths settings */
	{ MUIOPTION_LIST_MODE,				"5",       OPTION_INTEGER,                 NULL },
	{ MUIOPTION_SPLITTERS,				MUIDEFAULT_SPLITTERS, OPTION_STRING,       NULL },
	{ MUIOPTION_LIST_FONT,				"-8,0,0,0,400,0,0,0,0,0,0,0,MS Sans Serif", OPTION_STRING, NULL },
	{ MUIOPTION_COLUMN_WIDTHS,			"185,78,84,84,64,88,74,108,60,144,84,60", OPTION_STRING, NULL },
	{ MUIOPTION_COLUMN_ORDER,			"0,1,2,3,4,5,6,7,8,9,10,11", OPTION_STRING, NULL },
	{ MUIOPTION_COLUMN_SHOWN,			"1,1,1,1,1,1,0,0,0,0,0,0", OPTION_STRING,  NULL },
	{ NULL,						NULL,       OPTION_HEADER,     "INTERFACE OPTIONS" },
	{ MUIOPTION_LANGUAGE,				"english",  OPTION_STRING,                 NULL },
	{ MUIOPTION_CHECK_GAME,				"0",        OPTION_BOOLEAN,    NULL },
	{ MUIOPTION_JOYSTICK_IN_INTERFACE,		"1",        OPTION_BOOLEAN,    NULL },
	{ MUIOPTION_KEYBOARD_IN_INTERFACE,		"0",        OPTION_BOOLEAN,    NULL },
	{ MUIOPTION_HIDE_MOUSE,				"0",        OPTION_BOOLEAN,    NULL },
	{ MUIOPTION_INHERIT_FILTER,			"0",        OPTION_BOOLEAN,    NULL },
	{ MUIOPTION_OFFSET_CLONES,			"0",        OPTION_BOOLEAN,    NULL },
	{ MUIOPTION_STRETCH_SCREENSHOT_LARGER,		"0",        OPTION_BOOLEAN,    NULL },
	{ MUIOPTION_CYCLE_SCREENSHOT,			"0",        OPTION_INTEGER,                 NULL },
	{ MUIOPTION_SCREENSHOT_BORDER_SIZE,		"11",       OPTION_INTEGER,                 NULL },
	{ MUIOPTION_SCREENSHOT_BORDER_COLOR,		"-1",       OPTION_INTEGER,                 NULL },
	{ MUIOPTION_EXEC_COMMAND,			"",         OPTION_STRING,                 NULL },
	{ MUIOPTION_EXEC_WAIT,				"0",        OPTION_INTEGER,                 NULL },
	{ NULL,						NULL,       OPTION_HEADER,     "SEARCH PATH OPTIONS" },
	{ MUIOPTION_FLYER_DIRECTORY,			"flyers",   OPTION_STRING,                 NULL },
	{ MUIOPTION_CABINET_DIRECTORY,			"cabinets", OPTION_STRING,                 NULL },
	{ MUIOPTION_MARQUEE_DIRECTORY,			"marquees", OPTION_STRING,                 NULL },
	{ MUIOPTION_TITLE_DIRECTORY,			"titles",   OPTION_STRING,                 NULL },
	{ MUIOPTION_CPANEL_DIRECTORY,			"cpanel",   OPTION_STRING,                 NULL },
	{ MUIOPTION_PCB_DIRECTORY,			"pcb",      OPTION_STRING,                 NULL },
	{ MUIOPTION_BACKGROUND_DIRECTORY,		"bkground", OPTION_STRING,                 NULL },
	{ MUIOPTION_FOLDER_DIRECTORY,			"folders",  OPTION_STRING,                 NULL },
	{ MUIOPTION_ICONS_DIRECTORY,			"icons",    OPTION_STRING,                 NULL },
	{ NULL,						NULL,       OPTION_HEADER,     "FILENAME OPTIONS" },
	{ MUIOPTION_HISTORY_FILE,			MUIHISTORY_FILE,  OPTION_STRING,              NULL },
	{ MUIOPTION_MAMEINFO_FILE,			MUIMAMEINFO_FILE, OPTION_STRING,             NULL },
	{ NULL,						NULL,       OPTION_HEADER,     "NAVIGATION KEY CODES" },
	{ MUIOPTION_UI_KEY_UP,				"KEYCODE_UP",						OPTION_STRING,          NULL },
	{ MUIOPTION_UI_KEY_DOWN,			"KEYCODE_DOWN", 					OPTION_STRING,          NULL },
	{ MUIOPTION_UI_KEY_LEFT,			"KEYCODE_LEFT", 					OPTION_STRING,          NULL },
	{ MUIOPTION_UI_KEY_RIGHT,			"KEYCODE_RIGHT",					OPTION_STRING,          NULL },
	{ MUIOPTION_UI_KEY_START,			"KEYCODE_ENTER NOT KEYCODE_LALT",	OPTION_STRING,			NULL },
	{ MUIOPTION_UI_KEY_PGUP,			"KEYCODE_PGUP", 					OPTION_STRING,          NULL },
	{ MUIOPTION_UI_KEY_PGDWN,			"KEYCODE_PGDN", 					OPTION_STRING,          NULL },
	{ MUIOPTION_UI_KEY_HOME,			"KEYCODE_HOME", 					OPTION_STRING,          NULL },
	{ MUIOPTION_UI_KEY_END,				"KEYCODE_END",						OPTION_STRING,          NULL },
	{ MUIOPTION_UI_KEY_SS_CHANGE,			"KEYCODE_INSERT",					OPTION_STRING,          NULL },
	{ MUIOPTION_UI_KEY_HISTORY_UP,			"KEYCODE_DEL",						OPTION_STRING,          NULL },
	{ MUIOPTION_UI_KEY_HISTORY_DOWN,		"KEYCODE_LALT KEYCODE_0",			OPTION_STRING,  		NULL },
	{ MUIOPTION_UI_KEY_CONTEXT_FILTERS,		"KEYCODE_LCONTROL KEYCODE_F", OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_SELECT_RANDOM,		"KEYCODE_LCONTROL KEYCODE_R", OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_GAME_AUDIT,			"KEYCODE_LALT KEYCODE_A",     OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_GAME_PROPERTIES,		"KEYCODE_LALT KEYCODE_ENTER", OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_HELP_CONTENTS,		"KEYCODE_F1",                 OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_UPDATE_GAMELIST,		"KEYCODE_F5",                 OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_VIEW_FOLDERS,		"KEYCODE_LALT KEYCODE_D",     OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_VIEW_FULLSCREEN,		"KEYCODE_F11",                OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_VIEW_PAGETAB,		"KEYCODE_LALT KEYCODE_B",     OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_VIEW_PICTURE_AREA,		"KEYCODE_LALT KEYCODE_P",     OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_VIEW_SOFTWARE_AREA,		"KEYCODE_LALT KEYCODE_W",     OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_VIEW_STATUS,			"KEYCODE_LALT KEYCODE_S",     OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_VIEW_TOOLBARS,		"KEYCODE_LALT KEYCODE_T",     OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_VIEW_TAB_CABINET,		"KEYCODE_LALT KEYCODE_3",     OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_VIEW_TAB_CPANEL,		"KEYCODE_LALT KEYCODE_6",     OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_VIEW_TAB_FLYER,		"KEYCODE_LALT KEYCODE_2",     OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_VIEW_TAB_HISTORY,		"KEYCODE_LALT KEYCODE_8",     OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_VIEW_TAB_MARQUEE,		"KEYCODE_LALT KEYCODE_4",     OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_VIEW_TAB_SCREENSHOT,		"KEYCODE_LALT KEYCODE_1",     OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_VIEW_TAB_TITLE,		"KEYCODE_LALT KEYCODE_5",     OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_VIEW_TAB_PCB,		"KEYCODE_LALT KEYCODE_7",     OPTION_STRING, NULL },
	{ MUIOPTION_UI_KEY_QUIT,			"KEYCODE_LALT KEYCODE_Q",     OPTION_STRING, NULL },
	{ NULL,						NULL,       OPTION_HEADER,     "NAVIGATION JOYSTICK CODES" },
	{ MUIOPTION_UI_JOY_UP,				"1,1,1,1",  OPTION_STRING,                 NULL },
	{ MUIOPTION_UI_JOY_DOWN,			"1,1,1,2",  OPTION_STRING,                 NULL },
	{ MUIOPTION_UI_JOY_LEFT,			"1,1,2,1",  OPTION_STRING,                 NULL },
	{ MUIOPTION_UI_JOY_RIGHT,			"1,1,2,2",  OPTION_STRING,                 NULL },
	{ MUIOPTION_UI_JOY_START,			"1,0,1,0",  OPTION_STRING,                 NULL },
	{ MUIOPTION_UI_JOY_PGUP,			"2,1,2,1",  OPTION_STRING,                 NULL },
	{ MUIOPTION_UI_JOY_PGDWN,			"2,1,2,2",  OPTION_STRING,                 NULL },
	{ MUIOPTION_UI_JOY_HOME,			"0,0,0,0",  OPTION_STRING,                 NULL },
	{ MUIOPTION_UI_JOY_END,				"0,0,0,0",  OPTION_STRING,                 NULL },
	{ MUIOPTION_UI_JOY_SS_CHANGE,			"2,0,3,0",  OPTION_STRING,                 NULL },
	{ MUIOPTION_UI_JOY_HISTORY_UP,			"2,0,4,0",  OPTION_STRING,                 NULL },
	{ MUIOPTION_UI_JOY_HISTORY_DOWN,		"2,0,1,0",  OPTION_STRING,                 NULL },
	{ MUIOPTION_UI_JOY_EXEC,			"0,0,0,0",  OPTION_STRING,                 NULL },
	{ NULL }
};

static const options_entry perGameOptions[] =
{
	// per game options in messui.ini - to transfer to commentpath
	{ "_extra_software",         "",         OPTION_STRING,  NULL },
	{ NULL }
};

static const options_entry filterOptions[] =
{
	// filters
	{ "_filters",                "0",        OPTION_INTEGER,                 NULL },
	{ NULL }
};



// Screen shot Page tab control text
// these must match the order of the options flags in options.h
// (TAB_...)
static const char *const image_tabs_long_name[MAX_TAB_TYPES] =
{
	"Snapshot",
	"Flyer",
	"Cabinet",
	"Marquee",
	"Title",
	"Control Panel",
	"PCB",
	"History",
};

static const char *const image_tabs_short_name[MAX_TAB_TYPES] =
{
	"snapshot",
	"flyer",
	"cabinet",
	"marquee",
	"title",
	"cpanel",
	"pcb",
	"history"
};


/***************************************************************************
    External functions
 ***************************************************************************/
winui_options::winui_options()
{
	add_entries(s_option_entries);
}

// NOT USED
/*static void memory_error(const char *message)
{
	win_message_box_utf8(NULL, message, NULL, MB_OK);
	exit(-1);
}
*/


// NOT USED
/*
static void AddOptions(winui_options *opts, const options_entry *entrylist, BOOL is_global)
{
	static const char *blacklist[] =
	{
		WINOPTION_SCREEN,
		WINOPTION_ASPECT,
		WINOPTION_RESOLUTION,
		WINOPTION_VIEW
	};
	options_entry entries[2];
	BOOL good_option = 0;
	int i = 0;

	for ( ; entrylist->name != NULL || (entrylist->flags & OPTION_HEADER); entrylist++)
	{
		good_option = TRUE;

		// check blacklist
		if (entrylist->name != NULL)
		{
			for (i = 0; i < ARRAY_LENGTH(blacklist); i++)
			{
				if (strcmp(entrylist->name, blacklist[i])==0)
				{
					good_option = FALSE;
					break;
				}
			}
		}

		// if is_global is FALSE, blacklist global options (mame only)
		if (entrylist->name && !is_global && IsGlobalOption(entrylist->name))
			good_option = FALSE;

		if (good_option)
		{
			memcpy(entries, entrylist, sizeof(options_entry));
			memset(&entries[1], 0, sizeof(entries[1]));
///			options_add_entries(opts, entries);
		}
	}
}
*/

void CreateGameOptions(windows_options &opts, int driver_index)
{
	//BOOL is_global = (driver_index == GLOBAL_OPTIONS);

	// customize certain options
	//if (is_global)
	//	opts.set_default_value(OPTION_INIPATH, "ini");

	MessSetupGameOptions(opts, driver_index);
}



BOOL OptionsInit()
{
	// create a memory pool for our data
	//options_memory_pool = pool_alloc_lib(memory_error);
	//if (!options_memory_pool)
	//	return FALSE;

	// set up the MAME32 settings (these get placed in MAME32ui.ini
	//settings = options_create(memory_error);
	//options_add_entries(settings, regSettings);
	MessSetupSettings(settings);

	// set up per game options
	{
		char buffer[128];
		int i, j;
		int game_option_count = 0;

		while(perGameOptions[game_option_count].name)
			game_option_count++;

		for (i = 0; i < driver_list::total(); i++)
		{
			for (j = 0; j < game_option_count; j++)
			{
				options_entry entry[2] = { { 0 }, { 0 } };
				snprintf(buffer, ARRAY_LENGTH(buffer), "%s%s", driver_list::driver(i).name, perGameOptions[j].name);

				entry[0].name = core_strdup(buffer);
				entry[0].defvalue = perGameOptions[j].defvalue;
				entry[0].flags = perGameOptions[j].flags;
				entry[0].description = perGameOptions[j].description;
				settings.add_entries(entry);
			}
		}
	}

	game_opts.add_entries();
	// set up global options
	CreateGameOptions(global, GLOBAL_OPTIONS);
	// now load the options and settings
	LoadOptionsAndSettings();

	return TRUE;

}

void OptionsExit(void)
{
	// free global options
	//options_free(global);
	//global = NULL;

	// free settings
	//options_free(settings);
	//settings = NULL;

	// free the memory pool
	//pool_free_lib(options_memory_pool);
	//options_memory_pool = NULL;
}

winui_options & MameUISettings(void)
{
	return settings;
}

windows_options & MameUIGlobal(void)
{
	return global;
}

// Restore ui settings to factory
void ResetGUI(void)
{
	settings.revert(OPTION_PRIORITY_NORMAL);
	// Save the new MAME32ui.ini
	SaveOptions();
}

const char * GetImageTabLongName(int tab_index)
{
	assert(tab_index >= 0);
	assert(tab_index < ARRAY_LENGTH(image_tabs_long_name));
	return image_tabs_long_name[tab_index];
}

const char * GetImageTabShortName(int tab_index)
{
	assert(tab_index >= 0);
	assert(tab_index < ARRAY_LENGTH(image_tabs_short_name));
	return image_tabs_short_name[tab_index];
}

//============================================================
//  OPTIONS WRAPPERS
//============================================================

static COLORREF options_get_color(winui_options &opts, const char *name)
{
	const char *value_str;
	unsigned int r, g, b;
	COLORREF value;

	value_str = opts.value( name);

	if (sscanf(value_str, "%u,%u,%u", &r, &g, &b) == 3)
		value = RGB(r,g,b);
	else
		value = (COLORREF) - 1;
	return value;
}

static void options_set_color(winui_options &opts, const char *name, COLORREF value)
{
	char value_str[32];

	if (value == (COLORREF) -1)
	{
		snprintf(value_str, ARRAY_LENGTH(value_str), "%d", (int) value);
	}
	else
	{
		snprintf(value_str, ARRAY_LENGTH(value_str), "%d,%d,%d",
			(((int) value) >>  0) & 0xFF,
			(((int) value) >>  8) & 0xFF,
			(((int) value) >> 16) & 0xFF);
	}
	std::string error_string;
	opts.set_value(name, value_str, OPTION_PRIORITY_CMDLINE,error_string);
}

static COLORREF options_get_color_default(winui_options &opts, const char *name, int default_color)
{
	COLORREF value = options_get_color(opts, name);
	if (value == (COLORREF) -1)
		value = GetSysColor(default_color);
	return value;
}

static void options_set_color_default(winui_options &opts, const char *name, COLORREF value, int default_color)
{
	if (value == GetSysColor(default_color))
		options_set_color(settings, name, (COLORREF) -1);
	else
		options_set_color(settings, name, value);
}

static input_seq *options_get_input_seq(winui_options &opts, const char *name)
{
/*  static input_seq seq;
	const char *seq_string;

	seq_string = opts.value( name);
	input_seq_from_tokens(NULL, seq_string, &seq);  // HACK
	return &seq;*/
	return NULL;
}



//============================================================
//  OPTIONS CALLS
//============================================================

void SetViewMode(int val)
{
	std::string error_string;
	settings.set_value(MUIOPTION_LIST_MODE, val, OPTION_PRIORITY_CMDLINE,error_string);
}

int GetViewMode(void)
{
	return settings.int_value(MUIOPTION_LIST_MODE);
}

void SetGameCheck(BOOL game_check)
{
	std::string error_string;
	settings.set_value(MUIOPTION_CHECK_GAME, game_check, OPTION_PRIORITY_CMDLINE,error_string);
}

BOOL GetGameCheck(void)
{
	return settings.bool_value(MUIOPTION_CHECK_GAME);
}

void SetJoyGUI(BOOL use_joygui)
{
	std::string error_string;
	settings.set_value(MUIOPTION_JOYSTICK_IN_INTERFACE, use_joygui, OPTION_PRIORITY_CMDLINE,error_string);
}

BOOL GetJoyGUI(void)
{
	return settings.bool_value( MUIOPTION_JOYSTICK_IN_INTERFACE);
}

void SetKeyGUI(BOOL use_keygui)
{
	std::string error_string;
	settings.set_value(MUIOPTION_KEYBOARD_IN_INTERFACE, use_keygui, OPTION_PRIORITY_CMDLINE,error_string);
}

BOOL GetKeyGUI(void)
{
	return settings.bool_value( MUIOPTION_KEYBOARD_IN_INTERFACE);
}

void SetCycleScreenshot(int cycle_screenshot)
{
	std::string error_string;
	settings.set_value(MUIOPTION_CYCLE_SCREENSHOT, cycle_screenshot, OPTION_PRIORITY_CMDLINE,error_string);
}

int GetCycleScreenshot(void)
{
	return settings.int_value(MUIOPTION_CYCLE_SCREENSHOT);
}

void SetStretchScreenShotLarger(BOOL stretch)
{
	std::string error_string;
	settings.set_value(MUIOPTION_STRETCH_SCREENSHOT_LARGER, stretch, OPTION_PRIORITY_CMDLINE,error_string);
}

BOOL GetStretchScreenShotLarger(void)
{
	return settings.bool_value( MUIOPTION_STRETCH_SCREENSHOT_LARGER);
}

void SetScreenshotBorderSize(int size)
{
	std::string error_string;
	settings.set_value(MUIOPTION_SCREENSHOT_BORDER_SIZE, size, OPTION_PRIORITY_CMDLINE,error_string);
}

int GetScreenshotBorderSize(void)
{
	return settings.int_value(MUIOPTION_SCREENSHOT_BORDER_SIZE);
}

void SetScreenshotBorderColor(COLORREF uColor)
{
	options_set_color_default(settings, MUIOPTION_SCREENSHOT_BORDER_COLOR, uColor, COLOR_3DFACE);
}

COLORREF GetScreenshotBorderColor(void)
{
	return options_get_color_default(settings, MUIOPTION_SCREENSHOT_BORDER_COLOR, COLOR_3DFACE);
}

void SetFilterInherit(BOOL inherit)
{
	std::string error_string;
	settings.set_value(MUIOPTION_INHERIT_FILTER, inherit, OPTION_PRIORITY_CMDLINE,error_string);
}

BOOL GetFilterInherit(void)
{
	return settings.bool_value( MUIOPTION_INHERIT_FILTER);
}

void SetOffsetClones(BOOL offset)
{
	std::string error_string;
	settings.set_value(MUIOPTION_OFFSET_CLONES, offset, OPTION_PRIORITY_CMDLINE,error_string);
}

BOOL GetOffsetClones(void)
{
	return settings.bool_value( MUIOPTION_OFFSET_CLONES);
}

void SetSavedFolderID(UINT val)
{
	std::string error_string;
	settings.set_value(MUIOPTION_DEFAULT_FOLDER_ID, (int) val, OPTION_PRIORITY_CMDLINE,error_string);
}

UINT GetSavedFolderID(void)
{
	return (UINT) settings.int_value(MUIOPTION_DEFAULT_FOLDER_ID);
}

void SetShowScreenShot(BOOL val)
{
	std::string error_string;
	settings.set_value(MUIOPTION_SHOW_IMAGE_SECTION, val, OPTION_PRIORITY_CMDLINE,error_string);
}

BOOL GetShowScreenShot(void)
{
	return settings.bool_value(MUIOPTION_SHOW_IMAGE_SECTION);
}

void SetShowSoftware(BOOL val)
{
	std::string error_string;
	settings.set_value(MUIOPTION_SHOW_SOFTWARE_SECTION, val, OPTION_PRIORITY_CMDLINE,error_string);
}

BOOL GetShowSoftware(void)
{
	return settings.bool_value(MUIOPTION_SHOW_SOFTWARE_SECTION);
}

void SetShowFolderList(BOOL val)
{
	std::string error_string;
	settings.set_value(MUIOPTION_SHOW_FOLDER_SECTION, val, OPTION_PRIORITY_CMDLINE,error_string);
}

BOOL GetShowFolderList(void)
{
	return settings.bool_value(MUIOPTION_SHOW_FOLDER_SECTION);
}

static void GetsShowFolderFlags(LPBITS bits)
{
	char s[2000];
	extern const FOLDERDATA g_folderData[];
	char *token;

	snprintf(s, ARRAY_LENGTH(s), "%s", settings.value( MUIOPTION_HIDE_FOLDERS));

	SetAllBits(bits, TRUE);

	token = strtok(s,", \t");
	while (token != NULL)
	{
		int j;

		for (j=0;g_folderData[j].m_lpTitle != NULL;j++)
		{
			if (strcmp(g_folderData[j].short_name,token) == 0)
			{
				ClearBit(bits, g_folderData[j].m_nFolderId);
				break;
			}
		}
		token = strtok(NULL,", \t");
	}
}

BOOL GetShowFolder(int folder)
{
	BOOL result;
	LPBITS show_folder_flags = NewBits(MAX_FOLDERS);
	GetsShowFolderFlags(show_folder_flags);
	result = TestBit(show_folder_flags, folder);
	DeleteBits(show_folder_flags);
	return result;
}

void SetShowFolder(int folder,BOOL show)
{
	LPBITS show_folder_flags = NewBits(MAX_FOLDERS);
	int i;
	int num_saved = 0;
	char str[10000];
	extern const FOLDERDATA g_folderData[];

	GetsShowFolderFlags(show_folder_flags);

	if (show)
		SetBit(show_folder_flags, folder);
	else
		ClearBit(show_folder_flags, folder);

	strcpy(str, "");

	// we save the ones that are NOT displayed, so we can add new ones
	// and upgraders will see them
	for (i=0;i<MAX_FOLDERS;i++)
	{
		if (TestBit(show_folder_flags, i) == FALSE)
		{
			int j;

			if (num_saved != 0)
				strcat(str,", ");

			for (j=0;g_folderData[j].m_lpTitle != NULL;j++)
			{
				if (g_folderData[j].m_nFolderId == i)
				{
					strcat(str,g_folderData[j].short_name);
					num_saved++;
					break;
				}
			}
		}
	}
	std::string error_string;
	settings.set_value(MUIOPTION_HIDE_FOLDERS, str, OPTION_PRIORITY_CMDLINE,error_string);
	DeleteBits(show_folder_flags);
}

void SetShowStatusBar(BOOL val)
{
	std::string error_string;
	settings.set_value(MUIOPTION_SHOW_STATUS_BAR, val, OPTION_PRIORITY_CMDLINE,error_string);
}

BOOL GetShowStatusBar(void)
{
	return settings.bool_value( MUIOPTION_SHOW_STATUS_BAR);
}

void SetShowTabCtrl (BOOL val)
{
	std::string error_string;
	settings.set_value(MUIOPTION_SHOW_TABS, val, OPTION_PRIORITY_CMDLINE,error_string);
}

BOOL GetShowTabCtrl (void)
{
	return settings.bool_value( MUIOPTION_SHOW_TABS);
}

void SetShowToolBar(BOOL val)
{
	std::string error_string;
	settings.set_value(MUIOPTION_SHOW_TOOLBAR, val, OPTION_PRIORITY_CMDLINE,error_string);
}

BOOL GetShowToolBar(void)
{
	return settings.bool_value( MUIOPTION_SHOW_TOOLBAR);
}

void SetCurrentTab(const char *shortname)
{
	std::string error_string;
	settings.set_value(MUIOPTION_CURRENT_TAB, shortname, OPTION_PRIORITY_CMDLINE,error_string);
}

const char *GetCurrentTab(void)
{
	return settings.value( MUIOPTION_CURRENT_TAB);
}

void SetDefaultGame(const char *name)
{
	std::string error_string;
	settings.set_value(MUIOPTION_DEFAULT_GAME, name, OPTION_PRIORITY_CMDLINE,error_string);
}

const char *GetDefaultGame(void)
{
	return settings.value( MUIOPTION_DEFAULT_GAME);
}

void SetWindowArea(const AREA *area)
{
	std::string error_string;
	settings.set_value(MUIOPTION_WINDOW_X,		area->x, OPTION_PRIORITY_CMDLINE,error_string);
	settings.set_value(MUIOPTION_WINDOW_Y,		area->y, OPTION_PRIORITY_CMDLINE,error_string);
	settings.set_value(MUIOPTION_WINDOW_WIDTH,	area->width, OPTION_PRIORITY_CMDLINE,error_string);
	settings.set_value(MUIOPTION_WINDOW_HEIGHT,	area->height, OPTION_PRIORITY_CMDLINE,error_string);
}

void GetWindowArea(AREA *area)
{
	area->x = settings.int_value(MUIOPTION_WINDOW_X);
	area->y = settings.int_value(MUIOPTION_WINDOW_Y);
	area->width  = settings.int_value(MUIOPTION_WINDOW_WIDTH);
	area->height = settings.int_value(MUIOPTION_WINDOW_HEIGHT);
}

void SetWindowState(UINT state)
{
	std::string error_string;
	settings.set_value(MUIOPTION_WINDOW_STATE, (int)state, OPTION_PRIORITY_CMDLINE,error_string);
}

UINT GetWindowState(void)
{
	return settings.int_value(MUIOPTION_WINDOW_STATE);
}

void SetCustomColor(int iIndex, COLORREF uColor)
{
	const char *custom_color_string;
	COLORREF custom_color[256];
	char buffer[80];

	custom_color_string = settings.value( MUIOPTION_CUSTOM_COLOR);
	CusColorDecodeString(custom_color_string, custom_color);

	custom_color[iIndex] = uColor;

	CusColorEncodeString(custom_color, buffer);
	std::string error_string;
	settings.set_value(MUIOPTION_CUSTOM_COLOR, buffer, OPTION_PRIORITY_CMDLINE,error_string);
}

COLORREF GetCustomColor(int iIndex)
{
	const char *custom_color_string;
	COLORREF custom_color[256];

	custom_color_string = settings.value( MUIOPTION_CUSTOM_COLOR);
	CusColorDecodeString(custom_color_string, custom_color);

	if (custom_color[iIndex] == (COLORREF)-1)
		return (COLORREF)RGB(0,0,0);

	return custom_color[iIndex];
}

void SetListFont(const LOGFONT *font)
{
	char font_string[10000];
	FontEncodeString(font, font_string);
	std::string error_string;
	settings.set_value(MUIOPTION_LIST_FONT, font_string, OPTION_PRIORITY_CMDLINE,error_string);
}

void GetListFont(LOGFONT *font)
{
	const char *font_string = settings.value( MUIOPTION_LIST_FONT);
	FontDecodeString(font_string, font);
}

void SetListFontColor(COLORREF uColor)
{
	options_set_color_default(settings, MUIOPTION_TEXT_COLOR, uColor, COLOR_WINDOWTEXT);
}

COLORREF GetListFontColor(void)
{
	return options_get_color_default(settings, MUIOPTION_TEXT_COLOR, COLOR_WINDOWTEXT);
}

void SetListCloneColor(COLORREF uColor)
{
	options_set_color_default(settings, MUIOPTION_CLONE_COLOR, uColor, COLOR_WINDOWTEXT);
}

COLORREF GetListCloneColor(void)
{
	return options_get_color_default(settings, MUIOPTION_CLONE_COLOR, COLOR_WINDOWTEXT);
}

int GetShowTab(int tab)
{
	const char *show_tabs_string;
	int show_tab_flags;

	show_tabs_string = settings.value( MUIOPTION_HIDE_TABS);
	TabFlagsDecodeString(show_tabs_string, &show_tab_flags);
	return (show_tab_flags & (1 << tab)) != 0;
}

void SetShowTab(int tab,BOOL show)
{
	const char *show_tabs_string;
	int show_tab_flags;
	char buffer[10000];

	show_tabs_string = settings.value( MUIOPTION_HIDE_TABS);
	TabFlagsDecodeString(show_tabs_string, &show_tab_flags);

	if (show)
		show_tab_flags |= 1 << tab;
	else
		show_tab_flags &= ~(1 << tab);

	TabFlagsEncodeString(show_tab_flags, buffer);
	std::string error_string;
	settings.set_value(MUIOPTION_HIDE_TABS, buffer, OPTION_PRIORITY_CMDLINE,error_string);
}

// don't delete the last one
BOOL AllowedToSetShowTab(int tab,BOOL show)
{
	const char *show_tabs_string;
	int show_tab_flags;

	if (show == TRUE)
		return TRUE;

	show_tabs_string = settings.value( MUIOPTION_HIDE_TABS);
	TabFlagsDecodeString(show_tabs_string, &show_tab_flags);

	show_tab_flags &= ~(1 << tab);
	return show_tab_flags != 0;
}

int GetHistoryTab(void)
{
	return settings.int_value(MUIOPTION_HISTORY_TAB);
}

void SetHistoryTab(int tab, BOOL show)
{
	std::string error_string;
	if (show)
		settings.set_value(MUIOPTION_HISTORY_TAB, tab, OPTION_PRIORITY_CMDLINE,error_string);
	else
		settings.set_value(MUIOPTION_HISTORY_TAB, TAB_NONE, OPTION_PRIORITY_CMDLINE,error_string);
}

void SetColumnWidths(int width[])
{
	char column_width_string[80];
	ColumnEncodeStringWithCount(width, column_width_string, COLUMN_MAX);
	std::string error_string;
	settings.set_value(MUIOPTION_COLUMN_WIDTHS, column_width_string, OPTION_PRIORITY_CMDLINE,error_string);
}

void GetColumnWidths(int width[])
{
	const char *column_width_string;
	column_width_string = settings.value(MUIOPTION_COLUMN_WIDTHS);
	ColumnDecodeStringWithCount(column_width_string, width, COLUMN_MAX);
}

void SetSplitterPos(int splitterId, int pos)
{
	const char *splitter_string;
	int *splitter;
	char buffer[80];

	if (splitterId < GetSplitterCount())
	{
		splitter_string = settings.value(MUIOPTION_SPLITTERS);
		splitter = (int *) alloca(GetSplitterCount() * sizeof(*splitter));
		SplitterDecodeString(splitter_string, splitter);

		splitter[splitterId] = pos;

		SplitterEncodeString(splitter, buffer);
		std::string error_string;
		settings.set_value(MUIOPTION_SPLITTERS, buffer, OPTION_PRIORITY_CMDLINE,error_string);
	}
}

int  GetSplitterPos(int splitterId)
{
	const char *splitter_string;
	int *splitter;

	splitter_string = settings.value( MUIOPTION_SPLITTERS);
	splitter = (int *) alloca(GetSplitterCount() * sizeof(*splitter));
	SplitterDecodeString(splitter_string, splitter);

	if (splitterId < GetSplitterCount())
		return splitter[splitterId];

	return -1; /* Error */
}

void SetColumnOrder(int order[])
{
	char column_order_string[80];
	ColumnEncodeStringWithCount(order, column_order_string, COLUMN_MAX);
	std::string error_string;
	settings.set_value(MUIOPTION_COLUMN_ORDER, column_order_string, OPTION_PRIORITY_CMDLINE,error_string);
}

void GetColumnOrder(int order[])
{
	const char *column_order_string;
	column_order_string = settings.value( MUIOPTION_COLUMN_ORDER);
	ColumnDecodeStringWithCount(column_order_string, order, COLUMN_MAX);
}

void SetColumnShown(int shown[])
{
	char column_shown_string[80];
	ColumnEncodeStringWithCount(shown, column_shown_string, COLUMN_MAX);
	std::string error_string;
	settings.set_value(MUIOPTION_COLUMN_SHOWN, column_shown_string, OPTION_PRIORITY_CMDLINE,error_string);
}

void GetColumnShown(int shown[])
{
	const char *column_shown_string;
	column_shown_string = settings.value( MUIOPTION_COLUMN_SHOWN);
	ColumnDecodeStringWithCount(column_shown_string, shown, COLUMN_MAX);
}

void SetSortColumn(int column)
{
	std::string error_string;
	settings.set_value(MUIOPTION_SORT_COLUMN, column, OPTION_PRIORITY_CMDLINE,error_string);
}

int GetSortColumn(void)
{
	return settings.int_value(MUIOPTION_SORT_COLUMN);
}

void SetSortReverse(BOOL reverse)
{
	std::string error_string;
	settings.set_value(MUIOPTION_SORT_REVERSED, reverse, OPTION_PRIORITY_CMDLINE,error_string);
}

BOOL GetSortReverse(void)
{
	return settings.bool_value( MUIOPTION_SORT_REVERSED);
}

const char* GetLanguage(void)
{
	return settings.value( MUIOPTION_LANGUAGE);
}

void SetLanguage(const char* lang)
{
	std::string error_string;
	settings.set_value(MUIOPTION_LANGUAGE, lang, OPTION_PRIORITY_CMDLINE,error_string);
}

const char* GetRomDirs(void)
{
	return global.media_path();
}

void SetRomDirs(const char* paths)
{
	std::string error_string;
	global.set_value(OPTION_MEDIAPATH, paths, OPTION_PRIORITY_CMDLINE,error_string);
}

const char* GetHashDirs(void)
{
	return global.hash_path();
}

void SetHashDirs(const char* paths)
{
	std::string error_string;
	global.set_value(OPTION_HASHPATH, paths, OPTION_PRIORITY_CMDLINE,error_string);
}

const char* GetSampleDirs(void)
{
	return global.value(OPTION_SAMPLEPATH);
}

void SetSampleDirs(const char* paths)
{
	std::string error_string;
	global.set_value(OPTION_SAMPLEPATH, paths, OPTION_PRIORITY_CMDLINE,error_string);
}

const char * GetIniDir(void)
{
	const char *ini_dir;
	const char *s;

	ini_dir = global.value(OPTION_INIPATH);
	while((s = strchr(ini_dir, ';')) != NULL)
	{
		ini_dir = s + 1;
	}
	return ini_dir;

}

void SetIniDir(const char *path)
{
	std::string error_string;
	global.set_value(OPTION_INIPATH, path, OPTION_PRIORITY_CMDLINE,error_string);
}

const char* GetCtrlrDir(void)
{
	return global.value(OPTION_CTRLRPATH);
}

void SetCtrlrDir(const char* path)
{
	std::string error_string;
	global.set_value(OPTION_CTRLRPATH, path, OPTION_PRIORITY_CMDLINE,error_string);
}

const char* GetCommentDir(void)
{
	return global.value(OPTION_COMMENT_DIRECTORY);
}

void SetCommentDir(const char* path)
{
	std::string error_string;
	global.set_value(OPTION_COMMENT_DIRECTORY, path, OPTION_PRIORITY_CMDLINE,error_string);
}

const char* GetCfgDir(void)
{
	return global.value(OPTION_CFG_DIRECTORY);
}

void SetCfgDir(const char* path)
{
	std::string error_string;
	global.set_value(OPTION_CFG_DIRECTORY, path, OPTION_PRIORITY_CMDLINE,error_string);
}

const char* GetNvramDir(void)
{
	return global.value(OPTION_NVRAM_DIRECTORY);
}

void SetNvramDir(const char* path)
{
	std::string error_string;
	global.set_value(OPTION_NVRAM_DIRECTORY, path, OPTION_PRIORITY_CMDLINE,error_string);
}

const char* GetInpDir(void)
{
	return global.value(OPTION_INPUT_DIRECTORY);
}

void SetInpDir(const char* path)
{
	std::string error_string;
	global.set_value(OPTION_INPUT_DIRECTORY, path, OPTION_PRIORITY_CMDLINE,error_string);
}

const char* GetImgDir(void)
{
	return global.value(OPTION_SNAPSHOT_DIRECTORY);
}

void SetImgDir(const char* path)
{
	std::string error_string;
	global.set_value(OPTION_SNAPSHOT_DIRECTORY, path, OPTION_PRIORITY_CMDLINE,error_string);
}

const char* GetStateDir(void)
{
	return global.value(OPTION_STATE_DIRECTORY);
}

void SetStateDir(const char* path)
{
	std::string error_string;
	global.set_value(OPTION_STATE_DIRECTORY, path, OPTION_PRIORITY_CMDLINE,error_string);
}

const char* GetArtDir(void)
{
	return global.value(OPTION_ARTPATH);
}

void SetArtDir(const char* path)
{
	std::string error_string;
	global.set_value(OPTION_ARTPATH, path, OPTION_PRIORITY_CMDLINE,error_string);
}

const char* GetFontDir(void)
{
	return global.value(OPTION_FONTPATH);
}

void SetFontDir(const char* paths)
{
	std::string error_string;
	global.set_value(OPTION_FONTPATH, paths, OPTION_PRIORITY_CMDLINE,error_string);
}

const char* GetCrosshairDir(void)
{
	return global.value(OPTION_CROSSHAIRPATH);
}

void SetCrosshairDir(const char* paths)
{
	std::string error_string;
	global.set_value(OPTION_CROSSHAIRPATH, paths, OPTION_PRIORITY_CMDLINE,error_string);
}

const char* GetFlyerDir(void)
{
	return settings.value( MUIOPTION_FLYER_DIRECTORY);
}

void SetFlyerDir(const char* path)
{
	std::string error_string;
	settings.set_value(MUIOPTION_FLYER_DIRECTORY, path, OPTION_PRIORITY_CMDLINE,error_string);
}

const char* GetCabinetDir(void)
{
	return settings.value( MUIOPTION_CABINET_DIRECTORY);
}

void SetCabinetDir(const char* path)
{
	std::string error_string;
	settings.set_value(MUIOPTION_CABINET_DIRECTORY, path, OPTION_PRIORITY_CMDLINE,error_string);
}

const char* GetMarqueeDir(void)
{
	return settings.value( MUIOPTION_MARQUEE_DIRECTORY);
}

void SetMarqueeDir(const char* path)
{
	std::string error_string;
	settings.set_value(MUIOPTION_MARQUEE_DIRECTORY, path, OPTION_PRIORITY_CMDLINE,error_string);
}

const char* GetTitlesDir(void)
{
	return settings.value( MUIOPTION_TITLE_DIRECTORY);
}

void SetTitlesDir(const char* path)
{
	std::string error_string;
	settings.set_value(MUIOPTION_TITLE_DIRECTORY, path, OPTION_PRIORITY_CMDLINE,error_string);
}

const char * GetControlPanelDir(void)
{
	return settings.value( MUIOPTION_CPANEL_DIRECTORY);
}

void SetControlPanelDir(const char *path)
{
	std::string error_string;
	settings.set_value(MUIOPTION_CPANEL_DIRECTORY, path, OPTION_PRIORITY_CMDLINE,error_string);
}

const char * GetPcbDir(void)
{
	return settings.value( MUIOPTION_PCB_DIRECTORY);
}

void SetPcbDir(const char *path)
{
	std::string error_string;
	settings.set_value(MUIOPTION_PCB_DIRECTORY, path, OPTION_PRIORITY_CMDLINE,error_string);
}

const char * GetDiffDir(void)
{
	return global.value(OPTION_DIFF_DIRECTORY);
}

void SetDiffDir(const char* path)
{
	std::string error_string;
	global.set_value(OPTION_DIFF_DIRECTORY, path, OPTION_PRIORITY_CMDLINE,error_string);
}

const char* GetIconsDir(void)
{
	return settings.value( MUIOPTION_ICONS_DIRECTORY);
}

void SetIconsDir(const char* path)
{
	std::string error_string;
	settings.set_value(MUIOPTION_ICONS_DIRECTORY, path, OPTION_PRIORITY_CMDLINE,error_string);
}

const char* GetBgDir (void)
{
	return settings.value( MUIOPTION_BACKGROUND_DIRECTORY);
}

void SetBgDir (const char* path)
{
	std::string error_string;
	settings.set_value(MUIOPTION_BACKGROUND_DIRECTORY, path, OPTION_PRIORITY_CMDLINE,error_string);
}

const char* GetFolderDir(void)
{
	return settings.value( MUIOPTION_FOLDER_DIRECTORY);
}

void SetFolderDir(const char* path)
{
	std::string error_string;
	settings.set_value(MUIOPTION_FOLDER_DIRECTORY, path, OPTION_PRIORITY_CMDLINE,error_string);
}

const char* GetCheatDir(void)
{
	return global.value(OPTION_CHEATPATH);
}

void SetCheatDir(const char* path)
{
	std::string error_string;
	global.set_value(OPTION_CHEATPATH, path, OPTION_PRIORITY_CMDLINE,error_string);
}

const char* GetHistoryFileName(void)
{
	return settings.value( MUIOPTION_HISTORY_FILE);
}

void SetHistoryFileName(const char* path)
{
	std::string error_string;
	settings.set_value(MUIOPTION_HISTORY_FILE, path, OPTION_PRIORITY_CMDLINE,error_string);
}


const char* GetMAMEInfoFileName(void)
{
	return settings.value( MUIOPTION_MAMEINFO_FILE);
}

void SetMAMEInfoFileName(const char* path)
{
	std::string error_string;
	settings.set_value(MUIOPTION_MAMEINFO_FILE, path, OPTION_PRIORITY_CMDLINE,error_string);
}

const char* GetSnapName(void)
{
	return global.value(OPTION_SNAPNAME);
}

void SetSnapName(const char* pattern)
{
	std::string error_string;
	global.set_value(OPTION_SNAPNAME, pattern, OPTION_PRIORITY_CMDLINE,error_string);
}

void ResetGameOptions(int driver_index)
{
	assert(0 <= driver_index && driver_index < driver_list::total());

	//save_options(NULL, OPTIONS_GAME, driver_index);
}

void ResetGameDefaults(void)
{
	// Walk the global settings and reset everything to defaults;
	ResetToDefaults(global, OPTION_PRIORITY_CMDLINE);
	save_options(global, GLOBAL_OPTIONS);
	//save_default_options = FALSE;
}

/*
 * Reset all game, vector and source options to defaults.
 * No reason to reboot if this is done.
 */
void ResetAllGameOptions(void)
{
	int i;

	for (i = 0; i < driver_list::total(); i++)
		ResetGameOptions(i);
}

//static void GetDriverOptionName(int driver_index, const char *option_name, char *buffer, size_t buffer_len)
//{
//	assert(0 <= driver_index && driver_index < driver_list::total());
//	snprintf(buffer, buffer_len, "%s_%s", driver_list::driver(driver_index).name, option_name);
//}

//int GetRomAuditResults(int driver_index)
//{
//	char buffer[256];
//	GetDriverOptionName(driver_index, "rom_audit", buffer, ARRAY_LENGTH(buffer));
//	return settings.int_value(buffer);
//}

//void SetRomAuditResults(int driver_index, int audit_results)
//{
//	char buffer[256];
//	GetDriverOptionName(driver_index, "rom_audit", buffer, ARRAY_LENGTH(buffer));
//	std::string error_string;
//	settings.set_value(buffer, audit_results, OPTION_PRIORITY_CMDLINE,error_string);
//}

int GetRomAuditResults(int driver_index)
{
	return game_opts.rom(driver_index);
}

void SetRomAuditResults(int driver_index, int audit_results)
{
	game_opts.rom(driver_index, audit_results);
}

int  GetSampleAuditResults(int driver_index)
{
	return game_opts.sample(driver_index);
}

void SetSampleAuditResults(int driver_index, int audit_results)
{
	game_opts.sample(driver_index, audit_results);
}

//static void IncrementPlayVariable(int driver_index, const char *play_variable, int increment)
//{
//	char buffer[256];
//	int count;

//	GetDriverOptionName(driver_index, play_variable, buffer, ARRAY_LENGTH(buffer));
//	count = settings.int_value(buffer);
//	std::string error_string;
//	settings.set_value(buffer, count + increment, OPTION_PRIORITY_CMDLINE,error_string);
//}

static void IncrementPlayVariable(int driver_index, const char *play_variable, int increment)
{
	int count;

	if (strcmp(play_variable, "count") == 0)
	{
		count = game_opts.play_count(driver_index);
		game_opts.play_count(driver_index, count + increment);
	}
	else if (strcmp(play_variable, "time") == 0)
	{
		count = game_opts.play_time(driver_index);
		game_opts.play_time(driver_index, count + increment);
	}
}

void IncrementPlayCount(int driver_index)
{
	IncrementPlayVariable(driver_index, "count", 1);
}

int GetPlayCount(int driver_index)
{
	return game_opts.play_count(driver_index);
}

//int GetPlayCount(int driver_index)
//{
//	char buffer[80];
//	GetDriverOptionName(driver_index, "play_count", buffer, ARRAY_LENGTH(buffer));
//	return settings.int_value(buffer);
//}

//static void ResetPlayVariable(int driver_index, const char *play_variable)
//{
//	int i;
//	if (driver_index < 0)
//	{
//		/* all games */
//		for (i = 0; i< driver_list::total(); i++)
//			ResetPlayVariable(i, play_variable);
//	}
//	else
//	{
//		char buffer[80];
//		GetDriverOptionName(driver_index, play_variable, buffer, ARRAY_LENGTH(buffer));
//		std::string error_string;
//		settings.set_value(buffer, 0, OPTION_PRIORITY_CMDLINE,error_string);
//	}
//}

static void ResetPlayVariable(int driver_index, const char *play_variable)
{
	int i;
	if (driver_index < 0)
	{
		/* all games */
		for (i = 0; i< driver_list::total(); i++)
		{
			/* 20070808 MSH - Was passing in driver_index instead of i. Doh! */
			ResetPlayVariable(i, play_variable);
		}
	}
	else
	{
		if (strcmp(play_variable, "count") == 0)
			game_opts.play_count(driver_index, 0);
		else if (strcmp(play_variable, "time") == 0)
			game_opts.play_time(driver_index, 0);
	}
}

void ResetPlayCount(int driver_index)
{
	ResetPlayVariable(driver_index, "count");
}

void ResetPlayTime(int driver_index)
{
	ResetPlayVariable(driver_index, "time");
}

int GetPlayTime(int driver_index)
{
	return game_opts.play_time(driver_index);
//	char buffer[80];
//	GetDriverOptionName(driver_index, "play_time", buffer, ARRAY_LENGTH(buffer));
//	return settings.int_value(buffer);
}

void IncrementPlayTime(int driver_index,int playtime)
{
	IncrementPlayVariable(driver_index, "time", playtime);
}

void GetTextPlayTime(int driver_index,char *buf)
{
	int hour, minute, second;
	int temp = GetPlayTime(driver_index);

	assert(0 <= driver_index && driver_index < driver_list::total());

	hour = temp / 3600;
	temp = temp - 3600*hour;
	minute = temp / 60; //Calc Minutes
	second = temp - 60*minute;

	if (hour == 0)
		sprintf(buf, "%d:%02d", minute, second );
	else
		sprintf(buf, "%d:%02d:%02d", hour, minute, second );
}

input_seq* Get_ui_key_up(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_UP);
}

input_seq* Get_ui_key_down(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_DOWN);
}

input_seq* Get_ui_key_left(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_LEFT);
}

input_seq* Get_ui_key_right(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_RIGHT);
}

input_seq* Get_ui_key_start(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_START);
}

input_seq* Get_ui_key_pgup(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_PGUP);
}

input_seq* Get_ui_key_pgdwn(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_PGDWN);
}

input_seq* Get_ui_key_home(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_HOME);
}

input_seq* Get_ui_key_end(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_END);
}

input_seq* Get_ui_key_ss_change(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_SS_CHANGE);
}

input_seq* Get_ui_key_history_up(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_HISTORY_UP);
}

input_seq* Get_ui_key_history_down(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_HISTORY_DOWN);
}

input_seq* Get_ui_key_context_filters(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_CONTEXT_FILTERS);
}

input_seq* Get_ui_key_select_random(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_SELECT_RANDOM);
}

input_seq* Get_ui_key_game_audit(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_GAME_AUDIT);
}

input_seq* Get_ui_key_game_properties(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_GAME_PROPERTIES);
}

input_seq* Get_ui_key_help_contents(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_HELP_CONTENTS);
}

input_seq* Get_ui_key_update_gamelist(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_UPDATE_GAMELIST);
}

input_seq* Get_ui_key_view_folders(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_VIEW_FOLDERS);
}

input_seq* Get_ui_key_view_fullscreen(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_VIEW_FULLSCREEN);
}

input_seq* Get_ui_key_view_pagetab(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_VIEW_PAGETAB);
}

input_seq* Get_ui_key_view_picture_area(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_VIEW_PICTURE_AREA);
}

input_seq* Get_ui_key_view_software_area(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_VIEW_SOFTWARE_AREA);
}

input_seq* Get_ui_key_view_status(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_VIEW_STATUS);
}

input_seq* Get_ui_key_view_toolbars(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_VIEW_TOOLBARS);
}

input_seq* Get_ui_key_view_tab_cabinet(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_VIEW_TAB_CABINET);
}

input_seq* Get_ui_key_view_tab_cpanel(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_VIEW_TAB_CPANEL);
}

input_seq* Get_ui_key_view_tab_flyer(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_VIEW_TAB_FLYER);
}

input_seq* Get_ui_key_view_tab_history(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_VIEW_TAB_HISTORY);
}

input_seq* Get_ui_key_view_tab_marquee(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_VIEW_TAB_MARQUEE);
}

input_seq* Get_ui_key_view_tab_screenshot(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_VIEW_TAB_SCREENSHOT);
}

input_seq* Get_ui_key_view_tab_title(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_VIEW_TAB_TITLE);
}

input_seq* Get_ui_key_view_tab_pcb(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_VIEW_TAB_PCB);
}

input_seq* Get_ui_key_quit(void)
{
	return options_get_input_seq(settings, MUIOPTION_UI_KEY_QUIT);
}

static int GetUIJoy(const char *option_name, int joycodeIndex)
{
	const char *joycodes_string;
	int joycodes[4];

	assert(0 <= joycodeIndex && joycodeIndex < 4);
	joycodes_string = settings.value( option_name);
	ColumnDecodeStringWithCount(joycodes_string, joycodes, ARRAY_LENGTH(joycodes));
	return joycodes[joycodeIndex];
}

static void SetUIJoy(const char *option_name, int joycodeIndex, int val)
{
	const char *joycodes_string;
	int joycodes[4];
	char buffer[1024];

	assert(0 <= joycodeIndex && joycodeIndex < 4);
	joycodes_string = settings.value( option_name);
	ColumnDecodeStringWithCount(joycodes_string, joycodes, ARRAY_LENGTH(joycodes));

	joycodes[joycodeIndex] = val;
	ColumnEncodeStringWithCount(joycodes, buffer, ARRAY_LENGTH(joycodes));
	std::string error_string;
	settings.set_value(option_name, buffer, OPTION_PRIORITY_CMDLINE,error_string);
}

int GetUIJoyUp(int joycodeIndex)
{
	return GetUIJoy(MUIOPTION_UI_JOY_UP, joycodeIndex);
}

void SetUIJoyUp(int joycodeIndex, int val)
{
	SetUIJoy(MUIOPTION_UI_JOY_UP, joycodeIndex, val);
}

int GetUIJoyDown(int joycodeIndex)
{
	return GetUIJoy(MUIOPTION_UI_JOY_DOWN, joycodeIndex);
}

void SetUIJoyDown(int joycodeIndex, int val)
{
	SetUIJoy(MUIOPTION_UI_JOY_DOWN, joycodeIndex, val);
}

int GetUIJoyLeft(int joycodeIndex)
{
	return GetUIJoy(MUIOPTION_UI_JOY_LEFT, joycodeIndex);
}

void SetUIJoyLeft(int joycodeIndex, int val)
{
	SetUIJoy(MUIOPTION_UI_JOY_LEFT, joycodeIndex, val);
}

int GetUIJoyRight(int joycodeIndex)
{
	return GetUIJoy(MUIOPTION_UI_JOY_RIGHT, joycodeIndex);
}

void SetUIJoyRight(int joycodeIndex, int val)
{
	SetUIJoy(MUIOPTION_UI_JOY_RIGHT, joycodeIndex, val);
}

int GetUIJoyStart(int joycodeIndex)
{
	return GetUIJoy(MUIOPTION_UI_JOY_START, joycodeIndex);
}

void SetUIJoyStart(int joycodeIndex, int val)
{
	SetUIJoy(MUIOPTION_UI_JOY_START, joycodeIndex, val);
}

int GetUIJoyPageUp(int joycodeIndex)
{
	return GetUIJoy(MUIOPTION_UI_JOY_PGUP, joycodeIndex);
}

void SetUIJoyPageUp(int joycodeIndex, int val)
{
	SetUIJoy(MUIOPTION_UI_JOY_PGUP, joycodeIndex, val);
}

int GetUIJoyPageDown(int joycodeIndex)
{
	return GetUIJoy(MUIOPTION_UI_JOY_PGDWN, joycodeIndex);
}

void SetUIJoyPageDown(int joycodeIndex, int val)
{
	SetUIJoy(MUIOPTION_UI_JOY_PGDWN, joycodeIndex, val);
}

int GetUIJoyHome(int joycodeIndex)
{
	return GetUIJoy(MUIOPTION_UI_JOY_HOME, joycodeIndex);
}

void SetUIJoyHome(int joycodeIndex, int val)
{
	SetUIJoy(MUIOPTION_UI_JOY_HOME, joycodeIndex, val);
}

int GetUIJoyEnd(int joycodeIndex)
{
	return GetUIJoy(MUIOPTION_UI_JOY_END, joycodeIndex);
}

void SetUIJoyEnd(int joycodeIndex, int val)
{
	SetUIJoy(MUIOPTION_UI_JOY_END, joycodeIndex, val);
}

int GetUIJoySSChange(int joycodeIndex)
{
	return GetUIJoy(MUIOPTION_UI_JOY_SS_CHANGE, joycodeIndex);
}

void SetUIJoySSChange(int joycodeIndex, int val)
{
	SetUIJoy(MUIOPTION_UI_JOY_SS_CHANGE, joycodeIndex, val);
}

int GetUIJoyHistoryUp(int joycodeIndex)
{
	return GetUIJoy(MUIOPTION_UI_JOY_HISTORY_UP, joycodeIndex);
}

void SetUIJoyHistoryUp(int joycodeIndex, int val)
{
	SetUIJoy(MUIOPTION_UI_JOY_HISTORY_UP, joycodeIndex, val);
}

int GetUIJoyHistoryDown(int joycodeIndex)
{
	return GetUIJoy(MUIOPTION_UI_JOY_HISTORY_DOWN, joycodeIndex);
}

void SetUIJoyHistoryDown(int joycodeIndex, int val)
{
	SetUIJoy(MUIOPTION_UI_JOY_HISTORY_DOWN, joycodeIndex, val);
}

void SetUIJoyExec(int joycodeIndex, int val)
{
	SetUIJoy(MUIOPTION_UI_JOY_EXEC, joycodeIndex, val);
}

int GetUIJoyExec(int joycodeIndex)
{
	return GetUIJoy(MUIOPTION_UI_JOY_EXEC, joycodeIndex);
}

const char * GetExecCommand(void)
{
	return settings.value( MUIOPTION_EXEC_COMMAND);
}

void SetExecCommand(char *cmd)
{
	std::string error_string;
	settings.set_value(MUIOPTION_EXEC_COMMAND, cmd, OPTION_PRIORITY_CMDLINE,error_string);
}

int GetExecWait(void)
{
	return settings.int_value(MUIOPTION_EXEC_WAIT);
}

void SetExecWait(int wait)
{
	std::string error_string;
	settings.set_value(MUIOPTION_EXEC_WAIT, wait, OPTION_PRIORITY_CMDLINE,error_string);
}

BOOL GetHideMouseOnStartup(void)
{
	return settings.bool_value( MUIOPTION_HIDE_MOUSE);
}

void SetHideMouseOnStartup(BOOL hide)
{
	std::string error_string;
	settings.set_value(MUIOPTION_HIDE_MOUSE, hide, OPTION_PRIORITY_CMDLINE,error_string);
}

BOOL GetRunFullScreen(void)
{
	return settings.bool_value( MUIOPTION_FULL_SCREEN);
}

void SetRunFullScreen(BOOL fullScreen)
{
	std::string error_string;
	settings.set_value(MUIOPTION_FULL_SCREEN, fullScreen, OPTION_PRIORITY_CMDLINE,error_string);
}

/***************************************************************************
    Internal functions
 ***************************************************************************/

static void  CusColorEncodeString(const COLORREF *value, char* str)
{
	int  i;
	char tmpStr[256];

	sprintf(tmpStr, "%u", (int) value[0]);

	strcpy(str, tmpStr);

	for (i = 1; i < 16; i++)
	{
		sprintf(tmpStr, ",%u", (unsigned) value[i]);
		strcat(str, tmpStr);
	}
}

static void CusColorDecodeString(const char* str, COLORREF *value)
{
	int  i;
	char *s, *p;
	char tmpStr[256];

	strcpy(tmpStr, str);
	p = tmpStr;

	for (i = 0; p && i < 16; i++)
	{
		s = p;

		if ((p = strchr(s,',')) != NULL && *p == ',')
		{
			*p = '\0';
			p++;
		}
		value[i] = atoi(s);
	}
}


void ColumnEncodeStringWithCount(const int *value, char *str, int count)
{
	int  i;
	char buffer[256];

	snprintf(buffer,sizeof(buffer),"%d",value[0]);

	strcpy(str,buffer);

	for (i = 1; i < count; i++)
	{
		snprintf(buffer,sizeof(buffer),",%d",value[i]);
		strcat(str,buffer);
	}
}

void ColumnDecodeStringWithCount(const char* str, int *value, int count)
{
	int  i;
	char *s, *p;
	char tmpStr[256];

	if (str == NULL)
		return;

	strcpy(tmpStr, str);
	p = tmpStr;

	for (i = 0; p && i < count; i++)
	{
		s = p;

		if ((p = strchr(s,',')) != NULL && *p == ',')
		{
			*p = '\0';
			p++;
		}
		value[i] = atoi(s);
	}
}

static void SplitterEncodeString(const int *value, char* str)
{
	int  i;
	char tmpStr[256];

	sprintf(tmpStr, "%d", value[0]);

	strcpy(str, tmpStr);

	for (i = 1; i < GetSplitterCount(); i++)
	{
		sprintf(tmpStr, ",%d", value[i]);
		strcat(str, tmpStr);
	}
}

static void SplitterDecodeString(const char *str, int *value)
{
	int  i;
	char *s, *p;
	char tmpStr[256];

	strcpy(tmpStr, str);
	p = tmpStr;

	for (i = 0; p && i < GetSplitterCount(); i++)
	{
		s = p;

		if ((p = strchr(s,',')) != NULL && *p == ',')
		{
			*p = '\0';
			p++;
		}
		value[i] = atoi(s);
	}
}

/* Parse the given comma-delimited string into a LOGFONT structure */
static void FontDecodeString(const char* str, LOGFONT *f)
{
	const char*	ptr;
	TCHAR*		t_ptr;

	sscanf(str, "%li,%li,%li,%li,%li,%i,%i,%i,%i,%i,%i,%i,%i",
		   &f->lfHeight,
		   &f->lfWidth,
		   &f->lfEscapement,
		   &f->lfOrientation,
		   &f->lfWeight,
		   (int*)&f->lfItalic,
		   (int*)&f->lfUnderline,
		   (int*)&f->lfStrikeOut,
		   (int*)&f->lfCharSet,
		   (int*)&f->lfOutPrecision,
		   (int*)&f->lfClipPrecision,
		   (int*)&f->lfQuality,
		   (int*)&f->lfPitchAndFamily);
	ptr = strrchr(str, ',');
	if (ptr != NULL) {
		t_ptr = tstring_from_utf8(ptr + 1);
		if( !t_ptr )
			return;
		_tcscpy(f->lfFaceName, t_ptr);
		osd_free(t_ptr);
	}
}

/* Encode the given LOGFONT structure into a comma-delimited string */
static void FontEncodeString(const LOGFONT *f, char *str)
{
	char* utf8_FaceName = utf8_from_tstring(f->lfFaceName);
	if( !utf8_FaceName )
		return;

	sprintf(str, "%li,%li,%li,%li,%li,%i,%i,%i,%i,%i,%i,%i,%i,%s",
			f->lfHeight,
			f->lfWidth,
			f->lfEscapement,
			f->lfOrientation,
			f->lfWeight,
			f->lfItalic,
			f->lfUnderline,
			f->lfStrikeOut,
			f->lfCharSet,
			f->lfOutPrecision,
			f->lfClipPrecision,
			f->lfQuality,
			f->lfPitchAndFamily,
			utf8_FaceName);

	osd_free(utf8_FaceName);
}

static void TabFlagsEncodeString(int data, char *str)
{
	int num_saved = 0;

	strcpy(str,"");

	// we save the ones that are NOT displayed, so we can add new ones
	// and upgraders will see them
	for ( int i=0;i<MAX_TAB_TYPES;i++)
	{
		if (((data & (1 << i)) == 0) && GetImageTabShortName(i))
		{
			if (num_saved != 0)
				strcat(str, ", ");

			strcat(str,GetImageTabShortName(i));
			num_saved++;
		}
	}
}

static void TabFlagsDecodeString(const char *str, int *data)
{
	char s[2000];
	char *token;

	snprintf(s, ARRAY_LENGTH(s), "%s", str);

	// simple way to set all tab bits "on"
	*data = (1 << MAX_TAB_TYPES) - 1;

	token = strtok(s,", \t");
	while (token != NULL)
	{
		int j;

		for (j=0;j<MAX_TAB_TYPES;j++)
		{
			if (!GetImageTabShortName(j) || (strcmp(GetImageTabShortName(j), token) == 0))
			{
				// turn off this bit
				*data &= ~(1 << j);
				break;
			}
		}
		token = strtok(NULL,", \t");
	}

	if (*data == 0)
	{
		// not allowed to hide all tabs, because then why even show the area?
		*data = (1 << TAB_SCREENSHOT);
	}
}

static void LoadSettingsFile(winui_options &opts, const char *filename)
{
	file_error filerr;
	core_file *file;

	filerr = core_fopen(filename, OPEN_FLAG_READ, &file);
	if (filerr == FILERR_NONE)
	{
		std::string error_string;
		opts.parse_ini_file(*file, OPTION_PRIORITY_CMDLINE, OPTION_PRIORITY_CMDLINE, error_string);
		core_fclose(file);
	}
}

static void LoadSettingsFile(windows_options &opts, const char *filename)
{
	file_error filerr;
	core_file *file;

	filerr = core_fopen(filename, OPEN_FLAG_READ, &file);
	if (filerr == FILERR_NONE)
	{
		std::string error_string;
		opts.parse_ini_file(*file, OPTION_PRIORITY_CMDLINE, OPTION_PRIORITY_CMDLINE, error_string);
		core_fclose(file);
	}
}

// This saves changes to MESSUI.INI only
static void SaveSettingsFile(winui_options &opts, const char *filename)
{
	file_error filerr = FILERR_NONE;
	core_file *file;

	filerr = core_fopen(filename, OPEN_FLAG_WRITE | OPEN_FLAG_CREATE | OPEN_FLAG_CREATE_PATHS, &file);

	if (filerr == FILERR_NONE)
	{
		std::string inistring;
		opts.output_ini(inistring);
		core_fputs(file,inistring.c_str());
		core_fclose(file);
	}
}



// This saves changes to <game>.INI or MESS.INI only
static void SaveSettingsFile(windows_options &opts, const char *filename)
{
	file_error filerr = FILERR_NONE;
	core_file *file;

	filerr = core_fopen(filename, OPEN_FLAG_WRITE | OPEN_FLAG_CREATE | OPEN_FLAG_CREATE_PATHS, &file);

	if (filerr == FILERR_NONE)
	{
		std::string inistring;
		opts.output_ini(inistring);
		// printf("=====%s=====\n%s\n",filename,inistring.c_str());  // for debugging
		core_fputs(file,inistring.c_str());
		core_fclose(file);
	}
}



/* Register access functions below */
static void LoadOptionsAndSettings(void)
{
//	char buffer[MAX_PATH];

	// parse MESSUI.ini - always in the current directory.
	LoadSettingsFile(settings, UI_INI_FILENAME);

	// parse MESS_g.ini - game options.
	game_opts.load_file(GAMEINFO_INI_FILENAME);

	// parse global options ini/mess.ini, if it doesn't exist then get ./mess.ini instead
	load_options(global, GLOBAL_OPTIONS);
}

void SetDirectories(windows_options &opts)
{
	std::string error_string;
	opts.set_value(OPTION_MEDIAPATH, GetRomDirs(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OPTION_SAMPLEPATH, GetSampleDirs(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OPTION_INIPATH, GetIniDir(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OPTION_CFG_DIRECTORY, GetCfgDir(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OPTION_SNAPSHOT_DIRECTORY, GetImgDir(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OPTION_INPUT_DIRECTORY, GetInpDir(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OPTION_STATE_DIRECTORY, GetStateDir(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OPTION_ARTPATH, GetArtDir(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OPTION_NVRAM_DIRECTORY, GetNvramDir(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OPTION_CTRLRPATH, GetCtrlrDir(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OPTION_CHEATPATH, GetCheatDir(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OPTION_CROSSHAIRPATH, GetCrosshairDir(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OPTION_FONTPATH, GetFontDir(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OPTION_DIFF_DIRECTORY, GetDiffDir(), OPTION_PRIORITY_CMDLINE, error_string);
	opts.set_value(OPTION_SNAPNAME, GetSnapName(), OPTION_PRIORITY_CMDLINE, error_string);
}

const char * GetFolderNameByID(UINT nID)
{
	UINT i;
	extern const FOLDERDATA g_folderData[];
	extern LPEXFOLDERDATA ExtraFolderData[];

	for (i = 0; i < MAX_EXTRA_FOLDERS * MAX_EXTRA_SUBFOLDERS; i++)
		if( ExtraFolderData[i] )
			if (ExtraFolderData[i]->m_nFolderId == nID)
				return ExtraFolderData[i]->m_szTitle;

	for( i = 0; i < MAX_FOLDERS; i++)
		if (g_folderData[i].m_nFolderId == nID)
			return g_folderData[i].m_lpTitle;

	return NULL;
}


DWORD GetFolderFlags(int folder_index)
{
	LPTREEFOLDER lpFolder = GetFolder(folder_index);

	if (lpFolder)
		return lpFolder->m_dwFlags & F_MASK;

	return 0;
}


/* Decode the flags into a DWORD */
static DWORD DecodeFolderFlags(const char *buf)
{
	DWORD flags = 0;
	int shift = 0;
	const char *ptr = buf;

	while (*ptr && (1 << shift) & F_MASK)
	{
		if (*ptr++ == '1')
		{
			flags |= (1 << shift);
		}
		shift++;
	}
	return flags;
}

/* Encode the flags into a string */
static const char * EncodeFolderFlags(DWORD value)
{
	static char buf[40];
	int shift = 0;

	memset(buf,'\0', sizeof(buf));

	while ((1 << shift) & F_MASK) {
		buf[shift] = (value & (1 << shift)) ? '1' : '0';
		shift++;
	}

	return buf;
}

/* MSH 20080813
 * Read the folder filters from MESSui.ini.  This must only
 * be called AFTER the folders have all been created.
 */
void LoadFolderFlags(void)
{
	winui_options opts;
	int numFolders;
	LPTREEFOLDER lpFolder;
	int i;
	options_entry entries[2] = { { 0 }, { 0 } };

	memcpy(entries, filterOptions, sizeof(filterOptions));

	numFolders = GetNumFolders();

	for (i = 0; i < numFolders; i++)
	{
		lpFolder = GetFolder(i);

		if (lpFolder)
		{
			char folder_name[256];
			char *ptr;

			// Convert spaces to underscores
			strcpy(folder_name, lpFolder->m_lpTitle);
			ptr = folder_name;
			while (*ptr && *ptr != '\0')
			{
				if (*ptr == ' ')
				{
					*ptr = '_';
				}
				ptr++;
			}

			std::string option_name = std::string(folder_name) + "_filters";
			// create entry
			entries[0].name = option_name.c_str();
			opts.add_entries(entries);
		}
	}

	// These are overlaid at the end of our UI ini
	// The normal read will skip them.
	LoadSettingsFile(opts, UI_INI_FILENAME);

	// retrieve the stored values
	for (i = 0; i < numFolders; i++)
	{
		lpFolder = GetFolder(i);

		if (lpFolder)
		{
			char folder_name[256];
			char *ptr;
			const char *value;

			// Convert spaces to underscores
			strcpy(folder_name, lpFolder->m_lpTitle);
			ptr = folder_name;
			while (*ptr && *ptr != '\0')
			{
				if (*ptr == ' ')
				{
					*ptr = '_';
				}
				ptr++;
			}
			std::string option_name = std::string(folder_name) + "_filters";
			// get entry and decode it
			value = opts.value(option_name.c_str());

			if (value)
			{
				lpFolder->m_dwFlags |= DecodeFolderFlags(value) & F_MASK;
			}
		}
	}
}



// Adds our folder flags to a temporary winui_options, for saving.
static void AddFolderFlags(winui_options &opts)
{
	int numFolders;
	int i;
	LPTREEFOLDER lpFolder;
	int num_entries = 0;
	options_entry entries[2] = { { 0 }, { 0 } };

	entries[0].name = NULL;
	entries[0].defvalue = NULL;
	entries[0].flags = OPTION_HEADER;
	entries[0].description = "FOLDER FILTERS";
	opts.add_entries(entries);

	memcpy(entries, filterOptions, sizeof(filterOptions));

	numFolders = GetNumFolders();

	for (i = 0; i < numFolders; i++)
	{
		lpFolder = GetFolder(i);
		if (lpFolder && (lpFolder->m_dwFlags & F_MASK) != 0)
		{
			char folder_name[256];
			char *ptr;

			// Convert spaces to underscores
			strcpy(folder_name, lpFolder->m_lpTitle);
			ptr = folder_name;
			while (*ptr && *ptr != '\0')
			{
				if (*ptr == ' ')
				{
					*ptr = '_';
				}
				ptr++;
			}

			std::string option_name = std::string(folder_name) + "_filters";

			// create entry
			entries[0].name = option_name.c_str();
			opts.add_entries(entries);

			// store entry
			std::string error_string;
			opts.set_value(option_name.c_str(), EncodeFolderFlags(lpFolder->m_dwFlags), OPTION_PRIORITY_CMDLINE,error_string);

			// increment counter
			num_entries++;
		}
	}
}

// Save MESSUI.ini
void SaveOptions(void)
{
	// Add the folder flag to settings.
	AddFolderFlags(settings);
	// Save opts if it is non-null, else save settings.
	// It will be null if there are no filters set.
	SaveSettingsFile(settings, UI_INI_FILENAME);
}

void SaveGameListOptions(void)
{
	// Save GameInfo.ini - game options.
	game_opts.save_file(GAMEINFO_INI_FILENAME);
}

// change ini\mess.ini back to default (why?)
void SaveDefaultOptions(void)
{
	std::string fname = std::string(GetIniDir()) + PATH_SEPARATOR + std::string(emulator_info::get_configname()).append(".ini");
	SaveSettingsFile(global, fname.c_str());
}

const char * GetVersionString(void)
{
	return build_version;
}

// NOT USED
/*
static BOOL IsGlobalOption(const char *option_name)
{
	static const char *global_options[] =
	{
		OPTION_ROMPATH,
		OPTION_HASHPATH,
		OPTION_SAMPLEPATH,
		OPTION_ARTPATH,
		OPTION_CTRLRPATH,
		OPTION_INIPATH,
		OPTION_FONTPATH,
		OPTION_CFG_DIRECTORY,
		OPTION_NVRAM_DIRECTORY,
		OPTION_INPUT_DIRECTORY,
		OPTION_STATE_DIRECTORY,
		OPTION_SNAPSHOT_DIRECTORY,
		OPTION_DIFF_DIRECTORY,
		OPTION_COMMENT_DIRECTORY
	};

	char command_name[128];
	char *s;

	// determine command name
	snprintf(command_name, ARRAY_LENGTH(command_name), "%s", option_name);
	s = strchr(command_name, ';');
	if (s)
		*s = '\0';

	for (int i = 0; i < ARRAY_LENGTH(global_options); i++)
	{
		if (!strcmp(command_name, global_options[i]))
			return TRUE;
	}
	return FALSE;
}
*/


/*  get options, based on passed in game number. */
void load_options(windows_options &opts, int game_num)
{
	const game_driver *driver = NULL;

	// Try base ini first
	std::string fname = std::string(emulator_info::get_configname()).append(".ini");
	LoadSettingsFile(opts, fname.c_str());

	if (game_num > -2)
	{
		// Now try global ini
		fname = std::string(GetIniDir()) + PATH_SEPARATOR + std::string(emulator_info::get_configname()).append(".ini");
		LoadSettingsFile(opts, fname.c_str());

		if (game_num > -1)
		{
			// Lastly, gamename.ini
			driver = &driver_list::driver(game_num);
			if (driver != NULL)
			{
				fname = std::string(GetIniDir()) + PATH_SEPARATOR + std::string(driver->name).append(".ini");
				LoadSettingsFile(opts, fname.c_str());
			}
		}
	}
	CreateGameOptions(opts, game_num);
	SetDirectories(opts);
}

/* Save ini file based on game_number. */
void save_options(windows_options &opts, int game_num)
{
	const game_driver *driver = NULL;
	std::string filename, filepath;

	if (game_num >= 0)
	{
		driver = &driver_list::driver(game_num);
		if (driver != NULL)
			filename.assign(driver->name);
	}
	else
	if (game_num == -1)
		filename = std::string(emulator_info::get_configname());

	if (!filename.empty())
		filepath = std::string(GetIniDir()).append(PATH_SEPARATOR).append(filename.c_str()).append(".ini");

	if (game_num == -2)
		filepath = std::string(emulator_info::get_configname()).append(".ini");

	if (!filepath.empty())
	{
		SetDirectories(opts);
		SaveSettingsFile(opts, filepath.c_str());
		//printf("Settings saved to %s\n",filepath.c_str());
	}
	else
		printf("Unable to save settings\n");
}


// Reset the given windows_options to their default settings.
static void ResetToDefaults(windows_options &opts, int priority)
{
	// iterate through the options setting each one back to the default value.
	opts.revert(priority);
}

int GetDriverCache(int driver_index)
{
	return game_opts.cache(driver_index);
}

void SetDriverCache(int driver_index, int val)
{
	game_opts.cache(driver_index, val);
}

BOOL RequiredDriverCache(void)
{
	bool ret = false;

	if ( strcmp(settings.value(MUIOPTION_VERSION), GetVersionString()) != 0 )
		ret = true; // full recache needed

	std::string error_string;
	settings.set_value(MUIOPTION_VERSION, GetVersionString(), OPTION_PRIORITY_CMDLINE,error_string);

	return ret;
}



