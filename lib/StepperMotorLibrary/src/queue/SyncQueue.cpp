#include "SyncQueue.h"

#include "../platform/Platform.h"

#define VOLATILE_UINT8(x) (*(volatile uint8_t*) &x)

uint8_t SyncQueue::widx = 0;
uint8_t SyncQueue::ridx = 0;
uint8_t SyncQueue::data[256*MOTOR_COUNT*2];
uint8_t SyncQueue::end_detection[MOTOR_COUNT];

bool SyncQueue::full() {
	auto intr = Platform::InterruptProtectionFrame();
	return widx + 1 == ridx;
}

uint8_t SyncQueue::free() {
	auto intr = Platform::InterruptProtectionFrame();
	return ridx - (widx + 1);
}

void SyncQueue::enq(uint8_t *vel_dir) {
	if (SyncQueue::full()) {
		return;
	}
	auto intr = Platform::InterruptProtectionFrame();
	widx = widx + 1;
	uint16_t j = widx*MOTOR_COUNT*2;
	for (uint8_t i = 0; i < MOTOR_COUNT*2; i++) {
		/*if (Platform::is_blocked(i/2)) {
			VOLATILE_UINT8(data[j+i]) = 0;
		} else {*/
			VOLATILE_UINT8(data[j+i]) = vel_dir[i];
	//	}
	}
}

void SyncQueue::isr_next() {
	if (SyncQueue::widx == SyncQueue::ridx) {
		// motor is empty
		// => no more new chunks

		// increment the motors write index, so it doesn't fall behind
		SyncQueue::widx = SyncQueue::ridx+1;
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
