/*---------------------------------------------------------*\
| RGBController_IQUNIX.h                                    |
|                                                           |
|   IQUNIX EV63 keyboard lighting controller                |
|                                                           |
|   huacnlee                                    21 Oct 2026 |
|                                                           |
|   SPDX-License-Identifier: GPL-2.0-or-later                |
\*---------------------------------------------------------*/

#pragma once
#include "RGBController.h"
#include "IQUNIXController.h"

class RGBController_IQUNIX : public RGBController
{
public:
    RGBController_IQUNIX(IQUNIXController* controller_ptr);
    ~RGBController_IQUNIX();
    void SetupZones();
    void DeviceUpdateLEDs();
    void DeviceUpdateZoneLEDs(int zone);
    void DeviceUpdateSingleLED(int led);
    void DeviceUpdateMode();
private:
    IQUNIXController* controller;
};
