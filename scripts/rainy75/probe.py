#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-or-later
"""Read Rainy 75 EVision capabilities without changing keyboard settings."""

import argparse
import json
import os
from pathlib import Path
import select
import sys
import time


def find_devices():
    for entry in sorted(Path('/sys/class/hidraw').glob('hidraw*')):
        properties = dict(line.split('=', 1) for line in
                          (entry / 'device/uevent').read_text().splitlines()
                          if '=' in line)
        if properties.get('HID_ID') != '0003:0000320F:00005055':
            continue
        descriptor = (entry / 'device/report_descriptor').read_bytes()
        # EVision vendor collection: usage page FF1C, usage 92, report ID 4.
        if bytes.fromhex('061cff0992a1018504') in descriptor:
            yield Path('/dev') / entry.name


def read_packet(command, size):
    packet = bytearray(64)
    packet[0] = 4
    packet[3] = command
    packet[4] = size
    checksum = sum(packet[3:])
    packet[1:3] = checksum.to_bytes(2, 'little')
    return bytes(packet)


def query(fd, packet, timeout=1.0):
    if os.write(fd, packet) != len(packet):
        raise OSError('Incomplete HID write')
    deadline = time.monotonic() + timeout
    while True:
        remaining = deadline - time.monotonic()
        if remaining <= 0 or not select.select([fd], [], [], remaining)[0]:
            return {'status': 'timeout'}
        try:
            response = os.read(fd, 64)
        except BlockingIOError:
            continue
        if not response:
            raise OSError('Keyboard disconnected')
        # The same interface carries keyboard input. Never log those reports.
        if response[0] != 4:
            continue
        if len(response) != 64 or response[1:4] != packet[1:4] or response[5:7] != packet[5:7]:
            continue
        if response[4] > 56:
            return {'status': 'invalid_length'}
        if response[7]:
            return {'status': 'device_error', 'code': response[7]}
        if response[4] != packet[4]:
            return {'status': 'unexpected_length', 'length': response[4]}
        return {'status': 'ok', 'data': response[8:8 + response[4]].hex()}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.parse_args()
    devices = list(find_devices())
    if not devices:
        print('No wired Rainy 75 (320F:5055) EVision interface found.', file=sys.stderr)
        return 1
    failed = False
    for device in devices:
        try:
            fd = os.open(device, os.O_RDWR | os.O_NONBLOCK)
        except PermissionError:
            print(f'Permission denied: {device}. Grant access with:\n'
                  f'  sudo setfacl -m u:{os.getuid()}:rw {device}', file=sys.stderr)
            failed = True
            continue
        try:
            result = {'device': str(device)}
            # Only read commands: no begin/end transaction, settings writes,
            # custom color writes, save, reset, or firmware commands.
            result['capabilities'] = query(fd, read_packet(0x03, 34))
            # Current profile byte followed by all eight lighting bytes.
            result['lighting_parameters'] = query(fd, read_packet(0x05, 9))
            print(json.dumps(result, indent=2))
            failed |= any(result[key]['status'] != 'ok'
                          for key in ('capabilities', 'lighting_parameters'))
        finally:
            os.close(fd)
    return int(failed)


if __name__ == '__main__':
    sys.exit(main())
