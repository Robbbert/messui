// license:BSD-3-Clause
// copyright-holders:Curt Coder
#pragma once

#ifndef MAME_INCLUDES_PROF80_H
#define MAME_INCLUDES_PROF80_H

#include "bus/ecbbus/ecbbus.h"
#include "bus/rs232/rs232.h"
#include "cpu/z80/z80.h"
#include "machine/z80daisy.h"
#include "machine/prof80mmu.h"
#include "machine/74259.h"
#include "machine/ram.h"
#include "machine/rescap.h"
#include "machine/upd1990a.h"
#include "machine/upd765.h"

#define Z80_TAG         "z1"
#define MMU_TAG         "mmu"
#define UPD765_TAG      "z38"
#define UPD1990A_TAG    "z43"
#define RS232_A_TAG     "rs232a"
#define RS232_B_TAG     "rs232b"
#define FLR_A_TAG       "z44"
#define FLR_B_TAG       "z45"

// ------------------------------------------------------------------------

#define UNIO_Z80STI_TAG         "z5"
#define UNIO_Z80SIO_TAG         "z15"
#define UNIO_Z80PIO_TAG         "z13"
#define UNIO_CENTRONICS1_TAG    "n3"
#define UNIO_CENTRONICS2_TAG    "n4"

class prof80_state : public driver_device
{
public:
	prof80_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
			m_maincpu(*this, Z80_TAG),
			m_mmu(*this, MMU_TAG),
			m_rtc(*this, UPD1990A_TAG),
			m_fdc(*this, UPD765_TAG),
			m_ram(*this, RAM_TAG),
			m_floppy0(*this, UPD765_TAG":0"),
			m_floppy1(*this, UPD765_TAG":1"),
			m_ecb(*this, ECBBUS_TAG),
			m_rs232a(*this, RS232_A_TAG),
			m_rs232b(*this, RS232_B_TAG),
			m_flra(*this, FLR_A_TAG),
			m_flrb(*this, FLR_B_TAG),
			m_rom(*this, Z80_TAG),
			m_j4(*this, "J4"),
			m_j5(*this, "J5")
	{ }

	required_device<cpu_device> m_maincpu;
	required_device<prof80_mmu_device> m_mmu;
	required_device<upd1990a_device> m_rtc;
	required_device<upd765a_device> m_fdc;
	required_device<ram_device> m_ram;
	required_device<floppy_connector> m_floppy0;
	required_device<floppy_connector> m_floppy1;
	required_device<ecbbus_device> m_ecb;
	required_device<rs232_port_device> m_rs232a;
	required_device<rs232_port_device> m_rs232b;
	required_device<ls259_device> m_flra;
	required_device<ls259_device> m_flrb;
	required_memory_region m_rom;
	required_ioport m_j4;
	required_ioport m_j5;

	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;
	virtual void machine_start() override;

	enum
	{
		TIMER_ID_MOTOR
	};

	DECLARE_WRITE8_MEMBER(flr_w);
	DECLARE_READ8_MEMBER(status_r);
	DECLARE_READ8_MEMBER(status2_r);

	void motor(int mon);

	DECLARE_WRITE_LINE_MEMBER(ready_w);
	DECLARE_WRITE_LINE_MEMBER(inuse_w);
	DECLARE_WRITE_LINE_MEMBER(motor_w);
	DECLARE_WRITE_LINE_MEMBER(select_w);
	DECLARE_WRITE_LINE_MEMBER(resf_w);
	DECLARE_WRITE_LINE_MEMBER(mini_w);
	DECLARE_WRITE_LINE_MEMBER(mstop_w);

	// floppy state
	int m_motor;
	int m_ready;
	int m_select;

	// timers
	emu_timer   *m_floppy_motor_off_timer;

	void prof80(machine_config &config);
	void prof80_io(address_map &map);
	void prof80_mem(address_map &map);
	void prof80_mmu(address_map &map);
};

#endif
