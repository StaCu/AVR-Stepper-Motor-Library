#include "Queue.h"

#include "../platform/Platform.h"

#define IDLE_STATUS 100

#define VOLATILE_UINT8(x) (*(volatile uint8_t*) &x)

uint8_t Queue::widx[MOTOR_COUNT] = {0,0,0,0};
uint8_t Queue::ridx = 0;
uint8_t Queue::data[256*MOTOR_COUNT*2];
uint8_t Queue::next_data[MOTOR_COUNT*2];
uint8_t Queue::end_detection[MOTOR_COUNT];

bool Queue::idle() {
	for (uint8_t i = 0; i < MOTOR_COUNT; i++) {
		if (!Queue::idle(i)) {
			return false;
		}
	}
	return true;
}

bool Queue::idle(uint8_t m) {
	return VOLATILE_UINT8(next_data[m*2+1]) == IDLE_STATUS;
}

bool Queue::idle_bits(uint8_t m) {
	for (uint8_t i = 0, bit = 1; i < MOTOR_COUNT; i++, bit <<= 1) {
		if ((bit & m) && !Queue::idle(i)) {
			return false;
		}
	}
	return true;
}

bool Queue::_full(uint8_t m) {
	return ((uint8_t)(VOLATILE_UINT8(widx[m])+1)) == VOLATILE_UINT8(ridx);
}

bool Queue::full(uint8_t m) {
	auto intr = Platform::InterruptProtectionFrame();
	return _full(m);
}

uint8_t Queue::_free(uint8_t m) {
	return VOLATILE_UINT8(ridx) - ((uint8_t)(VOLATILE_UINT8(widx[m])+1));
}

uint8_t Queue::free(uint8_t m) {
	auto intr = Platform::InterruptProtectionFrame();
	return _free(m);
}

uint8_t Queue::getWidx(uint8_t m) {
	return VOLATILE_UINT8(widx[m]);
}

uint8_t Queue::getRidx() {
	return VOLATILE_UINT8(ridx);
}

uint8_t Queue::ahead(uint8_t idx) {
	return VOLATILE_UINT8(idx) - VOLATILE_UINT8(ridx);
}

void Queue::enq(uint8_t m, uint8_t dir, uint8_t vel) {
	if (Platform::is_blocked(m)) {
		return;
	}
	auto intr = Platform::InterruptProtectionFrame();
	if (Queue::_full(m)) {
		return;
	}
	uint8_t vwidx = VOLATILE_UINT8(widx[m]);
	VOLATILE_UINT8(widx[m]) = vwidx + 1;
	uint16_t idx = vwidx*MOTOR_COUNT*2+m*2;
	VOLATILE_UINT8(data[idx])   = vel;
	VOLATILE_UINT8(data[idx+1]) = dir != 1;
}

void Queue::next() {
	for (uint8_t m = 0; m < MOTOR_COUNT; m++) {
		if (widx[m] == ridx) {
			// motor is empty
			// => no more new chunks

			// increment the motors write index, so it doesn't fall behind
			widx[m] = ridx+1;
		}
	}
	ridx += 1;
}

void Queue::next(uint8_t m) {
	if (widx[m] == ridx) {
		// motor is empty
		// => no more new chunks
		Queue::next_data[m*2] = 0;
		Queue::next_data[m*2+1] = IDLE_STATUS;
	} else {
		uint16_t data_idx = ridx*MOTOR_COUNT*2+m*2;
		// motor is not empty
		// => add the new chunk
		// => invalidate it
		uint8_t vel = Queue::data[data_idx];
		uint8_t dir = Queue::data[data_idx+1];
		#ifdef TRACK_ISR_POSITION
			if (dir == 0) {
				StepperMotors::motors[m].isr_pos += vel;
			} else {
				StepperMotors::motors[m].isr_pos -= vel;
			}
			StepperMotors::motors[m].isr_vel = vel;
			StepperMotors::motors[m].isr_dir = dir;
		#endif
		Queue::next_data[m*2]   = vel;
		Queue::next_data[m*2+1] = dir;
		Queue::data[data_idx] = 0;
	}
}

// # ================================================================ #
// # ::                                                            :: #
// # ================================================================ #

void Queue::reset() {
	for (uint16_t i = 0; i < 256*MOTOR_COUNT*2; i+=1) {
		VOLATILE_UINT8(data[i]) = 0;
	}
	for (uint8_t m = 0; m < MOTOR_COUNT; m++) {
		VOLATILE_UINT8(end_detection[m]) = 0;
		VOLATILE_UINT8(next_data[m*2]) = 0;
		VOLATILE_UINT8(next_data[m*2+1]) = IDLE_STATUS;
	}

	auto intr = Platform::InterruptProtectionFrame();
	for (uint8_t m = 0; m < MOTOR_COUNT; m++) {
		VOLATILE_UINT8(widx[m]) = VOLATILE_UINT8(ridx);
	}
}

void Queue::reset(uint8_t m) {
	for (uint16_t i = m*2; i < 256*MOTOR_COUNT*2; i+=MOTOR_COUNT*2) {
		VOLATILE_UINT8(data[i]) = 0;
	}
	VOLATILE_UINT8(next_data[m*2]) = 0;
	VOLATILE_UINT8(next_data[m*2+1]) = IDLE_STATUS;
	VOLATILE_UINT8(end_detection[m]) = 0;

	auto intr = Platform::InterruptProtectionFrame();
	VOLATILE_UINT8(widx[m]) = VOLATILE_UINT8(ridx);
}

void Queue::skip(uint8_t m, uint8_t steps) {
	VOLATILE_UINT8(widx[m]) = VOLATILE_UINT8(ridx) + steps;
}

uint8_t Queue::sync(uint8_t motor_bits) {
	auto intr = Platform::InterruptProtectionFrame();
	uint8_t min = 255;
	for (uint8_t i = 0, bit = 1; i < MOTOR_COUNT; i++, bit <<= 1) {
		if (bit & motor_bits) {
			uint8_t r = _free(i);
			if (r < min) {
				min = r;
			}
		}
	}
	uint8_t vridx = VOLATILE_UINT8(ridx);
	for (uint8_t i = 0, bit = 1; i < MOTOR_COUNT; i++, bit <<= 1) {
		if (bit & motor_bits) {
			VOLATILE_UINT8(widx[i]) = vridx - min - 1;
		}
	}
	return min;
}

void Queue::sync(uint8_t m0, uint8_t m1) {
	auto intr = Platform::InterruptProtectionFrame();
	if (_free(m0) < _free(m1)) {
		VOLATILE_UINT8(widx[m1]) = VOLATILE_UINT8(widx[m0]);
	} else {
		VOLATILE_UINT8(widx[m0]) = VOLATILE_UINT8(widx[m1]);
	}
}
