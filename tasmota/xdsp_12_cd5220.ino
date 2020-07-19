
/*
  xdsp_12_cd5220.ino - Display VFD 5220 support for Tasmota

  Copyright (C) 2020  Kiran Shila, Theo Arends

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef USE_DISPLAY
#ifdef USE_DISPLAY_CD5220

#define XDSP_12 12

#include <renderer.h>
#include <TasmotaSerial.h>

TasmotaSerial *serial;
/*********************************************************************************************/

uint16_t CD5220CursorCommand(uint16_t x, uint16_t y){
  uint16_t baseCommand = 0x1B6C << 16;
  baseCommand |= x << 8;
  baseCommand |= y;
  return baseCommand;
}

void CD5220DisplayClear(void)
{
  serial->write(0x0C);
}

void CD5220DrawString(void)
{
  serial->write(CD5220CursorCommand(dsp_x,dsp_y));
  serial->print(dsp_str);
}

void CD5220InitDriver(void)
{
  if (!Settings.display_model)
  {
    Settings.display_model = XDSP_12;
  }

  if (XDSP_12 == Settings.display_model)
  {

    Settings.display_width = 20;
    Settings.display_height = 2;

    // init serial
    serial = new TasmotaSerial(-1, PinUsed(GPIO_CD5220_TX) ? Pin(GPIO_CD5220_TX) : -1, 1);
    serial->begin();
    // initialize display
    serial->write(0x1B40);
    // Clear display
    serial->write(0x0C);
#ifdef SHOW_SPLASH
    serial->println("CD5220 on Tasmota!");
#endif
  }
}

/*********************************************************************************************\
 * Interface
\*********************************************************************************************/

bool Xdsp12(byte function)
{
  bool result = false;

  if (FUNC_DISPLAY_INIT_DRIVER == function)
  {
    CD5220InitDriver();
  }
  else if (XDSP_12 == Settings.display_model)
  {
    switch (function)
    {
    case FUNC_DISPLAY_MODEL:
      result = true;
      break;
    case FUNC_DISPLAY_CLEAR:
      CD5220DisplayClear();
      break;
    case FUNC_DISPLAY_DRAW_STRING:
      CD5220DrawString();
      break;
    }
  }
  return result;
}

#endif // USE_DISPLAY_CD5220
#endif // USE_DISPLAY
