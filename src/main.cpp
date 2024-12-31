#include <Arduino.h>
#include <stdint.h>
#include <StepperMotors.h>

void setup() {
    Serial.begin(115200);
    while (!Serial);
    Serial.println("start");
    StepperMotors::init();
}

uint8_t read() {
    while(!Serial.available());
    uint8_t c = Serial.read();
    return c;
}

int32_t pos[] = { 0, 0, 0, 0 };

void loop() {
    uint8_t c = read();
    if (c == 64) {
        for (uint8_t m = 0; m < StepperMotors::motor_count(); m++) {
            uint8_t direction = read();
            uint8_t value = read();
            if (direction == 0) {
                pos[m] -= value;
            } else {
                pos[m] += value;
            }
            StepperMotors::move(m, direction, value);
        }
    } else if (c == 100) {
        StepperMotors::turn_on();
        pos[0] = 0;
        pos[1] = 0;
        pos[2] = 0;
        pos[3] = 0;
    } else if (c == 101) {
        StepperMotors::turn_off();
    } else if (c == 102) {
        StepperMotors::stop();
    } else if (c == 103) {
        uint8_t free = StepperMotors::free();
        Serial.write(free);
        uint8_t is_blocked = 0;
        uint8_t is_at_end = 0;
        for (uint8_t m = 0; m < StepperMotors::motor_count(); m++) {
            uint8_t state = (StepperMotors::is_blocked(m) << 1) | StepperMotors::is_at_end(m);
            Serial.write(state);
        }
    } else if (c == 104) {
        uint8_t motor = read();
        StepperMotors::unblock(motor);
    } else if (c == 105) {
        uint8_t free = StepperMotors::free();
        Serial.write(free);
    } else if (c == 106) {
        for (uint8_t i = 0; i < 4*4; i++) {
            Serial.write(((uint8_t*) pos)[i]);
        }
    }
}
