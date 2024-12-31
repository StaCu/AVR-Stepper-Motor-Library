#pragma once

#include <stdint.h>
#include "../Config.h"
#include "../queue/SyncQueue.h"

class Platform {

public:

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
