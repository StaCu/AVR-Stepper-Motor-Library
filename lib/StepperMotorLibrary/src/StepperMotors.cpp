#include "StepperMotors.h"

#include "queue/Queue.h"
#include "platform/Platform.h"

// # ================================================================ #
// # ::  SETTINGS                                                  :: #
// # ================================================================ #

void StepperMotors::init() {
	Queue::reset();
	Platform::init();
	Platform::start();
}

// # ================================================================ #
// # ::  COMMANDS                                                  :: #
// # ================================================================ #

uint8_t StepperMotors::motor_count() {
	return MOTOR_COUNT;
}

bool StepperMotors::idle() {
	return Queue::idle();
}

uint8_t StepperMotors::free() {
	return Queue::free(0);
}

void StepperMotors::move(uint8_t motor, uint8_t direction, uint8_t value) {
	Queue::enq(motor, direction, value);
}

bool StepperMotors::is_at_end(uint8_t motor) {
	return Platform::is_at_end(motor);
}

bool StepperMotors::is_blocked(uint8_t motor) {
	return Platform::is_blocked(motor);
}

void StepperMotors::unblock(uint8_t motor) {
	Platform::unblock(motor);
}

void StepperMotors::turn_on() {
	for (uint8_t m = 0; m < MOTOR_COUNT; m++) {
		Platform::turn_on(m);
	}
}

void StepperMotors::turn_on(uint8_t motor) {
	Platform::turn_on(motor);
}

void StepperMotors::turn_off() {
	for (uint8_t m = 0; m < MOTOR_COUNT; m++) {
		Platform::turn_off(m);
	}
}

void StepperMotors::turn_off(uint8_t motor) {
	Platform::turn_off(motor);
}

void StepperMotors::stop() {
	auto intr = Platform::InterruptProtectionFrame();
	Queue::reset();
}

void StepperMotors::stop(uint8_t motor) {
	Queue::reset(motor);
}
