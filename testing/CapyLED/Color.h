#include <Arduino.h>

class Color {
  private:
    int _r;
    int _g;
    int _b;

  public:
    Color(int red, int green, int blue);
    static int r();
    static int g();
    static int b();
};