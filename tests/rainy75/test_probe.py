# SPDX-License-Identifier: GPL-2.0-or-later
"""Exercise probe response filtering without accessing hardware."""

import importlib.util
from pathlib import Path
import unittest
from unittest.mock import patch


spec = importlib.util.spec_from_file_location(
    'rainy75_probe', Path(__file__).resolve().parents[2] / 'scripts/rainy75/probe.py')
probe = importlib.util.module_from_spec(spec)
spec.loader.exec_module(probe)


class ProbeTests(unittest.TestCase):
    def run_query(self, responses):
        request = probe.read_packet(5, 8)
        with patch.object(probe.os, 'write', return_value=64), \
                patch.object(probe.select, 'select', return_value=([3], [], [])), \
                patch.object(probe.os, 'read', side_effect=responses):
            return probe.query(3, request)

    def test_ignores_input_and_unrelated_responses(self):
        response = bytearray(probe.read_packet(5, 8))
        response[8:16] = bytes(range(8))
        result = self.run_query([b'\x01private keyboard input',
                                 probe.read_packet(3, 34), bytes(response)])
        self.assertEqual(result, {'status': 'ok', 'data': '0001020304050607'})

    def test_device_error_is_not_success(self):
        response = bytearray(probe.read_packet(5, 8))
        response[7] = 2
        self.assertEqual(self.run_query([response]), {'status': 'device_error', 'code': 2})

    def test_bad_payload_length_is_not_success(self):
        response = bytearray(probe.read_packet(5, 8))
        response[4] = 57
        self.assertEqual(self.run_query([response]), {'status': 'invalid_length'})

    def test_silent_device_has_bounded_wait(self):
        with patch.object(probe.os, 'write', return_value=64), \
                patch.object(probe.select, 'select', return_value=([], [], [])):
            self.assertEqual(probe.query(3, probe.read_packet(5, 8)), {'status': 'timeout'})


if __name__ == '__main__':
    unittest.main()
