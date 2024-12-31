#include "SyncQueue.h"

#include "../platform/Platform.h"

#define VOLATILE_UINT8(x) (*(volatile uint8_t*) &x)

uint8_t SyncQueue::widx = 0;
uint8_t SyncQueue::ridx = 0;
uint16_t SyncQueue::data[256*MOTOR_COUNT];
uint8_t SyncQueue::end_detection[MOTOR_COUNT];

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

void SyncQueue::enq(uint16_t *vel) {
	if (SyncQueue::full()) {
		return;
	}
	auto intr = Platform::InterruptProtectionFrame();
	widx = widx + 1;
	uint16_t j = widx*MOTOR_COUNT;
	for (uint8_t i = 0; i < MOTOR_COUNT; i++) {
		if (Platform::is_blocked(i)) {
			VOLATILE_UINT8(data[j+i]) = 0;
		} else {
			VOLATILE_UINT8(data[j+i]) = vel[i];
		}
	}
}

void SyncQueue::isr_next() {
	if (widx == ridx) {
		// motor is empty
		// => no more new chunks

		// increment the motors write index, so it doesn't fall behind
		widx = ridx+1;
	}
	ridx += 1;
}

uint16_t SyncQueue::isr_next(uint8_t m) {
	if (widx == ridx) {
		// motor is empty
		// => no more new chunks
		return 0;
	} else {
		// motor is not empty
		uint16_t data_idx = ridx*MOTOR_COUNT+m;
		return SyncQueue::data[data_idx];
	}
}

// # ================================================================ #
// # ::                                                            :: #
// # ================================================================ #

void SyncQueue::reset() {
	for (uint16_t i = 0; i < 256*MOTOR_COUNT; i+=1) {
		VOLATILE_UINT8(data[i]) = 0;
	}
	auto intr = Platform::InterruptProtectionFrame();
	VOLATILE_UINT8(widx) = VOLATILE_UINT8(ridx);
}

void SyncQueue::reset(uint8_t m) {
	for (uint16_t i = m; i < 256*MOTOR_COUNT; i+=MOTOR_COUNT) {
		VOLATILE_UINT8(data[i]) = 0;
	}
}
