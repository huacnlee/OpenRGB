# WOBKEY Rainy 75 (wired ANSI)

The Rainy 75 is detected as **WOBKEY Rainy 75**, using the existing EVision V2
controller with an 81-key ANSI layout. This supports Direct per-key RGB,
Static color, brightness, and the existing EVision hardware effects. It exposes
one keyboard zone; no fictitious logo or edge devices are created.

## Device and protocol

Validated on a keyboard reporting manufacturer `RDR`, product `Rainy 75`,
USB VID:PID `320f:5055`, interface 1, usage page `0xff1c`, usage `0x92`,
report ID 4. This VID/PID is shared by other keyboards, so detection also
requires the product string and a valid EVision capability response with a
128-entry LED map. Other firmware revisions, ISO layouts, Bluetooth, and the
2.4 GHz receiver have not been validated.

The connected keyboard responded to 64-byte EVision packets:

- `0x03`: read capabilities, beginning `aa 55 00 00 03 80 18`.
- `0x05`: read configuration; byte 0 selects a profile, profile 0 starts at 1.
- `0x07`: read three-byte keymap entries. These establish LED addresses in
  the 128-entry map, including unused ISO/JIS positions on the ANSI board.
- `0x06`: write the first eight lighting bytes of the selected profile.
  Static mode, brightness, and RGB were written and read back. The original
  effect was restored; the firmware normalized its color flag from `ff` to
  `fd`. The remaining configuration bytes were checked for preservation.
- `0x12`: volatile Direct RGB in seven chunks (384 bytes total).
- `0x13`: leave Direct mode and restore the hardware effect.

The packet checksum is the unsigned sum of bytes 3–63, stored little endian
in bytes 1–2. Responses echo the request checksum, command, offset, and size.
Requests have a one-second deadline, including unrelated input reports on
the same HID interface. The Direct keepalive uses unused LED address 15,
so it does not overwrite F6 (the generic EVision keepalive address).

Hardware effect names come from the existing EVision V2 driver; individual
animations have not all been visually verified. Custom flash color slots
are omitted until their storage behavior is verified. Static and other
hardware settings use the existing driver's automatic-save behavior;
Direct updates do not write flash.

This device's working EVision interface was established by local probing.
Some [other Rainy 75 firmware](https://github.com/scholzri/rainy75-zmk/blob/main/docs/protocols.md)
has been reported to leave that interface silent; adding only its USB ID
without checking responses would produce an unusable controller.

## Build and use

From the repository root, with the normal OpenRGB build dependencies:

```sh
qmake6 OpenRGB.pro
make -j8
./openrgb --noautoconnect --list-devices
./openrgb --noautoconnect --device 'WOBKEY Rainy 75' --mode Static --color 2050a0
```

For persistent Linux access:

```sh
sudo install -m644 scripts/rainy75/60-openrgb-rainy75.rules /etc/udev/rules.d/60-openrgb-rainy75.rules
sudo udevadm control --reload-rules
```

Reconnect the keyboard after installing the rule. To grant temporary access
instead, run `python3 -B scripts/rainy75/probe.py` and use the specific
`setfacl` command it prints. The probe selects the EVision interface and sends
only read requests; it does not print keyboard input or change settings.

Regression tests (no hardware access):

```sh
bash tests/rainy75/run.sh
```
