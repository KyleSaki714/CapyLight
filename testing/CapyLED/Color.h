#include <Arduino.h>

class Color {
  private:
    int _r;
    int _g;
    int _b;

  public:
    Color(int red, int green, int blue);
    int r();
    int g();
    int b();
    Color Color::subtract(Color other);
};