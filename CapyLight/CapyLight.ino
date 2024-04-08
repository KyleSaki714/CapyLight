#include <CapacitiveSensor.h>
typedef enum {
  CROSSFADE,
  LOFI,
  HATS,
  NUMMODES
} MODE;

// MODE SWITCH

const int INPUT_BUTTON_MODESWITCH = 7;
const int MODE_SWITCH_DELAY = 40; // in milliseconds

MODE _currentMode;
unsigned long _modeswitchbtn_lastontime;
bool _modeswitchbtn_laststate;

// CROSSFADE



// LOFI
CapacitiveSensor cs_4_2 = CapacitiveSensor(4,2);        // 10M resistor between pins 4 & 2, pin 2 is sensor pin, add a wire and or foil if desired
int petCounter;
bool lastCapState;
unsigned long lastLowState;
const int petThreshold = 1500; // how long until the pet sensing resets (ms)

// HATS



void setup() {
  Serial.begin(9600);
  
  _currentMode = CROSSFADE;
  _modeswitchbtn_lastontime = millis();
  _modeswitchbtn_laststate = LOW;
  pinMode(INPUT_BUTTON_MODESWITCH, INPUT_PULLUP);

  petCounter = 0;
}

void loop() {

  checkChangeMode();

  switch (_currentMode) {
    case CROSSFADE:
      crossfadeMode();
      break;
    case LOFI:
      lofiMode();
      break;
    case HATS:
      hatsMode();
      break;
  }
  delay(50);
}

void crossfadeMode() {
  Serial.println("CROSSFADE MODE");
}

void lofiMode() {
  // Serial.println("LOFI MODE");
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
  petCounter = petCounter % 100;
}

void hatsMode() {
  Serial.println("HATS MODE");
}

void checkChangeMode() {
  int buttonState = readButtonPullUp(INPUT_BUTTON_MODESWITCH);
  // Serial.println(buttonState);
  if (buttonState != -1 && buttonState == HIGH) {
    _currentMode = (_currentMode + 1) % NUMMODES;
  }
}

/**
 * Reads the chosen input pin assuming it is an INTERNAL_PULL_UP input pin.
 * Reverses the values so that returning LOW means switch open, and HIGH means switch closed.
 * Provides debouncing; returns -1 if called during a debounce window.
*/
int readButtonPullUp(int inputPin) {
  int buttonPullUpVal = digitalRead(inputPin);
  if (buttonPullUpVal != _modeswitchbtn_laststate && millis() - _modeswitchbtn_lastontime >= MODE_SWITCH_DELAY) {
    _modeswitchbtn_laststate = buttonPullUpVal;
    _modeswitchbtn_lastontime = millis();

    return !_modeswitchbtn_laststate;
  }
  return -1;
}