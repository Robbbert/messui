// license:BSD-3-Clause
// copyright-holders:hap

// MM76 opcode handlers

#include "emu.h"
#include "mm76.h"


// internal helpers

inline u8 mm76_device::ram_r()
{
	return m_data->read_byte(m_ram_addr & m_datamask) & 0xf;
}

inline void mm76_device::ram_w(u8 data)
{
	m_data->write_byte(m_ram_addr & m_datamask, data & 0xf);
}

void mm76_device::pop_pc()
{
	m_pc = m_stack[0] & m_prgmask;
	for (int i = 0; i < m_stack_levels-1; i++)
		m_stack[i] = m_stack[i+1];
}

void mm76_device::push_pc()
{
	for (int i = m_stack_levels-1; i >= 1; i--)
		m_stack[i] = m_stack[i-1];
	m_stack[0] = m_pc;
}

void mm76_device::op_tr()
{
	// TR x: prefix for extended opcode
}

void mm76_device::op_illegal()
{
	logerror("unknown opcode $%02X at $%03X\n", m_op, m_prev_pc);
}

void mm76_device::op_todo()
{
	logerror("unimplemented opcode $%02X at $%03X\n", m_op, m_prev_pc);
}


// opcodes

// RAM addressing instructions

void mm76_device::op_xab()
{
	// XAB: exchange A with Bl
	u8 a = m_a;
	m_a = m_b & 0xf;
	m_b = (m_b & ~0xf) | a;
	m_ram_delay = true;
}

void mm76_device::op_lba()
{
	// LBA: load Bl from A
	m_b = (m_b & ~0xf) | m_a;
	m_ram_delay = true;
}

void mm76_device::op_lb()
{
	// LB x: load B from x (successive LB/EOB are ignored)
	if (!(op_is_lb(m_prev_op) && !op_is_tr(m_prev2_op)) && !op_is_eob(m_prev_op))
		m_b = m_op & 0xf;
}

void mm76_device::op_eob()
{
	// EOB x: XOR Bu with x (successive LB/EOB are ignored, except after first executed LB)
	bool first_lb = (op_is_lb(m_prev_op) && !op_is_tr(m_prev2_op)) && !(op_is_lb(m_prev2_op) && !op_is_tr(m_prev3_op)) && !op_is_eob(m_prev2_op);
	if ((!(op_is_lb(m_prev_op) && !op_is_tr(m_prev2_op)) && !op_is_eob(m_prev_op)) || first_lb)
		m_b ^= m_op << 4 & m_datamask;
}


// bit manipulation instructions

void mm76_device::op_sb()
{
	// Bu != 3: SB x: set memory bit
	// Bu == 3: SOS: set output
	op_todo();
}

void mm76_device::op_rb()
{
	// Bu != 3: RB x: reset memory bit
	// Bu == 3: ROS: reset output
	op_todo();
}

void mm76_device::op_skbf()
{
	// Bu != 3: SKBF x: test memory bit
	// Bu == 3: SKISL: test input
	op_todo();
}


// register to register instructions

void mm76_device::op_xas()
{
	// XAS: exchange A with S
	u8 a = m_a;
	m_a = m_s;
	m_s = a;
}

void mm76_device::op_lsa()
{
	// LSA: load S from A
	m_s = m_a;
}


// register memory instructions

void mm76_device::op_l()
{
	// L x: load A from memory, XOR Bu with x
	m_a = ram_r();
	m_b ^= m_op << 4 & 0x30;
}

void mm76_device::op_x()
{
	// X x: exchange A with memory, XOR Bu with x
	u8 a = m_a;
	m_a = ram_r();
	ram_w(a);
	m_b ^= m_op << 4 & 0x30;
}

void mm76_device::op_xdsk()
{
	// XDSK x: X x + decrement Bl
	op_x();
	m_b = (m_b & ~0xf) | ((m_b - 1) & 0xf);
	m_skip = (m_b & 0xf) == 0xf;
	m_ram_delay = true;
}

void mm76_device::op_xnsk()
{
	// XNSK x: X x + increment Bl
	op_x();
	m_b = (m_b & ~0xf) | ((m_b + 1) & 0xf);
	m_skip = (m_b & 0xf) == 0;
	m_ram_delay = true;
}


// arithmetic instructions

void mm76_device::op_a()
{
	// A: add memory to A
	m_a = (m_a + ram_r()) & 0xf;
}

void mm76_device::op_ac()
{
	// AC: add memory and carry to A
	m_a += ram_r() + m_c_in;
	m_c = m_a >> 4 & 1;
	m_a &= 0xf;
	m_c_delay = true;
}

void mm76_device::op_acsk()
{
	// ACSK: AC + skip on no overflow
	op_ac();
	m_skip = !m_c;
}

void mm76_device::op_ask()
{
	// ASK: A + skip on no overflow
	u8 a = m_a;
	op_a();
	m_skip = m_a >= a;
}

void mm76_device::op_com()
{
	// COM: complement A
	m_a ^= 0xf;
}

void mm76_device::op_rc()
{
	// RC: reset carry
	m_c = 0;
}

void mm76_device::op_sc()
{
	// SC: set carry
	m_c = 1;
}

void mm76_device::op_sknc()
{
	// SKNC: skip on no carry
	m_skip = !m_c_in;
}

void mm76_device::op_lai()
{
	// LAI x: load A from x (successive LAI are ignored)
	if (!(op_is_lai(m_prev_op) && !op_is_tr(m_prev2_op)))
		m_a = m_op & 0xf;
}

void mm76_device::op_aisk()
{
	// AISK x: add x to A, skip on no overflow
	m_a += m_op & 0xf;
	m_skip = !(m_a & 0x10);
	m_a &= 0xf;
}


// ROM addressing instructions

void mm76_device::op_rt()
{
	// RT: return from subroutine
	cycle();
	pop_pc();
}

void mm76_device::op_rtsk()
{
	// RTSK: RT + skip next instruction
	op_rt();
	m_skip = true;
}

void mm76_device::op_t()
{
	// T x: transfer on-page
	cycle();

	// jumps from subroutine pages reset page to SR1
	u16 mask = m_prgmask ^ 0x7f;
	if ((m_pc & mask) == mask)
		m_pc &= ~0x40;

	m_pc = (m_pc & ~0x3f) | (~m_op & 0x3f);
}

void mm76_device::op_tl()
{
	// TL x: transfer long off-page
	cycle();
	m_pc = (~m_prev_op & 0xf) << 6 | (~m_op & 0x3f);
}

void mm76_device::op_tm()
{
	// TM x: transfer and mark to SR0
	cycle();

	// calls from subroutine pages don't push PC
	u16 mask = m_prgmask ^ 0x7f;
	if ((m_pc & mask) != mask)
		push_pc();

	m_pc = ((~m_op & 0x3f) | ~0x3f) & m_prgmask;
}

void mm76_device::op_tml()
{
	// TML x: transfer and mark long
	cycle();
	push_pc();
	m_pc = (~m_prev_op & 0xf) << 6 | (~m_op & 0x3f);
}

void mm76_device::op_nop()
{
	// NOP: no operation
}


// logical comparison instructions

void mm76_device::op_skmea()
{
	// SKMEA: skip on memory equals A
	m_skip = m_a == ram_r();
}

void mm76_device::op_skbei()
{
	// SKBEI x: skip on Bl equals x
	m_skip = (m_b & 0xf) == (m_op & 0xf);
}

void mm76_device::op_skaei()
{
	// SKAEI x: skip on A equals X
	m_skip = m_a == (~m_op & 0xf);
}


// input/output instructions

void mm76_device::op_ibm()
{
	// IBM: input channel B to A
}

void mm76_device::op_ob()
{
	// OB: output from A to channel B
	op_todo();
}

void mm76_device::op_iam()
{
	// IAM: input channel A to A
	op_todo();
}

void mm76_device::op_oa()
{
	// OA: output from A to channel A
	op_todo();
}

void mm76_device::op_ios()
{
	// IOS: start serial I/O
	op_todo();
}

void mm76_device::op_i1()
{
	// I1: input channel 1 to A
	op_todo();
}

void mm76_device::op_i2c()
{
	// I2C: input channel 2 to A
	op_todo();
}

void mm76_device::op_int1h()
{
	// INT1H: skip on INT1
	op_todo();
}

void mm76_device::op_din1()
{
	// DIN1: test INT1 flip-flop
	op_todo();
}

void mm76_device::op_int0l()
{
	// INT1H: skip on INT0
	op_todo();
}

void mm76_device::op_din0()
{
	// DIN0: test INT0 flip-flop
	op_todo();
}

void mm76_device::op_seg1()
{
	// SEG1: output A+carry through PLA to channel A
	op_todo();
}

void mm76_device::op_seg2()
{
	// SEG2: output A+carry through PLA to channel B
	op_todo();
}