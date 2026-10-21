#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
install -Dm755 openrgb /usr/local/bin/openrgb
install -Dm644 60-openrgb-iqunix.rules /etc/udev/rules.d/60-openrgb-iqunix.rules
udevadm control --reload-rules
udevadm trigger --subsystem-match=hidraw
printf 'Installed /usr/local/bin/openrgb. Restart your OpenRGB server or Omarchy shell.\n'
