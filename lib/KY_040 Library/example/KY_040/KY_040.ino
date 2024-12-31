#include <Arduino.h>

//#include "../../src/KY_040.h"
#include "src/KY_040.h"

KY_040 rot_enc;
Pin pin;

void setup() {
  Serial.begin(19200);
  while (!Serial);
  
  rot_enc.init(
    Pin(PORTD, 0b10000),
    Pin(PORTD, 0b1000)
  );

  pin = Pin(PORTD, 0b100);
  pin.setModeDigitalRead();
  
  if (pin.read()) {
    Serial.println("+");
  } else {
    Serial.println("-");
  }
  Serial.print("start");
}

void loop() {  
  int8_t dir = rot_enc.poll();
  int16_t pos = rot_enc.get();
 
  if (dir == 0) {
    return;
  }
  
  Serial.print(dir);
  Serial.print(" ");
  Serial.println(pos);
}
