// Copyright (C) 2000-2016 Hideki EIRAKU
// SPDX-License-Identifier: GPL-2.0-or-later

#include "jjoy.h"

// Bit 0: Joystick 1 X
// Bit 1: Joystick 1 Y
// Bit 2: Joystick 2 X
// Bit 3: Joystick 2 Y
// Bit 4: Joystick 1 Button 1
// Bit 5: Joystick 1 Button 2
// Bit 6: Joystick 2 Button 1
// Bit 7: Joystick 2 Button 2
// X, Y: Pulse started by out
// Pulse width: 24.2 + 0.011 * R us
// R: resistance value 0-100 k ohm

jjoy::joystick::joystick()
{
  x = 0;
  y = 0;
  b = 0;
  connected = false;
}

void jjoy::joystick::connect (bool yes)
{
  connected = yes;
}

void jjoy::joystick::set_state (int xr, int yr, bool b1, bool b2)
{
  unsigned char v = 0;
  if (!b1)
    v |= 0x10;
  if (!b2)
    v |= 0x20;
  x = ((242 + 110 * xr) * 1431818LL) / 1000000LL;
  y = ((242 + 110 * yr) * 1431818LL) / 1000000LL;
  b = v;
}

unsigned char jjoy::joystick::get_value()
{
  if (connected)
  {
    int d = b;
    if (x)
      d |= 1;
    if (y)
      d |= 2;
    return d;
  }
  return 0x33;
}

void jjoy::joystick::clk (int d)
{
  if (d > x)
    x = 0;
  else
    x -= d;
  if (d > y)
    y = 0;
  else
    y -= d;
}

jjoy::jjoy()
{
  joy1.connect (false);
  joy2.connect (false);
}

void jjoy::update_state()
{
  joy1.set_state (50, 50, false, false);
  joy2.set_state (50, 50, false, false);
}

unsigned char jjoy::in201()
{
  return joy1.get_value() | (joy2.get_value() << 2);
}

void jjoy::out201 (unsigned char data)
{
  update_state();
}

void jjoy::clk (int d)
{
  joy1.clk (d);
  joy2.clk (d);
}
