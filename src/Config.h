
#define MOTOR_COUNT 5
#define MOTOR_BITS  ((1 << MOTOR_COUNT) - 1)
#define COMMAND_QUEUE_SIZE 16

#define PLATFORM_MEGA2560_RAMPS1_4
#include <Arduino.h>

//#define TRACK_ISR_POSITION