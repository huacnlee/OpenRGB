/*---------------------------------------------------------*\
| IQUNIXController.cpp                                      |
|                                                           |
|   IQUNIX EV63 keyboard lighting controller                |
|                                                           |
|   huacnlee                                    21 Oct 2026 |
|                                                           |
|   SPDX-License-Identifier: GPL-2.0-or-later                |
\*---------------------------------------------------------*/

#include <algorithm>
#include <cmath>
#include "IQUNIXController.h"
#include "LogManager.h"

IQUNIXController::IQUNIXController(hid_device* handle, const char* path)
{
    dev      = handle;
    location = "HID: " + std::string(path);
}

IQUNIXController::~IQUNIXController()
{
    hid_close(dev);
}

std::string IQUNIXController::GetLocation()
{
    return location;
}

bool IQUNIXController::Send(unsigned char* packet)
{
    /*-----------------------------------------------------*\
    | Report ID 0 followed by a 32-byte output report.      |
    | Wait for the matching echo before the next command.  |
    \*-----------------------------------------------------*/
    if(hid_write(dev, packet, 33) != 33)
    {
        LOG_WARNING("[IQUNIX] HID write failed");
        return false;
    }
    unsigned char response[32];
    for(unsigned int attempt = 0; attempt < 4; attempt++)
    {
        int size = hid_read_timeout(dev, response, sizeof(response), 100);
        if(size == 32 && response[0] == packet[1] && response[1] == packet[2] && response[2] == packet[3])
        {
            return true;
        }
        if(size <= 0)
        {
            break;
        }
    }
    LOG_WARNING("[IQUNIX] Missing command acknowledgement");
    return false;
}

bool IQUNIXController::SetMode(unsigned char value)
{
    unsigned char packet[33] = { 0, 0x07, 0x03, 0x02, value };
    return Send(packet);
}

bool IQUNIXController::SetBrightness(unsigned char value)
{
    unsigned char packet[33] = { 0, 0x07, 0x03, 0x05, value };
    return Send(packet);
}

bool IQUNIXController::SetKey(unsigned char row, unsigned char column, RGBColor color)
{
    double red   = RGBGetRValue(color) / 255.0;
    double green = RGBGetGValue(color) / 255.0;
    double blue  = RGBGetBValue(color) / 255.0;
    double high  = std::max(red, std::max(green, blue));
    double low   = std::min(red, std::min(green, blue));
    double delta = high - low;
    double hue   = 0;
    if(delta > 0)
    {
        if(high == red)        hue = std::fmod((green - blue) / delta, 6.0);
        else if(high == green) hue = (blue - red) / delta + 2.0;
        else                   hue = (red - green) / delta + 4.0;
        if(hue < 0) hue += 6.0;
    }
    unsigned char packet[33] = { 0, 0xB1, row, column };
    packet[4] = (unsigned char)std::round(hue * 255.0 / 6.0);
    packet[5] = (unsigned char)std::round(high == 0 ? 0 : delta / high * 255.0);
    packet[6] = (unsigned char)std::round(high * 255.0);
    return Send(packet);
}
