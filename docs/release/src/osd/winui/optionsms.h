// For licensing and usage information, read docs/winui_license.txt
//****************************************************************************
#ifndef OPTIONSMS_H
#define OPTIONSMS_H

enum
{
	MESS_COLUMN_IMAGES,
	MESS_COLUMN_MAX
};

enum
{
	SWLIST_COLUMN_IMAGES,
	SWLIST_COLUMN_GOODNAME,
	SWLIST_COLUMN_MANUFACTURER,
	SWLIST_COLUMN_YEAR,
	SWLIST_COLUMN_PLAYABLE,
	SWLIST_COLUMN_USAGE,
	SWLIST_COLUMN_MAX
};

void MessSetupSettings(winui_options &settings);
void MessSetupGameOptions(windows_options &opts, int driver_index);

void SetMessColumnWidths(int widths[]);
void GetMessColumnWidths(int widths[]);
void SetMessColumnOrder(int order[]);
void GetMessColumnOrder(int order[]);
void SetMessColumnShown(int shown[]);
void GetMessColumnShown(int shown[]);
void SetMessSortColumn(int column);
int  GetMessSortColumn(void);
void SetMessSortReverse(BOOL reverse);
BOOL GetMessSortReverse(void);

void SetSWListColumnWidths(int widths[]);
void GetSWListColumnWidths(int widths[]);
void SetSWListColumnOrder(int order[]);
void GetSWListColumnOrder(int order[]);
void SetSWListColumnShown(int shown[]);
void GetSWListColumnShown(int shown[]);
void SetSWListSortColumn(int column);
int  GetSWListSortColumn(void);
void SetSWListSortReverse(BOOL reverse);
BOOL GetSWListSortReverse(void);

const char* GetSoftwareDirs(void);
void  SetSoftwareDirs(const char* paths);

void SetSelectedSoftware(int driver_index, const machine_config *config, const device_image_interface *device, const char *software);
const char *GetSelectedSoftware(int driver_index, const machine_config *config, const device_image_interface *device);

//void SetExtraSoftwarePaths(int driver_index, const char *extra_paths);
const char *GetExtraSoftwarePaths(int driver_index, bool sw);

void SetCurrentSoftwareTab(const char *shortname);
const char *GetCurrentSoftwareTab(void);

#endif /* OPTIONSMS_H */

