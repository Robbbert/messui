// For licensing and usage information, read docs/winui_license.txt
//****************************************************************************

#ifndef DATAFILE_H
#define DATAFILE_H

struct tDatafileIndex
{
	long offset;
	const game_driver *driver;
};

extern int load_driver_history (const game_driver *drv, char *buffer, int bufsize);

#endif
