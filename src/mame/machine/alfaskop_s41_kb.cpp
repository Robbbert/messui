// license:BSD-3-Clause
// copyright-holders: Joakim Larsson Edström
/**********************************************************************

  Alfaskop 4110 keyboard, emulation of the KeyBoard Unit KBU 4140.  which often
  was attached to the numerical Keyboard eXpansion Unit KXE 4141 which is not
  emulated yet. There is also an optional Magnetic Identification Device MID 4131

  TTL-level bi-directional serial matrix keyboard

  The KBU 4140 keyboard is controlled by a MC6802@3'579'000Hz in combo with a MC6846.
  The key matrix is scanned with the help of two 74LS164 which is also extended to the
  KXE 4141. The columns are read through a 74LS240. There are 8 LEDs which are
  latched by a 74LS374. The I/O port on the MC6846 is connected to a magnet strip
  reader through a MIA interface

  The keyboard serial interface line is interpreted and generated by the timer in
  the MC6846 where both TX and Rx shares the same line on the keyboard side. The
  keyboard remains silent until spoken to by the host computer to avoid collisions.

 Credits
 -------
 The MC6846 ROM and address decoder ROM were dumped by Dalby Datormuseum whom
 also provided documentation and schematics of the keyboard

   https://sites.google.com/site/dalbydatormuseum/home

 **********************************************************************/

#include "emu.h"
#include "alfaskop_s41_kb.h"
#include "cpu/m6800/m6800.h"

//**************************************************************************
//  CONFIGURABLE LOGGING
//**************************************************************************
#define LOG_IRQ     (1U <<  1)
#define LOG_RESET   (1U <<  2)
#define LOG_BITS    (1U <<  3)
#define LOG_UI      (1U <<  4)
#define LOG_LEDS    (1U <<  5)
#define LOG_ROM     (1U <<  6)
#define LOG_ADEC    (1U <<  7)
#define LOG_IO      (1U <<  8)

//#define VERBOSE (LOG_GENERAL|LOG_IO|LOG_IRQ|LOG_LEDS|LOG_BITS|LOG_RESET)
//#define LOG_OUTPUT_STREAM std::cout

#include "logmacro.h"

#define LOGIRQ(...)   LOGMASKED(LOG_IRQ,   __VA_ARGS__)
#define LOGRST(...)   LOGMASKED(LOG_RESET, __VA_ARGS__)
#define LOGBITS(...)  LOGMASKED(LOG_BITS,  __VA_ARGS__)
#define LOGUI(...)    LOGMASKED(LOG_UI,    __VA_ARGS__)
#define LOGLEDS(...)  if (!machine().side_effects_disabled()) LOGMASKED(LOG_LEDS,  __VA_ARGS__)
#define LOGROM(...)   if (!machine().side_effects_disabled()) LOGMASKED(LOG_ROM,   __VA_ARGS__)
#define LOGADEC(...)  if (!machine().side_effects_disabled()) LOGMASKED(LOG_ADEC,  __VA_ARGS__)
#define LOGIO(...)    if (!machine().side_effects_disabled()) LOGMASKED(LOG_IO,    __VA_ARGS__)

//**************************************************************************
//  MACROS / CONSTANTS
//**************************************************************************

#define M6800_TAG       "mcu"

namespace {

INPUT_PORTS_START(alfaskop_s41_kb)
	PORT_START("P15")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("KP 6")    PORT_CODE(KEYCODE_6_PAD) PORT_CODE(KEYCODE_RIGHT) PORT_CHAR('6') PORT_CHAR(UCHAR_MAMEKEY(RIGHT)) // 77
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("KP +")    PORT_CODE(KEYCODE_PLUS_PAD) PORT_CHAR('+')                                  // 78
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("KP 5")    PORT_CODE(KEYCODE_5_PAD)    PORT_CHAR('5')                                  // 76
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("* PRINT") PORT_CODE(KEYCODE_TILDE)    PORT_CHAR('*') PORT_CHAR(UCHAR_MAMEKEY(PRTSCR)) // 55
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("R Shift") PORT_CODE(KEYCODE_RSHIFT)   PORT_CHAR(UCHAR_SHIFT_1)                        // 54
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_KEYBOARD )                      PORT_CODE(KEYCODE_MINUS)    PORT_CHAR('-') PORT_CHAR('_')                   // 53
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME(". :")     PORT_CODE(KEYCODE_STOP)     PORT_CHAR('.') PORT_CHAR(':')                   // 52
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("F5")      PORT_CODE(KEYCODE_F5)       PORT_CHAR(UCHAR_MAMEKEY(F5))                    // 63
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("F6")      PORT_CODE(KEYCODE_F6)       PORT_CHAR(UCHAR_MAMEKEY(F6))                    // 64
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_UNUSED )                                                                                                    // no scancode is sent
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("CTRL")    PORT_CODE(KEYCODE_LCONTROL) PORT_CHAR(UCHAR_MAMEKEY(LCONTROL))              // 29
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME(", ;")     PORT_CODE(KEYCODE_COMMA)    PORT_CHAR(',') PORT_CHAR(';')                   // 51
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_KEYBOARD )                      PORT_CODE(KEYCODE_D)        PORT_CHAR('D') PORT_CHAR('d')                   // 32
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_KEYBOARD )                      PORT_CODE(KEYCODE_X)        PORT_CHAR('X') PORT_CHAR('x')                   // 45
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_KEYBOARD )                      PORT_CODE(KEYCODE_C)        PORT_CHAR('C') PORT_CHAR('c')                   // 46
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_KEYBOARD )                      PORT_CODE(KEYCODE_J)        PORT_CHAR('J') PORT_CHAR('j')                   // 36

	PORT_START("P14")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_UNUSED )                                                                                                        // 00 - keyboard error
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("BREAK")   PORT_CODE(KEYCODE_PAUSE)     PORT_CHAR(UCHAR_MAMEKEY(PAUSE))                    // 70
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("KP 7")    PORT_CODE(KEYCODE_7_PAD)     PORT_CHAR('7') PORT_CHAR(UCHAR_MAMEKEY(HOME))      // 71
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_UNUSED )                                                                                                        // ff - keyboard error
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_KEYBOARD )                      PORT_CODE(KEYCODE_TILDE)     PORT_CHAR('^') PORT_CHAR('~') PORT_CHAR(']')       // 27
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_KEYBOARD )                      PORT_CODE(KEYCODE_OPENBRACE) PORT_CHAR(0x00e5) PORT_CHAR(0x00c5) PORT_CHAR('[') // 26 å Å
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_KEYBOARD )                      PORT_CODE(KEYCODE_P)         PORT_CHAR('P') PORT_CHAR('p')                      // 25
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("F1")      PORT_CODE(KEYCODE_F1)        PORT_CHAR(UCHAR_MAMEKEY(F1))                       // 59
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("F2")      PORT_CODE(KEYCODE_F2)        PORT_CHAR(UCHAR_MAMEKEY(F2))                       // 60
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_KEYBOARD )                      PORT_CODE(KEYCODE_W)         PORT_CHAR('w') PORT_CHAR('W')                      // 17
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_KEYBOARD )                      PORT_CODE(KEYCODE_E)         PORT_CHAR('e') PORT_CHAR('E')                      // 18
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_KEYBOARD )                      PORT_CODE(KEYCODE_O)         PORT_CHAR('o') PORT_CHAR('O')                      // 24
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_KEYBOARD )                      PORT_CODE(KEYCODE_R)         PORT_CHAR('r') PORT_CHAR('R')                      // 19
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_KEYBOARD )                      PORT_CODE(KEYCODE_T)         PORT_CHAR('t') PORT_CHAR('T')                      // 20
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_KEYBOARD )                      PORT_CODE(KEYCODE_Y)         PORT_CHAR('y') PORT_CHAR('Y')                      // 21
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_KEYBOARD )                      PORT_CODE(KEYCODE_I)         PORT_CHAR('i') PORT_CHAR('I')                      // 23

	PORT_START("P13")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_KEYBOARD )                     PORT_CODE(KEYCODE_NUMLOCK)      PORT_CHAR(UCHAR_MAMEKEY(NUMLOCK))        // 69
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_UNUSED )                                                                                                // ff - keyboard error
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("BS DEL") PORT_CODE(KEYCODE_BACKSPACE)  PORT_CHAR(8) PORT_CHAR(UCHAR_MAMEKEY(DEL)) // 14
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_KEYBOARD )                     PORT_CODE(KEYCODE_EQUALS)     PORT_CHAR('=') PORT_CHAR('+')              // 13
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_KEYBOARD )                     PORT_CODE(KEYCODE_MINUS)      PORT_CHAR('-') PORT_CHAR('_')              // 12
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_KEYBOARD )                     PORT_CODE(KEYCODE_0)          PORT_CHAR('0') PORT_CHAR(')')              // 11
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_KEYBOARD )                     PORT_CODE(KEYCODE_9)          PORT_CHAR('9') PORT_CHAR('(')              // 10
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_KEYBOARD )                     PORT_CODE(KEYCODE_1)          PORT_CHAR('1') PORT_CHAR('!')              // 02
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("ESC")    PORT_CODE(KEYCODE_ESC)        PORT_CHAR(UCHAR_MAMEKEY(ESC))              // 01
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_KEYBOARD )                     PORT_CODE(KEYCODE_2)          PORT_CHAR('2') PORT_CHAR('@')              // 03
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_KEYBOARD )                     PORT_CODE(KEYCODE_3)          PORT_CHAR('3') PORT_CHAR('#')              // 04
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_KEYBOARD )                     PORT_CODE(KEYCODE_8)          PORT_CHAR('8') PORT_CHAR('*')              // 09
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_KEYBOARD )                     PORT_CODE(KEYCODE_4)          PORT_CHAR('4') PORT_CHAR('$')              // 05
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_KEYBOARD )                     PORT_CODE(KEYCODE_5)          PORT_CHAR('5') PORT_CHAR('%')              // 06
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_KEYBOARD )                     PORT_CODE(KEYCODE_6)          PORT_CHAR('6') PORT_CHAR('^')              // 07
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_KEYBOARD )                     PORT_CODE(KEYCODE_7)          PORT_CHAR('7') PORT_CHAR('&')              // 08

	PORT_START("P12")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("KP 9")      PORT_CODE(KEYCODE_9_PAD)     PORT_CHAR('9') PORT_CHAR(UCHAR_MAMEKEY(PGUP))   // 73
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("KP -")      PORT_CODE(KEYCODE_MINUS_PAD) PORT_CHAR(UCHAR_MAMEKEY(MINUS_PAD))             // 74
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("KP 8")      PORT_CODE(KEYCODE_8_PAD) PORT_CODE(KEYCODE_UP) PORT_CHAR('8') PORT_CHAR(UCHAR_MAMEKEY(UP)) // 72
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_KEYBOARD )                        PORT_CODE(KEYCODE_TILDE)     PORT_CHAR('`') PORT_CHAR('~')       // 41
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_KEYBOARD )                        PORT_CODE(KEYCODE_QUOTE)     PORT_CHAR('\'') PORT_CHAR('"')      // 40
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_KEYBOARD )                        PORT_CODE(KEYCODE_COLON)     PORT_CHAR(';') PORT_CHAR(':')       // 39
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_KEYBOARD )                        PORT_CODE(KEYCODE_L)         PORT_CHAR('l') PORT_CHAR('L')       // 38
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("F3")        PORT_CODE(KEYCODE_F3)        PORT_CHAR(UCHAR_MAMEKEY(F3))        // 61
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("F4")        PORT_CODE(KEYCODE_F4)        PORT_CHAR(UCHAR_MAMEKEY(F4))        // 62
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_KEYBOARD )                        PORT_CODE(KEYCODE_Q)         PORT_CHAR('q') PORT_CHAR('Q')       // 16
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("TAB")       PORT_CODE(KEYCODE_TAB)       PORT_CHAR(9)                        // 15
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_KEYBOARD )                        PORT_CODE(KEYCODE_K)         PORT_CHAR('k') PORT_CHAR('K')       // 37
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_KEYBOARD )                        PORT_CODE(KEYCODE_F)         PORT_CHAR('f') PORT_CHAR('F')       // 33
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_KEYBOARD )                        PORT_CODE(KEYCODE_G)         PORT_CHAR('g') PORT_CHAR('G')       // 34
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_KEYBOARD )                        PORT_CODE(KEYCODE_H)         PORT_CHAR('h') PORT_CHAR('H')       // 35
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_KEYBOARD )                        PORT_CODE(KEYCODE_U)         PORT_CHAR('u') PORT_CHAR('U')       // 22

	PORT_START("P11")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_KEYBOARD )                     PORT_CODE(KEYCODE_DEL_PAD)       PORT_CHAR(UCHAR_MAMEKEY(COMMA_PAD))        // 83
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("RETURN") PORT_CODE(KEYCODE_ENTER)         PORT_CHAR(13)                              // 28
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("KP 0")    PORT_CODE(KEYCODE_0_PAD)        PORT_CHAR('0') PORT_CHAR(UCHAR_MAMEKEY(INSERT)) // 82
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_UNUSED )                                                                                                   // 89 - no key
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_UNUSED )                                                                                                   // 86 - no key
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_UNUSED )                                                                                                   // 87 - no key
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNUSED )                                                                                                   // 88 - no key
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("F9")         PORT_CODE(KEYCODE_F9)        PORT_CHAR(UCHAR_MAMEKEY(F9))               // 67
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("F10")        PORT_CODE(KEYCODE_F10)       PORT_CHAR(UCHAR_MAMEKEY(F10))              // 68
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_UNUSED )                                                                                                   // scan code ff - keyboard error
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_KEYBOARD )                         PORT_CODE(KEYCODE_BACKSLASH) PORT_CHAR('\\')                            // 43
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("CAPS LOCK")  PORT_CODE(KEYCODE_CAPSLOCK)  PORT_CHAR(UCHAR_MAMEKEY(CAPSLOCK))         // 58
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("SHIFT LOCK") PORT_CODE(KEYCODE_LALT)                                                 // 56
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_UNUSED )                                                                                                   // 85 - no key
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_KEYBOARD )                         PORT_CODE(KEYCODE_V)         PORT_CHAR('v') PORT_CHAR('V')              // 47
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_KEYBOARD )                         PORT_CODE(KEYCODE_SPACE)     PORT_CHAR(' ')                             // 57

	PORT_START("P10")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("KP 3")     PORT_CODE(KEYCODE_3_PAD) PORT_CODE(KEYCODE_PGDN)                           // 81
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_UNUSED )                                                                                                    // ff - keyboard error
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("KP 2")     PORT_CODE(KEYCODE_2_PAD) PORT_CODE(KEYCODE_DOWN) PORT_CHAR(UCHAR_MAMEKEY(2_PAD)) // 80
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("NEW LINE") PORT_CODE(KEYCODE_ENTER_PAD)  PORT_CHAR(UCHAR_MAMEKEY(ENTER_PAD))          // 84 (programmable, default is 28)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("KP 1")     PORT_CODE(KEYCODE_1_PAD) PORT_CHAR(UCHAR_MAMEKEY(1_PAD))                   // 79
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("KP 4")     PORT_CODE(KEYCODE_4_PAD) PORT_CODE(KEYCODE_LEFT) PORT_CHAR('4') PORT_CHAR(UCHAR_MAMEKEY(LEFT))     // 75
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNUSED )                                                                                                    // ff - keyboard error
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("F7")       PORT_CODE(KEYCODE_F7)         PORT_CHAR(UCHAR_MAMEKEY(F7))                 // 65
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("F8")       PORT_CODE(KEYCODE_F8)         PORT_CHAR(UCHAR_MAMEKEY(F8))                 // 66
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_KEYBOARD )                       PORT_CODE(KEYCODE_LSHIFT) PORT_CHAR(UCHAR_SHIFT_1)                         // 42
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_KEYBOARD )                       PORT_CODE(KEYCODE_Z)          PORT_CHAR('z') PORT_CHAR('Z')                // 44
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_KEYBOARD )                       PORT_CODE(KEYCODE_M)          PORT_CHAR('m') PORT_CHAR('M')                // 50
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_KEYBOARD )                       PORT_CODE(KEYCODE_A)          PORT_CHAR('a') PORT_CHAR('A')                // 30
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_KEYBOARD )                       PORT_CODE(KEYCODE_S)          PORT_CHAR('s') PORT_CHAR('S')                // 31
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_KEYBOARD )                       PORT_CODE(KEYCODE_B)          PORT_CHAR('b') PORT_CHAR('B')                // 48
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_KEYBOARD )                       PORT_CODE(KEYCODE_N)          PORT_CHAR('n') PORT_CHAR('N')                // 49
INPUT_PORTS_END


//-------------------------------------------------
//  ROM( alfaskop_s41_kb )
//-------------------------------------------------

ROM_START( alfaskop_s41_kb )
	ROM_REGION( 0x800, M6800_TAG, 0 )
	ROM_LOAD( "kbc_e34066_0000_ic3.bin", 0x000, 0x800, CRC(a1232241) SHA1(45d1b038bfbd04e1b7e3718a27202280c5257989) )
	ROM_REGION( 0x20, "ad_rom", 0 ) // Address decoder
	ROM_LOAD( "kbc_e34066_0000_ic4_e3405970280400.bin", 0x00, 0x20, CRC(f26acca3) SHA1(ac8c607d50bb588c1da4ad1602665c785ebd29b3) )
ROM_END

} // anonymous namespace

//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

DEFINE_DEVICE_TYPE(ALFASKOP_S41_KB, alfaskop_s41_keyboard_device, "alfaskop_s41_kb", "Alfaskop 4110 keyboard")

//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  alfaskop_s41_keyboard_device - constructor
//-------------------------------------------------

alfaskop_s41_keyboard_device::alfaskop_s41_keyboard_device(
		machine_config const &mconfig,
		char const *tag,
		device_t *owner,
		uint32_t clock)
	: device_t(mconfig, ALFASKOP_S41_KB, tag, owner, clock)
	, m_mcu(*this, M6800_TAG)
	, m_mc6846(*this, "mc6846")
	, m_rows(*this, "P1%u", 0)
	, m_txd_cb(*this)
	, m_leds_cb(*this)
	, m_rxd_high(true)
	, m_txd_high(true)
	, m_hold(true)
	, m_col_select(0)
	, m_p1(0)
	, m_leds(0)
{
}

WRITE_LINE_MEMBER(alfaskop_s41_keyboard_device::rxd_w)
{
	LOGBITS("KBD bit presented: %d\n", state);
	//m_rxd_high = CLEAR_LINE != state;
}

// WRITE_LINE_MEMBER(alfaskop_s41_keyboard_device::hold_w)
// {
//  m_hold = CLEAR_LINE == state;
// }

WRITE_LINE_MEMBER(alfaskop_s41_keyboard_device::rst_line_w)
{
	if (state == CLEAR_LINE)
	{
		m_mcu->set_input_line(INPUT_LINE_RESET, CLEAR_LINE);
		LOGRST("KBD: Keyboard mcu reset line is cleared\n");
	}
	else
	{
		m_mcu->set_input_line(INPUT_LINE_RESET, ASSERT_LINE);
		LOGRST("KBD: Keyboard mcu reset line is asserted\n");
	}
}

//-------------------------------------------------
//  device_start - device specific startup
//-------------------------------------------------

void alfaskop_s41_keyboard_device::device_start()
{
	m_txd_cb.resolve_safe();
	m_leds_cb.resolve_safe();

	save_item(NAME(m_rxd_high));
	save_item(NAME(m_txd_high));
	save_item(NAME(m_col_select));

	m_rxd_high = true;
	m_txd_high = true;
	m_col_select = 0;
}



//-------------------------------------------------
//  device_add_mconfig - add device configuration
//-------------------------------------------------

void alfaskop_s41_keyboard_device::device_add_mconfig(machine_config &config)
{
	M6802(config, m_mcu, XTAL(3'579'545)); // Crystal verified from schematics
	m_mcu->set_addrmap(AS_PROGRAM, &alfaskop_s41_keyboard_device::alfaskop_s41_kb_mem);

	MC6846(config, m_mc6846, XTAL(3'579'545) / 4); // Driven by E from 6802 == XTAL / 4
	m_mc6846->irq().set([this](bool state)
			{
				LOGIRQ(" [timer IRQ: %s] ", state == ASSERT_LINE ? "asserted" : "cleared");
				m_mcu->set_input_line(M6802_IRQ_LINE, state);
			});
	m_mc6846->cp2().set([this](bool state){ LOG("CP2:%d\n", state); });
	// MIA ID interrupt: m_mc6846->set_input_cp1(1);
	m_mc6846->cto().set([this](bool state){ LOG("CTO:%d\n", state); }); // Not connected to anything
}

//-------------------------------------------------
//  input_ports - device-specific input ports
//-------------------------------------------------

ioport_constructor alfaskop_s41_keyboard_device::device_input_ports() const
{
	return INPUT_PORTS_NAME( alfaskop_s41_kb );
}

//-------------------------------------------------
//  ADDRESS_MAP( alfaskop_s41_kb_mem )
//-------------------------------------------------

void alfaskop_s41_keyboard_device::alfaskop_s41_kb_mem(address_map &map)
{
	map(0x0000, 0x007f).ram(); // Internal RAM
	/* A 32 byte decoder ROM connects A0-A4 to A11, A12, A13, A15 and to an external port pin, A14 is not decoded!
	 * c3 c3 c3 c3 c3 c3 c3 c3 c3 c3 c3 c3 c3 c3 c3 c3 - if external port pin is low
	 * c3 c3 c3 c3 c5 c6 cb c3 d3 c3 c3 c3 c3 c3 43 f3 - if external port pin is high - normal operation
	 *
	 * Decoder rom byte bits as follows
	 * 0 - 0x01 Start scanning
	 * 1 - 0x02 read col
	     * 2 - 0x04 shift col clock
	 * 3 - 0x08 set lamp data
	 * 4 - 0x10 CS1 mc6846 I/O access - active low
	 * 5 - 0x20 CS0 mc6846 ROM access - active high
	 * 6 - 0x40 n/c?
	 * 7 - 0x80 to KBX
	 *
	 * F3 <- 0x0f <- 1-11 1 == b800-bfff, f800-ffff // 1111 0011 MC6846 Internal ROM access (CS0 - active high)
	 * 43 <- 0x0e <- 1-11 0 == b000-b7ff, f000-f7ff // 0100 0011
	 * c3 <- 0x0d <- 1-10 1 == a800-afff, e800-efff // 1100 0011
	 * c3 <- 0x0c <- 1-10 0 == a000-a7ff, e000-e7ff // 1100 0011
	 * c3 <- 0x0b <- 1-01 1 == 9800-9fff, d800-dfff // 1100 0011
	 * c3 <- 0x0a <- 1-01 0 == 9000-97ff, d000-d7ff // 1100 0011
	 * c3 <- 0x09 <- 1-00 1 == 8800-8fff, c800-cfff // 1100 0011
	 * d3 <- 0x08 <- 1-00 0 == 8000-87ff, c000-c7ff // 1101 0011 MC6846 Internal I/O access (CS1 - active high)

	 * c3 <- 0x07 <- 0-11 1 == 3800-3fff, 7800-7fff // 1100 0011
	 * cb <- 0x06 <- 0-11 0 == 3000-37ff, 7000-77ff // 1100 1011 Set lamp data
	 * c6 <- 0x05 <- 0-10 1 == 2800-2fff, 6800-6fff // 1100 0110 start scan
	 * c5 <- 0x04 <- 0-10 0 == 2000-27ff, 6000-67ff // 1100 0101 read col
	 * c3 <- 0x03 <- 0-01 1 == 1800-1fff, 5800-5fff // 1100 0011
	 * c3 <- 0x02 <- 0-01 0 == 1000-17ff, 5000-57ff // 1100 0011
	 * c3 <- 0x01 <- 0-00 1 == 0800-0fff, 4800-4fff // 1100 0011
	 * c3 <- 0x00 <- 0-00 0 == 0000-07ff, 4000-47ff // 1100 0011
	 */
	map(0x0080, 0xffff).lrw8(NAME([this](offs_t offset) -> uint8_t
				{
					uint16_t addr = offset + 0x80;
					uint8_t index = 0x10 | ((((addr & 0x8000) | ((addr & 0x3800) << 1)) >> 12) & 0x000f); // 0x10 | (((A15 | A13 | A12 | A11) >> 12)
					uint8_t ret = 0;
					LOGADEC("address_r %04x -> index %02x: %02x\n", offset + 0x80, index, memregion("ad_rom")->base ()[index]);
					switch (memregion("ad_rom")->base ()[index])
					{
					case 0xf3:
						ret = memregion(M6800_TAG)->base ()[addr & 0x07ff];
						LOGROM(" - ROM %04x: %02x\n", addr & 0x7ff, ret);
						break;
					case 0xd3:
						LOGIO(" - I/O read mc6846 %04x", addr & 0x07);
						ret = m_mc6846->read((offs_t)(addr & 0x07));
						LOGIO(": %02x\n", ret);
						break;
					case 0xc3:
						break; // Idle
					default:
						logerror(" - unmapped read access at %04x through %02x\n", addr, memregion("ad_rom")->base ()[index]);
					}
					return ret;
				}),
				NAME( [this](offs_t offset, uint8_t data)
				{
					uint16_t addr = offset + 0x80;
					uint8_t index = 0x10 | ((((addr & 0x8000) | ((addr & 0x3800) << 1)) >> 12) & 0x000f); // 0x10 | (((A15 | A13 | A12 | A11) >> 12)
					LOGADEC("address_w %04x -> index %02x: %02x\n", offset + 0x80, index, memregion("ad_rom")->base ()[index]);
					switch (memregion("ad_rom")->base ()[index])
					{
					case 0xd3:
						LOGIO(" - I/O write mc6846 %04x, %02x\n", addr & 0x07, data);
						m_mc6846->write((offs_t)(addr & 0x07), data);
						break;
					case 0xcb: // Leds
						if (m_leds != data)
						{
							m_leds_cb(data); m_leds = data;
							LOGLEDS(" - I/O write LEDs %04x, %02x\n", addr, data);
						}
						break;
					case 0xc3:
						break; // Idle
					default:
						logerror(" - unmapped write access at %04x, %02x through %02x\n", addr, data, memregion("ad_rom")->base ()[index]);
					}
				}));
				  //    map(0xf800, 0xffff).rom().region(M6800_TAG,0);
}

//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const tiny_rom_entry *alfaskop_s41_keyboard_device::device_rom_region() const
{
	return ROM_NAME( alfaskop_s41_kb );
}
