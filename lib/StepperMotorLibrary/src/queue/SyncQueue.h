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
	static uint8_t data[256*MOTOR_COUNT*2];
	static uint8_t next_data[MOTOR_COUNT*2];
	static uint8_t end_detection[MOTOR_COUNT];

	static bool idle();

	static bool idle(uint8_t m);

	static bool idle_bits(uint8_t m);

	static bool full();

	static uint8_t free();

	static uint8_t get_widx();

	static uint8_t get_ridx();

	static void enq(uint8_t *dir_vel);

	static void next();

	static void next(uint8_t m);

	static void reset();

	static void reset(uint8_t m);

};
