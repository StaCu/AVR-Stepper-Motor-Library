# AVR Stepper Motor Library

A stepper motor library for the AVR MEGA RAMPS 1.4 board.
This library can drive up to five stepper motors with up to 63750 steps/second each.

## Build

The `UartMachine` example controls the stepper motors according to incoming uart data.
Build it using:

```sh
platformio run examples/UartMachine
```

## How it works

The library manages a steps-queue, which contains the number of steps and direction for every motor divided into 4ms intervals.
A timer ISR is called 256 times in a 4ms interval.
Motors may change direction on the first call and step during the remaining.

Calling `StepperMotors::move` enqueues step and direction values for one 4ms interval into an asynchronous queue.
The ISR will work through the queue one 4ms interval at a time.
A simple useage example:

```c++
// start the ISR
StepperMotors::init();

uint8_t steps_dir[10] = {
    255, 0, // motor[0] will move 255 steps forwards
     10, 1, // motor[1] will move 10 steps backwards
      0, 0, // motor[2-4] will not move
      0, 0,
      0, 0
};
// enqueue next 4ms interval
StepperMotors::move(steps_dir);
```

