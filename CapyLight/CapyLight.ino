
typedef enum {
  CROSSFADE,
  LOFI,
  HATS,
  NUMMODES
} MODE;

MODE _currentMode; 
const int INPUT_BUTTON_MODESWITCH = 2;
unsigned long _modeswitchbtn_lastontime;
// int _modeswitch_lastbuttonstate;
const int MODE_SWITCH_WINDOW = 40; // in milliseconds

void setup() {
  Serial.begin(9600);
  _currentMode = CROSSFADE;
  _modeswitchbtn_lastontime = millis();
  // _modeswitch_lastbuttonstate = 0;
  pinMode(INPUT_BUTTON_MODESWITCH, INPUT_PULLUP);
}

void loop() {
  
  checkChangeMode();

  switch (_currentMode) {
    case CROSSFADE:
      // Serial.println("CROSSFADE MODE");
      break;
    case LOFI:
      // Serial.println("LOFI MODE");
      break;
    case HATS:
      // Serial.println("HATS MODE");
      break;
  }
  delay(5);
}

void checkChangeMode() {
  if (millis() - _modeswitchbtn_lastontime >= MODE_SWITCH_WINDOW) {
    res = digitalRead(inputPin);
    _modeswitchbtn_lastontime = millis();
  }

  int buttonState = readButtonPullUp(INPUT_BUTTON_MODESWITCH);
  Serial.println(buttonState);
  // Serial.print(" ");
  // Serial.println(_modeswitch_lastbuttonstate);
  if (buttonState == 1 && _modeswitch_lastbuttonstate == 0) {
    _currentMode = (_currentMode + 1) % NUMMODES;
  }
  _modeswitch_lastbuttonstate = buttonState;
}

// Using internal pull up resistor,
// false is off, true is on.
// Provides debouncing
// int readButtonPullUp(int inputPin) {
//   int res = 1;
//   if (millis() - _modeswitchbtn_lastontime >= MODE_SWITCH_WINDOW) {
//     res = digitalRead(inputPin);
//     // Serial.println(buttonState);
//     _modeswitchbtn_lastontime = millis();
//     // Serial.println(_modeswitchbtn_lastontime);
//   }

//   if (res == 1) {
//     return 0;
//   } else {
//     return 1;
//   }
// }