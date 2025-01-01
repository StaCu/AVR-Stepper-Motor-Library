#include "Platform.h"
#ifdef PLATFORM_MEGA2560_RAMPS1_4

#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
/*
#define MOTOR0_CLK_PORT PORTF
#define MOTOR0_CLK_PIN  0b00000001
#define MOTOR0_DIR_PORT PORTF
#define MOTOR0_DIR_PIN  0b00000010
#define MOTOR0_EN_PORT  PORTD
#define MOTOR0_EN_PIN   0b10000000
#define MOTOR0_MIN_PORT PORTE
#define MOTOR0_MIN_PIN  0b00100000
#define MOTOR0_MAX_PORT PORTE
#define MOTOR0_MAX_PIN  0b00010000
*/
#define MOTOR0_CLK_PORT PORTC
#define MOTOR0_CLK_PIN  0b00000010
#define MOTOR0_DIR_PORT PORTC
#define MOTOR0_DIR_PIN  0b00001000
#define MOTOR0_EN_PORT  PORTC
#define MOTOR0_EN_PIN   0b10000000
#define MOTOR0_MIN_PORT PORTE
#define MOTOR0_MIN_PIN  0b00100000
#define MOTOR0_MAX_PORT PORTE
#define MOTOR0_MAX_PIN  0b00010000

#define MOTOR1_CLK_PORT PORTF
#define MOTOR1_CLK_PIN  0b01000000
#define MOTOR1_DIR_PORT PORTF
#define MOTOR1_DIR_PIN  0b10000000
#define MOTOR1_EN_PORT  PORTF
#define MOTOR1_EN_PIN   0b00000100
#define MOTOR1_MIN_PORT PORTJ
#define MOTOR1_MIN_PIN  0b00000010
#define MOTOR1_MAX_PORT PORTJ
#define MOTOR1_MAX_PIN  0b00000001

#define MOTOR2_CLK_PORT PORTL
#define MOTOR2_CLK_PIN  0b00001000
#define MOTOR2_DIR_PORT PORTL
#define MOTOR2_DIR_PIN  0b00000010
#define MOTOR2_EN_PORT  PORTK
#define MOTOR2_EN_PIN   0b00000001
#define MOTOR2_MIN_PORT PORTD
#define MOTOR2_MIN_PIN  0b00001000
#define MOTOR2_MAX_PORT PORTD
#define MOTOR2_MAX_PIN  0b00000100

#define MOTOR3_CLK_PORT PORTA
#define MOTOR3_CLK_PIN  0b00010000
#define MOTOR3_DIR_PORT PORTA
#define MOTOR3_DIR_PIN  0b01000000
#define MOTOR3_EN_PORT  PORTA
#define MOTOR3_EN_PIN   0b00000100
#define MOTOR3_MIN_PORT PORTD
#define MOTOR3_MIN_PIN  0b00001000
#define MOTOR3_MAX_PORT PORTD
#define MOTOR3_MAX_PIN  0b00000100

#define MOTOR4_CLK_PORT PORTC
#define MOTOR4_CLK_PIN  0b00000010
#define MOTOR4_DIR_PORT PORTC
#define MOTOR4_DIR_PIN  0b00001000
#define MOTOR4_EN_PORT  PORTC
#define MOTOR4_EN_PIN   0b10000000
#define MOTOR4_MIN_PORT PORTD
#define MOTOR4_MIN_PIN  0b00001000
#define MOTOR4_MAX_PORT PORTD
#define MOTOR4_MAX_PIN  0b00000100

#define GET_DDR(PORT) (*((&PORT)-1))
#define GET_PIN(PORT) (*((&PORT)-2))

uint8_t intr_buffer[MOTOR_COUNT*2];
uint8_t end_detection[MOTOR_COUNT*2];
uint8_t blocked[MOTOR_COUNT];
uint8_t intr_iteration;

void Platform::init() {
	Platform::stop();

	// CLK: output low
	// DIR: output low
	// EN : output high
	// MIN: input  high
	// MAX: input  high
	GET_DDR(MOTOR0_CLK_PORT) |=  MOTOR0_CLK_PIN;
			MOTOR0_CLK_PORT  &= ~MOTOR0_CLK_PIN;
	GET_DDR(MOTOR0_DIR_PORT) |=  MOTOR0_DIR_PIN;
			MOTOR0_DIR_PORT  &= ~MOTOR0_DIR_PIN;
	GET_DDR(MOTOR0_EN_PORT)  |=  MOTOR0_EN_PIN;
	        MOTOR0_EN_PORT   |=  MOTOR0_EN_PIN;
	GET_DDR(MOTOR0_MIN_PORT) &=  MOTOR0_MIN_PIN;
	        MOTOR0_MIN_PORT  |=  MOTOR0_MIN_PIN;
	GET_DDR(MOTOR0_MAX_PORT) &=  MOTOR0_MAX_PIN;
	        MOTOR0_MAX_PORT  |=  MOTOR0_MAX_PIN;

	GET_DDR(MOTOR1_CLK_PORT) |=  MOTOR1_CLK_PIN;
			MOTOR1_CLK_PORT  &= ~MOTOR1_CLK_PIN;
	GET_DDR(MOTOR1_DIR_PORT) |=  MOTOR1_DIR_PIN;
			MOTOR1_DIR_PORT  &= ~MOTOR1_DIR_PIN;
	GET_DDR(MOTOR1_EN_PORT)  |=  MOTOR1_EN_PIN;
	        MOTOR1_EN_PORT   |=  MOTOR1_EN_PIN;
	GET_DDR(MOTOR1_MIN_PORT) &=  MOTOR1_MIN_PIN;
	        MOTOR1_MIN_PORT  |=  MOTOR1_MIN_PIN;
	GET_DDR(MOTOR1_MAX_PORT) &=  MOTOR1_MAX_PIN;
	        MOTOR1_MAX_PORT  |=  MOTOR1_MAX_PIN;

	GET_DDR(MOTOR2_CLK_PORT) |=  MOTOR2_CLK_PIN;
			MOTOR2_CLK_PORT  &= ~MOTOR2_CLK_PIN;
	GET_DDR(MOTOR2_DIR_PORT) |=  MOTOR2_DIR_PIN;
			MOTOR2_DIR_PORT  &= ~MOTOR2_DIR_PIN;
	GET_DDR(MOTOR2_EN_PORT)  |=  MOTOR2_EN_PIN;
	        MOTOR2_EN_PORT   |=  MOTOR2_EN_PIN;
	GET_DDR(MOTOR2_MIN_PORT) &=  MOTOR2_MIN_PIN;
	        MOTOR2_MIN_PORT  |=  MOTOR2_MIN_PIN;
	GET_DDR(MOTOR2_MAX_PORT) &=  MOTOR2_MAX_PIN;
	        MOTOR2_MAX_PORT  |=  MOTOR2_MAX_PIN;

	GET_DDR(MOTOR3_CLK_PORT) |=  MOTOR3_CLK_PIN;
			MOTOR3_CLK_PORT  &= ~MOTOR3_CLK_PIN;
	GET_DDR(MOTOR3_DIR_PORT) |=  MOTOR3_DIR_PIN;
			MOTOR3_DIR_PORT  &= ~MOTOR3_DIR_PIN;
	GET_DDR(MOTOR3_EN_PORT)  |=  MOTOR3_EN_PIN;
	        MOTOR3_EN_PORT   |=  MOTOR3_EN_PIN;
	GET_DDR(MOTOR3_MIN_PORT) &=  MOTOR3_MIN_PIN;
	        MOTOR3_MIN_PORT  |=  MOTOR3_MIN_PIN;
	GET_DDR(MOTOR3_MAX_PORT) &=  MOTOR3_MAX_PIN;
	        MOTOR3_MAX_PORT  |=  MOTOR3_MAX_PIN;

	GET_DDR(MOTOR4_CLK_PORT) |=  MOTOR4_CLK_PIN;
			MOTOR4_CLK_PORT  &= ~MOTOR4_CLK_PIN;
	GET_DDR(MOTOR4_DIR_PORT) |=  MOTOR4_DIR_PIN;
			MOTOR4_DIR_PORT  &= ~MOTOR4_DIR_PIN;
	GET_DDR(MOTOR4_EN_PORT)  |=  MOTOR4_EN_PIN;
	        MOTOR4_EN_PORT   |=  MOTOR4_EN_PIN;
	GET_DDR(MOTOR4_MIN_PORT) &=  MOTOR4_MIN_PIN;
	        MOTOR4_MIN_PORT  |=  MOTOR4_MIN_PIN;
	GET_DDR(MOTOR4_MAX_PORT) &=  MOTOR4_MAX_PIN;
	        MOTOR4_MAX_PORT  |=  MOTOR4_MAX_PIN;

	// set up timer 1
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1B |= (1<<CS10);   // Prescaler 0
	TCCR1B |= (1<<WGM12);  // Clear Timer on Compare Match

	// compare value
	// we want this ISR to fire 256 times per 4ms period.
	// 1÷16000000×256×250 == 0.004
	OCR1A = 250;
	OCR1B = 60;
}

void Platform::turn_on(uint8_t motor) {
	if (motor == 0) {
		MOTOR0_EN_PORT &= ~MOTOR0_EN_PIN;
	} else if (motor == 1) {
		MOTOR1_EN_PORT &= ~MOTOR1_EN_PIN;
	} else if (motor == 2) {
		MOTOR2_EN_PORT &= ~MOTOR2_EN_PIN;
	} else if (motor == 3) {
		MOTOR3_EN_PORT &= ~MOTOR3_EN_PIN;
	} else if (motor == 4) {
		MOTOR4_EN_PORT &= ~MOTOR4_EN_PIN;
	}
}

void Platform::turn_off(uint8_t motor) {
	if (motor == 0) {
		MOTOR0_EN_PORT |= MOTOR0_EN_PIN;
	} else if (motor == 1) {
		MOTOR1_EN_PORT |= MOTOR1_EN_PIN;
	} else if (motor == 2) {
		MOTOR2_EN_PORT |= MOTOR2_EN_PIN;
	} else if (motor == 3) {
		MOTOR3_EN_PORT |= MOTOR3_EN_PIN;
	} else if (motor == 4) {
		MOTOR4_EN_PORT |= MOTOR4_EN_PIN;
	}
}

bool Platform::is_at_end(uint8_t motor) {
	if (motor == 0) {
		return (GET_PIN(MOTOR0_MIN_PORT) & MOTOR0_MIN_PIN) == 0;
	}
	return end_detection[motor<<1] != 0;
}

bool Platform::is_blocked(uint8_t motor) {
	return blocked[motor] != 0;
}

void Platform::unblock(uint8_t motor) {
	blocked[motor] = 0;
}

void Platform::start() {
	if (TIMSK1 & (1<<OCIE1A)) {
		// interrupt already enabled
		return;
	}

    intr_iteration = 0;
	
	// enable output compare A interrupt
	TIMSK1 |= (1<<OCIE1A);
	// enable interrupts  
	Platform::enableInterrupts();
}

void Platform::stop() {
	Platform::disableInterrupts();
	// disable output compare interrupt
	TIMSK1 &= ~((1<<OCIE1A)|(1<<OCIE1B)|(1<<OCIE1C));

    for (uint8_t i = 0; i < MOTOR_COUNT*2; i += 2) {
        intr_buffer[i] = 0;
        intr_buffer[i+1] = 127;
    }
}

void Platform::enableInterrupts() {
	sei();
}

void Platform::disableInterrupts() {
	cli();
}

ISR(TIMER1_COMPA_vect) {
	if (intr_iteration == 0) {
		intr_iteration = 1;
		if (SyncQueue::widx == SyncQueue::ridx) {
			// queue is empty
			intr_buffer[0] = 0;
			intr_buffer[2] = 0;
			intr_buffer[4] = 0;
			intr_buffer[6] = 0;
			// increment the motors write index, so it doesn't fall behind
			SyncQueue::ridx += 1;
			SyncQueue::widx = SyncQueue::ridx;
		} else {
			// queue is not empty
			uint16_t ridx = SyncQueue::ridx*MOTOR_COUNT*2;
			uint8_t *data = &SyncQueue::data[ridx];

			intr_buffer[0] = data[0];
        	intr_buffer[1] = 255;
			if (data[1] == 0) {
				MOTOR0_DIR_PORT &= ~MOTOR0_DIR_PIN;
			} else {
				MOTOR0_DIR_PORT |= MOTOR0_DIR_PIN;
			}

			intr_buffer[2] = data[2];
        	intr_buffer[3] = 255;
			if (data[3] == 0) {
				MOTOR1_DIR_PORT &= ~MOTOR1_DIR_PIN;
			} else {
				MOTOR1_DIR_PORT |= MOTOR1_DIR_PIN;
			}

			intr_buffer[4] = data[4];
        	intr_buffer[5] = 255;
			if (data[5] == 0) {
				MOTOR2_DIR_PORT &= ~MOTOR2_DIR_PIN;
			} else {
				MOTOR2_DIR_PORT |= MOTOR2_DIR_PIN;
			}

			intr_buffer[6] = data[6];
        	intr_buffer[7] = 255;
			if (data[7] == 0) {
				MOTOR3_DIR_PORT &= ~MOTOR3_DIR_PIN;
			} else {
				MOTOR3_DIR_PORT |= MOTOR3_DIR_PIN;
			}

			SyncQueue::ridx += 1;
		}
	} else {
		// drive motor[0]
		// ~17 cycles == 1105ns
		uint8_t vel = intr_buffer[0];
		uint8_t cnt = intr_buffer[1];
		uint8_t next_cnt = cnt + vel;
		if (next_cnt < cnt) {
			MOTOR0_CLK_PORT |= MOTOR0_CLK_PIN;
		}
		intr_buffer[1] = next_cnt;

		// drive motor[1]
		// ~15 cycles == 975ns
		vel = intr_buffer[2];
		cnt = intr_buffer[3];
		next_cnt = cnt + vel;
		if (next_cnt < cnt) {
			MOTOR1_CLK_PORT |= MOTOR1_CLK_PIN;
		}
		intr_buffer[3] = next_cnt;

		vel = intr_buffer[4];
		cnt = intr_buffer[5];
		next_cnt = cnt + vel;
		if (next_cnt < cnt) {
			MOTOR2_CLK_PORT |= MOTOR2_CLK_PIN;
		}
		intr_buffer[5] = next_cnt;

		vel = intr_buffer[6];
		cnt = intr_buffer[7];
		next_cnt = cnt + vel;
		if (next_cnt < cnt) {
			MOTOR3_CLK_PORT |= MOTOR3_CLK_PIN;
		}
		intr_buffer[7] = next_cnt;
	#if MOTOR_COUNT >= 5
		vel = intr_buffer[8];
		cnt = intr_buffer[9];
		next_cnt = cnt + vel;
		if (next_cnt < cnt) {
			MOTOR4_CLK_PORT |= MOTOR4_CLK_PIN;
		}
		intr_buffer[9] = next_cnt;
	#endif

		intr_iteration += 1;

		asm volatile (
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			: : :
		);

		// clear the step pin
		// ~8 cycles == 520ns
		MOTOR0_CLK_PORT &= ~MOTOR0_CLK_PIN;
		MOTOR1_CLK_PORT &= ~MOTOR1_CLK_PIN;
		MOTOR2_CLK_PORT &= ~MOTOR2_CLK_PIN;
		MOTOR3_CLK_PORT &= ~MOTOR3_CLK_PIN;
	#if MOTOR_COUNT >= 5
		MOTOR4_CLK_PORT &= ~MOTOR4_CLK_PIN;
	#endif
	}

/*
	switch (intr_iteration) {
	/*case 11: {
		uint8_t end = (GET_PIN(MOTOR0_MIN_PORT) & MOTOR0_MIN_PIN) == 0;
		if (end != end_detection[0]) {
			end_detection[0] = end;
			intr_buffer[0] = 0;
		}
	} break;*//*
	case 12: {
		uint8_t end = (GET_PIN(MOTOR1_MIN_PORT) & MOTOR1_MIN_PIN) != 0;
		if (end != end_detection[2]) {
			end_detection[2] = end;
			intr_buffer[2] = 0;
		}
	} break;
	case 13: {
		uint8_t end = (GET_PIN(MOTOR2_MIN_PORT) & MOTOR2_MIN_PIN) != 0;
		if (end != end_detection[4]) {
			end_detection[4] = end;
			intr_buffer[4] = 0;
		}
	} break;
	case 14: {
		uint8_t end = (GET_PIN(MOTOR3_MIN_PORT) & MOTOR3_MIN_PIN) != 0;
		if (end != end_detection[6]) {
			end_detection[6] = end;
			intr_buffer[6] = 0;
		}
	} break;*//*
#if MOTOR_COUNT >= 5
	case 15: {
		uint8_t end = (GET_PIN(MOTOR4_MIN_PORT) & MOTOR4_MIN_PIN) != 0;
		if (end != end_detection[8]) {
			end_detection[8] = end;
			intr_buffer[8] = 0;
		}
	} break;
#endif

	case 16: {
		TIMSK1 |= (1<<OCIE1B);
	} break;

	default: break;
	}
*/

}

ISR(TIMER1_COMPB_vect) {
	TIMSK1 &= ~(1<<OCIE1B);
	sei();
	for (int m = 0; m < MOTOR_COUNT; m++) {
		if (end_detection[m<<1] != end_detection[(m<<1) + 1]) {
			end_detection[(m<<1) + 1] = end_detection[m<<1];
			//blocked[m] = 1;
			//SyncQueue::reset(m);
		}
	}
}

#endif
