#pragma once
#include <string.h>
#include "SDL.h"

#include "emu8080.h"

#define SCREEN_WIDTH 224
#define SCREEN_HEIGHT 256
#define FPS 59.541985
#define CLOCK_SPEED 1996800
#define CYCLES_PER_FRAME (CLOCK_SPEED / FPS)

#define VRAM_ADDR 0x2400

typedef struct invaders invaders;

struct invaders {
	i8080 cpu;
	uint8_t memory[0x4000];

	uint8_t next_interrupt;
	bool coloured_screen;

	uint8_t port1, port2;
	uint8_t shift_msb, shift_lsb, shift_offset;
	//uint8_t last_out_port3, last_out_port5;

	uint8_t screenbuffer[SCREEN_HEIGHT][SCREEN_WIDTH][4];

	void(*update_screen)(invaders* const space_invaders);
};