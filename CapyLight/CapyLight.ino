#include <CapacitiveSensor.h>
#include "src/RGBConverter/RGBConverter.h"

typedef enum {
  CROSSFADE,
  LOFI,
  NUMMODES
} MODE;

// RGB LED

typedef struct {
  int r;
  int g;
  int b;
} Color;


// Change this to based on whether you are using a common anode or common cathode
// RGB LED. See: https://makeabilitylab.github.io/physcomp/arduino/rgb-led
// If you are working with a common cathode RGB LED, set this to false.
const boolean COMMON_ANODE = false;

const int RGB_RED_PIN = 3;
const int RGB_GREEN_PIN  = 5;
const int RGB_BLUE_PIN  = 6;
const int DELAY_INTERVAL = 5; // interval in ms between incrementing hues
const byte MAX_RGB_VALUE = 255;

float _hue = 0; //hue varies between 0 - 1
RGBConverter _rgbConverter;

// MODE SWITCH

const int INPUT_BUTTON_MODESWITCH = 7;
const int MODE_SWITCH_DELAY = 40; // in milliseconds

MODE _currentMode;
unsigned long _modeswitchbtn_lastontime;
bool _modeswitchbtn_laststate;


// CROSSFADE
typedef struct {
  Color white;
  Color yellow;
  Color warmYellow;
  Color red;
} Mood;

const float CROSSFADE_SPEED = 1000.0; // defines how fast the LED fades thru the entire _colorSet. 100=fast 1000=slow (looks nice tho)
const int COLORSET_SIZE = 7;
const uint8_t PHOTOCELL_PIN = A0;
const int PHOTOCELL_OFFSET = 190; // offset to account for ambient light (ceiling lights, etc.) so it's not white all the time
Color _colorSet[COLORSET_SIZE];
float _crossfade_currFraction;
int _i = 0;
float _currBrightness = 0.0;

// LOFI
const int petThreshold = 1500; // how long until the pet sensing resets (ms)
const int LOFI_HUE_STEP = 5;
const int LOFI_RES_CHANGE_THRESHOLD = 2;

const int LOFI_SUBMODE_PIN = 8; // pin for switch to select petting or lofi resistor.

CapacitiveSensor cs_4_2 = CapacitiveSensor(4,2);        // 10M resistor between pins 4 & 2, pin 2 is sensor pin, add a wire and or foil if desired
int _petCounter;
bool lastCapState;
unsigned long lastLowState;
int _lastLofiResVal;

// HATS



void setup() {
  Serial.begin(9600);
  
  // Set the RGB pins to output
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);

  setColor(255, 255, 255);

  _currentMode = CROSSFADE;
  Mood moods;
  moods.white = {255, 255, 255};
  moods.yellow = {150, 40, 0};
  moods.warmYellow = {220, 40, 0};
  moods.red = {220, 0, 0};

  _colorSet[0] = moods.white;
  _colorSet[1] = moods.yellow;
  _colorSet[2] = moods.warmYellow;
  _colorSet[3] = moods.red;
  _colorSet[4] = moods.warmYellow;
  _colorSet[5] = moods.yellow;
  _colorSet[6] = moods.white;

  _modeswitchbtn_lastontime = millis();
  _modeswitchbtn_laststate = LOW;
  pinMode(INPUT_BUTTON_MODESWITCH, INPUT_PULLUP);

  pinMode(8, INPUT_PULLUP);

  _petCounter = 0;
}

void loop() {

  checkChangeMode();

  switch (_currentMode) {
    case LOFI:
      lofiMode();
      break;
    case CROSSFADE:
    default:
      crossfadeMode();
      break;
  }
  delay(50);
}

void crossfadeMode() {
  Serial.println("CROSSFADE MODE");

  int lightVal = analogRead(A0);
  
  // add offset
  lightVal += PHOTOCELL_OFFSET;

  if (lightVal > 1023) {
    setColor(0,0,0);
    return;
  }

  Serial.println(lightVal);
  Serial.print(" ");

  float inverse = 1.0 - (lightVal / 1023.0);
  _currBrightness = inverse;
  Serial.print(_currBrightness);
  Serial.println(" ");

  _crossfade_currFraction = _i / CROSSFADE_SPEED;
  Color currentColor = interpolateColorSet(_crossfade_currFraction);

  double hsl[3];
  _rgbConverter.rgbToHsl(currentColor.r, currentColor.g, currentColor.b, hsl);

  hsl[2] = _currBrightness;

  byte rgb[3];
  _rgbConverter.hslToRgb(hsl[0], hsl[1], hsl[2], rgb);

  setColor(rgb[0], rgb[1], rgb[2]);


  // setColor(currentColor.r, currentColor.g, currentColor.b);
  _i = (_i + 1) % static_cast<int>(CROSSFADE_SPEED);
  // _currBrightness = (_currBrightness + 1) % 1
  // _currBrightness = _crossfade_currFraction;
  // Serial.println(_currBrightness);

}

void lofiMode() {
  // Serial.println("LOFI MODE");

  int lofiSubmode = digitalRead(LOFI_SUBMODE_PIN);

  if (lofiSubmode == HIGH) {
    lofiPettingMode();
  } else {
    lofiVariableReistorMode();
  }

  // // // if selecting hue by wire, override petting
  // int lofiResVal = analogRead(A5);

  // // only if there is change (WITHIN LOFI_RES_CHANGE_THRESHOLD) in the lofiRes, change color
  // int delta = abs(lofiResVal - _lastLofiResVal);
  // if (delta > LOFI_RES_CHANGE_THRESHOLD) {
  //   Serial.println("changing with lofi resistor");
  //   Serial.println(lofiResVal);

  //   _hue = lofiResVal / 1023.0;

  //   byte rgb[3];
  //   _rgbConverter.hslToRgb(_hue, 1, 0.5, rgb);

  //   setColor(rgb[0], rgb[1], rgb[2]); 

  //   _petCounter = 0;

  //   _lastLofiResVal = lofiResVal;
  //   return;
  // }
  // Serial.print("_lastLofiResVal");
  // Serial.println(_lastLofiResVal);


  // long sense =  cs_4_2.capacitiveSensor(30);
  // Serial.print(sense);
  // bool isPetting = sense > 0 ? HIGH : LOW;

  // // enter low state
  // // time since last low state
  // if (isPetting != lastCapState && isPetting == HIGH) {
  //   _petCounter += LOFI_HUE_STEP;

  //   _hue = _petCounter / 100.0;

  //   byte rgb[3];
  //   _rgbConverter.hslToRgb(_hue, 1, 0.5, rgb);

  //   setColor(rgb[0], rgb[1], rgb[2]); 

  //   lastLowState = 0;
  // } else if (isPetting != lastCapState && isPetting == LOW) {
  //   lastLowState = millis();
  // }
  
  // // reset pet counter after 1.5 seconds
  // if (isPetting == LOW && millis() - lastLowState > petThreshold) {
  //   _petCounter = 0;
  // }

  // lastCapState = isPetting;

  // Serial.print(isPetting);                  // print sensor output 1
  // Serial.print(" ");
  // Serial.print(sense);                  // print sensor output 1
  // Serial.print(" ");
  // Serial.println(_petCounter);
  // _petCounter = _petCounter % 100;
}

void lofiPettingMode() {
  Serial.println("petting mode");
  long sense =  cs_4_2.capacitiveSensor(30);
  Serial.print(sense);
  bool isPetting = sense > 0 ? HIGH : LOW;

  // enter low state
  // time since last low state
  if (isPetting != lastCapState && isPetting == HIGH) {
    _petCounter += LOFI_HUE_STEP;

    _hue = _petCounter / 100.0;

    byte rgb[3];
    _rgbConverter.hslToRgb(_hue, 1, 0.5, rgb);

    setColor(rgb[0], rgb[1], rgb[2]); 

    lastLowState = 0;
  } else if (isPetting != lastCapState && isPetting == LOW) {
    lastLowState = millis();
  }
  
  // reset pet counter after 1.5 seconds
  if (isPetting == LOW && millis() - lastLowState > petThreshold) {
    _petCounter = 0;
  }

  lastCapState = isPetting;

  Serial.print(isPetting);                  // print sensor output 1
  Serial.print(" ");
  Serial.print(sense);                  // print sensor output 1
  Serial.print(" ");
  Serial.println(_petCounter);
  _petCounter = _petCounter % 100;
}

void lofiVariableReistorMode() {
  int lofiResVal = analogRead(A5);

  // only if there is change (WITHIN LOFI_RES_CHANGE_THRESHOLD) in the lofiRes, change color
  // int delta = abs(lofiResVal - _lastLofiResVal);
  // if (delta > LOFI_RES_CHANGE_THRESHOLD) {
  Serial.println("changing with lofi resistor");
  Serial.println(lofiResVal);

  _hue = lofiResVal / 1023.0;

  byte rgb[3];
  _rgbConverter.hslToRgb(_hue, 1, 0.5, rgb);

  setColor(rgb[0], rgb[1], rgb[2]); 

  _petCounter = 0;

  _lastLofiResVal = lofiResVal;
  // }
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

/**
 * This is from https://makeabilitylab.github.io/physcomp/arduino/rgb-led-fade.html
 * setColor takes in values between 0 - 255 for the amount of red, green, and blue, respectively
 * where 255 is the maximum amount of that color and 0 is none of that color. You can illuminate
 * all colors by intermixing different combinations of red, green, and blue.
 *
 * This function is based on https://gist.github.com/jamesotron/766994
 */
void setColor(int red, int green, int blue)
{
  // If a common anode LED, invert values
  if(COMMON_ANODE == true){
    red = MAX_RGB_VALUE - red;
    green = MAX_RGB_VALUE - green;
    blue = MAX_RGB_VALUE - blue;
  }
  analogWrite(RGB_RED_PIN, red);
  analogWrite(RGB_GREEN_PIN, green);
  analogWrite(RGB_BLUE_PIN, blue);
}

/**
 * Interpolate between two RGB colors
 * From https://sl.bing.net/j1UrqXYF0k8
*/
Color interpolate(const Color& color1, const Color& color2, float fraction) {
    Color result;
    result.r = static_cast<int>((color2.r - color1.r) * fraction + color1.r);
    result.g = static_cast<int>((color2.g - color1.g) * fraction + color1.g);
    result.b = static_cast<int>((color2.b - color1.b) * fraction + color1.b);
    return result;
}

/**
 * Given two Colors, call setColor() every DELAY_INTERVAL,
 * linearly interpolating between these two Colors.
*/
void interpolateColor(Color color1, Color color2) {
  for (int i = 1; i < 101; i++) {
    float fraction = i / 100.0;
    Color interpolatedColor = interpolate(color1, color2, fraction);
    setColor(interpolatedColor.r, interpolatedColor.g, interpolatedColor.b);
    delay(DELAY_INTERVAL);
  }
}

// Interpolate between a set of colors
// Thanks Chat https://sl.bing.net/dADkpPUXIRw
Color interpolateColorSet(float fraction) {
    int segmentCount = COLORSET_SIZE - 1;
    int segmentIndex = static_cast<int>(fraction * segmentCount);
    float segmentFraction = fraction * segmentCount - segmentIndex;

    const Color& color1 = _colorSet[segmentIndex];
    const Color& color2 = _colorSet[segmentIndex + 1];

    Color interpolatedColor;
    interpolatedColor.r = static_cast<int>((color2.r - color1.r) * segmentFraction + color1.r);
    interpolatedColor.g = static_cast<int>((color2.g - color1.g) * segmentFraction + color1.g);
    interpolatedColor.b = static_cast<int>((color2.b - color1.b) * segmentFraction + color1.b);

    return interpolatedColor;
}
