/*---------------------------------------------------------*\
| IQUNIXControllerDetect.cpp                                |
|                                                           |
|   IQUNIX EV63 keyboard lighting controller                |
|                                                           |
|   huacnlee                                    21 Oct 2026 |
|                                                           |
|   SPDX-License-Identifier: GPL-2.0-or-later                |
\*---------------------------------------------------------*/

#include "DetectionManager.h"
#include "RGBController_IQUNIX.h"

DetectedControllers DetectIQUNIXControllers(hid_device_info* info, const std::string& /*name*/)
{
    DetectedControllers controllers;
    hid_device* dev = hid_open_path(info->path);
    if(dev)
    {
        controllers.push_back(new RGBController_IQUNIX(new IQUNIXController(dev, info->path)));
    }
    return controllers;
}

REGISTER_HID_DETECTOR_PU("IQUNIX EV63", DetectIQUNIXControllers, 0x3869, 0x63E1, 0xFF60, 0x61);
