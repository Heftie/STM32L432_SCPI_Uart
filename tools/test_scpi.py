#!/usr/bin/env python3
"""
Minimal SCPI smoke test over a serial port (UART/VCP).

Usage:
    python3 tools/test_scpi.py -p /dev/ttyACM0
    python3 tools/test_scpi.py -p COM5 -b 115200

Requires pyserial (pip install pyserial).
"""

import time
import argparse
import sys
import unittest

import serial

PORT = None
BAUDRATE = 115200
TIMEOUT = 2.0


class ScpiLink:
    """Thin request/response wrapper around a pyserial connection."""

    def __init__(self, port, baudrate, timeout):
        self.ser = serial.Serial(port=port, baudrate=baudrate, timeout=timeout)
        self.ser.reset_input_buffer()

    def close(self):
        self.ser.close()

    def send(self, command):
        self.ser.write(command.encode("ascii") + b"\r\n")

    def query(self, command):
        self.send(command)
        line = self.ser.readline()
        if not line:
            raise TimeoutError(f"no response within {self.ser.timeout}s")
        return line.decode("ascii", errors="replace").strip()


class ScpiTestCase(unittest.TestCase):
    link: ScpiLink = None

    @classmethod
    def setUpClass(cls):
        cls.link = ScpiLink(PORT, BAUDRATE, TIMEOUT)

    @classmethod
    def tearDownClass(cls):
        cls.link.close()

    def setUp(self):
        self.link.send("*CLS")

    def test_idn(self):
        idn = self.link.query("*IDN?")
        fields = idn.split(",")
        self.assertEqual(len(fields), 4, f"*IDN? reply malformed: {idn!r}")
        manufacturer, model, _serial_no, _fw = fields
        self.assertTrue(manufacturer)
        self.assertTrue(model)
        print(fields)

    def test_output_roundtrip(self):
        self.link.send("OUTPut ON")
        self.assertEqual(self.link.query("OUTPut?"), "1")
        time.sleep(1) # to visually check the led
        self.link.send("OUTPut OFF")
        self.assertEqual(self.link.query("OUTPut?"), "0")


def main():
    global PORT, BAUDRATE, TIMEOUT

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("-p", "--port", required=True, help="serial port, e.g. /dev/ttyACM0 or COM5")
    parser.add_argument("-b", "--baudrate", type=int, default=BAUDRATE)
    parser.add_argument("-t", "--timeout", type=float, default=TIMEOUT)
    parser.add_argument("unittest_args", nargs="*", help="extra args forwarded to unittest")
    args = parser.parse_args()

    PORT = args.port
    BAUDRATE = args.baudrate
    TIMEOUT = args.timeout

    sys.argv[1:] = args.unittest_args
    unittest.main()


if __name__ == "__main__":
    main()
