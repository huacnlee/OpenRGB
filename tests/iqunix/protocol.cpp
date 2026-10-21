// SPDX-License-Identifier: GPL-2.0-or-later
// Build and run with tests/iqunix/run.sh.
#include <cassert>
#include <cstring>
#include <vector>
#include "IQUNIXController.h"
#include "LogManager.h"

static std::vector<unsigned char> last_packet;
static bool fail_write = false;
static bool fail_read = false;
static bool wrong_reply = false;
LogManager::LogManager() {}
LogManager::~LogManager() {}
LogManager* LogManager::get() { static LogManager logger; return &logger; }
void LogManager::LogEntry(const char*, int, unsigned int, const char*, ...) {}

int hid_write(hid_device*, const unsigned char* data, size_t length)
{
    last_packet.assign(data, data + length);
    return fail_write ? -1 : (int)length;
}
int hid_read_timeout(hid_device*, unsigned char* data, size_t length, int)
{
    assert(length == 32);
    if(fail_read) return 0;
    memcpy(data, last_packet.data() + 1, 32);
    if(wrong_reply) data[1] ^= 1;
    return 32;
}
void hid_close(hid_device*) {}

static void CheckColor(IQUNIXController& controller, RGBColor rgb, int h, int s, int v)
{
    assert(controller.SetKey(4, 8, rgb));
    assert(last_packet.size() == 33);
    assert(last_packet[0] == 0 && last_packet[1] == 0xB1);
    assert(last_packet[2] == 4 && last_packet[3] == 8);
    assert(last_packet[4] == h && last_packet[5] == s && last_packet[6] == v);
    for(unsigned int i = 7; i < 33; i++) assert(last_packet[i] == 0);
}
int main()
{
    IQUNIXController controller(nullptr, "test");
    assert(controller.GetLocation() == "HID: test");
    assert(controller.SetMode(0x2E));
    assert(last_packet[1] == 7 && last_packet[2] == 3 && last_packet[3] == 2 && last_packet[4] == 0x2E);
    assert(controller.SetBrightness(190));
    assert(last_packet[3] == 5 && last_packet[4] == 190);
    CheckColor(controller, ToRGBColor(255, 0, 0), 0, 255, 255);
    CheckColor(controller, ToRGBColor(0, 255, 0), 85, 255, 255);
    CheckColor(controller, ToRGBColor(0, 0, 255), 170, 255, 255);
    CheckColor(controller, ToRGBColor(255, 0, 255), 213, 255, 255);
    CheckColor(controller, ToRGBColor(255, 255, 255), 0, 0, 255);
    CheckColor(controller, ToRGBColor(128, 128, 128), 0, 0, 128);
    CheckColor(controller, ToRGBColor(0, 0, 0), 0, 0, 0);
    fail_write = true;
    assert(!controller.SetMode(0x2E));
    fail_write = false;
    fail_read = true;
    assert(!controller.SetMode(0x2E));
    fail_read = false;
    wrong_reply = true;
    assert(!controller.SetMode(0x2E));
}
