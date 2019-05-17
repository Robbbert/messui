// license:BSD-3-Clause
// copyright-holders:AJR
/***************************************************************************

    Atari 810 Disk Drive

***************************************************************************/

#include "emu.h"
#include "atari810.h"

#include "cpu/m6502/m6507.h"


// device type definition
DEFINE_DEVICE_TYPE(ATARI810, atari810_device, "atari810", "Atari 810 Disk Drive")

atari810_device::atari810_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
	: device_t(mconfig, ATARI810, tag, owner, clock)
	, device_a8sio_card_interface(mconfig, *this)
	, m_pia(*this, "pia")
	, m_fdc(*this, "fdc")
{
}

void atari810_device::device_start()
{
}

void atari810_device::step_w(u8 data)
{
	m_a8sio->data_in_w(BIT(data, 0));
}

void atari810_device::mem_map(address_map &map)
{
	map(0x0000, 0x0003).mirror(0x167c).rw(m_fdc, FUNC(fd1771_device::read), FUNC(fd1771_device::write));
	map(0x0080, 0x00ff).mirror(0x1400).ram(); // MCM6810
	map(0x0100, 0x017f).mirror(0x1480).m(m_pia, FUNC(mos6532_new_device::ram_map));
	map(0x0300, 0x031f).mirror(0x14e0).m(m_pia, FUNC(mos6532_new_device::io_map));
	map(0x0800, 0x0fff).mirror(0x1000).rom().region("rom", 0);
}


WRITE_LINE_MEMBER(atari810_device::data_out_w)
{
	m_pia->pb7_w(state);
}

WRITE_LINE_MEMBER(atari810_device::command_w)
{
	m_pia->pb6_w(state);
}

WRITE_LINE_MEMBER(atari810_device::ready_w)
{
	m_pia->pb1_w(state);
}


static INPUT_PORTS_START(atari810)
	PORT_START("SELECT") // values not verified
	PORT_DIPNAME(0x05, 0x00, "Drive Code") PORT_DIPLOCATION("S101:1,2")
	PORT_DIPSETTING(0x00, "No. 1")
	PORT_DIPSETTING(0x01, "No. 2")
	PORT_DIPSETTING(0x04, "No. 3")
	PORT_DIPSETTING(0x05, "No. 4")
	PORT_BIT(0xfa, IP_ACTIVE_LOW, IPT_UNUSED)
INPUT_PORTS_END

ioport_constructor atari810_device::device_input_ports() const
{
	return INPUT_PORTS_NAME(atari810);
}

void atari810_device::device_add_mconfig(machine_config &config)
{
	m6507_device &fdcpu(M6507(config, "fdcpu", 1_MHz_XTAL / 2));
	fdcpu.set_addrmap(AS_PROGRAM, &atari810_device::mem_map);

	MOS6532_NEW(config, m_pia, 1_MHz_XTAL / 2);
	m_pia->pa_rd_callback().set_ioport("SELECT");
	m_pia->pb_wr_callback().set(FUNC(atari810_device::step_w));
	//m_pia->irq_wr_callback().set(m_fdc, FUNC(fd1771_device::ip_w));

	FD1771(config, m_fdc, 1_MHz_XTAL);
	m_fdc->drq_wr_callback().set(m_pia, FUNC(mos6532_new_device::pa7_w));
	m_fdc->intrq_wr_callback().set(m_pia, FUNC(mos6532_new_device::pa6_w));
}


ROM_START(atari810)
	ROM_REGION(0x0800, "rom", 0)
	ROM_SYSTEM_BIOS(0, "c", "Rev. C (9:1 skew)")
	ROMX_LOAD("8229_c68168_c011299c-03.bin", 0x0000, 0x0800, CRC(0896f03d) SHA1(b676b2a46ac3903a6729b92650c5faf0111ba8b6), ROM_BIOS(0))
	ROM_SYSTEM_BIOS(1, "b", "Rev. B (12:1 skew)")
	ROMX_LOAD("810rom_b.bin", 0x0000, 0x0800, CRC(19227d33) SHA1(075b1948a5736599d5501f0125c745b1b9fcced4), ROM_BIOS(1))
ROM_END

const tiny_rom_entry *atari810_device::device_rom_region() const
{
	return ROM_NAME(atari810);
}
