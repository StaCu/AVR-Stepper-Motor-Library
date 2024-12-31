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
uint8_t intr_timing;

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
	OCR1A = 50; // the correct value is set inside the isr
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
	intr_timing = 0;
	
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
	switch (intr_iteration) {
	case 0:
	case 1:
	case 2:
	case 3:
#if MOTOR_COUNT >= 5
	case 4:
#endif
	{
		Queue::next(intr_iteration);
	} break;
	case 5: {
		Queue::next();
	} break;

	case 6: {
        // ~17 cycles == 1105ns
        intr_buffer[0] = Queue::next_data[0];
		// no need to reset this, because the counter always ends up with the same value it started
        //intr_buffer[1] = 127; 
        if (Queue::next_data[1]) {
            MOTOR0_DIR_PORT &= ~MOTOR0_DIR_PIN;
        } else {
            MOTOR0_DIR_PORT |= MOTOR0_DIR_PIN;
        }
	} break;
	case 7: {
        // ~17 cycles == 1105ns
        intr_buffer[2] = Queue::next_data[2];
		// no need to reset this, because the counter always ends up with the same value it started
        //intr_buffer[1] = 127; 
        if (Queue::next_data[3]) {
            MOTOR1_DIR_PORT &= ~MOTOR1_DIR_PIN;
        } else {
            MOTOR1_DIR_PORT |= MOTOR1_DIR_PIN;
        }
	} break;
	case 8: {
        // ~17 cycles == 1105ns
        intr_buffer[4] = Queue::next_data[4];
		// no need to reset this, because the counter always ends up with the same value it started
        //intr_buffer[1] = 127; 
        if (Queue::next_data[5]) {
            MOTOR2_DIR_PORT &= ~MOTOR2_DIR_PIN;
        } else {
            MOTOR2_DIR_PORT |= MOTOR2_DIR_PIN;
        }
	} break;
	case 9: {
        // ~17 cycles == 1105ns
        intr_buffer[6] = Queue::next_data[6];
		// no need to reset this, because the counter always ends up with the same value it started
        //intr_buffer[1] = 127; 
        if (Queue::next_data[7]) {
            MOTOR3_DIR_PORT &= ~MOTOR3_DIR_PIN;
        } else {
            MOTOR3_DIR_PORT |= MOTOR3_DIR_PIN;
        }
	} break;
#if MOTOR_COUNT >= 5
	case 10: {
        // ~17 cycles == 1105ns
        intr_buffer[8] = Queue::next_data[8];
		// no need to reset this, because the counter always ends up with the same value it started
        //intr_buffer[1] = 127; 
        if (Queue::next_data[9]) {
            MOTOR4_DIR_PORT &= ~MOTOR4_DIR_PIN;
        } else {
            MOTOR4_DIR_PORT |= MOTOR4_DIR_PIN;
        }
	} break;
#endif

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
	} break;*/
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
	
	// we want this ISR to fire 255 times per 4ms period.
	// 1÷16000000×255×250 == 0.003984375 (too small)
	// 1÷16000000×255×251 == 0.004000313 (too large)
	// => combine both:
	// 1÷16000000×255×(251×50+250)÷51 = 0.004
	// => subtract one because OCR1AL is a zero based counter
	if (intr_timing == 0) {
		OCR1AL = 249;
		intr_timing = 50;
	} else {
		OCR1AL = 250;
		intr_timing -= 1;
	}

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

ISR(TIMER1_COMPB_vect) {
	TIMSK1 &= ~(1<<OCIE1B);
	sei();
	for (int m = 0; m < MOTOR_COUNT; m++) {
		if (end_detection[m<<1] != end_detection[(m<<1) + 1]) {
			end_detection[(m<<1) + 1] = end_detection[m<<1];
			blocked[m] = 1;
			Queue::reset(m);
		}
	}
}

#endif
