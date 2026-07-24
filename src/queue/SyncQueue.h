#pragma once

#include <stdint.h>
#include "../Config.h"

class SyncQueue {

public:

	static uint8_t widx;
	static uint8_t ridx;
	static uint8_t data[256*MOTOR_COUNT*2];
	static uint8_t end_detection[MOTOR_COUNT];

	static bool full();

	static uint8_t free();

	static void enq(uint8_t *vel_dir);

	static void isr_next();

	static uint16_t isr_next(uint8_t m);

	static void reset();

	static void reset(uint8_t m);

};
