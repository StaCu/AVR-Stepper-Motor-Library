#pragma once

#include <stdint.h>
#include "../Config.h"
#include "../queue/SyncQueue.h"

class Platform {

public:

    static uint8_t intr_buffer[MOTOR_COUNT*2];
    static uint8_t end_detection[MOTOR_COUNT*2];
    static uint8_t blocked[MOTOR_COUNT];
    static uint8_t intr_iteration;

    static volatile int32_t isr_pos[MOTOR_COUNT];
    static int8_t isr_dir[MOTOR_COUNT];
    static int8_t isr_steps[MOTOR_COUNT];

    static void init();

    static void start();

    static void stop();

    static void turn_on(uint8_t motor);

    static void turn_off(uint8_t motor);

    static bool is_at_end(uint8_t motor);

    static bool is_blocked(uint8_t motor);

    static void unblock(uint8_t motor);

    static void enableInterrupts();

    static void disableInterrupts();

    class InterruptProtectionFrame {
    public:
        InterruptProtectionFrame() {
            Platform::disableInterrupts();
        }
        ~InterruptProtectionFrame() {
            Platform::enableInterrupts();
        }
    };

};
