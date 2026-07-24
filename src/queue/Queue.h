#pragma once

#include <stdint.h>
#include "../Config.h"

class Queue {

private:

	static bool _full(uint8_t m);

	static uint8_t _free(uint8_t m);

public:

	static uint8_t widx[MOTOR_COUNT];
	static uint8_t ridx;
	static uint8_t data[256*MOTOR_COUNT*2];
	static uint8_t next_data[MOTOR_COUNT*2];
	static uint8_t end_detection[MOTOR_COUNT];

	static bool idle();

	static bool idle(uint8_t m);

	static bool idle_bits(uint8_t m);

	static bool full(uint8_t m);

	static uint8_t free(uint8_t m);

	static uint8_t getWidx(uint8_t m);

	static uint8_t getRidx();

	static uint8_t ahead(uint8_t idx);

	static void enq(uint8_t m, uint8_t dir, uint8_t vel);

	static void next();

	static void next(uint8_t m);

	static void reset();

	static void reset(uint8_t m);

	static void skip(uint8_t m, uint8_t steps);

	static uint8_t sync(uint8_t motor_bits);

	static void sync(uint8_t m0, uint8_t m1);

};
