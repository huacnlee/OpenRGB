// SPDX-License-Identifier: GPL-2.0-or-later
// Rainy 75 EVision V2 regression tests; no hardware required.
#include <cassert>
#include <cstring>
#include <set>
#include <vector>
#include "EVisionV2KeyboardController.h"
#include "StringUtils.h"

static std::vector<unsigned char> request;
static std::vector<std::vector<unsigned char>> writes;
static bool fail_write = false;
static bool timeout_read = false;
static bool malformed_reply = false;
static bool input_report = false;
static unsigned char capability_map_size = 128;

int hid_write(hid_device*, const unsigned char* data, size_t size)
{
    request.assign(data, data + size);
    writes.push_back(request);
    return fail_write ? -1 : (int)size;
}

int hid_read_timeout(hid_device*, unsigned char* data, size_t size, int milliseconds)
{
    assert(milliseconds > 0 && milliseconds <= 1000);
    assert(size == 64);
    if(timeout_read) return 0;
    if(input_report)
    {
        input_report = false;
        data[0] = 1;
        return 16;
    }
    memcpy(data, request.data(), 64);
    if(data[3] == 3)
    {
        data[8] = 0xAA;
        data[9] = 0x55;
        data[13] = capability_map_size;
    }
    if(data[3] == 5)
    {
        memset(data + 8, 0, data[4]);
    }
    if(malformed_reply) data[4] = 57;
    return 64;
}

void hid_close(hid_device*) {}
int hid_get_serial_number_string(hid_device*, wchar_t*, size_t) { return -1; }
std::string StringUtils::wstring_to_string(const std::wstring) { return ""; }

int main()
{
    EVisionV2KeyboardController controller(nullptr, "test", RAINY75_KEYBOARD_LAYOUT, "WOBKEY Rainy 75");
    assert(controller.IsValid());
    assert(controller.GetLocation() == "HID: test");

    std::set<unsigned int> addresses;
    std::set<unsigned int> positions;
    for(const Rainy75Key& key : rainy75_keys)
    {
        assert(key.index < 128);
        assert(key.row < 6 && key.column < 16);
        assert(addresses.insert(key.index).second);
        assert(positions.insert(key.row * 16 + key.column).second);
    }
    assert(addresses.size() == 81);
    for(unsigned int ghost : {15, 60, 65, 76, 96, 127}) assert(!addresses.count(ghost));

    std::vector<RGBColor> colors(81);
    for(unsigned int i = 0; i < 81; i++) colors[i] = ToRGBColor(i + 1, 100, 200);
    writes.clear();
    controller.SetLedsDirect(colors);
    assert(writes.size() == 7);
    std::vector<unsigned char> frame;
    for(const std::vector<unsigned char>& packet : writes)
    {
        assert(packet[3] == 0x12);
        unsigned int offset = packet[5] | packet[6] << 8;
        assert(offset == frame.size());
        frame.insert(frame.end(), packet.begin() + 8, packet.begin() + 8 + packet[4]);
    }
    assert(frame.size() == 384);
    for(unsigned int i = 0; i < 81; i++)
    {
        unsigned int offset = rainy75_keys[i].index * 3;
        assert(frame[offset] == i + 1 && frame[offset + 1] == 100 && frame[offset + 2] == 200);
    }
    for(unsigned int i = 0; i < 128; i++)
    {
        if(!addresses.count(i)) assert(frame[i * 3] == 0 && frame[i * 3 + 1] == 0 && frame[i * 3 + 2] == 0);
    }

    controller.RefreshLedDirect();
    assert(request[3] == 0x12 && request[5] == 45 && request[4] == 1);
    controller.SetLedDirect(80, ToRGBColor(1, 2, 3));
    assert((request[5] | request[6] << 8) == 95 * 3);
    size_t before = writes.size();
    controller.SetLedDirect(-1, 0);
    controller.SetLedDirect(81, 0);
    assert(writes.size() == before);

    EvisionV2ModeConfig config = {};
    config.mode = 6;
    config.brightness = 2;
    config.colour = ToRGBColor(1, 2, 3);
    writes.clear();
    controller.SetMode(EVISION_V2_KEYBOARD_PART_KEYBOARD, config);
    assert(writes.size() == 4);
    assert(writes[2][3] == 6 && writes[2][4] == 8 && writes[2][5] == 1);
    assert(writes[2][8] == 6 && writes[2][9] == 2);
    assert(writes[2][13] == 1 && writes[2][14] == 2 && writes[2][15] == 3);

    before = writes.size();
    assert(controller.Query(5, 0, nullptr, 57) < 0);
    assert(writes.size() == before);
    fail_write = true;
    assert(controller.Query(5) < 0);
    fail_write = false;
    timeout_read = true;
    assert(controller.Query(5) < 0);
    timeout_read = false;
    malformed_reply = true;
    assert(controller.Query(5) < 0);
    malformed_reply = false;
    input_report = true;
    assert(controller.Query(5) == 0);

    capability_map_size = 64;
    EVisionV2KeyboardController unsupported(nullptr, "test", RAINY75_KEYBOARD_LAYOUT, "test");
    assert(!unsupported.IsValid());
}
