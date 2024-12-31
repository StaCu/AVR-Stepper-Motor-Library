#pragma once

#include <stdint.h>
#include "Config.h"

class StepperMotors {

public:
    static void init();

    static uint8_t motor_count();

    static bool free();

    static void move(uint16_t *vel);

    static bool is_at_end(uint8_t motor);
    static bool is_blocked(uint8_t motor);
    static void unblock(uint8_t motor);

    static void turn_on();
    static void turn_on(uint8_t motor);

    static void turn_off();
    static void turn_off(uint8_t motor);

    static void stop();
    static void stop(uint8_t motor);

};
