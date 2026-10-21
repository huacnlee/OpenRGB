# IQUNIX EV63

This fork supports the wired IQUNIX EV63 (`3869:63e1`), including its 64
addressable key positions, per-key colors, brightness, and a matrix layout.
Both Direct and Static expose per-LED colors, allowing the OpenRGB CLI and SDK
to apply single colors or palettes. Detection opens only usage page `0xFF60`,
usage `0x61` (USB interface 1).

## Linux / Omarchy

The release archive contains an Arch Linux x86_64 build, its license, and an
installer. Runtime packages: `qt6-base`, `hidapi`, `libusb`, `mbedtls3`, and
`libglvnd`. This native build uses the system libraries.

Use either the mise or curl installer in the [README](../README.md#install-on-arch-linux--omarchy-x86_64), or extract the archive and run `sudo ./install.sh`. The installer places the
executable at `/usr/local/bin/openrgb` and adds the EV63 udev rule. Keep the
distribution's OpenRGB package installed for its dependencies and other device
rules. Restart the OpenRGB SDK server; with omarchy-theme-rgb, run
`omarchy restart shell`. The theme plugin needs its OpenRGB 1.1 detailed-list
compatibility change (included in huacnlee/omarchy-theme-rgb).

Inspect the device with `openrgb --list-detailed`, or set a color with:

```sh
openrgb -d 'IQUNIX EV63' -m Direct -c FF0000
```

To restore the distribution executable, remove `/usr/local/bin/openrgb` and
`/etc/udev/rules.d/60-openrgb-iqunix.rules`, reload udev rules, and restart the
SDK server.

## Protocol

The command formats were traced from the official WebHID driver at
<https://ev.iqunix.com/> (asset `index-B1-Os4Nr.js`, inspected 2026-10-21)
and verified on a physical EV63. No firmware replacement is needed.

Every command is a 32-byte output report, padded with zeroes. HIDAPI prefixes
the report with report ID zero, giving a 33-byte write. The device returns a
32-byte input report echoing each write; the controller waits for this reply
and stops an update when communication fails.

| Payload | Operation |
| --- | --- |
| `07 03 02 2E` | Select custom per-key lighting |
| `07 03 05 VV` | Set brightness, 0–255 |
| `B1 RR CC HH SS VV` | Set one key, zero-based row and column, HSV channels 0–255 |
| `08 03 02` | Read lighting mode (value in byte 3) |
| `08 03 05` | Read brightness (value in byte 3) |
| `41 AH AL NN` | Read NN lighting bytes starting at big-endian offset AH:AL |

The last three commands are useful for hardware verification and are not
needed during normal color updates. Lighting readback contains HSV triples
with a row stride of 14 keys, including empty positions. Key counts are
14, 14, 13, 14, and 9. Colors are converted from OpenRGB's RGB to the
keyboard's 8-bit HSV; firmware readback can differ slightly through rounding.
Only lighting commands are sent by the controller. No profile-save,
calibration, reset, or firmware-update commands are used. Persistence across
power cycles and high-rate effect streaming have not been characterized.

## Validation

Run `tests/iqunix/run.sh` for mocked HID tests covering primary colors,
achromatic colors, zero padding, row/column addressing, configuration packets,
write failures, timeouts, and mismatched replies. Build the full application
with the standard QMake workflow.

Physical validation on Linux included device detection, CLI colors, SDK mode
and brightness changes, and omarchy-theme-rgb's five-color flow layout. All
64 key colors were read back and compared with the theme palette, with a
maximum allowed HSV channel rounding error of two.
