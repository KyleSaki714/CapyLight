
// Change this to based on whether you are using a common anode or common cathode
// RGB LED. See: https://makeabilitylab.github.io/physcomp/arduino/rgb-led
// If you are working with a common cathode RGB LED, set this to false.
const boolean COMMON_ANODE = false;

const int RGB_RED_PIN = 3;
const int RGB_GREEN_PIN  = 5;
const int RGB_BLUE_PIN  = 6;
const int DELAY_INTERVAL = 5; // interval in ms between incrementing hues
const byte MAX_RGB_VALUE = 255;

typedef struct {
  int r;
  int g;
  int b;
} Color;

typedef struct {
  Color white;
  Color yellow;
  Color warmYellow;
  Color red;
} Mood;

const int COLORSET_SIZE = 7;
Color _colorSet[COLORSET_SIZE];

void setup() {
  // put your setup code here, to run once:
  // Set the RGB pins to output
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);

  setColor(255, 255, 255);

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


  // _colorSet.insert(_colorSet.begin(), Mood.white);
  // _colorSet.insert(_colorSet.begin(), Mood.yellow);
  // _colorSet.insert(_colorSet.begin(), Mood.warmYellow);
  // _colorSet.insert(_colorSet.begin(), Mood.red);
  // _colorSet.insert(_colorSet.begin(), Mood.warmYellow);
  // _colorSet.insert(_colorSet.begin(), Mood.yellow);

  // Turn on Serial so we can verify expected colors via Serial Monitor
  Serial.begin(9600);
}

void loop() {

  for (int i = 0; i < 101; i++) {
    float fraction = i / 100.0;
    Color currentColor = interpolateColorSet(fraction);
    setColor(currentColor.r, currentColor.g, currentColor.b);
    delay(50);
  }


  // // white to yellow
  // Serial.println("white to yellow");
  // Color bruh1 = {255, 255, 255};
  // Color bruh2 = {150, 40, 0};

  // interpolateColor(bruh1, bruh2);

  // Serial.println("yellow to warm yellow");

  // bruh1 = {150, 40, 0};
  // bruh2 = {220, 40, 0};

  // interpolateColor(bruh1, bruh2);

  // Serial.println("warm yellow to red");

  // bruh1 = {220, 40, 0};
  // bruh2 = {220, 0, 0};

  // interpolateColor(bruh1, bruh2);

  // Serial.println("red to warm yellow");

  // bruh1 = {220, 0, 0};
  // bruh2 = {220, 40, 0};

  // interpolateColor(bruh1, bruh2);

  // Serial.println("warm yellow to yellow");

  // bruh1 = {220, 40, 0};
  // bruh2 = {150, 40, 0};

  // interpolateColor(bruh1, bruh2);

  // Serial.println("yellow to white");
  // bruh1 = {150, 40, 0};
  // bruh2 = {255, 255, 255};

  // interpolateColor(bruh1, bruh2);
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
