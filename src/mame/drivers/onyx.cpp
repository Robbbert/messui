// license:BSD-3-Clause
// copyright-holders:Robbbert
/***********************************************************************************************

Onyx C8002

2013-08-18 Skeleton Driver

The C8002 is one of the earliest minicomputers to use Unix as an operating system.

The system consists of a main CPU (Z8002), and a slave CPU for Mass Storage control (Z80)

The Z80 board contains a 19.6608 and 16 MHz crystals; 2x Z80CTC; 3x Z80SIO/0; Z80DMA; 3x Z80PIO;
2 eproms marked 459-3 and 460-3, plus 2 proms.

The Z8002 board contains a 16 MHz crystal; 3x Z80CTC; 5x Z80SIO/0; 3x Z80PIO; 2 eproms marked
466-E and 467E, plus the remaining 7 small proms.

The system can handle 8 RS232 terminals, 7 hard drives, a tape cartridge drive, parallel i/o,
and be connected to a RS422 network.

Status:
- Main screen prints an error with CTC (because there's no clock into it atm)
- Subcpu screen (after a while) prints various statuses then waits for the fdc to respond

To Do:
- Hook up daisy chains (see p8k.cpp for how to hook up a 16-bit chain)
  (keyboard input depends on interrupts)
- Remaining devices
- Whatever hooks up to the devices
- Eventually we'll need software
- Manuals / schematics would be nice

*************************************************************************************************/

#include "emu.h"
#include "cpu/z80/z80.h"
#include "cpu/z8000/z8000.h"
#include "machine/clock.h"
#include "bus/rs232/rs232.h"
//#include "machine/z80daisy.h"
#include "machine/z80ctc.h"
#include "machine/z80pio.h"
#include "machine/z80sio.h"
//#include "machine/z80dma.h"

class onyx_state : public driver_device
{
public:
	onyx_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_sio(*this, "sio%u", 1U)
		, m_ctc(*this, "ctc%u", 1U)
		, m_pio(*this, "pio%u", 1U)
	{ }

	void c8002(machine_config &config);
	void c5000(machine_config &config);
	void c5000_io(address_map &map);
	void c5000_mem(address_map &map);
	void c8002_io(address_map &map);
	void c8002_mem(address_map &map);
	void subio(address_map &map);
	void submem(address_map &map);
private:
	DECLARE_MACHINE_RESET(c8002);
	void z8002_m1_w(uint8_t data);

	required_device<cpu_device> m_maincpu;
	optional_device_array<z80sio_device, 5> m_sio;
	optional_device_array<z80ctc_device, 3> m_ctc;
	optional_device_array<z80pio_device, 2> m_pio;
};


/* Input ports */
static INPUT_PORTS_START( c8002 )
INPUT_PORTS_END


MACHINE_RESET_MEMBER(onyx_state, c8002)
{
}

void onyx_state::c8002_mem(address_map &map)
{
	map(0x00000, 0x00fff).rom().share("share0");
	map(0x01000, 0x07fff).ram().share("share1");
	map(0x08000, 0x0ffff).ram().share("share2"); // Z8002 has 64k memory
}

void onyx_state::z8002_m1_w(uint8_t data)
{
	// ED 4D (Z80 RETI opcode) is written here
	for (auto &sio : m_sio)
		sio->z80daisy_decode(data);
	for (auto &ctc : m_ctc)
		ctc->z80daisy_decode(data);
	for (auto &pio : m_pio)
		pio->z80daisy_decode(data);
}

void onyx_state::c8002_io(address_map &map)
{
	map(0xff00, 0xff07).lrw8("sio1_rw", [this](address_space &space, offs_t offset, u8 mem_mask) { return m_sio[0]->cd_ba_r(space, offset >> 1, mem_mask); }, [this](address_space &space, offs_t offset, u8 data, u8 mem_mask) { m_sio[0]->cd_ba_w(space, offset >> 1, data, mem_mask); });
	map(0xff08, 0xff0f).lrw8("sio2_rw", [this](address_space &space, offs_t offset, u8 mem_mask) { return m_sio[1]->cd_ba_r(space, offset >> 1, mem_mask); }, [this](address_space &space, offs_t offset, u8 data, u8 mem_mask) { m_sio[1]->cd_ba_w(space, offset >> 1, data, mem_mask); });
	map(0xff10, 0xff17).lrw8("sio3_rw", [this](address_space &space, offs_t offset, u8 mem_mask) { return m_sio[2]->cd_ba_r(space, offset >> 1, mem_mask); }, [this](address_space &space, offs_t offset, u8 data, u8 mem_mask) { m_sio[2]->cd_ba_w(space, offset >> 1, data, mem_mask); });
	map(0xff18, 0xff1f).lrw8("sio4_rw", [this](address_space &space, offs_t offset, u8 mem_mask) { return m_sio[3]->cd_ba_r(space, offset >> 1, mem_mask); }, [this](address_space &space, offs_t offset, u8 data, u8 mem_mask) { m_sio[3]->cd_ba_w(space, offset >> 1, data, mem_mask); });
	map(0xff20, 0xff27).lrw8("sio5_rw", [this](address_space &space, offs_t offset, u8 mem_mask) { return m_sio[4]->cd_ba_r(space, offset >> 1, mem_mask); }, [this](address_space &space, offs_t offset, u8 data, u8 mem_mask) { m_sio[4]->cd_ba_w(space, offset >> 1, data, mem_mask); });
	map(0xff30, 0xff37).lrw8("ctc1_rw", [this](address_space &space, offs_t offset, u8 mem_mask) { return m_ctc[0]->read(space, offset >> 1, mem_mask); }, [this](address_space &space, offs_t offset, u8 data, u8 mem_mask) { m_ctc[0]->write(space, offset >> 1, data, mem_mask); });
	map(0xff38, 0xff3f).lrw8("ctc2_rw", [this](address_space &space, offs_t offset, u8 mem_mask) { return m_ctc[1]->read(space, offset >> 1, mem_mask); }, [this](address_space &space, offs_t offset, u8 data, u8 mem_mask) { m_ctc[1]->write(space, offset >> 1, data, mem_mask); });
	map(0xff40, 0xff47).lrw8("ctc3_rw", [this](address_space &space, offs_t offset, u8 mem_mask) { return m_ctc[2]->read(space, offset >> 1, mem_mask); }, [this](address_space &space, offs_t offset, u8 data, u8 mem_mask) { m_ctc[2]->write(space, offset >> 1, data, mem_mask); });
	map(0xff50, 0xff57).lrw8("pio1_rw", [this](address_space &space, offs_t offset, u8 mem_mask) { return m_pio[0]->read(space, offset >> 1, mem_mask); }, [this](address_space &space, offs_t offset, u8 data, u8 mem_mask) { m_pio[0]->write(space, offset >> 1, data, mem_mask); });
	map(0xff58, 0xff5f).lrw8("pio2_rw", [this](address_space &space, offs_t offset, u8 mem_mask) { return m_pio[1]->read(space, offset >> 1, mem_mask); }, [this](address_space &space, offs_t offset, u8 data, u8 mem_mask) { m_pio[1]->write(space, offset >> 1, data, mem_mask); });
	map(0xffb9, 0xffb9).w(FUNC(onyx_state::z8002_m1_w));
}

void onyx_state::submem(address_map &map)
{
	map(0x0000, 0x0fff).rom();
	map(0x1000, 0xffff).ram();
}

void onyx_state::subio(address_map &map)
{
	map.global_mask(0xff);
	map(0x00, 0x03).rw("pio1s", FUNC(z80pio_device::read), FUNC(z80pio_device::write));
	map(0x04, 0x04).nopr();   // disk status?
	map(0x0c, 0x0f).rw("sio1s", FUNC(z80sio_device::cd_ba_r), FUNC(z80sio_device::cd_ba_w));
}

/***************************************************************************

    Machine Drivers

****************************************************************************/

MACHINE_CONFIG_START(onyx_state::c8002)
	/* basic machine hardware */
	MCFG_DEVICE_ADD("maincpu", Z8002, XTAL(4'000'000) )
	//MCFG_Z80_DAISY_CHAIN(main_daisy_chain)
	MCFG_DEVICE_PROGRAM_MAP(c8002_mem)
	//MCFG_DEVICE_DATA_MAP(c8002_data)
	MCFG_DEVICE_IO_MAP(c8002_io)

	MCFG_DEVICE_ADD("subcpu", Z80, XTAL(4'000'000) )
	//MCFG_Z80_DAISY_CHAIN(sub_daisy_chain)
	MCFG_DEVICE_PROGRAM_MAP(submem)
	MCFG_DEVICE_IO_MAP(subio)
	MCFG_MACHINE_RESET_OVERRIDE(onyx_state, c8002)

	clock_device &sio1_clock(CLOCK(config, "sio1_clock", 307200));
	sio1_clock.signal_handler().set(m_sio[0], FUNC(z80sio_device::rxca_w));
	sio1_clock.signal_handler().append(m_sio[0], FUNC(z80sio_device::txca_w));

	/* peripheral hardware */
	Z80PIO(config, m_pio[0], XTAL(16'000'000)/4);
	//m_pio[0]->out_int_callback().set_inputline(m_maincpu, INPUT_LINE_IRQ0);
	Z80PIO(config, m_pio[1], XTAL(16'000'000)/4);
	//m_pio[1]->out_int_callback().set_inputline(m_maincpu, INPUT_LINE_IRQ0);
	Z80CTC(config, m_ctc[0], XTAL(16'000'000) /4);
	Z80CTC(config, m_ctc[1], XTAL(16'000'000) /4);
	Z80CTC(config, m_ctc[2], XTAL(16'000'000) /4);
	Z80SIO(config, m_sio[0], XTAL(16'000'000) /4);
	m_sio[0]->out_txda_callback().set("rs232", FUNC(rs232_port_device::write_txd));
	m_sio[0]->out_dtra_callback().set("rs232", FUNC(rs232_port_device::write_dtr));
	m_sio[0]->out_rtsa_callback().set("rs232", FUNC(rs232_port_device::write_rts));
	Z80SIO(config, m_sio[1], XTAL(16'000'000) /4);
	Z80SIO(config, m_sio[2], XTAL(16'000'000) /4);
	Z80SIO(config, m_sio[3], XTAL(16'000'000) /4);
	Z80SIO(config, m_sio[4], XTAL(16'000'000) /4);

	MCFG_DEVICE_ADD("rs232", RS232_PORT, default_rs232_devices, "terminal")
	MCFG_RS232_RXD_HANDLER(WRITELINE("sio1", z80sio_device, rxa_w))
	MCFG_RS232_DCD_HANDLER(WRITELINE("sio1", z80sio_device, dcda_w))
	MCFG_RS232_CTS_HANDLER(WRITELINE("sio1", z80sio_device, ctsa_w))

	Z80PIO(config, "pio1s", XTAL(16'000'000)/4);
	//z80pio_device& pio1s(Z80PIO(config, "pio1s", XTAL(16'000'000)/4));
	//pio1s->out_int_callback().set_inputline("subcpu", INPUT_LINE_IRQ0);

	clock_device &sio1s_clock(CLOCK(config, "sio1s_clock", 614400));
	sio1s_clock.signal_handler().set("sio1s", FUNC(z80sio_device::rxtxcb_w));
	//sio1s_clock.signal_handler().append("sio1s", FUNC(z80sio_device::txca_w));

	z80sio_device& sio1s(Z80SIO(config, "sio1s", XTAL(16'000'000) /4));
	sio1s.out_txdb_callback().set("rs232s", FUNC(rs232_port_device::write_txd));
	sio1s.out_dtrb_callback().set("rs232s", FUNC(rs232_port_device::write_dtr));
	sio1s.out_rtsb_callback().set("rs232s", FUNC(rs232_port_device::write_rts));

	MCFG_DEVICE_ADD("rs232s", RS232_PORT, default_rs232_devices, "terminal")
	MCFG_RS232_RXD_HANDLER(WRITELINE("sio1s", z80sio_device, rxb_w))
	MCFG_RS232_DCD_HANDLER(WRITELINE("sio1s", z80sio_device, dcdb_w))
	MCFG_RS232_CTS_HANDLER(WRITELINE("sio1s", z80sio_device, ctsb_w))
MACHINE_CONFIG_END

/* ROM definition */
ROM_START( c8002 )
	ROM_REGION16_BE( 0x100000, "maincpu", 0 )
	ROM_LOAD16_BYTE("466-e", 0x0001, 0x0800, CRC(13534bcb) SHA1(976c76c69af40b0c0a5038e428a10b39a619a036))
	ROM_LOAD16_BYTE("467-e", 0x0000, 0x0800, CRC(0d5b557f) SHA1(0802bc6c2774f4e7de38a9c92e8558d710eed287))

	ROM_REGION( 0x10000, "subcpu", 0 )
	ROM_LOAD("459-3",   0x0000, 0x0800, CRC(c8906653) SHA1(7dea9fffa974479ef5926df567261f2aaa7a3283))
	ROM_LOAD("460-3",   0x0800, 0x0800, CRC(ce6c0214) SHA1(f69ee4c6c0d1e72574a9cf828dbb3e08f06d029a))

	ROM_REGION( 0x900, "proms", 0 )
	// for main cpu
	ROM_LOAD("468-a",  0x000, 0x100, CRC(89781491) SHA1(f874d0cf42a733eb2b92b15647aeac7178d7b9b1))
	ROM_LOAD("469-a",  0x100, 0x100, CRC(45e439de) SHA1(4f1af44332ae709d92e919c9e48433f29df5e632))
	ROM_LOAD("470a-3", 0x200, 0x100, CRC(c50622a9) SHA1(deda0df93fc4e4b5f4be313e4bfe0c5fc669a024))
	ROM_LOAD("471-a",  0x300, 0x100, CRC(c09ca06b) SHA1(cb99172f5342427c68a109ee108a0c49b44e7010))
	ROM_LOAD("472-a",  0x400, 0x100, CRC(e1316fed) SHA1(41ed2d822c74da4e1ce06eb229629576e7f5035f))
	ROM_LOAD("473-a",  0x500, 0x100, CRC(5e8efd7f) SHA1(647064e0c3b0d795a333febc57228472b1b32345))
	ROM_LOAD("474-a",  0x600, 0x100, CRC(0052edfd) SHA1(b5d18c9a6adce7a6d627ece40a60aab8c55a6597))
	// for sub cpu
	ROM_LOAD("453-a",  0x700, 0x100, CRC(7bc3871e) SHA1(6f75eb04911fa1ff66714276b8a88be62438a1b0))
	ROM_LOAD("454-a",  0x800, 0x100, CRC(aa2233cd) SHA1(4ec3a8c06cccda02f080e89831ecd8a9c96d3650))
ROM_END

/* Driver */

//    YEAR  NAME   PARENT  COMPAT  MACHINE  INPUT  CLASS       INIT        COMPANY         FULLNAME  FLAGS
COMP( 1982, c8002, 0,      0,      c8002,   c8002, onyx_state, empty_init, "Onyx Systems", "C8002",  MACHINE_NOT_WORKING | MACHINE_NO_SOUND_HW )



/********************************************************************************************************************************

Onyx Systems C5000.
(says C8000 on the backplate)

Chips: 256k dynamic RAM, Z80A, Z80DMA, 5x Z80PIO, 2x Z80SIO/0, 2x Z80CTC, 5? undumped proms, 3 red leds, 1x 4-sw DIP
Crystals: 16.000000, 19.660800
Labels of proms: 339, 153, XMN4, 2_1, 1_2

*********************************************************************************************************************************/

void onyx_state::c5000_mem(address_map &map)
{
	map(0x0000, 0x1fff).rom();
	map(0x2000, 0xffff).ram();
}

void onyx_state::c5000_io(address_map &map)
{
	map.global_mask(0xff);
	map(0x10, 0x13).rw(m_sio[0], FUNC(z80sio_device::cd_ba_r), FUNC(z80sio_device::cd_ba_w));
}

MACHINE_CONFIG_START(onyx_state::c5000)
	/* basic machine hardware */
	MCFG_DEVICE_ADD("maincpu", Z80, XTAL(16'000'000) / 4 )
	//MCFG_Z80_DAISY_CHAIN(sub_daisy_chain)
	MCFG_DEVICE_PROGRAM_MAP(c5000_mem)
	MCFG_DEVICE_IO_MAP(c5000_io)
	//MCFG_MACHINE_RESET_OVERRIDE(onyx_state, c8002)

	MCFG_DEVICE_ADD("sio1_clock", CLOCK, 614400)
	MCFG_CLOCK_SIGNAL_HANDLER(WRITELINE(m_sio[0], z80sio_device, rxtxcb_w))
	//MCFG_DEVCB_CHAIN_OUTPUT(WRITELINE(m_sio[0] ,z80sio_device, txca_w))

	/* peripheral hardware */
	//Z80PIO(config, m_pio[0], XTAL(16'000'000)/4);
	//m_pio[0]->out_int_callback().set_inputline(m_maincpu, INPUT_LINE_IRQ0);
	//Z80PIO(config, m_pio[1], XTAL(16'000'000)/4);
	//m_pio[1]->out_int_callback().set_inputline(m_maincpu, INPUT_LINE_IRQ0);
	//Z80CTC(config, m_ctc[0], XTAL(16'000'000) /4);
	//Z80CTC(config, m_ctc[1], XTAL(16'000'000) /4);
	//Z80CTC(config, m_ctc[2], XTAL(16'000'000) /4);

	Z80SIO(config, m_sio[0], XTAL(16'000'000) /4);
	m_sio[0]->out_txdb_callback().set("rs232", FUNC(rs232_port_device::write_txd));
	m_sio[0]->out_dtrb_callback().set("rs232", FUNC(rs232_port_device::write_dtr));
	m_sio[0]->out_rtsb_callback().set("rs232", FUNC(rs232_port_device::write_rts));

	MCFG_DEVICE_ADD("rs232", RS232_PORT, default_rs232_devices, "terminal")
	MCFG_RS232_RXD_HANDLER(WRITELINE(m_sio[0], z80sio_device, rxb_w))
	MCFG_RS232_DCD_HANDLER(WRITELINE(m_sio[0], z80sio_device, dcdb_w))
	MCFG_RS232_CTS_HANDLER(WRITELINE(m_sio[0], z80sio_device, ctsb_w))

	//MCFG_DEVICE_ADD(m_sio[1]", Z80SIO, XTAL(16'000'000) /4)
MACHINE_CONFIG_END


ROM_START( c5000 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "860-3.prom1", 0x0000, 0x1000, CRC(31b52df3) SHA1(e221c7829b4805805cde1bde763bd5a936e7db1a) )
	ROM_LOAD( "861-3.prom2", 0x1000, 0x1000, CRC(d1eba182) SHA1(850035497975b821fc1e51fbb73642cba3ff9784) )
ROM_END

/* Driver */

//    YEAR  NAME   PARENT  COMPAT   MACHINE    INPUT  CLASS       INIT        COMPANY          FULLNAME  FLAGS
COMP( 1981, c5000, 0,      0,       c5000,     c8002, onyx_state, empty_init, "Onyx Systems",  "C5000",  MACHINE_NOT_WORKING | MACHINE_NO_SOUND_HW )
