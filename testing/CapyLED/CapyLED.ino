#include "Color.h"

// Change this to based on whether you are using a common anode or common cathode
// RGB LED. See: https://makeabilitylab.github.io/physcomp/arduino/rgb-led
// If you are working with a common cathode RGB LED, set this to false.
const boolean COMMON_ANODE = false;

const int RGB_RED_PIN = 3;
const int RGB_GREEN_PIN  = 5;
const int RGB_BLUE_PIN  = 6;
const int DELAY_INTERVAL = 50; // interval in ms between incrementing hues
const byte MAX_RGB_VALUE = 255;

const Color STARTING_COLOR(255, 255, 218);

void setup() {
  // put your setup code here, to run once:
  // Set the RGB pins to output
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);

  setColor(STARTING_COLOR.r(), STARTING_COLOR.g(), STARTING_COLOR.b());

  // Turn on Serial so we can verify expected colors via Serial Monitor
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  // white
  // setColor(255, 255, 255);
  // delay(5000);

  Color bruh1(220, 0, 0);
  Color bruh2(220, 40, 0);

  interpolateColor(bruh1, bruh2);

  // // green go from 0-40
  // for (int g = 0; g < 40; g++) {
  //   setColor(220, g, 0);
  //   Serial.print("1 change green ");
  //   Serial.println(g);
  //   delay(50);
  // }

  // // red go from 255-180
  // for (int r = 220; r > 179; r--) {
  //   setColor(r, 40, 0);
  //   Serial.print("2 change red ");
  //   Serial.println(r);
  //   delay(50);
  // }

  // // red go from 180-220
  // for (int r = 180; r < 221; r++) {
  //   setColor(r, 40, 0);
  //   Serial.print("3 change red ");
  //   Serial.println(r);
  //   delay(50);
  // }

  // // green go from 40-0
  // for (int g = 40; g >= 0; g--) {
  //   setColor(220, g, 0);
  //   Serial.print("4 change green ");
  //   Serial.println(g);
  //   delay(50);
  // }

  // for (int r = 150; r < 256; r++) {
  //   setColor(r, 40, 0);
  //   Serial.print("5 change red ");
  //   Serial.println(r);
  //   delay(50);
  // }
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
 * Given two Colors, call setColor() every DELAY_INTERVAL,
 * linearly interpolating between these two Colors.
*/
void interpolateColor(Color a, Color b) {
  Color bMinusA = b.subtract(a);




  for (int g = 0; g < 40; g++) {
    setColor(220, g, 0);
    delay(DELAY_INTERVAL);
  }
}
