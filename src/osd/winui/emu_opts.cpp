// For licensing and usage information, read docs/winui_license.txt
// MASTER
//****************************************************************************

 /***************************************************************************

  emu_opts.cpp

  Stores global options and per-game options;

***************************************************************************/

// standard windows headers
#include <windows.h>
#include <windowsx.h>

// standard C headers
#include <tchar.h>

// MAME/MAMEUI headers
#include "emu.h"
#include "winui.h"
#include "winmain.h"


// char names
void emu_set_value(windows_options *o, const char* name, float value)
{
	std::ostringstream ss;
	ss << value;
	string svalue(ss.str());
	string sname = string(name);
	o->set_value(sname, svalue, OPTION_PRIORITY_CMDLINE);
}

void emu_set_value(windows_options *o, const char* name, int value)
{
	string svalue = std::to_string(value);
	string sname = string(name);
	o->set_value(sname, svalue, OPTION_PRIORITY_CMDLINE);
}

void emu_set_value(windows_options *o, const char* name, string value)
{
	string sname = string(name);
	o->set_value(sname, value, OPTION_PRIORITY_CMDLINE);
}

// string names
void emu_set_value(windows_options *o, string name, float value)
{
	std::ostringstream ss;
	ss << value;
	string svalue(ss.str());
	o->set_value(name, svalue, OPTION_PRIORITY_CMDLINE);
}

void emu_set_value(windows_options *o, string name, int value)
{
	string svalue = std::to_string(value);
	o->set_value(name, svalue, OPTION_PRIORITY_CMDLINE);
}

void emu_set_value(windows_options *o, string name, string value)
{
	o->set_value(name, value, OPTION_PRIORITY_CMDLINE);
}

// char names
void emu_set_value(windows_options &o, const char* name, float value)
{
	std::ostringstream ss;
	ss << value;
	string svalue(ss.str());
	string sname = string(name);
	o.set_value(sname, svalue, OPTION_PRIORITY_CMDLINE);
}

void emu_set_value(windows_options &o, const char* name, int value)
{
	string svalue = std::to_string(value);
	string sname = string(name);
	o.set_value(sname, svalue, OPTION_PRIORITY_CMDLINE);
}

void emu_set_value(windows_options &o, const char* name, string value)
{
	string sname = string(name);
	o.set_value(sname, value, OPTION_PRIORITY_CMDLINE);
}

// string names
void emu_set_value(windows_options &o, string name, float value)
{
	std::ostringstream ss;
	ss << value;
	string svalue(ss.str());
	o.set_value(name, svalue, OPTION_PRIORITY_CMDLINE);
}

void emu_set_value(windows_options &o, string name, int value)
{
	string svalue = std::to_string(value);
	o.set_value(name, svalue, OPTION_PRIORITY_CMDLINE);
}

void emu_set_value(windows_options &o, string name, string value)
{
	o.set_value(name, value, OPTION_PRIORITY_CMDLINE);
}

