// For licensing and usage information, read docs/winui_license.txt
// MASTER
//****************************************************************************

#ifndef WINUI_EMU_OPTS_H
#define WINUI_EMU_OPTS_H

#include "winmain.h"
//#include "winui.h"

void emu_set_value(windows_options *o, const char* name, float value);
void emu_set_value(windows_options *o, const char* name, int value);
void emu_set_value(windows_options *o, const char* name, string value);
void emu_set_value(windows_options *o, string name, float value);
void emu_set_value(windows_options *o, string name, int value);
void emu_set_value(windows_options *o, string name, string value);
void emu_set_value(windows_options &o, const char* name, float value);
void emu_set_value(windows_options &o, const char* name, int value);
void emu_set_value(windows_options &o, const char* name, string value);
void emu_set_value(windows_options &o, string name, float value);
void emu_set_value(windows_options &o, string name, int value);
void emu_set_value(windows_options &o, string name, string value);


#endif

