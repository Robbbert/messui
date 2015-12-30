// For licensing and usage information, read docs/winui_license.txt
//****************************************************************************
#define WIN32_LEAN_AND_MEAN
#include <assert.h>
#include <string.h>
#include <windows.h>
#include <shellapi.h>
#include <commctrl.h>
#include <commdlg.h>
#include <wingdi.h>
#include <winuser.h>

#include "emu.h"
#include "mui_util.h"
#include "mui_opts.h"
#include "optionsms.h"
#include "emuopts.h"
#include "winui.h"
#include "drivenum.h"

#define MESSUI_SWLIST_COLUMN_SHOWN		"swlist_column_shown"
#define MESSUI_SWLIST_COLUMN_WIDTHS		"swlist_column_widths"
#define MESSUI_SWLIST_COLUMN_ORDER		"swlist_column_order"
#define MESSUI_SWLIST_SORT_REVERSED		"swlist_sort_reversed"
#define MESSUI_SWLIST_SORT_COLUMN		"swlist_sort_column"
#define MESSUI_SOFTWARE_COLUMN_SHOWN		"mess_column_shown"
#define MESSUI_SOFTWARE_COLUMN_WIDTHS		"mess_column_widths"
#define MESSUI_SOFTWARE_COLUMN_ORDER		"mess_column_order"
#define MESSUI_SOFTWARE_SORT_REVERSED		"mess_sort_reversed"
#define MESSUI_SOFTWARE_SORT_COLUMN		"mess_sort_column"
#define MESSUI_SOFTWARE_TAB			"current_software_tab"
#define MESSUI_SOFTWAREPATH			"softwarepath"

#define LOG_SOFTWARE 0

static const options_entry mess_wingui_settings[] =
{
	{ MESSUI_SWLIST_COLUMN_WIDTHS,    "100,75,223,46,120,120", OPTION_STRING, NULL },
	{ MESSUI_SWLIST_COLUMN_ORDER,     "0,1,2,3,4,5", OPTION_STRING, NULL }, // order of columns
	{ MESSUI_SWLIST_COLUMN_SHOWN,     "1,1,1,1,1,1", OPTION_STRING, NULL }, // 0=hide,1=show
	{ MESSUI_SWLIST_SORT_COLUMN,      "0", OPTION_INTEGER, NULL },
	{ MESSUI_SWLIST_SORT_REVERSED,    "0", OPTION_BOOLEAN, NULL },
	{ MESSUI_SOFTWARE_COLUMN_WIDTHS,  "400", OPTION_STRING, NULL },
	{ MESSUI_SOFTWARE_COLUMN_ORDER,   "0", OPTION_STRING, NULL }, // 1= dummy column
	{ MESSUI_SOFTWARE_COLUMN_SHOWN,   "1", OPTION_STRING, NULL }, // 0=don't show it
	{ MESSUI_SOFTWARE_SORT_COLUMN,    "0", OPTION_INTEGER, NULL },
	{ MESSUI_SOFTWARE_SORT_REVERSED,  "0", OPTION_BOOLEAN, NULL },
	{ MESSUI_SOFTWARE_TAB,            "0", OPTION_STRING, NULL },
	{ MESSUI_SOFTWAREPATH,            "software", OPTION_STRING, NULL },
	{ NULL }
};

void MessSetupSettings(winui_options &settings)
{
	settings.add_entries(mess_wingui_settings);
}

void MessSetupGameOptions(windows_options &opts, int driver_index)
{
	if (driver_index >= 0)
		opts.set_system_name(driver_list::driver(driver_index).name);
}

void SetSWListColumnOrder(int order[])
{
	char column_order_string[80];
	ColumnEncodeStringWithCount(order, column_order_string, SWLIST_COLUMN_MAX);
	std::string error_string;
	MameUISettings().set_value(MESSUI_SWLIST_COLUMN_ORDER, column_order_string, OPTION_PRIORITY_CMDLINE,error_string);
}

void GetSWListColumnOrder(int order[])
{
	const char *column_order_string;
	column_order_string = MameUISettings().value(MESSUI_SWLIST_COLUMN_ORDER);
	ColumnDecodeStringWithCount(column_order_string, order, SWLIST_COLUMN_MAX);
}

void SetSWListColumnShown(int shown[])
{
	char column_shown_string[80];
	ColumnEncodeStringWithCount(shown, column_shown_string, SWLIST_COLUMN_MAX);
	std::string error_string;
	MameUISettings().set_value(MESSUI_SWLIST_COLUMN_SHOWN, column_shown_string, OPTION_PRIORITY_CMDLINE,error_string);
}

void GetSWListColumnShown(int shown[])
{
	const char *column_shown_string;
	column_shown_string = MameUISettings().value(MESSUI_SWLIST_COLUMN_SHOWN);
	ColumnDecodeStringWithCount(column_shown_string, shown, SWLIST_COLUMN_MAX);
}

void SetSWListColumnWidths(int width[])
{
	char column_width_string[80];
	ColumnEncodeStringWithCount(width, column_width_string, SWLIST_COLUMN_MAX);
	std::string error_string;
	MameUISettings().set_value(MESSUI_SWLIST_COLUMN_WIDTHS, column_width_string, OPTION_PRIORITY_CMDLINE,error_string);
}

void GetSWListColumnWidths(int width[])
{
	const char *column_width_string;
	column_width_string = MameUISettings().value(MESSUI_SWLIST_COLUMN_WIDTHS);
	ColumnDecodeStringWithCount(column_width_string, width, SWLIST_COLUMN_MAX);
}

void SetSWListSortColumn(int column)
{
	std::string error_string;
	MameUISettings().set_value(MESSUI_SWLIST_SORT_COLUMN, column, OPTION_PRIORITY_CMDLINE,error_string);
}

int GetSWListSortColumn(void)
{
	return MameUISettings().int_value(MESSUI_SWLIST_SORT_COLUMN);
}

void SetSWListSortReverse(BOOL reverse)
{
	std::string error_string;
	MameUISettings().set_value( MESSUI_SWLIST_SORT_REVERSED, reverse, OPTION_PRIORITY_CMDLINE,error_string);
}

BOOL GetSWListSortReverse(void)
{
	return MameUISettings().bool_value(MESSUI_SWLIST_SORT_REVERSED);
}

void SetMessColumnOrder(int order[])
{
	char column_order_string[80];
	ColumnEncodeStringWithCount(order, column_order_string, MESS_COLUMN_MAX);
	std::string error_string;
	MameUISettings().set_value(MESSUI_SOFTWARE_COLUMN_ORDER, column_order_string, OPTION_PRIORITY_CMDLINE,error_string);
}

void GetMessColumnOrder(int order[])
{
	const char *column_order_string;
	column_order_string = MameUISettings().value(MESSUI_SOFTWARE_COLUMN_ORDER);
	ColumnDecodeStringWithCount(column_order_string, order, MESS_COLUMN_MAX);
}

void SetMessColumnShown(int shown[])
{
	char column_shown_string[80];
	ColumnEncodeStringWithCount(shown, column_shown_string, MESS_COLUMN_MAX);
	std::string error_string;
	MameUISettings().set_value(MESSUI_SOFTWARE_COLUMN_SHOWN, column_shown_string, OPTION_PRIORITY_CMDLINE,error_string);
}

void GetMessColumnShown(int shown[])
{
	const char *column_shown_string;
	column_shown_string = MameUISettings().value(MESSUI_SOFTWARE_COLUMN_SHOWN);
	ColumnDecodeStringWithCount(column_shown_string, shown, MESS_COLUMN_MAX);
}

void SetMessColumnWidths(int width[])
{
	char column_width_string[80];
	ColumnEncodeStringWithCount(width, column_width_string, MESS_COLUMN_MAX);
	std::string error_string;
	MameUISettings().set_value(MESSUI_SOFTWARE_COLUMN_WIDTHS, column_width_string, OPTION_PRIORITY_CMDLINE,error_string);
}

void GetMessColumnWidths(int width[])
{
	const char *column_width_string;
	column_width_string = MameUISettings().value(MESSUI_SOFTWARE_COLUMN_WIDTHS);
	ColumnDecodeStringWithCount(column_width_string, width, MESS_COLUMN_MAX);
}

void SetMessSortColumn(int column)
{
	std::string error_string;
	MameUISettings().set_value(MESSUI_SOFTWARE_SORT_COLUMN, column, OPTION_PRIORITY_CMDLINE,error_string);
}

int GetMessSortColumn(void)
{
	return MameUISettings().int_value(MESSUI_SOFTWARE_SORT_COLUMN);
}

void SetMessSortReverse(BOOL reverse)
{
	std::string error_string;
	MameUISettings().set_value( MESSUI_SOFTWARE_SORT_REVERSED, reverse, OPTION_PRIORITY_CMDLINE,error_string);
}

BOOL GetMessSortReverse(void)
{
	return MameUISettings().bool_value(MESSUI_SOFTWARE_SORT_REVERSED);
}

const char* GetSoftwareDirs(void)
{
	return MameUISettings().value(MESSUI_SOFTWAREPATH);
}

void SetSoftwareDirs(const char* paths)
{
	std::string error_string;
	MameUISettings().set_value(MESSUI_SOFTWAREPATH, paths, OPTION_PRIORITY_CMDLINE,error_string);
}

void SetSelectedSoftware(int driver_index, const machine_config *config, const device_image_interface *dev, const char *software)
{
	const char *opt_name = dev->instance_name();
	windows_options o;
	std::string error_string;

	if (LOG_SOFTWARE)
	{
		dprintf("SetSelectedSoftware(): dev=%p (\'%s\') software='%s'\n",
			dev, driver_list::driver(driver_index).name, software);
	}

	load_options(o, driver_index);
	o.set_value(opt_name, software, OPTION_PRIORITY_CMDLINE,error_string);
	save_options(o, driver_index);
}

// not used
const char *GetSelectedSoftware(int driver_index, const machine_config *config, const device_image_interface *dev)
{
	const char *opt_name = dev->instance_name();
	windows_options o;

	load_options(o, driver_index);
	return o.value(opt_name);
}

#if 0
// old extra software
// To be deleted when conversion to "comments_directory" is complete
void SetExtraSoftwarePaths(int driver_index, const char *extra_paths)
{
	char opt_name[256];

	assert(0 <= driver_index && driver_index < driver_list::total());

	snprintf(opt_name, ARRAY_LENGTH(opt_name), "%s_extra_software", driver_list::driver(driver_index).name);
	std::string error_string;
	MameUISettings().set_value(opt_name, extra_paths, OPTION_PRIORITY_CMDLINE,error_string);
}
#endif

// sw = 0 when looking for the path string
// sw = 1 when doing search for compatible software
const char *GetExtraSoftwarePaths(int driver_index, bool sw)
{
	if (driver_index == -1)
		return GetCommentDir();

//	char opt_name[256];
	const char *paths;

	assert(0 <= driver_index && driver_index < driver_list::total());

//	snprintf(opt_name, ARRAY_LENGTH(opt_name), "%s_extra_software", driver_list::driver(driver_index).name);
//	paths = MameUISettings().value(opt_name);
//	if (paths && (paths[0] > 64)) return paths; // must start with a drive letter

	// Try comments
	windows_options o;
	load_options(o, driver_index);
	paths = o.value(OPTION_COMMENT_DIRECTORY);
	if (paths && (paths[0] > 64)) return paths; // must start with a drive letter

	// search deeper when looking for software
	if (sw)
	{
		// not specified in driver, try parent if it has one
		int nParentIndex = -1;
		if (DriverIsClone(driver_index) == TRUE)
		{
			nParentIndex = GetParentIndex(&driver_list::driver(driver_index));
			if (nParentIndex >= 0)
			{
//				snprintf(opt_name, ARRAY_LENGTH(opt_name), "%s_extra_software", driver_list::driver(nParentIndex).name);
//				paths = MameUISettings().value(opt_name);
//				if (paths && (paths[0] > 64))
//				{} else
//				{
					// Try comments
					load_options(o, nParentIndex);
					paths = o.value(OPTION_COMMENT_DIRECTORY);
//				}
			}
		}
		if (paths && (paths[0] > 64)) return paths; // must start with a drive letter

		// still nothing, try for a system in the 'compat' field
		if (nParentIndex >= 0)
			driver_index = nParentIndex;

		// now recycle variable as a compat system number
		nParentIndex = GetCompatIndex(&driver_list::driver(driver_index));
		if (nParentIndex >= 0)
		{
//			snprintf(opt_name, ARRAY_LENGTH(opt_name), "%s_extra_software", driver_list::driver(nParentIndex).name);
//			paths = MameUISettings().value(opt_name);
//			if (paths && (paths[0] > 64))
//			{} else
//			{
				// Try comments
				load_options(o, nParentIndex);
				paths = o.value(OPTION_COMMENT_DIRECTORY);
//			}
		}
	}

	return paths ? paths : "";
}

void SetCurrentSoftwareTab(const char *shortname)
{
	std::string error_string;
	MameUISettings().set_value(MESSUI_SOFTWARE_TAB, shortname, OPTION_PRIORITY_CMDLINE,error_string);
}

const char *GetCurrentSoftwareTab(void)
{
	return MameUISettings().value(MESSUI_SOFTWARE_TAB);
}
