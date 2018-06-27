// For licensing and usage information, read docs/winui_license.txt
// MASTER
//****************************************************************************

#ifndef WINUI_INIOPTS_H
#define WINUI_INIOPTS_H

typedef struct
{
	string   optname;
	string   optvalue;
	int           opttype;
	string   optmin;
	string   optmax;
	string   optstep;
	string   opthelp;
}
INIOPTS;
// INI options
const INIOPTS optentries[] =
{
	{ "readconfig",             "1",         0, "", "", "",    "enable loading of configuration files" },
	{ "writeconfig",            "1",         0, "", "", "",    "writes configuration to (driver).ini on exit" },
	{ "rompath",                "roms",      4, "", "", "",     "path to ROMsets and hard disk images" },
	{ "hashpath",               "hash",      4, "", "", "",     "path to hash files" },
	{ "samplepath",             "samples",   4, "", "", "",     "path to samplesets" },
	{ "artpath",                "artwork",   4, "", "", "",     "path to artwork files" },
	{ "ctrlrpath",              "ctrlr",     4, "", "", "",     "path to controller definitions" },
	{ "inipath",                "ini",         4, "", "", "",     "path to ini files" },
	{ "fontpath",               ".",         4, "", "", "",     "path to font files" },
	{ "cheatpath",              "cheat",     4, "", "", "",     "path to cheat files" },
	{ "crosshairpath",          "crosshair", 4, "", "", "",     "path to crosshair files" },
	{ "pluginspath",            "plugins",   4, "", "", "",     "path to plugin files" },
	{ "languagepath",           "language",  4, "", "", "",     "path to language files" },
	{ "swpath",                 "software",  4, "", "", "",     "path to loose software" },
	{ "cfg_directory",          "cfg",       4, "", "", "",     "directory to save configurations" },
	{ "nvram_directory",        "nvram",     4, "", "", "",     "directory to save nvram contents" },
	{ "input_directory",        "inp",       4, "", "", "",     "directory to save input device logs" },
	{ "state_directory",        "sta",       4, "", "", "",     "directory to save states" },
	{ "snapshot_directory",     "snap",      4, "", "", "",     "directory to save/load screenshots" },
	{ "diff_directory",         "diff",      4, "", "", "",     "directory to save hard drive image difference files" },
	{ "comment_directory",      "comments",  4, "", "", "",     "directory to save debugger comments" },
	{ "state",                  "",     4, "", "", "",     "saved state to load" },
	{ "autosave",               "0",         0, "", "", "",    "enable automatic restore at startup, and automatic save at exit time" },
	{ "rewind",                 "0",         0, "", "", "",    "enable rewind savestates" },
	{ "rewind_capacity",        "100",       1, "0", "0", "0",    "rewind buffer size in megabytes" },
	{ "playback",               "",     4, "", "", "",     "playback an input file" },
	{ "record",                 "",     4, "", "", "",     "record an input file" },
	{ "record_timecode",        "0",         0, "", "", "",    "record an input timecode file (requires -record option)" },
	{ "exit_after_playback",    "0",         0, "", "", "",    "close the program at the end of playback" },
	{ "mngwrite",               "",     4, "", "", "",     "optional filename to write a MNG movie of the current session" },
	{ "aviwrite",               "",     4, "", "", "",     "optional filename to write an AVI movie of the current session" },
	{ "wavwrite",               "",     4, "", "", "",     "optional filename to write a WAV file of the current session" },
	{ "snapname",               "%g/%i",     4, "", "", "",     "override of the default snapshot/movie naming; %g == gamename, %i == index" },
	{ "snapsize",               "auto",      4, "", "", "",     "specify snapshot/movie resolution (<width>x<height>) or 'auto' to use minimal size " },
	{ "snapview",               "internal",  4, "", "", "",     "specify snapshot/movie view or 'internal' to use internal pixel-aspect views" },
	{ "snapbilinear",           "1",         0, "", "", "",    "specify if the snapshot/movie should have bilinear filtering applied" },
	{ "statename",              "%g",        4, "", "", "",     "override of the default state subfolder naming; %g == gamename" },
	{ "burnin",                 "0",         0, "", "", "",    "create burn-in snapshots for each screen" },
	{ "autoframeskip",          "0",         0, "", "", "",    "enable automatic frameskip selection" },
	{ "frameskip",              "0",         1, "0", "0", "0",    "set frameskip to fixed value, 0-10 (autoframeskip must be disabled)" },
	{ "seconds_to_run",         "0",         1, "0", "0", "0",    "number of emulated seconds to run before automatically exiting" },
	{ "throttle",               "1",         0, "", "", "",    "enable throttling to keep game running in sync with real time" },
	{ "sleep",                  "1",         0, "", "", "",    "enable sleeping, which gives time back to other applications when idle" },
	{ "speed",                  "1.0",       2, "0", "0", "0",      "controls the speed of gameplay, relative to realtime; smaller numbers are slower" },
	{ "refreshspeed",           "0",         0, "", "", "",    "automatically adjusts the speed of gameplay to keep the refresh rate lower than the screen" },
	{ "keepaspect",             "1",         0, "", "", "",    "constrain to the proper aspect ratio" },
	{ "unevenstretch",          "1",         0, "", "", "",    "allow non-integer stretch factors" },
	{ "unevenstretchx",         "0",         0, "", "", "",    "allow non-integer stretch factors only on horizontal axis"},
	{ "unevenstretchy",         "0",         0, "", "", "",    "allow non-integer stretch factors only on vertical axis"},
	{ "autostretchxy",          "0",         0, "", "", "",    "automatically apply -unevenstretchx/y based on source native orientation"},
	{ "intoverscan",            "0",         0, "", "", "",    "allow overscan on integer scaled targets"},
	{ "intscalex",              "0",         1, "0", "0", "0",    "set horizontal integer scale factor."},
	{ "intscaley",              "0",         1, "0", "0", "0",    "set vertical integer scale."},
	{ "rotate",                 "1",         0, "", "", "",    "rotate the game screen according to the game's orientation needs it" },
	{ "ror",                    "0",         0, "", "", "",    "rotate screen clockwise 90 degrees" },
	{ "rol",                    "0",         0, "", "", "",    "rotate screen counterclockwise 90 degrees" },
	{ "autoror",                "0",         0, "", "", "",    "automatically rotate screen clockwise 90 degrees if vertical" },
	{ "autorol",                "0",         0, "", "", "",    "automatically rotate screen counterclockwise 90 degrees if vertical" },
	{ "flipx",                  "0",         0, "", "", "",    "flip screen left-right" },
	{ "flipy",                  "0",         0, "", "", "",    "flip screen upside-down" },
	{ "artwork_crop",           "0",         0, "", "", "",    "crop artwork to game screen size" },
	{ "use_backdrops",          "1",         0, "", "", "",    "enable backdrops if artwork is enabled and available" },
	{ "use_overlays",           "1",         0, "", "", "",    "enable overlays if artwork is enabled and available" },
	{ "use_bezels",             "1",         0, "", "", "",    "enable bezels if artwork is enabled and available" },
	{ "use_cpanels",            "1",         0, "", "", "",    "enable cpanels if artwork is enabled and available" },
	{ "use_marquees",           "1",         0, "", "", "",    "enable marquees if artwork is enabled and available" },
	{ "fallback_artwork",       "",     4, "", "", "",     "fallback artwork if no external artwork or internal driver layout defined" },
	{ "override_artwork",       "",     4, "", "", "",     "override artwork for external artwork and internal driver layout" },
	{ "brightness",             "1.0",       2, "0", "0", "0",      "default game screen brightness correction" },
	{ "contrast",               "1.0",       2, "0", "0", "0",      "default game screen contrast correction" },
	{ "gamma",                  "1.0",       2, "0", "0", "0",      "default game screen gamma correction" },
	{ "pause_brightness",       "0.65",      2, "0", "0", "0",      "amount to scale the screen brightness when paused" },
	{ "effect",                 "none",      4, "", "", "",     "name of a PNG file to use for visual effects, or 'none'" },
	{ "beam_width_min",         "1.0",       2, "0", "0", "0",      "set vector beam width minimum" },
	{ "beam_width_max",         "1.0",       2, "0", "0", "0",      "set vector beam width maximum" },
	{ "beam_intensity_weight",  "0",         2, "0", "0", "0",      "set vector beam intensity weight " },
	{ "flicker",                "0",         2, "0", "0", "0",      "set vector flicker effect" },
	{ "samplerate",             "48000",     1, "0", "0", "0",    "set sound output sample rate" },
	{ "samples",                "1",         0, "", "", "",    "enable the use of external samples if available" },
	{ "volume",                 "0",         1, "0", "0", "0",    "sound volume in decibels (-32 min, 0 max)" },
	{ "coin_lockout",           "1",         0, "", "", "",    "enable coin lockouts to actually lock out coins" },
	{ "ctrlr",                  "",     4, "", "", "",     "preconfigure for specified controller" },
	{ "mouse",                  "0",         0, "", "", "",    "enable mouse input" },
	{ "joystick",               "1",         0, "", "", "",    "enable joystick input" },
	{ "lightgun",               "0",         0, "", "", "",    "enable lightgun input" },
	{ "multikeyboard",          "0",         0, "", "", "",    "enable separate input from each keyboard device (if present)" },
	{ "multimouse",             "0",         0, "", "", "",    "enable separate input from each mouse device (if present)" },
	{ "steadykey",              "0",         0, "", "", "",    "enable steadykey support" },
	{ "ui_active",              "0",         0, "", "", "",    "enable user interface on top of emulated keyboard (if present)" },
	{ "offscreen_reload",       "0",         0, "", "", "",    "convert lightgun button 2 into offscreen reload" },
	{ "joystick_map",           "auto",      4, "", "", "",     "explicit joystick map, or auto to auto-select" },
	{ "joystick_deadzone",      "0.3",       2, "0", "0", "0",      "center deadzone range for joystick where change is ignored (0.0 center, 1.0 end)" },
	{ "joystick_saturation",    "0.85",      2, "0", "0", "0",      "end of axis saturation range for joystick where change is ignored (0.0 center, 1.0 end)" },
	{ "natural",                "0",         0, "", "", "",    "specifies whether to use a natural keyboard or not" },
	{ "joystick_contradictory", "0",         0, "", "", "",    "enable contradictory direction digital joystick input at the same time" },
	{ "coin_impulse",           "0",         1, "0", "0", "0",    "set coin impulse time (n<0 disable impulse, n==0 obey driver, 0<n set time n)" },
	{ "paddle_device",          "keyboard",  4, "", "", "",     "enable (none|keyboard|mouse|lightgun|joystick) if a paddle control is present" },
	{ "adstick_device",         "keyboard",  4, "", "", "",     "enable (none|keyboard|mouse|lightgun|joystick) if an analog joystick control is present" },
	{ "pedal_device",           "keyboard",  4, "", "", "",     "enable (none|keyboard|mouse|lightgun|joystick) if a pedal control is present" },
	{ "dial_device",            "keyboard",  4, "", "", "",     "enable (none|keyboard|mouse|lightgun|joystick) if a dial control is present" },
	{ "trackball_device",       "keyboard",  4, "", "", "",     "enable (none|keyboard|mouse|lightgun|joystick) if a trackball control is present" },
	{ "lightgun_device",        "keyboard",  4, "", "", "",     "enable (none|keyboard|mouse|lightgun|joystick) if a lightgun control is present" },
	{ "positional_device",      "keyboard",  4, "", "", "",     "enable (none|keyboard|mouse|lightgun|joystick) if a positional control is present" },
	{ "mouse_device",           "mouse",     4, "", "", "",     "enable (none|keyboard|mouse|lightgun|joystick) if a mouse control is present" },
	{ "log",                    "0",         0, "", "", "",    "generate an error.log file" },
	{ "debug",                  "0",         0, "", "", "",    "enable/disable debugger" },
	{ "oslog",                  "0",         0, "", "", "",    "output error.log data to the system debugger" },
	{ "verbose",                "0",         0, "", "", "",    "display additional diagnostic information" },
	{ "update_in_pause",        "0",         0, "", "", "",    "keep calling video updates while in pause" },
	{ "debugscript",            "",     4, "", "", "",     "script for debugger" },
	{ "drc",                    "1",         0, "", "", "",    "enable DRC cpu core if available" },
	{ "drc_use_c",              "0",         0, "", "", "",    "force DRC use C backend" },
	{ "drc_log_uml",            "0",         0, "", "", "",    "write DRC UML disassembly log" },
	{ "drc_log_native",         "0",         0, "", "", "",    "write DRC native disassembly log" },
	{ "bios",                   "",     4, "", "", "",     "select the system BIOS to use" },
	{ "cheat",                  "0",         0, "", "", "",    "enable cheat subsystem" },
	{ "skip_gameinfo",          "1",         0, "", "", "",    "skip displaying the information screen at startup" },
	{ "uifont",                 "default",   4, "", "", "",     "specify a font to use" },
	{ "ui",                     "cabinet",   4, "", "", "",     "type of UI (simple|cabinet)" },
	{ "ramsize",                "",     4, "", "", "",     "size of RAM (if supported by driver)" },
	{ "comm_localhost",         "0.0.0.0",   4, "", "", "",     "local address to bind to" },
	{ "comm_localport",         "15112",     4, "", "", "",     "local port to bind to" },
	{ "comm_remotehost",        "127.0.0.1", 4, "", "", "",     "remote address to connect to" },
	{ "comm_remoteport",        "15112",     4, "", "", "",     "remote port to connect to" },
	{ "comm_framesync",         "0",         0, "", "", "",    "sync frames" },
	{ "confirm_quit",           "0",         0, "", "", "",    "display confirm quit screen on exit" },
	{ "ui_mouse",               "1",         0, "", "", "",    "display ui mouse cursor" },
	{ "language",               "English",   4, "", "", "",     "display language" },
	{ "nvram_save",             "1",   4, "", "", "",     "save nvram" },
	{ "autoboot_command",       "",     4, "", "", "",     "command to execute after machine boot" },
	{ "autoboot_delay",         "0",         1, "0", "0", "0",    "timer delay in sec to trigger command execution on autoboot" },
	{ "autoboot_script",        "",     4, "", "", "",     "lua script to execute after machine boot" },
	{ "console",                "0",         0, "", "", "",    "enable emulator LUA console" },
	{ "plugins",                "1",         0, "", "", "",    "enable LUA plugin support" },
	{ "plugin",                 "data",     4, "", "", "",     "list of plugins to enable" },
	{ "noplugin",               "",     4, "", "", "",     "list of plugins to disable" },
	{ "http",                   "0",         0, "", "", "",    "HTTP server enable" },
	{ "http_port",              "8080",      1, "0", "0", "0",    "HTTP server port" },
	{ "http_root",              "web",       4, "", "", "",     "HTTP server document root" },
	{ "uimodekey",              "SCRLOCK",         4, "", "", "",    "Key to toggle keyboard mode" },
	{ "uifontprovider",         "auto",   4, "", "", "",    "provider for ui font: " },
	{ "output",                 "auto",   4, "", "", "",    "provider for output: " },
	{ "keyboardprovider",       "auto",   4, "", "", "",    "provider for keyboard input: " },
	{ "mouseprovider",          "auto",   4, "", "", "",    "provider for mouse input: " },
	{ "lightgunprovider",       "auto",   4, "", "", "",    "provider for lightgun input: " },
	{ "joystickprovider",       "auto",   4, "", "", "",    "provider for joystick input: " },
	{ "debugger",               "auto",   4, "", "", "",    "debugger used: " },
	{ "debugger_font",          "auto",   4, "", "", "",    "specifies the font to use for debugging" },
	{ "debugger_font_size",     "0",           2, "0", "0", "0",     "specifies the font size to use for debugging" },
	{ "watchdog",               "0",              1, "0", "0", "0",   "force the program to terminate if no updates within specified number of seconds" },
	{ "numprocessors",          "auto",   4, "", "", "",    "number of processors; this overrides the number the system reports" },
	{ "bench",                  "0",              1, "0", "0", "0",   "benchmark for the given number of emulated seconds; implies -video none -sound none -nothrottle" },
	{ "video",                  "auto",   4, "", "", "",    "video output method: " },
	{ "numscreens",             "1",              1, "0", "0", "0",   "number of screens to create; usually, you want just one" },
	{ "window",                 "1",              0, "", "", "",   "enable window mode; otherwise, full screen mode is assumed" },
	{ "maximize",               "1",              0, "", "", "",   "default to maximized windows; otherwise, windows will be minimized" },
	{ "waitvsync",              "0",              0, "", "", "",   "enable waiting for the start of VBLANK before flipping screens; reduces tearing effects" },
	{ "syncrefresh",            "0",              0, "", "", "",   "enable using the start of VBLANK for throttling instead of the game time" },
	{ "monitorprovider",        "auto",   4, "", "", "",    "monitor discovery method" },
	{ "screen",                 "auto",   4, "", "", "",    "explicit name of the first screen; 'auto' here will try to make a best guess" },
	{ "aspect",                 "auto",   4, "", "", "",    "aspect ratio for all screens; 'auto' here will try to make a best guess" },
	{ "resolution",             "auto",   4, "", "", "",    "preferred resolution for all screens; format is <width>x<height>[@<refreshrate>] or 'auto'" },
	{ "view",                   "auto",   4, "", "", "",    "preferred view for all screens" },
	{ "screen0",                "auto",   4, "", "", "",    "explicit name of the first screen; 'auto' here will try to make a best guess" },
	{ "aspect0",                "auto",   4, "", "", "",    "aspect ratio of the first screen; 'auto' here will try to make a best guess" },
	{ "resolution0",            "auto",   4, "", "", "",    "preferred resolution of the first screen; format is <width>x<height>[@<refreshrate>] or 'auto'" },
	{ "view0",                  "auto",   4, "", "", "",    "preferred view for the first screen" },
	{ "screen1",                "auto",   4, "", "", "",    "explicit name of the second screen; 'auto' here will try to make a best guess" },
	{ "aspect1",                "auto",   4, "", "", "",    "aspect ratio of the second screen; 'auto' here will try to make a best guess" },
	{ "resolution1",            "auto",   4, "", "", "",    "preferred resolution of the second screen; format is <width>x<height>[@<refreshrate>] or 'auto'" },
	{ "view1",                  "auto",   4, "", "", "",    "preferred view for the second screen" },
	{ "screen2",                "auto",   4, "", "", "",    "explicit name of the third screen; 'auto' here will try to make a best guess" },
	{ "aspect2",                "auto",   4, "", "", "",    "aspect ratio of the third screen; 'auto' here will try to make a best guess" },
	{ "resolution2",            "auto",   4, "", "", "",    "preferred resolution of the third screen; format is <width>x<height>[@<refreshrate>] or 'auto'" },
	{ "view2",                  "auto",   4, "", "", "",    "preferred view for the third screen" },
	{ "screen3",                "auto",   4, "", "", "",    "explicit name of the fourth screen; 'auto' here will try to make a best guess" },
	{ "aspect3",                "auto",   4, "", "", "",    "aspect ratio of the fourth screen; 'auto' here will try to make a best guess" },
	{ "resolution3",            "auto",   4, "", "", "",    "preferred resolution of the fourth screen; format is <width>x<height>[@<refreshrate>] or 'auto'" },
	{ "view3",                  "auto",   4, "", "", "",    "preferred view for the fourth screen" },
	{ "switchres",              "0",              0, "", "", "",   "enable resolution switching" },
	{ "filter",                 "0",              0, "", "", "",   "enable bilinear filtering on screen output" },
	{ "prescale",               "1",              1, "0", "0", "0",   "scale screen rendering by this amount in software" },
	{ "gl_forcepow2texture",    "0",              0, "", "", "",   "force power of two textures  (default no)" },
	{ "gl_notexturerect",       "0",              0, "", "", "",   "don't use OpenGL GL_ARB_texture_rectangle (default on)" },
	{ "gl_vbo",                 "1",              0, "", "", "",   "enable OpenGL VBO,  if available (default on)" },
	{ "gl_pbo",                 "1",              0, "", "", "",   "enable OpenGL PBO,  if available (default on)" },
	{ "gl_glsl",                "0",              0, "", "", "",   "enable OpenGL GLSL, if available (default off)" },
	{ "gl_glsl_filter",         "1",              4, "", "", "",    "enable OpenGL GLSL filtering instead of FF filtering 0-plain, 1-bilinear (default)" },
	{ "glsl_shader_mame0",      "none",   4, "", "", "",    "custom OpenGL GLSL shader set mame bitmap 0" },
	{ "glsl_shader_mame1",      "none",   4, "", "", "",    "custom OpenGL GLSL shader set mame bitmap 1" },
	{ "glsl_shader_mame2",      "none",   4, "", "", "",    "custom OpenGL GLSL shader set mame bitmap 2" },
	{ "glsl_shader_mame3",      "none",   4, "", "", "",    "custom OpenGL GLSL shader set mame bitmap 3" },
	{ "glsl_shader_mame4",      "none",   4, "", "", "",    "custom OpenGL GLSL shader set mame bitmap 4" },
	{ "glsl_shader_mame5",      "none",   4, "", "", "",    "custom OpenGL GLSL shader set mame bitmap 5" },
	{ "glsl_shader_mame6",      "none",   4, "", "", "",    "custom OpenGL GLSL shader set mame bitmap 6" },
	{ "glsl_shader_mame7",      "none",   4, "", "", "",    "custom OpenGL GLSL shader set mame bitmap 7" },
	{ "glsl_shader_mame8",      "none",   4, "", "", "",    "custom OpenGL GLSL shader set mame bitmap 8" },
	{ "glsl_shader_mame9",      "none",   4, "", "", "",    "custom OpenGL GLSL shader set mame bitmap 9" },
	{ "glsl_shader_screen0",    "none",   4, "", "", "",    "custom OpenGL GLSL shader screen bitmap 0" },
	{ "glsl_shader_screen1",    "none",   4, "", "", "",    "custom OpenGL GLSL shader screen bitmap 1" },
	{ "glsl_shader_screen2",    "none",   4, "", "", "",    "custom OpenGL GLSL shader screen bitmap 2" },
	{ "glsl_shader_screen3",    "none",   4, "", "", "",    "custom OpenGL GLSL shader screen bitmap 3" },
	{ "glsl_shader_screen4",    "none",   4, "", "", "",    "custom OpenGL GLSL shader screen bitmap 4" },
	{ "glsl_shader_screen5",    "none",   4, "", "", "",    "custom OpenGL GLSL shader screen bitmap 5" },
	{ "glsl_shader_screen6",    "none",   4, "", "", "",    "custom OpenGL GLSL shader screen bitmap 6" },
	{ "glsl_shader_screen7",    "none",   4, "", "", "",    "custom OpenGL GLSL shader screen bitmap 7" },
	{ "glsl_shader_screen8",    "none",   4, "", "", "",    "custom OpenGL GLSL shader screen bitmap 8" },
	{ "glsl_shader_screen9",    "none",   4, "", "", "",    "custom OpenGL GLSL shader screen bitmap 9" },
	{ "sound",                  "auto",            4, "", "", "",    "sound output method: " },
	{ "audio_latency",          "2",               1, "0", "0", "0",   "set audio latency (increase to reduce glitches, decrease for responsiveness)" },
	{ "pa_api",                 "none",            4, "", "", "",    "PortAudio API" },
	{ "pa_device",              "none",            4, "", "", "",    "PortAudio device" },
	{ "pa_latency",             "0",               2, "0", "0", "0",     "suggested latency in seconds, 0 for default" },
	{ "bgfx_path",              "bgfx",            4, "", "", "", "path to BGFX-related files" },
	{ "bgfx_backend",           "auto",            4, "", "", "", "BGFX backend to use (d3d9, d3d11, metal, opengl, gles)" },
	{ "bgfx_debug",             "0",               0, "", "", "", "enable BGFX debugging statistics" },
	{ "bgfx_screen_chains",     "default",         4, "", "", "", "comma-delimited list of screen chain JSON names, colon-delimited per-window" },
	{ "bgfx_shadow_mask",       "slot-mask.png",   4, "", "", "", "shadow mask texture name" },
	{ "bgfx_avi_name",          "auto",            4, "", "", "", "filename for BGFX output logging" },
	{ "priority",               "0",               1, "0", "0", "0",    "thread priority for the main game thread; range from -15 to 1" },
	{ "profile",                "0",               1, "0", "0", "0",    "enables profiling, specifying the stack depth to track" },
	{ "menu",                   "0",               0, "", "", "",    "enables menu bar if available by UI implementation" },
	{ "hlslpath",               "hlsl",            4, "", "", "",     "path to hlsl files" },
	{ "hlsl_enable",            "0",               0, "", "", "",    "enables HLSL post-processing (PS3.0 required)" },
	{ "hlsl_oversampling",      "0",               0, "", "", "",    "enables HLSL oversampling" },
	{ "hlsl_write",             "auto",            4, "", "", "",     "enables HLSL AVI writing (huge disk bandwidth suggested)" },
	{ "hlsl_snap_width",        "2048",            4, "", "", "",     "HLSL upscaled-snapshot width" },
	{ "hlsl_snap_height",       "1536",            4, "", "", "",     "HLSL upscaled-snapshot height" },
	{ "shadow_mask_tile_mode",  "0",               1, "0", "0", "0",    "shadow mask tile mode (0 for screen based, 1 for source based)" },
	{ "shadow_mask_alpha",      "0.0",             2, "0", "0", "0",      "shadow mask alpha-blend value (1.0 is fully blended, 0.0 is no mask)" },
	{ "shadow_mask_texture",    "shadow-mask.png", 4, "", "", "",     "shadow mask texture name" },
	{ "shadow_mask_x_count",    "6",               1, "0", "0", "0",    "shadow mask tile width, in screen dimensions" },
	{ "shadow_mask_y_count",    "4",               1, "0", "0", "0",    "shadow mask tile height, in screen dimensions" },
	{ "shadow_mask_usize",      "0.1875",          2, "0", "0", "0",      "shadow mask texture width, in U/V dimensions" },
	{ "shadow_mask_vsize",      "0.25",            2, "0", "0", "0",      "shadow mask texture height, in U/V dimensions" },
	{ "shadow_mask_uoffset",    "0.0",             2, "0", "0", "0",      "shadow mask texture offset, in U direction" },
	{ "shadow_mask_voffset",    "0.0",             2, "0", "0", "0",      "shadow mask texture offset, in V direction" },
	{ "reflection",             "0.0",             2, "0", "0", "0",      "screen reflection amount" },
	{ "distortion",             "0.0",             2, "0", "0", "0",      "screen distortion amount" },
	{ "cubic_distortion",       "0.0",             2, "0", "0", "0",      "screen cubic distortion amount" },
	{ "distort_corner",         "0.0",             2, "0", "0", "0",      "screen distort corner amount" },
	{ "round_corner",           "0.0",             2, "0", "0", "0",      "screen round corner amount" },
	{ "smooth_border",          "0.0",             2, "0", "0", "0",      "screen smooth border amount" },
	{ "vignetting",             "0.0",             2, "0", "0", "0",      "image vignetting amount" },
	{ "scanline_alpha",         "0.0",             2, "0", "0", "0",      "overall alpha scaling value for scanlines" },
	{ "scanline_size",          "1.0",             2, "0", "0", "0",      "overall height scaling value for scanlines" },
	{ "scanline_height",        "1.0",             2, "0", "0", "0",      "individual height scaling value for scanlines" },
	{ "scanline_variation",     "1.0",             2, "0", "0", "0",      "individual height varying value for scanlines" },
	{ "scanline_bright_scale",  "1.0",             2, "0", "0", "0",      "overall brightness scaling value for scanlines (multiplicative)" },
	{ "scanline_bright_offset", "0.0",             2, "0", "0", "0",      "overall brightness offset value for scanlines (additive)" },
	{ "scanline_jitter",        "0.0",             2, "0", "0", "0",      "overall interlace jitter scaling value for scanlines" },
	{ "hum_bar_alpha",          "0.0",             2, "0", "0", "0",      "overall alpha scaling value for hum bar" },
	{ "defocus",                "0.0,0.0",         4, "", "", "",     "overall defocus value in screen-relative coords" },
	{ "converge_x",             "0.0,0.0,0.0",     4, "", "", "",     "convergence in screen-relative X direction" },
	{ "converge_y",             "0.0,0.0,0.0",     4, "", "", "",     "convergence in screen-relative Y direction" },
	{ "radial_converge_x",      "0.0,0.0,0.0",     4, "", "", "",     "radial convergence in screen-relative X direction" },
	{ "radial_converge_y",      "0.0,0.0,0.0",     4, "", "", "",     "radial convergence in screen-relative Y direction" },
	{ "red_ratio",              "1.0,0.0,0.0",     4, "", "", "",     "red output signal generated by input signal" },
	{ "grn_ratio",              "0.0,1.0,0.0",     4, "", "", "",     "green output signal generated by input signal" },
	{ "blu_ratio",              "0.0,0.0,1.0",     4, "", "", "",     "blue output signal generated by input signal" },
	{ "offset",                 "0.0,0.0,0.0",     4, "", "", "",     "signal offset value (additive)" },
	{ "scale",                  "1.0,1.0,1.0",     4, "", "", "",     "signal scaling value (multiplicative)" },
	{ "power",                  "1.0,1.0,1.0",     4, "", "", "",     "signal power value (exponential)" },
	{ "floor",                  "0.0,0.0,0.0",     4, "", "", "",     "signal floor level" },
	{ "phosphor_life",          "0.0,0.0,0.0",     4, "", "", "",     "phosphorescence decay rate (0.0 is instant, 1.0 is forever)" },
	{ "saturation",             "1.0",             2, "0", "0", "0",      "saturation scaling value" },
	{ "yiq_enable",             "0",               0, "", "", "",    "enables YIQ-space HLSL post-processing" },
	{ "yiq_jitter",             "0.0",             2, "0", "0", "0",      "Jitter for the NTSC signal processing" },
	{ "yiq_cc",                 "3.57954545",      2, "0", "0", "0",      "Color Carrier frequency for NTSC signal processing" },
	{ "yiq_a",                  "0.5",             2, "0", "0", "0",      "A value for NTSC signal processing" },
	{ "yiq_b",                  "0.5",             2, "0", "0", "0",      "B value for NTSC signal processing" },
	{ "yiq_o",                  "0.0",             2, "0", "0", "0",      "Outgoing Color Carrier phase offset for NTSC signal processing" },
	{ "yiq_p",                  "1.0",             2, "0", "0", "0",      "Incoming Pixel Clock scaling value for NTSC signal processing" },
	{ "yiq_n",                  "1.0",             2, "0", "0", "0",      "Y filter notch width for NTSC signal processing" },
	{ "yiq_y",                  "6.0",             2, "0", "0", "0",      "Y filter cutoff frequency for NTSC signal processing" },
	{ "yiq_i",                  "1.2",             2, "0", "0", "0",      "I filter cutoff frequency for NTSC signal processing" },
	{ "yiq_q",                  "0.6",             2, "0", "0", "0",      "Q filter cutoff frequency for NTSC signal processing" },
	{ "yiq_scan_time",          "52.6",            2, "0", "0", "0",      "Horizontal scanline duration for NTSC signal processing (in usec)" },
	{ "yiq_phase_count",        "2",               1, "0", "0", "0",    "Phase Count value for NTSC signal processing" },
	{ "vector_beam_smooth",     "0.0",             2, "0", "0", "0",      "The vector beam smoothness" },
	{ "vector_length_scale",    "0.5",             2, "0", "0", "0",      "The maximum vector attenuation" },
	{ "vector_length_ratio",    "0.5",             2, "0", "0", "0",      "The minimum vector length (vector length to screen size ratio) that is affected by the attenuation" },
	{ "bloom_blend_mode",       "0",               1, "0", "0", "0",    "bloom blend mode (0 for brighten, 1 for darken)" },
	{ "bloom_scale",            "0.0",             2, "0", "0", "0",      "Intensity factor for bloom" },
	{ "bloom_overdrive",        "1.0,1.0,1.0",     4, "", "", "",     "Overdrive factor for bloom" },
	{ "bloom_lvl0_weight",      "1.0",             2, "0", "0", "0",      "Bloom level 0 weight (full-size target)" },
	{ "bloom_lvl1_weight",      "0.64",            2, "0", "0", "0",      "Bloom level 1 weight (1/4 smaller that level 0 target)" },
	{ "bloom_lvl2_weight",      "0.32",            2, "0", "0", "0",      "Bloom level 2 weight (1/4 smaller that level 1 target)" },
	{ "bloom_lvl3_weight",      "0.16",            2, "0", "0", "0",      "Bloom level 3 weight (1/4 smaller that level 2 target)" },
	{ "bloom_lvl4_weight",      "0.08",            2, "0", "0", "0",      "Bloom level 4 weight (1/4 smaller that level 3 target)" },
	{ "bloom_lvl5_weight",      "0.06",            2, "0", "0", "0",      "Bloom level 5 weight (1/4 smaller that level 4 target)" },
	{ "bloom_lvl6_weight",      "0.04",            2, "0", "0", "0",      "Bloom level 6 weight (1/4 smaller that level 5 target)" },
	{ "bloom_lvl7_weight",      "0.02",            2, "0", "0", "0",      "Bloom level 7 weight (1/4 smaller that level 6 target)" },
	{ "bloom_lvl8_weight",      "0.01",            2, "0", "0", "0",      "Bloom level 8 weight (1/4 smaller that level 7 target)" },
	{ "triplebuffer",           "0",               0, "", "", "",    "enables triple buffering" },
	{ "full_screen_brightness", "1.0",             2, "0", "0", "0",      "brightness value in full screen mode" },
	{ "full_screen_contrast",   "1.0",             2, "0", "0", "0",      "contrast value in full screen mode" },
	{ "full_screen_gamma",      "1.0",             2, "0", "0", "0",      "gamma value in full screen mode" },
	{ "global_inputs",          "0",               0, "", "", "",    "enables global inputs" },
	{ "dual_lightgun",          "0",               0, "", "", "",    "enables dual lightgun input" },
	{ "$end" }
};

class winui_ini_options
{
	std::map<string, INIOPTS> map_struct;
	std::map<string, string> map_default;
	std::map<string, string> map_ini;
	std::map<string, string> map_merge;
	std::map<string, string> map_done;
	string t_inipath; // temporary
	string m_inipath; // used for file saving
	string m_gamename;
	int m_gamenum;
	OPTIONS_TYPE m_opttype;
	string m_emuname;

	void trim_spaces_and_quotes(string &data)
	{
		// trim any whitespace
		strtrimspace(data);

		// trim quotes
		if (data.find_first_of('"') == 0 && data.find_last_of('"') == data.length() - 1)
		{
			data.erase(0, 1);
			data.erase(data.length() - 1, 1);
		}
	}

	// convert all other numbers, up to end-of-string/invalid-character. If number is too large, return 0.
	uint32_t convert_to_uint(const char* inp) // not used
	{
		if (!inp)
			return 0;
		uint32_t oup = 0;
		for (int i = 0; i < 11; i++)
		{
			int c = inp[i];
			if (c >= 0x30 && c <= 0x39)
				oup = oup * 10 + (c - 0x30);
			else
				return oup;
		}
		return 0; // numeric overflow
	}

	// true = there is data to process
	// action: true = accept any names; false = names must be in main struct
	bool read_ini(std::ifstream &fp, bool action)
	{
		map_ini.clear(); // start with a clean slate

		// does file exist?
		if (!fp.good())
			return false;

		string file_line, optdata, name, data;
		std::getline(fp, file_line);
		while (fp.good())
		{
			bool ok = false;
			size_t pos = file_line.find_first_of(" ");
			if (pos == string::npos)
			{
				name = file_line;
				data = "";
			}
			else
			{
				name = file_line.substr(0, pos);
				data = file_line.substr(pos);
			}
			if (name.length())
			{
				if (name[0] >= 'a' && name[0] <= 'z')
				{
					if (action)  // accept anything (slots & software)
						ok = true;
					else
					if (map_default.count(string(name))) // standard options only
						ok = true;

					if (ok)
					{
						trim_spaces_and_quotes(data);
						map_ini[string(name)] = data;
					}
				}
			}
			std::getline(fp, file_line);
		}

		fp.close();
		return true;
	}

	void merge_ini(string filename, bool action)
	{
		bool ok = true;
		string fname = t_inipath + "\\" + filename + ".ini";
		std::ifstream infile (fname.c_str());
		if (read_ini (infile, action))
			if (map_ini.count("readconfig")) // specifically need "readconfig 0" to prevent reading
				if (map_ini.find("readconfig")->second == "0")
					ok = false;

		if (ok)
		{
			for (auto const &it : map_ini)
				map_merge[it.first] = it.second;
					//map_ini.insert(map_merge.begin(),map_merge.end());
					//std::swap(map_merge, map_ini);
			if (map_ini.find("inipath") != map_ini.end())
				if (map_ini.find("inipath")->second != "")
					t_inipath = map_ini.find("inipath")->second;
		}
		map_ini.clear();
	}

	// get ini settings to the requested game
	// action: false = get diff ; true = get the game
	void read_merged_ini(int action)
	{
		t_inipath = ".";
		map_merge = map_default;
		// only wanted defaults
		if (action == 2)
			return;
		string filename = m_emuname;
		merge_ini(filename, false);
//		merge_ini(filename, false);
		// only wanted global settings
		if (action == 3)
			return;
		const game_driver *drv = &driver_list::driver(m_gamenum);
		if (!drv)
			return;
		// orientation
		if (drv->flags & ORIENTATION_SWAP_XY)
			filename = "vertical";
		else
			filename = "horizont";
		merge_ini(filename, false);
		// machine classification
		switch (drv->flags & machine_flags::MASK_TYPE)
		{
			case machine_flags::TYPE_ARCADE: filename = "arcade"; break;
			case machine_flags::TYPE_CONSOLE: filename = "console"; break;
			case machine_flags::TYPE_COMPUTER: filename = "computer"; break;
			case machine_flags::TYPE_OTHER: filename = "othersys"; break;
			default:break;
		}
		merge_ini(filename, false);
		// screen type
		machine_config config(*drv, MameUIGlobal());
		for (const screen_device &device : screen_device_iterator(config.root_device()))
		{
			// parse "raster.ini" for raster games
			if (device.screen_type() == SCREEN_TYPE_RASTER)
			{
				merge_ini("raster", false);
				break;
			}
			// parse "vector.ini" for vector games
			if (device.screen_type() == SCREEN_TYPE_VECTOR)
			{
				merge_ini("vector", false);
				break;
			}
			// parse "lcd.ini" for lcd games
			if (device.screen_type() == SCREEN_TYPE_LCD)
			{
				merge_ini("lcd", false);
				break;
			}
		}
		// source
		string sourcename = core_filename_extract_base(drv->type.source(), true).insert(0, "source" PATH_SEPARATOR);
		merge_ini(sourcename, false);
		if (m_opttype == OPTIONS_SOURCE)
			return;

		// then parse the grandparent, parent, and system-specific INIs
		int parent = driver_list::clone(*drv);
		int gparent = (parent != -1) ? driver_list::clone(parent) : -1;
		if (gparent != -1)
			merge_ini(driver_list::driver(gparent).name, false);
		if (parent != -1)
			merge_ini(driver_list::driver(parent).name, false);
		// if getting diff leave now
		if (action == 0)
			return;
		// at last, the actual game... and we want any preloaded slots & software
		merge_ini(drv->name, true);
	}

public:
	// construction/destruction
	winui_ini_options()
	{
		for (int i = 0; optentries[i].optname != "$end"; i++)
		{
			// set up indexed struct
			map_struct[optentries[i].optname] = optentries[i];
			// set up default settings
			map_default[optentries[i].optname] = optentries[i].optvalue;
		}
		m_gamenum = 0;
		t_inipath = ".";

		//printf("*** START DUMP OF DEFAULT ***\n");
		//for (auto const &it : map_struct)
			//printf("%s = %s\n", it.first.c_str(), it.second.optmin.c_str());
		//for (auto const &it : map_default)
			//printf("%s = %s\n", it.first.c_str(), it.second.c_str());
		//printf("*** END DUMP OF DEFAULT ***\n");
	}

	// we ignore the writeconfig flag
	void save_diff_ini()
	{
		bool file_ok = false;

		// get the diff
		if (m_opttype == OPTIONS_GAME)
			read_merged_ini(0);
		else
		if (m_opttype == OPTIONS_GLOBAL)
			read_merged_ini(2);
		else
			read_merged_ini(3);
		// now, map_merge has the diff, and map_done has the current to be saved
		// save lines that are different between them, also save any extra lines found in map_done, to game.ini

		// initialise with UTF-8 BOM
		string inistring = "\xef\xbb\xbf\n";
		for (auto const &it : map_done)
		{
			bool ok = true;
			if (map_merge.count(it.first)) // ==if key exists
				if (map_merge.find(it.first)->second == it.second)
					ok = false;

			if (ok)
			{
				inistring.append(it.first).append(" ").append(it.second).append("\n");
				file_ok = true;
			}
		}

		map_merge.clear();
		string fname = m_inipath + "\\" + m_gamename + ".ini";

		if (m_opttype == OPTIONS_GLOBAL)
			fname = m_gamename + ".ini";
		else
		if (m_opttype == OPTIONS_SOURCE)
			fname = m_inipath + "\\source\\" + m_gamename + ".ini";

		const char* filename = fname.c_str();
printf("Saving to: %s, file_ok = %d\n", filename,file_ok);
//		if (file_ok)
		{printf("inistring=%s\n",inistring.c_str());
			std::ofstream outfile (filename, std::ios::out | std::ios::trunc);
			size_t size = inistring.size();
			char t1[size+1];
			strcpy(t1, inistring.c_str());
			outfile.write(t1, size);
			outfile.close();
		}
//		else
//			unlink (filename);

		return;
	}

	// select which game to work with
	// This sets up everything for if we want to save changes
	void setgame(int gamenum, OPTIONS_TYPE opttype)
	{
		map_done.clear();
		map_merge.clear();
		m_emuname = emulator_info::get_configname();
		m_opttype = opttype;
		if (opttype == OPTIONS_GAME || opttype == OPTIONS_SOURCE)
		{
			m_gamenum = gamenum;
			m_gamename = driver_list::driver(gamenum).name;
			read_merged_ini(1);
		}
		else
		{
			switch(opttype)
			{
				case OPTIONS_GLOBAL:     m_gamename = m_emuname; break;
				case OPTIONS_HORIZONTAL: m_gamename = "horizont"; break;
				case OPTIONS_VERTICAL:   m_gamename = "vertical"; break;
				case OPTIONS_RASTER:     m_gamename = "raster"; break;
				case OPTIONS_VECTOR:     m_gamename = "vector"; break;
				case OPTIONS_LCD:        m_gamename = "lcd"; break;
				case OPTIONS_ARCADE:     m_gamename = "arcade"; break;
				case OPTIONS_CONSOLE:    m_gamename = "console"; break;
				case OPTIONS_COMPUTER:   m_gamename = "computer"; break;
				case OPTIONS_OTHERSYS:   m_gamename = "othersys"; break;
				default: m_gamename = ""; printf("iniopts: We shouldn't be here.\n"); return;
			}
			// for these special ones, read default then mame.ini then this one
			read_merged_ini(3);
			if (opttype != OPTIONS_GLOBAL)
				merge_ini(m_gamename, false);
		}
		map_done = std::move(map_merge);
		map_merge.clear();
		m_inipath = t_inipath;
	}

	// save a setting
	void setter(string name, string value)
	{
		map_done[name] = value;
	}

	// read a setting
	string getter(string name)
	{printf("Getting: %s\n",name.c_str());fflush(stdout);
		if (map_done.count(name)>0)
			return map_done.find(name)->second;
		else
			return "";
	}

#if 0
	void setter(const char* name, int value) // not used
	{
		map_merge[name] = std::to_string(value);
		//save_file(m_filename);
	}

	int int_value(const char* name) // not used
	{
		return std::stoi(getter(name));
	}

	bool bool_value(const char* name) // not used
	{
		return int_value(name) ? 1 : 0;
	}
#endif
};

#endif //  INI_OPTS_H

