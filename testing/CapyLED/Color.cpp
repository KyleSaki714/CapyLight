#include "Color.h"

Color::Color(int red, int green, int blue)
{
  _r = red;
  _g = green;
  _b = blue;
}

static Color::r()
{
  return _r;
}

static int Color::g()
{
  return _g;
}

static int Color::b()
{
  return _b;
}