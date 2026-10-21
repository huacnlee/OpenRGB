/*---------------------------------------------------------*\
| IQUNIXController.h                                        |
|                                                           |
|   IQUNIX EV63 keyboard lighting controller                |
|                                                           |
|   huacnlee                                    21 Oct 2026 |
|                                                           |
|   SPDX-License-Identifier: GPL-2.0-or-later                |
\*---------------------------------------------------------*/

#pragma once

#include <hidapi.h>
#include <string>
#include "RGBController.h"

class IQUNIXController
{
public:
    IQUNIXController(hid_device* handle, const char* path);
    ~IQUNIXController();
    std::string GetLocation();
    bool SetMode(unsigned char value);
    bool SetBrightness(unsigned char value);
    bool SetKey(unsigned char row, unsigned char column, RGBColor color);

private:
    hid_device* dev;
    std::string location;
    bool Send(unsigned char* packet);
};
