#include <CapacitiveSensor.h>
/*
 * CapitiveSense Library Demo Sketch
 * Paul Badger 2008
 * Uses a high value resistor e.g. 10M between send pin and receive pin
 * Resistor effects sensitivity, experiment with values, 50K - 50M. Larger resistor values yield larger sensor values.
 * Receive pin is the sensor pin - try different amounts of foil/metal on this pin
 */
CapacitiveSensor cs_4_2 = CapacitiveSensor(4,2);        // 10M resistor between pins 4 & 2, pin 2 is sensor pin, add a wire and or foil if desired
int petCounter;
bool lastCapState;
unsigned long lastLowState;
const int petThreshold = 1500; // how long until the pet sensing resets (ms)

void setup()                    
{
  //  cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
   Serial.begin(9600);
   petCounter = 0;
}

void loop()                    
{
  long sense =  cs_4_2.capacitiveSensor(30);
  bool isSense = sense > 0 ? HIGH : LOW;
  // enter low state
  // time since last low state
  if (isSense != lastCapState && isSense == HIGH) {
    petCounter += 1;
    lastLowState = 0;
  } else if (isSense != lastCapState && isSense == LOW) {
    lastLowState = millis();
  }
  
  if (isSense == LOW && millis() - lastLowState > petThreshold) {
    petCounter = 0;
  }

  lastCapState = sense;

  Serial.print(isSense);                  // print sensor output 1
  Serial.print(" ");
  Serial.print(sense);                  // print sensor output 1
  Serial.print(" ");
  Serial.println(petCounter);
  delay(50);
  petCounter = petCounter % 100;
}