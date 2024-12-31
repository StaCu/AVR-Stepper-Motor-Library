#pragma once

#if ARDUINO > 22
#include "Arduino.h"
#else
#include "WProgram.h"
#include "pins_arduino.h"
#endif
#include <avr/io.h>
#include <stdint.h>

class Pin {

public:

  uint8_t *port;
  uint8_t pin;

  Pin();
  Pin(uint8_t id);
  Pin(uint8_t *port, uint8_t pin);
  Pin(uint16_t port, uint8_t pin);
  Pin(const Pin &other);

  void setModeDigitalRead();
  
  void setModeDigitalWrite();
  
  bool read() const;
  
  void write(bool value);

  void writeHigh();
  
  void writeLow();
  
// # ================================================================ #
// # :: STATIC                                                     :: #
// # ================================================================ #

  template<uint16_t PORT, uint8_t PIN>
  static void setModeDigitalRead();
  
  template<uint16_t PORT, uint8_t PIN>
  static void setModeDigitalWrite();
  
  template<uint16_t PORT, uint8_t PIN>
  static bool read();
  
  template<uint16_t PORT, uint8_t PIN>
  static void write(const bool &value);
  
  template<uint16_t PORT, uint8_t PIN>
  static void writeHigh();
  
  template<uint16_t PORT, uint8_t PIN>
  static void writeLow();
  
  static uint16_t __attribute__((noinline)) readAnalog(uint8_t pin);
  
  static void addRandomness(uint8_t pin);
    
};

template<uint16_t PORT, uint8_t PIN>
void Pin::setModeDigitalRead() {
  //if (PORT >= 64) {
    return Pin(PORT, PIN).setModeDigitalRead();
 // }
  // Set the Data Direction of the pin to 0
  // => the DataDirectionRegister is located at PORT-Address - 1
  asm volatile (
    "cbi %[DDR], %[PIN] \n"
    :
    : [DDR] "I" (PORT - __SFR_OFFSET - 1), [PIN] "I" (PIN)
    :
  );
}

template<uint16_t PORT, uint8_t PIN>
void Pin::setModeDigitalWrite() {
 // if (PORT >= 64) {
    return Pin(PORT, PIN).setModeDigitalWrite();
  //}
  // Set the Data Direction of the pin to 1
  // => the DataDirectionRegister is located at PORT-Address - 1
  asm volatile (
    "sbi %[DDR], %[PIN] \n"
    :
    : [DDR] "I" (PORT - __SFR_OFFSET - 1), [PIN] "I" (PIN)
    :
  );
}

template<uint16_t PORT, uint8_t PIN>
bool Pin::read() {
//  if (PORT >= 64) {
    return Pin(PORT, PIN).read();
 // }
  // read the Pin value of the pin
  // => the PinRegister is located at PORT-Address - 2
  bool value;
  asm volatile (
    "clr  %[VALUE]        \n"
    "sbic %[PORT], %[PIN] \n"
    "ldi  %[VALUE], 1     \n"
    : [VALUE] "=d" (value)
    : [PORT] "I" (PORT - __SFR_OFFSET - 2), [PIN] "I" (PIN)
    :
  );
  return value;
}

template<uint16_t PORT, uint8_t PIN>
void Pin::write(const bool &value) {
//  if (PORT >= 64) {
    return Pin(PORT, PIN).write(value);
 // }
  // write the Port value of the pin
  // => the PortRegister is located at PORT-Address
  volatile uint8_t output = value; //LOW or HIGH
  asm volatile (
    "sbrc %[VALUE], 0    \n"
    "sbi %[PORT], %[PIN] \n"
    "sbrs %[VALUE], 0    \n"
    "cbi %[PORT], %[PIN] \n"
    : 
    : [PORT] "I" (PORT - __SFR_OFFSET), [PIN] "I" (PIN),
      [VALUE] "r" (output)
    :
  );
}

template<uint16_t PORT, uint8_t PIN>
void Pin::writeHigh() {
 // if (PORT >= 64) {
    return Pin(PORT, PIN).writeHigh();
 // }
  // write the Port value of the pin
  // => the PortRegister is located at PORT-Address
  asm volatile (
    "sbi %[PORT], %[PIN] \n"
    : 
    : [PORT] "I" (PORT - __SFR_OFFSET), [PIN] "I" (PIN)
    :
  );
}

template<uint16_t PORT, uint8_t PIN>
void Pin::writeLow() {
 // if (PORT >= 64) {
    return Pin(PORT, PIN).writeLow();
 // }
  // write the Port value of the pin
  // => the PortRegister is located at PORT-Address
  asm volatile (
    "cbi %[PORT], %[PIN] \n"
    : 
    : [PORT] "I" (PORT - __SFR_OFFSET), [PIN] "I" (PIN)
    :
  );
}
