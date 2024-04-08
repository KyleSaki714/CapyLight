#include <CapacitiveSensor.h>
#include "src/RGBConverter/RGBConverter.h"

typedef enum {
  CROSSFADE,
  LOFI,
  HATS,
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



// LOFI
const int petThreshold = 1500; // how long until the pet sensing resets (ms)
const int LOFI_HUE_STEP = 5;

CapacitiveSensor cs_4_2 = CapacitiveSensor(4,2);        // 10M resistor between pins 4 & 2, pin 2 is sensor pin, add a wire and or foil if desired
int _petCounter;
bool lastCapState;
unsigned long lastLowState;


// HATS



void setup() {
  Serial.begin(9600);
  
  // Set the RGB pins to output
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);

  setColor(255, 255, 255);

  _currentMode = CROSSFADE;
  _modeswitchbtn_lastontime = millis();
  _modeswitchbtn_laststate = LOW;
  pinMode(INPUT_BUTTON_MODESWITCH, INPUT_PULLUP);

  _petCounter = 0;
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
    _petCounter += LOFI_HUE_STEP;

    _hue = _petCounter / 100.0;

    byte rgb[3];
    _rgbConverter.hslToRgb(_hue, 1, 0.5, rgb);

    setColor(rgb[0], rgb[1], rgb[2]); 

    lastLowState = 0;
  } else if (isSense != lastCapState && isSense == LOW) {
    lastLowState = millis();
  }
  
  if (isSense == LOW && millis() - lastLowState > petThreshold) {
    _petCounter = 0;
  }

  lastCapState = sense;

  Serial.print(isSense);                  // print sensor output 1
  Serial.print(" ");
  Serial.print(sense);                  // print sensor output 1
  Serial.print(" ");
  Serial.println(_petCounter);
  _petCounter = _petCounter % 100;
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