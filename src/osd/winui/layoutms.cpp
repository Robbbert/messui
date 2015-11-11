// For licensing and usage information, read docs/winui_license.txt
//****************************************************************************
/***************************************************************************

  layoutms.c

  MESS specific TreeView definitions (and maybe more in the future)

***************************************************************************/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include "emu.h"
#include "bitmask.h"
#include "treeview.h"
#include "mui_util.h"
#include "resource.h"
#include "resourcems.h"
#include "mui_opts.h"
#include "splitters.h"
#include "help.h"
#include "mui_audit.h"
#include "msuiutil.h"
#include "winui.h"
#include "properties.h"
#include "propertiesms.h"

static BOOL FilterAvailable(int driver_index);

extern const FOLDERDATA g_folderData[] =
{
	{"All Systems",     "allgames",          FOLDER_ALLGAMES,     IDI_FOLDER,				0,             0,            NULL,                       NULL,              TRUE },
	{"Available",       "available",         FOLDER_AVAILABLE,    IDI_FOLDER_AVAILABLE,     F_AVAILABLE,   0,            NULL,                       FilterAvailable,              TRUE },
	{"Unavailable",     "unavailable",       FOLDER_UNAVAILABLE,  IDI_FOLDER_UNAVAILABLE,	0,             F_AVAILABLE,  NULL,                       FilterAvailable,              FALSE },
	{"Console",         "console",           FOLDER_CONSOLE,      IDI_FOLDER,             F_CONSOLE,     F_COMPUTER,   NULL,                       DriverIsConsole,  TRUE },
	{"Computer",        "computer",          FOLDER_COMPUTER,     IDI_FOLDER,             F_COMPUTER,    F_CONSOLE,    NULL,                       DriverIsComputer,  TRUE },
	{"Modified/Hacked", "modified",          FOLDER_MODIFIED,     IDI_FOLDER,				0,             0,            NULL,                       DriverIsModified,  TRUE },
	{"Manufacturer",    "manufacturer",      FOLDER_MANUFACTURER, IDI_FOLDER_MANUFACTURER,  0,             0,            CreateManufacturerFolders },
	{"Year",            "year",              FOLDER_YEAR,         IDI_FOLDER_YEAR,          0,             0,            CreateYearFolders },
	{"Source",          "source",            FOLDER_SOURCE,       IDI_FOLDER_SOURCE,        0,             0,            CreateSourceFolders },
	{"CPU",             "cpu",               FOLDER_CPU,          IDI_FOLDER,               0,             0,            CreateCPUFolders },
	{"Sound",           "sound",             FOLDER_SND,          IDI_FOLDER,               0,             0,            CreateSoundFolders },
	{"Imperfect",       "imperfect",         FOLDER_DEFICIENCY,   IDI_FOLDER,               0,             0,            CreateDeficiencyFolders },
	{"Dumping Status",  "dumping",           FOLDER_DUMPING,      IDI_FOLDER,               0,             0,            CreateDumpingFolders },
	{"Working",         "working",           FOLDER_WORKING,      IDI_WORKING,              F_WORKING,     F_NONWORKING, NULL,                       DriverIsBroken,    FALSE },
	{"Not Working",     "nonworking",        FOLDER_NONWORKING,   IDI_NONWORKING,           F_NONWORKING,  F_WORKING,    NULL,                       DriverIsBroken,    TRUE },
	{"Originals",       "originals",         FOLDER_ORIGINAL,     IDI_FOLDER,               F_ORIGINALS,   F_CLONES,     NULL,                       DriverIsClone,     FALSE },
	{"Clones",          "clones",            FOLDER_CLONES,       IDI_FOLDER,               F_CLONES,      F_ORIGINALS,  NULL,                       DriverIsClone,     TRUE },
	{"Raster",          "raster",            FOLDER_RASTER,       IDI_FOLDER,               F_RASTER,      F_VECTOR,     NULL,                       DriverIsVector,    FALSE },
	{"Vector",          "vector",            FOLDER_VECTOR,       IDI_FOLDER,               F_VECTOR,      F_RASTER,     NULL,                       DriverIsVector,    TRUE },
	{"Mouse",           "mouse",             FOLDER_MOUSE,        IDI_FOLDER,               0,             0,            NULL,                       DriverUsesMouse,	TRUE },
	{"Trackball",       "trackball",         FOLDER_TRACKBALL,    IDI_FOLDER,               0,             0,            NULL,                       DriverUsesTrackball,	TRUE },
	{"Stereo",          "stereo",            FOLDER_STEREO,       IDI_SOUND,                0,             0,            NULL,                       DriverIsStereo,    TRUE },
	{ NULL }
};

/* list of filter/control Id pairs */
extern const FILTER_ITEM g_filterList[] =
{
	{ F_COMPUTER,     IDC_FILTER_COMPUTER,    DriverIsComputer, TRUE },
	{ F_CONSOLE,      IDC_FILTER_CONSOLE,     DriverIsConsole, TRUE },
	{ F_MODIFIED,     IDC_FILTER_MODIFIED,    DriverIsModified, TRUE },
	{ F_CLONES,       IDC_FILTER_CLONES,      DriverIsClone, TRUE },
	{ F_NONWORKING,   IDC_FILTER_NONWORKING,  DriverIsBroken, TRUE },
	{ F_UNAVAILABLE,  IDC_FILTER_UNAVAILABLE, FilterAvailable, FALSE },
	{ F_RASTER,       IDC_FILTER_RASTER,      DriverIsVector, FALSE },
	{ F_VECTOR,       IDC_FILTER_VECTOR,      DriverIsVector, TRUE },
	{ F_ORIGINALS,    IDC_FILTER_ORIGINALS,   DriverIsClone, FALSE },
	{ F_WORKING,      IDC_FILTER_WORKING,     DriverIsBroken, FALSE },
	{ F_AVAILABLE,    IDC_FILTER_AVAILABLE,   FilterAvailable, TRUE },
	{ F_ARCADE,       IDC_FILTER_ARCADE,      DriverIsArcade, TRUE },
	{ F_MESS,         IDC_FILTER_MESS,        DriverIsArcade, FALSE },
	{ 0 }
};

extern const SPLITTERINFO g_splitterInfo[] =
{
	{ 0.2,	IDC_SPLITTER,	IDC_TREE,	IDC_LIST,	AdjustSplitter1Rect },
	{ 0.4,	IDC_SPLITTER2,	IDC_LIST,	IDC_SWLIST,	AdjustSplitter1Rect },
	{ 0.6,	IDC_SPLITTER3,	IDC_SWTAB,	IDC_SSFRAME,	AdjustSplitter2Rect },
	{ -1 }
};

// this doesn't seem to be used
extern const MAMEHELPINFO g_helpInfo[] =
{
	//{ ID_HELP_CONTENTS,	TRUE,	TEXT("messui.chm::/windows/main.htm") },
	{ ID_HELP_CONTENTS,	TRUE,	TEXT("messui.chm") },
	//{ ID_HELP_RELEASE,	TRUE,	TEXT("messui.chm") },
	//{ ID_HELP_WHATS_NEW,	TRUE,	TEXT("messui.chm::/messnew.txt") },
	{ ID_HELP_WHATS_NEW,	TRUE,	TEXT("http://messui.the-chronicles.org/mess/messnew.txt") },
	{ -1 }
};

extern const PROPERTYSHEETINFO g_propSheets[] =
{
	{ FALSE,	NULL,					IDD_PROP_GAME,			GamePropertiesDialogProc },
	{ FALSE,	NULL,					IDD_PROP_AUDIT,			GameAuditDialogProc },
	{ TRUE,		NULL,					IDD_PROP_DISPLAY,		GameOptionsProc },
	{ TRUE,		NULL,					IDD_PROP_ADVANCED,		GameOptionsProc },
	{ TRUE,		NULL,					IDD_PROP_SCREEN,		GameOptionsProc },
	{ TRUE,		NULL,					IDD_PROP_SOUND,			GameOptionsProc },
	{ TRUE,		NULL,					IDD_PROP_INPUT,			GameOptionsProc },
	{ TRUE,		NULL,					IDD_PROP_CONTROLLER,	GameOptionsProc },
	{ TRUE,		NULL,					IDD_PROP_DEBUG,			GameOptionsProc },
	{ TRUE,		NULL,					IDD_PROP_MISC,			GameOptionsProc },
	{ FALSE,	NULL,					IDD_PROP_SOFTWARE,		GameMessOptionsProc },
	{ FALSE,	PropSheetFilter_Config,	IDD_PROP_CONFIGURATION,	GameMessOptionsProc },
	{ TRUE,		PropSheetFilter_Vector,	IDD_PROP_VECTOR,		GameOptionsProc },
	{ FALSE }
};

extern const ICONDATA g_iconData[] =
{
	{ IDI_WIN_NOROMS,			"noroms" },
	{ IDI_WIN_ROMS,				"roms" },
	{ IDI_WIN_UNKNOWN,			"unknown" },
	{ IDI_WIN_CLONE,			"clone" },
	{ IDI_WIN_REDX,				"warning" },
	{ IDI_WIN_NOROMSNEEDED,		"noromsneeded" },
	{ IDI_WIN_MISSINGOPTROM,	"missingoptrom" },
	{ IDI_WIN_FLOP,				"floppy" },
	{ IDI_WIN_CASS,				"cassette" },
	{ IDI_WIN_SERL,				"serial" },
	{ IDI_WIN_SNAP,				"snapshot" },
	{ IDI_WIN_PRIN,				"printer" },
	{ IDI_WIN_HARD,				"hard" },
	{ 0 }
};

extern const TCHAR g_szPlayGameString[] = TEXT("&Run %s");
extern const char g_szGameCountString[] = "%d systems";
static const char g_szHistoryFileName[] = "history.dat";  // This can only be history.dat or sysinfo.dat
static const char g_szMameInfoFileName[] = "messinfo.dat"; // This can only be mameinfo.dat or messinfo.dat

static BOOL FilterAvailable(int driver_index)
{
	return !DriverUsesRoms(driver_index) || IsAuditResultYes(GetRomAuditResults(driver_index));
}
