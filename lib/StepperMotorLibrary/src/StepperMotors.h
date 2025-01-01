#pragma once

#include <stdint.h>
#include "Config.h"

class StepperMotors {

public:
    static void init();

    static uint8_t motor_count();

    static uint8_t free();

    static void move(uint8_t *vel_dir);

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
