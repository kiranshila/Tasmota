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

#include <TasmotaSerial.h>

TasmotaSerial *serial;

void CD5220SendBareCommand(CD5220Command command){
  serial->write((int)command);
  return;
}

void CD5220SendCommand(CD5220Command command, int numArgs, ...){
  va_list args;
  va_start(args, numArgs);
  uint32_t result = (int)command << (numArgs*8);
  for (int i = 0; i < numArgs; i++){
    // Quirk in variadic arguments due to promotion rules
    result |= va_arg(args,int) << ((numArgs-i-1) * 8);
  }
  va_end(args);
  serial->write(result);
  return;
}

void CD5220DisplayClear(void)
{
  CD5220SendBareCommand(CD5220Command::Clear);
  return;
}

void CD5220DisplayDrawStringAt(void)
{
  //FIXME
  serial->print(dsp_str);
  return;
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
    CD5220SendBareCommand(CD5220Command::Initialize);
    CD5220DisplayClear();
#ifdef SHOW_SPLASH
    serial->println("CD5220 on Tasmota!");
#endif
  }
  return;
}

#ifdef USE_DISPLAY_MODES1TO5
void CD5220PrintLog(void)
{
  disp_refresh--;
  if (!disp_refresh)
  {
    disp_refresh = Settings.display_refresh;
    // Grab log
    char *txt = DisplayLogBuffer('\370');
    if (txt != NULL)
    {
      serial->println(txt);
    }
  }
  return;
}

// Ticker for every second
void CD5220Refresh(void)
{
  if (Settings.display_mode)
  {
    switch (Settings.display_mode)
    {
    case 1:
      //CD5220Time();
      break;
    case 2:
    case 3:
    case 4:
    case 5:
      CD5220PrintLog();
      break;
    }
  }
  return;
}

#endif

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
      CD5220DisplayDrawStringAt();
      break;
#ifdef USE_DISPLAY_MODES1TO5
    case FUNC_DISPLAY_EVERY_SECOND:
      CD5220Refresh();
      break;
#endif
    }
  }
  return result;
}

#endif // USE_DISPLAY_CD5220
#endif // USE_DISPLAY
