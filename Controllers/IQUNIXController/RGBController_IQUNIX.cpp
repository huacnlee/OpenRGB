/*---------------------------------------------------------*\
| RGBController_IQUNIX.cpp                                 |
|                                                           |
|   IQUNIX EV63 keyboard lighting controller                |
|                                                           |
|   huacnlee                                    21 Oct 2026 |
|                                                           |
|   SPDX-License-Identifier: GPL-2.0-or-later                |
\*---------------------------------------------------------*/

#include "RGBController_IQUNIX.h"

static const char* key_names[5][14] =
{
    { "Escape", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "Backspace" },
    { "Tab", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[", "]", "\\ (ANSI)" },
    { "Caps Lock", "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "Enter" },
    { "Left Shift", "Z", "X", "C", "V", "B", "N", "M", ",", ".", "/", "Right Shift", "Up Arrow", "Delete" },
    { "Left Control", "Left Windows", "Left Alt", "Space", "Right Alt", "Right Fn", "Left Arrow", "Down Arrow", "Right Arrow" }
};
static const unsigned int row_lengths[] = { 14, 14, 13, 14, 9 };
// Key centres in quarter-key units preserve the stagger and spacebar.
static const unsigned int key_columns[5][14] =
{
    { 2, 6, 10, 14, 18, 22, 26, 30, 34, 38, 42, 46, 50, 56 },
    { 3, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 57 },
    { 3, 9, 13, 17, 21, 25, 29, 33, 37, 41, 45, 49, 56 },
    { 4, 10, 14, 18, 22, 26, 30, 34, 38, 42, 46, 50, 54, 58 },
    { 2, 7, 12, 27, 42, 46, 50, 54, 58 }
};

RGBController_IQUNIX::RGBController_IQUNIX(IQUNIXController* controller_ptr)
{
    controller  = controller_ptr;
    name        = "IQUNIX EV63";
    vendor      = "IQUNIX";
    description = "IQUNIX EV63 Hall Effect Keyboard";
    type        = DEVICE_TYPE_KEYBOARD;
    location    = controller->GetLocation();

    mode direct;
    direct.name           = "Direct";
    direct.value          = 0x2E;
    direct.flags          = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    direct.color_mode     = MODE_COLORS_PER_LED;
    direct.brightness_min = 0;
    direct.brightness_max = 255;
    direct.brightness     = 255;
    modes.push_back(direct);

    mode static_mode = direct;
    static_mode.name = "Static";
    modes.push_back(static_mode);
    SetupZones();
}

RGBController_IQUNIX::~RGBController_IQUNIX()
{
    Shutdown();
    delete controller;
}

void RGBController_IQUNIX::SetupZones()
{
    zone keyboard;
    keyboard.name = "Keyboard";
    keyboard.type = ZONE_TYPE_MATRIX;
    keyboard.matrix_map.height = 5;
    keyboard.matrix_map.width  = 60;
    keyboard.matrix_map.map.assign(300, 0xFFFFFFFF);
    for(unsigned int row = 0; row < 5; row++)
    {
        for(unsigned int column = 0; column < row_lengths[row]; column++)
        {
            led key;
            key.name  = "Key: " + std::string(key_names[row][column]);
            key.value = (row << 8) | column;
            keyboard.matrix_map.map[row * 60 + key_columns[row][column]] = (unsigned int)leds.size();
            leds.push_back(key);
        }
    }
    keyboard.leds_count = (unsigned int)leds.size();
    keyboard.leds_min   = keyboard.leds_count;
    keyboard.leds_max   = keyboard.leds_count;
    zones.push_back(keyboard);
    SetupColors();
}

void RGBController_IQUNIX::DeviceUpdateLEDs()
{
    if(!controller->SetMode(0x2E) || !controller->SetBrightness(modes[active_mode].brightness))
    {
        return;
    }
    for(unsigned int index = 0; index < leds.size(); index++)
    {
        if(!controller->SetKey(leds[index].value >> 8, leds[index].value & 0xFF, colors[index]))
        {
            break;
        }
    }
}

void RGBController_IQUNIX::DeviceUpdateZoneLEDs(int /*zone*/)
{
    DeviceUpdateLEDs();
}

void RGBController_IQUNIX::DeviceUpdateSingleLED(int index)
{
    controller->SetKey(leds[index].value >> 8, leds[index].value & 0xFF, colors[index]);
}

void RGBController_IQUNIX::DeviceUpdateMode()
{
    DeviceUpdateLEDs();
}
