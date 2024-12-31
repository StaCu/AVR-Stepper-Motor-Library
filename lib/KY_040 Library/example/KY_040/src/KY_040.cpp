#include "KY_040.h"

KY_040::KY_040() {

}

KY_040::KY_040(Pin p0, Pin p1) {
  init(p0, p1);
}

void KY_040::init(Pin p0, Pin p1) {
  this->p0 = p0;
  this->p1 = p1;
  p0.setModeDigitalRead();
  p1.setModeDigitalRead();
  state = poll_state();
}

uint8_t KY_040::poll_state() const {
  return (p0.read() << 1) | p1.read();
}

void KY_040::reset() {
  counter = 0;
}
  
int8_t KY_040::poll() {
  uint8_t new_state = poll_state();
  
  /**
   * state transitions
   * old state | new state | delta
   * ----------+-----------+-------
   * 00        | 00        |  0
   * 00        | 01        |  1
   * 00        | 10        | -1
   * 00        | 11        | error
   * 01        | 00        | -1
   * 01        | 01        |  0
   * 01        | 10        | error
   * 01        | 11        |  1
   * 10        | 00        |  1
   * 10        | 01        | error
   * 10        | 10        |  0
   * 10        | 11        | -1
   * 11        | 00        | error
   * 11        | 01        | -1
   * 11        | 10        |  1
   * 11        | 11        |  0
   */
   
   int8_t delta = 0;
   if (state == new_state || ((state ^ new_state) == 0b11)) {
     // delta = 0
     // or error
   } else {
     /*
      * the states shift through:
      * 110011001100110011
      *    xx
      *     ++
      *   --
      * xx: current state
      * --: next state (backwards)
      * ++: next state (forwards)
      * 
      * since it shifts one position at a time, there is one common bit
      * => if (lsb of the current state == msb of the next state) {
      *   delta = 1;
      * } else {
      *   delta = -1;
      * }
      */
     delta = (state & 0b1) == (new_state >> 1);
     delta += delta;
     delta -= 1;
     
     state = new_state;
   }
   
   //if (enable_reset && Pin::read()) {
     // the reset pin on the ky-040 has been pressed
     // => reset the counter
   //  counter = 0;
   //} else {
     // no reset
     // => increment in case the state changed
     counter += delta;
   //}
   
   return delta;
}
  
int16_t KY_040::get() const {
  return counter;
}
