// license:BSD-3-Clause
// copyright-holders:Juergen Buchmueller, Robbbert
/***************************************************************************

  machine.c

  Functions to emulate general aspects of the machine (RAM, ROM, interrupts,
  I/O ports)

MAX_LUMPS   192     crude storage units - don't know much about it
MAX_GRANULES    8       lumps consisted of granules.. aha
MAX_SECTORS     5       and granules of sectors

***************************************************************************/

#include "emu.h"
#include "includes/trs80.h"


#define IRQ_M1_RTC      0x80    /* RTC on Model I */
#define IRQ_M1_FDC      0x40    /* FDC on Model I */
#define IRQ_M4_RTC      0x04    /* RTC on Model 4 */
#define CASS_RISE       0x01    /* high speed cass on Model III/4) */
#define CASS_FALL       0x02    /* high speed cass on Model III/4) */
#define MODEL4_MASTER_CLOCK 20275200


TIMER_CALLBACK_MEMBER(trs80_state::cassette_data_callback)
{
/* This does all baud rates. 250 baud (trs80), and 500 baud (all others) set bit 7 of "cassette_data".
    1500 baud (trs80m3, trs80m4) is interrupt-driven and uses bit 0 of "cassette_data" */

	double new_val = (m_cassette->input());

	/* Check for HI-LO transition */
	if ( m_old_cassette_val > -0.2 && new_val < -0.2 )
	{
		m_cassette_data |= 0x80;        /* 500 baud */
		if (m_mask & CASS_FALL) /* see if 1500 baud */
		{
			m_cassette_data = 0;
			m_irq |= CASS_FALL;
			m_maincpu->set_input_line(0, HOLD_LINE);
		}
	}
	else
	if ( m_old_cassette_val < -0.2 && new_val > -0.2 )
	{
		if (m_mask & CASS_RISE) /* 1500 baud */
		{
			m_cassette_data = 1;
			m_irq |= CASS_RISE;
			m_maincpu->set_input_line(0, HOLD_LINE);
		}
	}

	m_old_cassette_val = new_val;
}


/*************************************
 *
 *              Port handlers.
 *
 *************************************/


READ8_MEMBER( trs80_state::sys80_f9_r )
{
/* UART Status Register - d6..d4 not emulated
    d7 Transmit buffer empty (inverted)
    d6 CTS pin
    d5 DSR pin
    d4 CD pin
    d3 Parity Error
    d2 Framing Error
    d1 Overrun
    d0 Data Available */

	uint8_t data = 70;
	m_uart->write_swe(0);
	data |= m_uart->tbmt_r() ? 0 : 0x80;
	data |= m_uart->dav_r( ) ? 0x01 : 0;
	data |= m_uart->or_r(  ) ? 0x02 : 0;
	data |= m_uart->fe_r(  ) ? 0x04 : 0;
	data |= m_uart->pe_r(  ) ? 0x08 : 0;
	m_uart->write_swe(1);

	return data;
}

READ8_MEMBER( trs80_state::lnw80_fe_r )
{
	return ((m_mode & 0x78) >> 3) | 0xf0;
}

READ8_MEMBER( trs80_state::trs80_ff_r )
{
/* ModeSel and cassette data
    d7 cassette data from tape
    d2 modesel setting */

	uint8_t data = (~m_mode & 1) << 5;
	return data | m_cassette_data;
}

WRITE8_MEMBER( trs80_state::sys80_f8_w )
{
/* not emulated
    d2 reset UART (XR pin)
    d1 DTR
    d0 RTS */
}

WRITE8_MEMBER( trs80_state::sys80_fe_w )
{
/* not emulated
    d4 select internal or external cassette player */

	m_tape_unit = (data & 0x10) ? 2 : 1;
}

/* lnw80 can switch out all the devices, roms and video ram to be replaced by graphics ram. */
WRITE8_MEMBER( trs80_state::lnw80_fe_w )
{
/* lnw80 video options
    d3 bankswitch lower 16k between roms and hires ram (1=hires)
    d2 enable colour    \
    d1 hres         /   these 2 are the bits from the MODE command of LNWBASIC
    d0 inverse video (entire screen) */

	/* get address space instead of io space */
	address_space &mem = m_maincpu->space(AS_PROGRAM);

	m_mode = (m_mode & 0x87) | ((data & 0x0f) << 3);

	if (data & 8)
	{
		mem.unmap_readwrite (0x0000, 0x3fff);
		mem.install_readwrite_handler (0x0000, 0x3fff, read8_delegate(FUNC(trs80_state::trs80_gfxram_r), this), write8_delegate(FUNC(trs80_state::trs80_gfxram_w), this));
	}
	else
	{
		mem.unmap_readwrite (0x0000, 0x3fff);
		mem.install_read_bank (0x0000, 0x2fff, "bank1");
		membank("bank1")->set_base(m_region_maincpu->base());
		mem.install_readwrite_handler (0x37e0, 0x37e3, read8_delegate(FUNC(trs80_state::trs80_irq_status_r), this), write8_delegate(FUNC(trs80_state::trs80_motor_w), this));
		mem.install_readwrite_handler (0x37e8, 0x37eb, read8_delegate(FUNC(trs80_state::trs80_printer_r), this), write8_delegate(FUNC(trs80_state::trs80_printer_w), this));
		mem.install_read_handler (0x37ec, 0x37ec, read8_delegate(FUNC(trs80_state::trs80_wd179x_r), this));
		mem.install_write_handler (0x37ec, 0x37ec, write8_delegate(FUNC(fd1793_device::cmd_w),(fd1793_device*)m_fdc));
		mem.install_readwrite_handler (0x37ed, 0x37ed, read8_delegate(FUNC(fd1793_device::track_r),(fd1793_device*)m_fdc), write8_delegate(FUNC(fd1793_device::track_w),(fd1793_device*)m_fdc));
		mem.install_readwrite_handler (0x37ee, 0x37ee, read8_delegate(FUNC(fd1793_device::sector_r),(fd1793_device*)m_fdc), write8_delegate(FUNC(fd1793_device::sector_w),(fd1793_device*)m_fdc));
		mem.install_readwrite_handler (0x37ef, 0x37ef, read8_delegate(FUNC(fd1793_device::data_r),(fd1793_device*)m_fdc),write8_delegate( FUNC(fd1793_device::data_w),(fd1793_device*)m_fdc));
		mem.install_read_handler (0x3800, 0x38ff, 0, 0x0300, 0, read8_delegate(FUNC(trs80_state::trs80_keyboard_r), this));
		mem.install_readwrite_handler (0x3c00, 0x3fff, read8_delegate(FUNC(trs80_state::trs80_videoram_r), this), write8_delegate(FUNC(trs80_state::trs80_videoram_w), this));
	}
}

WRITE8_MEMBER( trs80_state::trs80_ff_w )
{
/* Standard output port of Model I
    d3 ModeSel bit
    d2 Relay
    d1, d0 Cassette output */

	static const double levels[4] = { 0.0, -1.0, 0.0, 1.0 };
	static int init = 0;

	m_cassette->change_state(( data & 4 ) ? CASSETTE_MOTOR_ENABLED : CASSETTE_MOTOR_DISABLED,CASSETTE_MASK_MOTOR );
	m_cassette->output(levels[data & 3]);
	m_cassette_data &= ~0x80;

	m_mode = (m_mode & 0xfe) | ((data & 8) >> 3);

	if (!init)
	{
		init = 1;
		static int16_t speaker_levels[4] = { 0, -32768, 0, 32767 };
		m_speaker->set_levels(4, speaker_levels);

	}
	/* Speaker for System-80 MK II - only sounds if relay is off */
	if (~data & 4)
		m_speaker->level_w(data & 3);
}

/*************************************
 *
 *      Interrupt handlers.
 *
 *************************************/

INTERRUPT_GEN_MEMBER(trs80_state::trs80_rtc_interrupt)
{
/* This enables the processing of interrupts for the clock and the flashing cursor.
    The OS counts one tick for each interrupt. The Model I has 40 ticks per
    second, while the Model III/4 has 30. */

	if (m_model4)   // Model 4
	{
		if (m_mask & IRQ_M4_RTC)
		{
			m_irq |= IRQ_M4_RTC;
			device.execute().set_input_line(0, HOLD_LINE);
		}
	}
	else        // Model 1
	{
		m_irq |= IRQ_M1_RTC;
		device.execute().set_input_line(0, HOLD_LINE);
	}
}

void trs80_state::trs80_fdc_interrupt_internal()
{
	if (m_model4)
	{
		if (m_nmi_mask & 0x80)   // Model 4 does a NMI
		{
			m_nmi_data = 0x80;
			m_maincpu->pulse_input_line(INPUT_LINE_NMI, attotime::zero);
		}
	}
	else        // Model 1 does a IRQ
	{
		m_irq |= IRQ_M1_FDC;
		m_maincpu->set_input_line(0, HOLD_LINE);
	}
}

INTERRUPT_GEN_MEMBER(trs80_state::trs80_fdc_interrupt)/* not used - should it be? */
{
	trs80_fdc_interrupt_internal();
}

WRITE_LINE_MEMBER(trs80_state::trs80_fdc_intrq_w)
{
	if (state)
	{
		trs80_fdc_interrupt_internal();
	}
	else
	{
		if (m_model4)
			m_nmi_data = 0;
		else
			m_irq &= ~IRQ_M1_FDC;
	}
}


/*************************************
 *                                   *
 *      Memory handlers              *
 *                                   *
 *************************************/

READ8_MEMBER( trs80_state::trs80_wd179x_r )
{
	uint8_t data = 0xff;
	if (BIT(m_io_config->read(), 7))
		data = m_fdc->status_r(space, offset);

	return data;
}

READ8_MEMBER( trs80_state::trs80_printer_r )
{
	return m_cent_status_in->read();
}

WRITE8_MEMBER( trs80_state::trs80_printer_w )
{
	m_cent_data_out->write(data);
	m_centronics->write_strobe(0);
	m_centronics->write_strobe(1);
}

WRITE8_MEMBER( trs80_state::trs80_cassunit_w )
{
/* not emulated
    01 for unit 1 (default)
    02 for unit 2 */

	m_tape_unit = data;
}

READ8_MEMBER( trs80_state::trs80_irq_status_r )
{
/* (trs80l2) Whenever an interrupt occurs, 37E0 is read to see what devices require service.
    d7 = RTC
    d6 = FDC
    d2 = Communications (not emulated)
    All interrupting devices are serviced in a single interrupt. There is a mask byte,
    which is dealt with by the DOS. We take the opportunity to reset the cpu INT line. */

	int result = m_irq;
	m_maincpu->set_input_line(0, CLEAR_LINE);
	m_irq = 0;
	return result;
}


WRITE8_MEMBER( trs80_state::trs80_motor_w )
{
	floppy_image_device *floppy = nullptr;

	if (BIT(data, 0)) floppy = m_floppy0->get_device();
	if (BIT(data, 1)) floppy = m_floppy1->get_device();
	if (BIT(data, 2)) floppy = m_floppy2->get_device();
	if (BIT(data, 3)) floppy = m_floppy3->get_device();

	m_fdc->set_floppy(floppy);

	if (floppy)
	{
		floppy->mon_w(0);
		floppy->ss_w(BIT(data, 4));
	}

	// switch to fm
	m_fdc->dden_w(1);
}

/*************************************
 *      Keyboard         *
 *************************************/
READ8_MEMBER( trs80_state::trs80_keyboard_r )
{
	u8 i, result = 0;

	for (i = 0; i < 8; i++)
		if (BIT(offset, i))
			result |= m_io_keyboard[i]->read();

	return result;
}


/*************************************
 *  Machine              *
 *************************************/

void trs80_state::machine_start()
{
	m_tape_unit=1;
	m_reg_load=1;
	m_nmi_data=0xff;

	m_cassette_data_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(trs80_state::cassette_data_callback),this));
	m_cassette_data_timer->adjust( attotime::zero, 0, attotime::from_hz(11025) );
}

void trs80_state::machine_reset()
{
	m_cassette_data = 0;
}

MACHINE_RESET_MEMBER(trs80_state,lnw80)
{
	address_space &space = m_maincpu->space(AS_PROGRAM);
	m_cassette_data = 0;
	m_reg_load = 1;
	lnw80_fe_w(space, 0, 0);
}


/***************************************************************************
    PARAMETERS
***************************************************************************/

#define LOG 1

#define CMD_TYPE_OBJECT_CODE                            0x01
#define CMD_TYPE_TRANSFER_ADDRESS                       0x02
#define CMD_TYPE_END_OF_PARTITIONED_DATA_SET_MEMBER     0x04
#define CMD_TYPE_LOAD_MODULE_HEADER                     0x05
#define CMD_TYPE_PARTITIONED_DATA_SET_HEADER            0x06
#define CMD_TYPE_PATCH_NAME_HEADER                      0x07
#define CMD_TYPE_ISAM_DIRECTORY_ENTRY                   0x08
#define CMD_TYPE_END_OF_ISAM_DIRECTORY_ENTRY            0x0a
#define CMD_TYPE_PDS_DIRECTORY_ENTRY                    0x0c
#define CMD_TYPE_END_OF_PDS_DIRECTORY_ENTRY             0x0e
#define CMD_TYPE_YANKED_LOAD_BLOCK                      0x10
#define CMD_TYPE_COPYRIGHT_BLOCK                        0x1f

/***************************************************************************
    IMPLEMENTATION
***************************************************************************/

QUICKLOAD_LOAD_MEMBER( trs80_state, trs80_cmd )
{
	address_space &program = m_maincpu->space(AS_PROGRAM);

	uint8_t type, length;
	uint8_t data[0x100];
	uint8_t addr[2];
	void *ptr;

	while (!image.image_feof())
	{
		image.fread( &type, 1);
		image.fread( &length, 1);

		length -= 2;
		int block_length = length ? length : 256;

		switch (type)
		{
		case CMD_TYPE_OBJECT_CODE:
			{
			image.fread( &addr, 2);
			uint16_t address = (addr[1] << 8) | addr[0];
			if (LOG) logerror("/CMD object code block: address %04x length %u\n", address, block_length);
			ptr = program.get_write_ptr(address);
			image.fread( ptr, block_length);
			}
			break;

		case CMD_TYPE_TRANSFER_ADDRESS:
			{
			image.fread( &addr, 2);
			uint16_t address = (addr[1] << 8) | addr[0];
			if (LOG) logerror("/CMD transfer address %04x\n", address);
			m_maincpu->set_state_int(Z80_PC, address);
			}
			break;

		case CMD_TYPE_LOAD_MODULE_HEADER:
			image.fread( &data, block_length);
			if (LOG) logerror("/CMD load module header '%s'\n", data);
			break;

		case CMD_TYPE_COPYRIGHT_BLOCK:
			image.fread( &data, block_length);
			if (LOG) logerror("/CMD copyright block '%s'\n", data);
			break;

		default:
			image.fread( &data, block_length);
			logerror("/CMD unsupported block type %u!\n", type);
		}
	}

	return image_init_result::PASS;
}
