#include "Color.h"

Color::Color(int red, int green, int blue)
{
  _r = red;
  _g = green;
  _b = blue;
}

int Color::r()
{
  return _r;
}

int Color::g()
{
  return _g;
}

int Color::b()
{
  return _b;
}

Color Color::subtract(Color other)
{
  return new Color(_r - other.r, _g - other.g, _b - other.b);
}