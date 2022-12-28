#include "space_invaders_machine.h"

static uint8_t invaders_read_byte(void* userdata, uint16_t addr) {
	invaders* const space_invaders = (invaders*)userdata;

	if (addr >= 0x6000) {
		return 0;
	}

	if (addr >= 0x4000 && addr < 0x6000) {
		addr -= 0x2000;
	}

	return space_invaders->memory[addr];
}

static uint8_t invaders_write_byte(void* userdata, uint16_t addr, uint8_t val) {
	invaders* const space_invaders = (invaders*)userdata;

	if (addr >= 0x2400 && addr < 0x4000) {
		space_invaders->memory[addr] = val;
	}
}

static uint8_t port_in(void* userdata, uint8_t port) {
	invaders* const si = (invaders*)userdata;
	uint8_t value = 0xFF;

	// see https://computerarcheology.com/Arcade/SpaceInvaders/Hardware.html
	switch (port) {
	case 0: // 0000pr: INP0 (note: apparently never used by code)
		break;

	case 1: // 0001pr: INP1
		value = si->port1;
		break;

	case 2: // 0002pr: INP2
		value = si->port2;
		break;

	case 3: { // 0003pr: SHFT_IN
		// reading from port 3 returns the shift result:
		const uint16_t shift = (si->shift_msb << 8) | si->shift_lsb;
		value = (shift >> (8 - si->shift_offset)) & 0xFF;
	} break;

	default: fprintf(stderr, "error: unknown IN port %02x\n", port); break;
	}
}

static void port_out(void* userdata, uint8_t port, uint8_t value) {
	invaders* const si = (invaders*)userdata;

	// see https://computerarcheology.com/Arcade/SpaceInvaders/Hardware.html
	switch (port) {
	case 2: // 0002pw: SHFTAMNT
		// sets the shift offset for the 8 bit result
		si->shift_offset = value & 7; // 0b111
		break;

	case 3: // 0003pw: SOUND1
		break;

	case 4: // 0004pw: SHFT_DATA
		// writing to port 4 shifts MSB into LSB, and the new value into MSB:
		si->shift_lsb = si->shift_msb;
		si->shift_msb = value;
		break;

	case 5: // 0005pw: SOUND2
		// plays a sound from bank 2
		break;

	case 6: // 0006pw: WATCHDOG
		break;

	default: fprintf(stderr, "error: unknown OUT port %02x\n", port); break;
	}
}

