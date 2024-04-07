
typedef enum {
  CROSSFADE,
  LOFI,
  HATS,
  NUMMODES
} MODE;

const int INPUT_BUTTON_MODESWITCH = 2;
const int MODE_SWITCH_DELAY = 40; // in milliseconds

MODE _currentMode;
unsigned long _modeswitchbtn_lastontime;
bool _modeswitchbtn_laststate;

void setup() {
  Serial.begin(9600);
  _currentMode = CROSSFADE;
  _modeswitchbtn_lastontime = millis();
  _modeswitchbtn_laststate = LOW;
  pinMode(INPUT_BUTTON_MODESWITCH, INPUT_PULLUP);
}

void loop() {

  checkChangeMode();

  switch (_currentMode) {
    case CROSSFADE:
      Serial.println("CROSSFADE MODE");
      break;
    case LOFI:
      Serial.println("LOFI MODE");
      break;
    case HATS:
      Serial.println("HATS MODE");
      break;
  }
  delay(5);
}

void checkChangeMode() {
  int buttonState = readButtonPullUp(INPUT_BUTTON_MODESWITCH);
  Serial.println(buttonState);
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