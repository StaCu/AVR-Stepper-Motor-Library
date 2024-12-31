#include "Pin.h"

#define PS2   (1<<ADPS0)                             //8000kHz ADC clock freq
#define PS4   (1<<ADPS1)                             //4000kHz
#define PS8   ((1<<ADPS0) | (1<<ADPS1))              //2000kHz
#define PS16  (1<<ADPS2)                             //1000kHz
#define PS32  ((1<<ADPS2) | (1<<ADPS0))              //500kHz
#define PS64  ((1<<ADPS2) | (1<<ADPS1))              //250kHz
#define PS128 ((1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0)) //125kHz
#define ANALOG_V_REF     DEFAULT //INTERNAL, EXTERNAL, or DEFAULT
#define ADC_PRESCALE     PS128   //PS16, PS32, PS64 or P128(default)

/*
Each port pin consists of three register bits: DDxn, PORTxn, and PINxn. As shown in ”Register Description” on
page 100, the DDxn bits are accessed at the DDRx I/O address, the PORTxn bits at the PORTx I/O address,
and the PINxn bits at the PINx I/O address.

The DDxn bit in the DDRx Register selects the direction of this pin. If DDxn is written logic one, Pxn is
configured as an output pin. If DDxn is written logic zero, Pxn is configured as an input pin.

If PORTxn is written logic one when the pin is configured as an input pin, the pull-up resistor is activated. To
switch the pull-up resistor off, PORTxn has to be written logic zero or the pin has to be configured as an output
pin. The port pins are tri-stated when reset condition becomes active, even if no clocks are running.

If PORTxn is written logic one when the pin is configured as an output pin, the port pin is driven high (one). If
PORTxn is written logic zero when the pin is configured as an output pin, the port pin is driven low (zero).
*/

Pin::Pin() {
  pin = 0;
  port = nullptr;
}

Pin::Pin(uint8_t id) {
  pin = digitalPinToBitMask(id);
  port = (uint8_t*) (portInputRegister(digitalPinToPort(id))+2);
  /*
  Serial.print((uint16_t)port, DEC);
  Serial.print("  ");
  Serial.println(pin, DEC);*/
}

Pin::Pin(uint8_t *port, uint8_t pin) 
: port(port), pin(pin) {
  
}

Pin::Pin(uint16_t port, uint8_t pin) 
: port((uint8_t*)port), pin(pin) {
  
}

Pin::Pin(const Pin &other) 
: port(other.port), pin(other.pin) {
  
}

void Pin::setModeDigitalRead() {
  uint8_t *ddr = port-1;
  *ddr &= ~pin;
  /*
  asm volatile (
    "clr r27            \n"
    "mov r26, %[DDR]    \n"
    "ld  r18, X         \n" // read current port value
    "and r18, %[PIN]    \n"
    "st  X, r18         \n" // write new port value
    : 
    : [DDR] "r" (port - 1), [PIN] "r" (~pin)
    :  "r18", "r26", "r27"
  );*/
}

void Pin::setModeDigitalWrite() {
  uint8_t *ddr = port-1;
  *ddr |= pin;
  
  //pinMode(pin, OUTPUT);
 /* asm volatile (
    "clr r27            \n"
    "mov r26, %[DDR]    \n"
    "ld  r18, X         \n" // read current port value
    "or  r18, %[PIN]    \n"
    "st  X, r18         \n" // write new port value
    : 
    : [DDR] "r" (port - 1), [PIN] "r" (pin)
    :  "r18", "r26", "r27"
  );*/
}

bool Pin::read() const {
  uint8_t *data = port-2;
  uint8_t value = *data;
  return value & pin;
  /*
  uint8_t value;
  asm volatile (
    "clr r27              \n"
    "mov r26, %[PORT]     \n"
    "ld  %[VALUE], X      \n" // read current port value
    : [VALUE] "=r" (value)
    : [PORT] "r" (port - 2)
    : "r26", "r27"
  );
  value &= pin;
  return value != 0;*/
}

void Pin::write(bool value) {
  if (value) writeHigh();
  else writeLow();
}

void Pin::writeHigh() {
  *port |= pin;/*
  digitalWrite(pin, HIGH);
  asm volatile (
    "clr r27            \n"
    "mov r26, %[PORT]   \n"
    "ld  r18, X         \n" // read current port value
    "or  r18, %[PIN]    \n" // update
    "st  X, r18         \n" // write new port value
    : 
    : [PORT] "r" (port), [PIN] "r" (pin)
    :  "r18", "r26", "r27"
  );*/
}

void Pin::writeLow() {
  *port &= ~pin;/*
  digitalWrite(pin, LOW);
  asm volatile (
    "clr r27            \n"
    "mov r26, %[PORT]   \n"
    "ld  r18, X         \n" // read current port value
    "and  r18, %[PIN]   \n" // update
    "st  X, r18         \n" // write new port value
    : 
    : [PORT] "r" (port), [PIN] "r" (~pin)
    :  "r18", "r26", "r27"
  );*/
}

// # ================================================================ #
// # :: STATIC                                                     :: #
// # ================================================================ #

uint16_t Pin::readAnalog(uint8_t pin) {
  uint16_t analog = 0;
  asm volatile (
  "andi %1, 0x07    \n" //force pin==0 thru 7
  "ori  %1, (%6<<6) \n" //(pin | ADC Vref)
  "sts  %2, %1      \n" //set ADMUX

  "lds  r18, %3             \n" //get ADCSRA
  "andi r18, 0xf8           \n" //clear prescale bits
  "ori  r18, ((1<<%5) | %7) \n" //(new prescale | ADSC)
  "sts  %3, r18             \n" //set ADCSRA

  "_loop:       \n" //loop until ADSC cleared
  "lds  r18, %3 \n"
  "sbrc r18, %5 \n"
  "rjmp _loop   \n"

  "lds  %A0, %4   \n" //result = ADCL
  "lds  %B0, %4+1 \n" //ADCH

  : "=r" (analog) : "r" (pin), "M" (_SFR_MEM_ADDR(ADMUX)),
  "M" (_SFR_MEM_ADDR(ADCSRA)), "M" (_SFR_MEM_ADDR(ADCL)),
  "I" (ADSC), "I" (ANALOG_V_REF), "M" (ADC_PRESCALE)
  : "r18"
  );
  return analog;
}

void Pin::addRandomness(uint8_t pin) {
  uint16_t analog = Pin::readAnalog(pin);
  uint16_t rnd    = random();
  randomSeed(analog + rnd);
}
