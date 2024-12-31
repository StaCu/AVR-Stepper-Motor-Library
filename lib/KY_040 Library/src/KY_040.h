#pragma once

#include "Pin.h"

class KY_040 {

private:

  uint8_t state;
  int16_t counter;

  Pin p0;
  Pin p1;
  
  /**
   * Polls the current state value from the rotary encoder pins.
   * 
   * @return
   *  the state. The 2 lsb represent both sensor values
   */
  uint8_t poll_state() const;
  
public:

  KY_040();
  
  /**
   * Initializes the KY-040. 
   * Polls the initial values from the rotary encoder pins and
   * sets the counter to 0
   */
  KY_040(Pin p0, Pin p1);
  
  void init(Pin p0, Pin p1);
  
  /**
   * Resets the counter to 0
   */
  void reset();

  /**
   * Polls new values from the rotary encoder pins.
   * If the values changed
   *  - the new values are checked for possible errors.
   *  - the delta in rotation steps is computed
   *  - the delta is added to the counter
   *  - the delta is retured
   * If the reset switch of the encoder is pressed (disabled)
   *  - the counter is reset to 0
   * 
   * @return
   *  the delta [-1,0,1]
   */
  int8_t poll();
  
  /**
   * Returns the current counter
   * 
   * @return
   *  the current counter
   */
  int16_t get() const;
  
};
