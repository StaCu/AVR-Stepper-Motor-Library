#pragma once

#include <stdint.h>
#include "../Config.h"

class SyncQueue {

private:

	static bool _full();

	static uint8_t _free();

public:

	static uint8_t widx;
	static uint8_t ridx;
	static uint16_t data[256*MOTOR_COUNT];
	static uint8_t end_detection[MOTOR_COUNT];

	static bool full();

	static uint8_t free();

	static void enq(uint16_t *vel);

	static void isr_next();

	static uint16_t isr_next(uint8_t m);

	static void reset();

	static void reset(uint8_t m);

};
