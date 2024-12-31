#include "SyncQueue.h"

#include "../platform/Platform.h"

#define IDLE_STATUS 100

#define VOLATILE_UINT8(x) (*(volatile uint8_t*) &x)

uint8_t SyncQueue::widx = 0;
uint8_t SyncQueue::ridx = 0;
uint8_t SyncQueue::data[256*MOTOR_COUNT*2];
uint8_t SyncQueue::next_data[MOTOR_COUNT*2];
uint8_t SyncQueue::end_detection[MOTOR_COUNT];

bool SyncQueue::idle() {
	for (uint8_t i = 0; i < MOTOR_COUNT; i++) {
		if (!SyncQueue::idle(i)) {
			return false;
		}
	}
	return true;
}

bool SyncQueue::idle(uint8_t m) {
	return VOLATILE_UINT8(next_data[m*2+1]) == IDLE_STATUS;
}

bool SyncQueue::idle_bits(uint8_t m) {
	for (uint8_t i = 0, bit = 1; i < MOTOR_COUNT; i++, bit <<= 1) {
		if ((bit & m) && !SyncQueue::idle(i)) {
			return false;
		}
	}
	return true;
}

bool SyncQueue::_full() {
	return ((uint8_t)(VOLATILE_UINT8(widx)+1)) == VOLATILE_UINT8(ridx);
}

bool SyncQueue::full() {
	auto intr = Platform::InterruptProtectionFrame();
	return _full();
}

uint8_t SyncQueue::_free() {
	return VOLATILE_UINT8(ridx) - ((uint8_t)(VOLATILE_UINT8(widx)+1));
}

uint8_t SyncQueue::free() {
	auto intr = Platform::InterruptProtectionFrame();
	return _free();
}

uint8_t SyncQueue::get_widx() {
	return VOLATILE_UINT8(widx);
}

uint8_t SyncQueue::get_ridx() {
	return VOLATILE_UINT8(ridx);
}

void SyncQueue::enq(uint8_t *dir_vel) {
	if (SyncQueue::full()) {
		return;
	}
	auto intr = Platform::InterruptProtectionFrame();
	uint8_t vwidx = VOLATILE_UINT8(widx);
	VOLATILE_UINT8(widx) = vwidx + 1;
	uint16_t j = vwidx*MOTOR_COUNT*2;
	for (uint8_t i = 0; i < MOTOR_COUNT*2; i+=2) {
		if (Platform::is_blocked(i>>1)) {
			VOLATILE_UINT8(data[j+i]) = 0;
			VOLATILE_UINT8(data[j+i+1]) = IDLE_STATUS;
		} else {
			VOLATILE_UINT8(data[j+i]) = dir_vel[i];
			VOLATILE_UINT8(data[j+i+1]) = dir_vel[i+1] != 1;
		}
	}
}

void SyncQueue::next() {
	if (widx == ridx) {
		// motor is empty
		// => no more new chunks

		// increment the motors write index, so it doesn't fall behind
		widx = ridx+1;
	}
	ridx += 1;
}

void SyncQueue::next(uint8_t m) {
	if (widx == ridx) {
		// motor is empty
		// => no more new chunks
		SyncQueue::next_data[m*2] = 0;
		SyncQueue::next_data[m*2+1] = IDLE_STATUS;
	} else {
		uint16_t data_idx = ridx*MOTOR_COUNT*2+m*2;
		// motor is not empty
		// => add the new chunk
		// => invalidate it
		uint8_t vel = SyncQueue::data[data_idx];
		uint8_t dir = SyncQueue::data[data_idx+1];
		#ifdef TRACK_ISR_POSITION
			if (dir == 0) {
				StepperMotors::motors[m].isr_pos += vel;
			} else {
				StepperMotors::motors[m].isr_pos -= vel;
			}
			StepperMotors::motors[m].isr_vel = vel;
			StepperMotors::motors[m].isr_dir = dir;
		#endif
		SyncQueue::next_data[m*2]   = vel;
		SyncQueue::next_data[m*2+1] = dir;
		SyncQueue::data[data_idx] = 0;
	}
}

// # ================================================================ #
// # ::                                                            :: #
// # ================================================================ #

void SyncQueue::reset() {
	for (uint16_t i = 0; i < 256*MOTOR_COUNT*2; i+=1) {
		VOLATILE_UINT8(data[i]) = 0;
	}
	for (uint8_t m = 0; m < MOTOR_COUNT; m++) {
		VOLATILE_UINT8(end_detection[m]) = 0;
		VOLATILE_UINT8(next_data[m*2]) = 0;
		VOLATILE_UINT8(next_data[m*2+1]) = IDLE_STATUS;
	}

	auto intr = Platform::InterruptProtectionFrame();
	VOLATILE_UINT8(widx) = VOLATILE_UINT8(ridx);
}

void SyncQueue::reset(uint8_t m) {
	for (uint16_t i = m*2; i < 256*MOTOR_COUNT*2; i+=MOTOR_COUNT*2) {
		VOLATILE_UINT8(data[i]) = 0;
	}
	VOLATILE_UINT8(next_data[m*2]) = 0;
	VOLATILE_UINT8(next_data[m*2+1]) = IDLE_STATUS;
	VOLATILE_UINT8(end_detection[m]) = 0;
}
