#include "SyncQueue.h"

#include "../platform/Platform.h"

#define VOLATILE_UINT8(x) (*(volatile uint8_t*) &x)

uint8_t SyncQueue::widx = 0;
uint8_t SyncQueue::ridx = 0;
uint8_t SyncQueue::data[256*MOTOR_COUNT*2];
uint8_t SyncQueue::end_detection[MOTOR_COUNT];

bool SyncQueue::full() {
	auto intr = Platform::InterruptProtectionFrame();
	uint8_t next_widx = widx + 1;
	return next_widx == ridx;
}

uint8_t SyncQueue::free() {
	auto intr = Platform::InterruptProtectionFrame();
	uint8_t next_widx = widx + 1;
	return ridx - next_widx;
}

void SyncQueue::enq(uint8_t *vel_dir) {
	if (SyncQueue::full()) {
		return;
	}
	auto intr = Platform::InterruptProtectionFrame();
	uint8_t *ptr = &data[((uint16_t) widx)*MOTOR_COUNT*2];
	VOLATILE_UINT8(ptr[0]) = vel_dir[0];
	VOLATILE_UINT8(ptr[1]) = vel_dir[1];
	VOLATILE_UINT8(ptr[2]) = vel_dir[2];
	VOLATILE_UINT8(ptr[3]) = vel_dir[3];
	VOLATILE_UINT8(ptr[4]) = vel_dir[4];
	VOLATILE_UINT8(ptr[5]) = vel_dir[5];
	VOLATILE_UINT8(ptr[6]) = vel_dir[6];
	VOLATILE_UINT8(ptr[7]) = vel_dir[7];
	VOLATILE_UINT8(widx) = widx + 1;
}

void SyncQueue::isr_next() {
	if (SyncQueue::widx == SyncQueue::ridx) {
		// motor is empty
		// => no more new chunks

		// increment the motors write index, so it doesn't fall behind
		SyncQueue::widx = SyncQueue::ridx + 1;
	}
	SyncQueue::ridx += 1;
}

uint16_t SyncQueue::isr_next(uint8_t m) {
	if (SyncQueue::widx == SyncQueue::ridx) {
		// motor is empty
		// => no more new chunks
		return 0;
	} else {
		// motor is not empty
		return SyncQueue::data[SyncQueue::ridx*MOTOR_COUNT+m];
	}
}

// # ================================================================ #
// # ::                                                            :: #
// # ================================================================ #

void SyncQueue::reset() {
	auto intr = Platform::InterruptProtectionFrame();
	VOLATILE_UINT8(widx) = ridx;
}

void SyncQueue::reset(uint8_t m) {
	for (uint16_t i = m; i < 256*MOTOR_COUNT*2; i+=MOTOR_COUNT*2) {
		VOLATILE_UINT8(data[i]) = 0;
	}
}
